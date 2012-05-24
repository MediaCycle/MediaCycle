/*
 *  ACMediaLibrary.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 21/04/09
 *
 *  - XS 26/06/09 : removed size_library (= media_library.size())
 *  - XS 13/10/09 : added destructor to clean up vector <ACMedia*>
 *
 *  @copyright (c) 2009 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

// XS try openMP for parallelisation
// #include <omp.h>

#include "ACPluginManager.h"
#include "ACMediaLibrary.h"
#include "ACMediaFactory.h"

#include "ACMediaDocument.h"

#include <sstream>

// XS for sorting:
#include <algorithm>

namespace fs = boost::filesystem;

#define VERBOSE

//#if defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
//// CF FFmpeg for checking audio/video channels in containers
//#include <ACFFmpegInclude.h>
//#endif //defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)

using namespace std;
// to save library items in binary format, uncomment the following line:
// #define SAVE_LOOP_BIN

ACMediaLibrary::ACMediaLibrary() {
	this->cleanLibrary();
	media_type = MEDIA_TYPE_NONE;
	mPreProcessPlugin=0;
	mPreProcessInfo=0;
	mReaderPlugin=0;
}

ACMediaLibrary::ACMediaLibrary(ACMediaType aMediaType) {
	this->cleanLibrary();
	media_type = aMediaType;
	mPreProcessPlugin=0;
	mPreProcessInfo=0;
	mReaderPlugin=0;
	//#if defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
	//// Register all formats and codecs from FFmpeg
	//av_register_all();
	//#endif //defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
}

ACMediaLibrary::~ACMediaLibrary(){
	this->deleteAllMedia();
	
	if (mPreProcessInfo!=NULL)
		mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
}

bool ACMediaLibrary::changeMediaType(ACMediaType aMediaType){
	if (this->isEmpty()){
		media_type = aMediaType;
		return true;
	}
	else {
		// can't change mediatype of a library which is already loaded
		// need to clean library first (from elsewhere, e.g., from the app)
		return false;
	}
}

void ACMediaLibrary::deleteAllMedia(){
	// Clean up properly by calling destructor of each ACMedia*
	std::vector<ACMedia*>::iterator iter;
	for (iter = media_library.begin(); iter != media_library.end(); iter++) {
		delete *iter;
	}
	media_library.clear();
}

// this is the method to be called when re-initializing
void ACMediaLibrary::cleanLibrary() {
	cleanStats();
	deleteAllMedia();
	index_last_normalized = -1;
	synthesisID = -1;
	mediaID = 0; // index of the next one to be inserted (= the number of media already imported); not -1
	media_path ="";
	// XS leave the media type as is, suppose we'll keep loading the same type of media
	// (not this:) media_type=MEDIA_TYPE_NONE;
        metadata = ACMediaLibraryMetadata();
        failed_imports.clear();
}

std::vector<std::string> ACMediaLibrary::getExtensionsFromMediaType(ACMediaType media_type)
{
	if (mReaderPlugin!=NULL){
		if (mReaderPlugin->mediaTypeSuitable(media_type)){
			return mReaderPlugin->getExtensionsFromMediaType(media_type);
		}
	}
	
	return ACMediaFactory::getInstance().getExtensionsFromMediaType(media_type);
	
}


// this same function is used to import a whole directory, a single file or a list of files.
// it uses scanDirectory to construct a vector (filenames) containing the files to be analyzed.
// return values:
//	-1 if error
//  0 if empty directory (or no media of the required type found)
//	> 0 if no error (returns the number of files found)
int ACMediaLibrary::importDirectory(std::string _path, int _recursive, ACPluginManager *acpl, bool forward_order, bool doSegment, bool _save_timed_feat){ //, TiXmlElement* _medias) {
	std::vector<string> filenames;
 	int nf = scanDirectory(_path, _recursive, filenames);

	if (nf < 0) {
		cerr << " <ACMediaLibrary::importDirectory> Problem importing directory: " << _path << endl;
		return -1;
	}
	else if (nf == 0) {
		cout << "Warning : Empty directory: " << _path << endl;
		return 0;
	}

	// XS todo make this more flexible
	// here we force the plugins to save timed features for segmentation
	// they could also segment on-the-fly while calculating features (as in audioSegmentationPlugin)
	if (doSegment) _save_timed_feat = true;

	for (unsigned int i=0; i<filenames.size(); i++){
		int index = forward_order ? i : filenames.size()-1-i;//CF if reverse order (not forward_order), segments in subdirs are added to the library after the source recording
		this->importFile(filenames[index], acpl, doSegment, _save_timed_feat); //, _medias );
	}

	std::cout << "Library size : " << this->getSize() << std::endl;
        std::cout << "Failed imports: " << std::endl;
        if(failed_imports.size()>0){
        std::vector<std::string>::iterator failed_import;
            for (failed_import=failed_imports.begin();failed_import!=failed_imports.end();failed_import++)
                std::cout << "\t" << (*failed_import) << std::endl;
        }
	return this->getSize();
}


// import single file :
// computes features (= "import" media) using FEATURES plugins available in the plugin Manager
// doSegment = true : uses SEGMENTATION plugins on-the-fly
// save_timedfeat = true : save the timedFeatures on the disk
int ACMediaLibrary::importFile(std::string _filename, ACPluginManager *acpl, bool doSegment, bool _save_timed_feat){ //, TiXmlElement* _medias) {
	// check if file has already been imported.
	// XS TODO: isn't this going to be too heavy when library size gets large ?
	// may be add a flag to (in)activate the test ?
	int is_present = this->getMediaIndex(_filename);
	if (is_present >= 0) {
		cout << "<ACMediaLibrary::importFile> skipping "<< _filename << " : media already present at position " << is_present << endl;
		return 0;
	}

	string extension = fs::extension(_filename);

	ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(extension);
	ACMedia* media=0;
#if defined (SUPPORT_MULTIMEDIA) 
	//this->testFFMPEG(_filename);
	if (media_type==MEDIA_TYPE_MIXED){
		if (mReaderPlugin!=0)
		
			media=mReaderPlugin->mediaFactory(media_type);
		if (media==0)
			media=new ACMediaDocument();
		if (media==0)
			return 0;
		else {
			int nbMedia = media->import(_filename, this->getMediaID(), acpl);
			if (nbMedia<=0)
				return 0;
			this->addMedia(media);
			this->incrementMediaID();
			ACMediaContainer medias = (static_cast<ACMediaDocument*> (media))->getContainer();
			ACMediaContainer::iterator iter;
			for ( iter=medias.begin() ; iter!=medias.end(); ++iter ){
				this->incrementMediaID();
				this->addMedia(iter->second);
			}

			return 1;
		}
	}
#endif	
		// XS TODO: do we want to check the media type of the whole library (= impose a unique one)?
		
	if (media_type == fileMediaType){
		media = ACMediaFactory::getInstance().create(extension);
		cout << "extension:" << extension << endl;
	}
	else {
		cout << "<ACMediaLibrary::importFile> other media type, skipping " << _filename << " ... " << endl;
		cout << "-> media_type " << media_type << " ... " << endl;
		cout << "-> fileMediaType " << fileMediaType << " ... " << endl;
		return 0;
	}
	
	if (media == 0) {
		cout << "<ACMediaLibrary::importFile> extension unknown, skipping " << _filename << " ... " << endl;
		return 0;
	}
	// import has to be done before segmentation to have proper id.
	else if (media->import(_filename, this->getMediaID(), acpl, _save_timed_feat)){
		this->addMedia(media);
#ifdef VERBOSE
		cout << "imported " << _filename << " with mid = " <<  this->getMediaID() << endl;
#endif // VERBOSE
		this->incrementMediaID();
		if (doSegment){
			// segments are created without id
			media->segment(acpl, _save_timed_feat);
			
//			if(this->media_type == MEDIA_TYPE_VIDEO)//CF
//				std::cout << "\n\nWARNING! Video segments not added to the library to test segmentation until features are not re-calculated from each segment!\n\n" << std::endl;
//			else{
				std::vector<ACMedia*> mediaSegments;
				mediaSegments = media->getAllSegments();
				for (unsigned int i = 0; i < mediaSegments.size(); i++){
					// for the segments we do not save (again) timedFeatures
					// XS TODO but we should not re-calculate them either !=> TR I put the mtf files names of the media parent in all his segments
					if (mediaSegments[i]->import(_filename, this->getMediaID(), acpl)){
						this->addMedia(mediaSegments[i]);
						this->incrementMediaID();
						mediaSegments[i]->deleteData();//TR TODO verify that we must delete this data
					}
					
				}
//			}
			
		}
	}
	else {
		cerr << "<ACMediaLibrary::importFile> problem importing file : " << _filename << " ... " << endl;
                failed_imports.push_back(_filename);
                 return 0;
		
	}
	// XS TODO this was an attempt to save on-the-fly each media 
	// but it does not work well.
	
	// appending current media (if imported properly) to the project's XML file
	//media->saveXML(_medias);
	
	// deleting the data that have been used for analysis, keep media small.
	media->deleteData();
	return 1;
}

int ACMediaLibrary::scanDirectories(std::vector<string> _paths, int _recursive, std::vector<string>& filenames) {
	int cnt = 0;
	for (unsigned int i=0; i<_paths.size(); i++) {
		int c = scanDirectory(_paths[i], _recursive, filenames) ; // could be -1 if error
		if (c > 0) cnt += c;
	}
	return cnt;
}

// construct a vector (filenames) containing the files in a given directory (_path).
// when "recursive" is turned on, it will scan subdirectories too
// return values :
//    1 = it found files -- 	//XS TODO : return filenames.size()
//    0 = empty
//   -1 = directory not found

int ACMediaLibrary::scanDirectory(std::string _path, int _recursive, std::vector<string>& filenames) {

	fs::path full_path( fs::initial_path<fs::path>() );

// check boost version to adapt for change in boost api	(for boost > 1.46)
#if BOOST_FILESYSTEM_VERSION >= 3
	full_path = fs::system_complete( fs::path( _path) );
	if ( !fs::exists( full_path ) )	{
		cout << "File or directory not found: " << full_path.string() << endl;
		return -1;
	}
	if ( fs::is_directory( full_path ) ) 	{ // importing directory
		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter; ++dir_itr ){
			if ( _recursive && fs::is_directory( dir_itr->path() ) ) {
				scanDirectory((dir_itr->path()).string(), 1, filenames);
			}
			else if ( fs::is_regular( dir_itr->path() ) ){
				scanDirectory((dir_itr->path()).string(), 0, filenames);
			}
			else {
			}
		}
	}

#else
	full_path = fs::system_complete( fs::path( _path, fs::native ) );
	if ( !fs::exists( full_path ) )	{
		cout << "File or directory not found: " << full_path.native_file_string() << endl;
		return -1;
	}
	if ( fs::is_directory( full_path ) ) 	{ // importing directory
		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter; ++dir_itr ){
			if ( _recursive && fs::is_directory( dir_itr->path() ) ) {
				scanDirectory((dir_itr->path()).native_file_string(), 1, filenames);
			}
			else if ( fs::is_regular( dir_itr->path() ) ){
				scanDirectory((dir_itr->path()).native_file_string(), 0, filenames);
			}
			else {
			}
		}
	}

#endif
	else {
		// encounter a file in the directory
		// put it in the vector of files to be analyzed
		filenames.push_back(_path);
	}

	//XS TODO : return filenames.size()
	if (filenames.size() == 0) return 0;
	return 1;
}

int ACMediaLibrary::setPath(std::string path) {
	media_path = path;
	return 1;
}

// C++ version
int ACMediaLibrary::openACLLibrary(std::string _path, bool aInitLib){
	// this does not re-initialize the media_library
	// but appends new media to it.
	// except if aInitLib is set to true
	int ret, file_count=0;

	ifstream library_file;
	library_file.open(_path.c_str());

	if ( ! library_file ) {
		cerr << "<ACMediaLibrary::openACLLibrary> error reading file " << _path << endl;
		return 0;
	}
	else{
		cout << "opening " << _path << endl;
	}
	ACMedia* local_media;
	// --TODO-- ???  how does it know which type of media ?
	// have to be set up  at some point using setMediaType()
	if (aInitLib) {
		cleanLibrary();
	}
	//media_library.resize(0);
	do {
		local_media = ACMediaFactory::getInstance().create(media_type);
		if (local_media != 0) {
			if (!media_path.empty()) {
				ret = local_media->loadACL(media_path, library_file);
			}
			else {
				ret = local_media->loadACL("", library_file);
			}
			if (ret) {
				//std::cout << "Media Library Size : " << this->getSize() << std::endl;//CF free the console
				if (mPreProcessPlugin==NULL)
					local_media->defaultPreProcFeatureInit();
				media_library.push_back(local_media);
				file_count++;
			}
		}
		else {
			std::cout<<"<ACMediaLibrary::openACLLibrary> : Wrong Media Type" << std::endl;
		}
	}
	while (ret>0);
	library_file.close();
	return file_count;
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
int ACMediaLibrary::openMCSLLibrary(std::string _path, bool aInitLib){
	// this does not re-initialize the media_library
	// but appends new media to it.
	// except if aInitLib is set to true
	int ret, file_count=0;

	ifstream library_file;
	library_file.open(_path.c_str());

	if ( ! library_file ) {
		cerr << "<ACMediaLibrary::openMCSLLibrary> error reading file " << _path << endl;
		return 0;
	}
	else{
		cout << "opening " << _path << endl;
	}

	//CF Header checks
	std::string type;
	std::string version;
	getline(library_file, type);
	getline(library_file, version); //CF v. 0.1 on 31/05/2010, adding parentid to ACL v0.1
	if ( type != "MediaCycle Segmented Library") {
		if ( type.find_first_of('/') != string::npos || type.find_first_of('\\') != string::npos) //CF ugly: ACL using path delimiters/special caracters on OSX/Linux (/) or Windows (\)
			cerr << "<ACMediaLibrary::openMCSLLibrary> wrong library file type: original ACL" << endl;
		else
			cerr << "<ACMediaLibrary::openMCSLLibrary> wrong library file type: '" << type << "', instead of 'MediaCycle Segmented Library'" << endl;
		return 0;
	}
	if ( version != "0.1" ) {
		cerr << "<ACMediaLibrary::openMCSLLibrary> unsupported version: v." << version << ", instead of v.0.1" << endl;
		return 0;
	}

	ACMedia* local_media;
	// --TODO-- ???  how does it know which type of media ?
	// have to be set up  at some point using setMediaType()
	if (aInitLib) {
		cleanLibrary();
	}
	//media_library.resize(0);
	do {
		local_media = ACMediaFactory::getInstance().create(media_type);
		if (local_media != 0) {
			ret = local_media->loadMCSL(library_file);
			if (ret) {
				//std::cout << "Media Library Size : " << this->getSize() << std::endl;//CF free the console
				if (mPreProcessPlugin==NULL)
					local_media->defaultPreProcFeatureInit();
				media_library.push_back(local_media);
				file_count++;
			}
		}
		else {
			std::cout<<"<ACMediaLibrary::openMCSLLibrary> : Wrong Media Type" << std::endl;
		}
	}
	while (ret>0);
	library_file.close();
	return file_count;
}

int ACMediaLibrary::openXMLLibrary(std::string _path, bool aInitLib){
	TiXmlDocument doc( _path.c_str() );
	try {
                if (!doc.LoadFile( TIXML_ENCODING_UTF8 ))
			throw runtime_error("bad parse");
		//		doc.Print( stdout );
	} catch (const exception& e) {
		cout << e.what( ) << endl;
		return EXIT_FAILURE;
    }

	if (aInitLib) {
		cleanLibrary();
	}

	TiXmlHandle docHandle(&doc);
	// XS TODO ? make rootHandle a pointer ?
	TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
	try {
		this->openCoreXMLLibrary(rootHandle);
	}
	catch (const exception& e) {
		cout << e.what( ) << endl;
		return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}

int ACMediaLibrary::openCoreXMLLibrary(TiXmlHandle _rootHandle){

    // Library Metadata
    TiXmlText* nLibraryMetadata=_rootHandle.FirstChild( "LibraryMetadata" ).FirstChild().Text();
    TiXmlElement* nLibraryMetadataNode=_rootHandle.FirstChild( "LibraryMetadata" ).Element();
    if(nLibraryMetadataNode){
        if(nLibraryMetadataNode->Attribute("Title"))
            this->metadata.title = nLibraryMetadataNode->Attribute("Title");
        if(nLibraryMetadataNode->Attribute("Author"))
            this->metadata.author = nLibraryMetadataNode->Attribute("Author");
        //if(nLibraryMetadataNode->Attribute("Curator"))
        //    this->metadata.curator.name = nLibraryMetadataNode->Attribute("Curator");
        if(nLibraryMetadataNode->Attribute("Year"))
            this->metadata.year = nLibraryMetadataNode->Attribute("Year");
        if(nLibraryMetadataNode->Attribute("Publisher"))
            this->metadata.publisher = nLibraryMetadataNode->Attribute("Publisher");
        if(nLibraryMetadataNode->Attribute("License"))
            this->metadata.license = nLibraryMetadataNode->Attribute("License");
        if(nLibraryMetadataNode->Attribute("Website"))
            this->metadata.website = nLibraryMetadataNode->Attribute("Website");
        if(nLibraryMetadataNode->Attribute("Cover"))
            this->metadata.cover = nLibraryMetadataNode->Attribute("Cover");
        TiXmlElement* nLibraryCuratorNode=_rootHandle.FirstChild( "LibraryMetadata" ).FirstChild( "Curator" ).Element();
        if(nLibraryCuratorNode){
            if(nLibraryCuratorNode->Attribute("Name"))
                this->metadata.curator.name = nLibraryCuratorNode->Attribute("Name");
            if(nLibraryCuratorNode->Attribute("Email"))
                this->metadata.curator.email = nLibraryCuratorNode->Attribute("Email");
            if(nLibraryCuratorNode->Attribute("Website"))
                this->metadata.curator.website = nLibraryCuratorNode->Attribute("Website");
            if(nLibraryCuratorNode->Attribute("Location"))
                this->metadata.curator.location = nLibraryCuratorNode->Attribute("Location");
            if(nLibraryCuratorNode->Attribute("Picture"))
                this->metadata.curator.picture = nLibraryCuratorNode->Attribute("Picture");
        }
    }

	TiXmlText* nMediaText=_rootHandle.FirstChild( "NumberOfMedia" ).FirstChild().Text();
	std::stringstream tmp;
	int n_loops=0;

	if (!nMediaText)
		throw runtime_error("corrupted XML file, no number of media");
	else {
		string nof = nMediaText->ValueStr();
		tmp << nof;
		tmp >> n_loops;
	}
	TiXmlElement* pMediaNode=_rootHandle.FirstChild( "Medias" ).FirstChild().Element();
    #ifdef SUPPORT_MULTIMEDIA
    if (!pMediaNode)
        pMediaNode=_rootHandle.FirstChild( "MediaDocuments" ).FirstChild().Element();
    #endif
	if (!pMediaNode)
		throw runtime_error("corrupted XML file, no medias");
	else {
		// loop over all medias
		int cnt=0;
		for( pMediaNode; pMediaNode; pMediaNode=pMediaNode->NextSiblingElement()) {
			ACMediaType typ;
			int typi = -1;
			pMediaNode->QueryIntAttribute("MediaType", &typi);
			if (typi < 0)
				throw runtime_error("corrupted XML file, wrong media type");
			else {
				typ = (ACMediaType) typi;
				ACMedia* local_media = ACMediaFactory::getInstance().create(typ);
				local_media->loadXML(pMediaNode);
				if (mPreProcessPlugin==NULL)
					local_media->defaultPreProcFeatureInit();
				media_library.push_back(local_media);
				// make sure the library knows which id is the last one
				this->setMediaID(local_media->getId());

				#ifdef SUPPORT_MULTIMEDIA
                if(this->media_type == MEDIA_TYPE_MIXED){

                    string activeMediaKey ="";
                    activeMediaKey = pMediaNode->Attribute("ActiveMediaKey");
                    if (activeMediaKey == "")
                        throw runtime_error("corrupted XML file, no active submedia defined");
                    std::cout<<"activeMediaKey"<<activeMediaKey<<std::endl;

                    TiXmlHandle _pMediaNodeHandle(pMediaNode);
                    TiXmlElement* mediasElement = _pMediaNodeHandle.FirstChild( "Medias" ).Element();
                    if (!mediasElement)
                        throw runtime_error("corrupted XML file, no medias in mediadocuments");

                    TiXmlElement* mediaElement = _pMediaNodeHandle.FirstChild( "Medias" ).FirstChild( "Media" ).Element();
                    for( mediaElement; mediaElement; mediaElement=mediaElement->NextSiblingElement()) {
                        ACMediaType mtyp;
                        int mtypi = -1;
                        mediaElement->QueryIntAttribute("MediaType", &mtypi);
                        if (mtypi < 0)
                            throw runtime_error("corrupted XML file, wrong media type");
                        else {
                            mtyp = (ACMediaType) mtypi;
                            ACMedia* local_submedia = ACMediaFactory::getInstance().create(mtyp);
                            local_submedia->setParentId(local_media->getId());
                            local_submedia->loadXML(mediaElement);
                            if (mPreProcessPlugin==NULL)
                                local_submedia->defaultPreProcFeatureInit();
                            media_library.push_back(local_submedia);
                            this->setMediaID(local_submedia->getId());

                            string pKey ="";
                            pKey = mediaElement->Attribute("Key");
                            if (pKey == "")
                                throw runtime_error("corrupted XML file, no key for media in media document");
                            local_submedia->setKey(pKey);
                            ACMediaDocument *mediaDoc=static_cast<ACMediaDocument *> (local_media);
                            mediaDoc->addMedia(pKey,local_submedia);

                            if (activeMediaKey == pKey)
                                mediaDoc->setActiveSubMedia(pKey);
                        }
                    }
                }
				#endif
				cnt++;
			}
		}
		// consistency check (XS TODO quits the app -> exceptions )
		if (cnt != n_loops){
			cerr << "<ACMediaLibrary::openXMLLibrary>: inconsistent number of media"<< endl;
			cerr << "n_loops = " << n_loops << "; cnt = " << cnt << endl;
			return -1;
		}
	}
	return n_loops;
}

// C++ version
// XS TODO return value
int ACMediaLibrary::saveACLLibrary(std::string _path){
#ifdef SAVE_LOOP_BIN
	ofstream library_file (_path.c_str(), ios::binary);
#else
	ofstream library_file (_path.c_str());
#endif //SAVE_LOOP_BIN
	cout << "saving ACL file: " << _path << endl;

	int n_loops = this->getSize();
	// we save UNnormalized features
	//denormalizeFeatures();
	for(int i=0; i<n_loops; i++) {
		media_library[i]->saveACL(library_file);
	}
	library_file.close();
	//normalizeFeatures();
}

// XS TODO return value
// this only saves the equivalent of the ACL config
// will NOT contain header (browser, plugins, ...) information.
// (obsolete ? useful ?)
int ACMediaLibrary::saveXMLLibrary(std::string _path){
        // we save UNnormalized features
	//denormalizeFeatures();

    // CF this is not parsed anymore in favor of MediaCycle::saveXMLConfig

	TiXmlDocument MC_doc;
    TiXmlDeclaration* MC_decl = new TiXmlDeclaration( "1.0", "", "" );

	MC_doc.LinkEndChild( MC_decl );

    TiXmlElement* MC_e_root = new TiXmlElement( "MediaCycle" );
    MC_doc.LinkEndChild( MC_e_root );

    std::string media_identifier = "Medias";
    if(this->getMediaType() == MEDIA_TYPE_MIXED){
        media_identifier = "MediaDocuments";
    }
    TiXmlElement* MC_e_medias = new TiXmlElement(media_identifier);
    MC_e_root->LinkEndChild( MC_e_medias );

	this->saveCoreXMLLibrary(MC_e_root, MC_e_medias);

	// normalize features again
	//normalizeFeatures();
        MC_doc.SaveFile( _path.c_str());
}

// no headers, just media information
// can be called from MediaCycle's saveXML or from the app via mediaCycle
int ACMediaLibrary::saveCoreXMLLibrary( TiXmlElement* _MC_e_root, TiXmlElement* _MC_e_medias ){

    // library metadata
    TiXmlElement* MC_e_library_metadata = new TiXmlElement("LibraryMetadata");
    _MC_e_root->LinkEndChild(  MC_e_library_metadata );
    MC_e_library_metadata->SetAttribute("Title", this->metadata.title);
    MC_e_library_metadata->SetAttribute("Author", this->metadata.author);
    //MC_e_library_metadata->SetAttribute("Curator", this->metadata.curator.name);
    MC_e_library_metadata->SetAttribute("Year", this->metadata.year);
    MC_e_library_metadata->SetAttribute("Publisher", this->metadata.publisher);
    MC_e_library_metadata->SetAttribute("License", this->metadata.license);
    MC_e_library_metadata->SetAttribute("Website", this->metadata.website);
    MC_e_library_metadata->SetAttribute("Cover", this->metadata.cover);

    TiXmlElement* MC_e_library_curator = new TiXmlElement("Curator");
    MC_e_library_metadata->LinkEndChild(  MC_e_library_curator );
    MC_e_library_curator->SetAttribute("Name", this->metadata.curator.name);
    MC_e_library_curator->SetAttribute("Email", this->metadata.curator.email);
    MC_e_library_curator->SetAttribute("Website", this->metadata.curator.website);
    MC_e_library_curator->SetAttribute("Location", this->metadata.curator.location);
    MC_e_library_curator->SetAttribute("Picture", this->metadata.curator.picture);

    // "medias and features"
    TiXmlElement* MC_e_number_of_medias = new TiXmlElement( "NumberOfMedia" );
    _MC_e_root->LinkEndChild( MC_e_number_of_medias );
	int n_loops = this->getSize();
	std::string s_loops;
	std::stringstream tmp;
	tmp << n_loops;
	s_loops = tmp.str();

    int a=0;

    TiXmlText* MC_t_nm = new TiXmlText( s_loops );
    MC_e_number_of_medias->LinkEndChild( MC_t_nm );

	// XS TODO iterator
    std::vector<TiXmlElement*> medias;

	for(int i=0; i<n_loops; i++) {
        if(this->media_type != MEDIA_TYPE_MIXED){
            TiXmlElement* media = new TiXmlElement( "Media" );
            _MC_e_medias->LinkEndChild( media );
            media_library[i]->saveXML( media);
        }
        else{
            if(media_library[i]->getMediaType() == MEDIA_TYPE_MIXED){
                TiXmlElement* doc = new TiXmlElement( "MediaDocument" );
                _MC_e_medias->LinkEndChild( doc );
                media_library[i]->saveXML( doc );

                medias.push_back( new TiXmlElement( "Medias" ) );
                doc->LinkEndChild( medias.back() );

            }
            else{
                TiXmlElement* media = new TiXmlElement( "Media" );
                medias.back()->LinkEndChild( media );
                media_library[i]->saveXML( media );
            }
        }
	}
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
// XS TODO return value
int ACMediaLibrary::saveMCSLLibrary(std::string _path){
#ifdef SAVE_LOOP_BIN
	ofstream library_file (_path.c_str(), ios::binary);
#else
	ofstream library_file (_path.c_str());
#endif //SAVE_LOOP_BIN
	cout << "saving MCSL file: " << _path << endl;

	//CF Header
	library_file << "MediaCycle Segmented Library" << endl;
	library_file << 0.1 << endl; //CF v. 0.1 on 31/05/2010, adding parentid to ACL v0.1

	int n_loops = this->getSize();
	// we save UNnormalized features
	denormalizeFeatures();
	for(int i=0; i<n_loops; i++) {
		media_library[i]->saveMCSL(library_file);
	}
	library_file.close();
	normalizeFeatures();
}

int ACMediaLibrary::addMedia(ACMedia *aMedia) {
    // XS TODO remove media_type check
    // mediacycle should be able to manage a mix of any media
    // instead of only medias of one type
	if (aMedia != 0){
		if (aMedia->getType() == this->media_type || this->media_type == MEDIA_TYPE_MIXED) {
			if (mPreProcessPlugin==NULL)
				aMedia->defaultPreProcFeatureInit();
			this->media_library.push_back(aMedia);
			return 0;
		} else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

ACMedia* ACMediaLibrary::getMedia(int i){
	if (i < this->getSize() && i >=0){
		return media_library[i];
	}
	else {
		cerr << "<ACMediaLibrary::getMedia> index out of bounds: " << i << endl;
		return 0;
	}
}

int ACMediaLibrary::deleteMedia(int i){
	if (i < this->getSize() && i >=0){
		media_library.erase(media_library.begin()+i);
		return 1;
	}
	else {
		cerr << "<ACMediaLibrary::deleteMedia> index out of bounds: " << i << endl;
		return -1;
	}
}

int ACMediaLibrary::getMediaIndex(std::string media_file_name){
	for (int i=0;i<this->getSize();i++) {
		if (media_file_name==media_library[i]->getFileName()) {
			return media_library[i]->getId();
		}
	}
	return -1;

}

std::string ACMediaLibrary::getThumbnailFileName(int id) {
	int i;
	for (i=0;i<this->getSize();i++) {
		if (id==media_library[i]->getId()) {
			return media_library[i]->getThumbnailFileName();
		}
	}
	return "";
}
std::vector<int> ACMediaLibrary::getParentIds(void){
	std::vector<int> ret;
    for (int i=0;i<this->getSize();i++) {
		if (media_library[i]->getParentId()==-1 ) {
            //if (media_library[i]->getId()==i) // CF wrong and useless test!
				ret.push_back(i);
		}
		else {
			if (media_library[i]->getMediaType()==media_library[media_library[i]->getParentId()]->getMediaType() )
				ret.push_back(i);
				
		}
	}
	return ret;
}

bool ACMediaLibrary::isEmpty() {
	if (this->getSize() <= 0) {
		cout << "empty library" << endl;
		return true;
	}

	// XS assumes each item has same number of features
	// and each feature has the same number of floats in its vector

	// XS TODO do we really want to check both together ?
	// we could have loaded media without (yet) features...
	int number_of_features = media_library[0]->getNumberOfFeaturesVectors();
	if (number_of_features <= 0) {
		cout << "no features in vector [0]" << endl;
		return true;
	}
	return false;
}

void ACMediaLibrary::cleanStats() {
	cout << "cleaning features mean and stdev"<< endl;
	mean_features.clear();
	stdev_features.clear();
}

void ACMediaLibrary::calculateStats() {
	cleanStats();
	if ( isEmpty() ) return; // takes car of case n==0
	int n = this->getSize() ;
	int number_of_features = media_library[0]->getNumberOfFeaturesVectors();
	int mt = 0; // number of media files of the library that have the same media type as the library (might be different in case of multimedia documents)

	// initialize to zero
	int i,j,k;
	for (i=0; i< number_of_features; i++) {
		if (media_type != MEDIA_TYPE_MIXED || (media_type == MEDIA_TYPE_MIXED && media_library[i]->getMediaType() == media_type)){ //CF
			vector<double> tmp_vect;
			for (j=0; j< media_library[0]->getFeaturesVector(i)->getSize(); j++) {
				tmp_vect.push_back(0.0);
			}
			mean_features.push_back(tmp_vect);
			stdev_features.push_back(tmp_vect);
			mt++; //CF
		}	
	}

	// computing sums
	for(i=0; i<n; i++) {
		ACMedia* item = media_library[i];
		if (item->getMediaType() == this->media_type){ //CF
			for(j=0; j<(int)mean_features.size(); j++){
				for(k=0; k<(int)mean_features[j].size(); k++){
					double val = item->getFeaturesVector(j)->getFeatureElement(k);
					mean_features[j][k] += val;
					stdev_features[j][k] += val * val;
				}
			}
		}	
	}

	// before: divide by n --> biased variance estimator
	// now : divide by (n-1) -- unless n=1
	/*int nn;
	if (n==1) nn = n;
	else nn = n-1;*/
	int nn;
	if (n==1) nn = n;
	else nn = n-1; 

	for(j=0; j<(int)mean_features.size(); j++) {
		cout << "calculating stats for feature" << j << endl;
		for(k=0; k<(int)mean_features[j].size(); k++) {
			mean_features[j][k] /= n;
			stdev_features[j][k] /= n;
			double tmp = stdev_features[j][k] - mean_features[j][k] * mean_features[j][k];
			if ( tmp < 0 )
				stdev_features[j][k] = 0;
			else {
				stdev_features[j][k] = sqrt( tmp*((1.0*n)/(nn)));
			}
			//printf("\t[%d] mean_features = %f, stddev = %f\n", k, mean_features[j][k], stdev_features[j][k]);//CF free the console
		}
	}
}

void ACMediaLibrary::normalizeFeatures(int needsNormalize) {

	int start;
	
	ACMediaFeatures* feature,* featureDest;
	cout << "normalizing features" << endl;
	if ( isEmpty() )  return;

	if (index_last_normalized < 0) {
		// *first* normalization
		if (mPreProcessPlugin!=NULL){
			if (mPreProcessInfo!=NULL)
				mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
			mPreProcessInfo=mPreProcessPlugin->update(media_library);			
		}
		else {
			calculateStats();
		}
	}
	else {
		// *subsequent* normalization
		if (needsNormalize) {
			// *first* normalization
			if (mPreProcessPlugin!=NULL){
				if (mPreProcessInfo!=NULL)
					mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
				mPreProcessInfo=mPreProcessPlugin->update(media_library);
			}
			else {
//				denormalizeFeatures();
				calculateStats();
			}
		}
	}	
	unsigned int i,j,k;
	vector<int> currId=this->getParentIds();
	//TR normalize just Parent Nodes
	unsigned int n = currId.size();
	
	//unsigned int n = this->getSize() ;

	if (needsNormalize) {
		start = 0;
	}
	else {
		start = index_last_normalized+1;
	}
	if (mPreProcessPlugin!=NULL){	
		for(i=start; i<n; i++){
			ACMedia* item = media_library[currId[i]];
			item->cleanPreProcFeaturesVector();
			std::vector<ACMediaFeatures*> tempFeatVect;
			#ifdef SUPPORT_MULTIMEDIA
			if (item->getMediaType()==MEDIA_TYPE_MIXED)
				tempFeatVect=mPreProcessPlugin->apply(mPreProcessInfo,((ACMediaDocument*)item)->getActiveMedia());
			else
			#endif//def SUPPORT_MULTIMEDIA	
				tempFeatVect=mPreProcessPlugin->apply(mPreProcessInfo,item);
			
			item->getAllPreProcFeaturesVectors()=tempFeatVect;
			tempFeatVect.clear();
		}		
	}
	else {
		for(i=start; i<n; i++){
			ACMedia* item = media_library[currId[i]];
			if (item->getMediaType() == this->media_type){ //CF
				for(j=0; j<mean_features.size(); j++) {
					feature = item->getFeaturesVector(j);
					featureDest=item->getPreProcFeaturesVector(j);
					if (feature->getNeedsNormalization()) {
						for(k=0; k<mean_features[j].size(); k++) {
							float old = feature->getFeatureElement(k);
							featureDest->setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//CF TI_MAX(stdev_features[j][k] , 0.00001)));
						}
					}
					else {
						for(k=0; k<mean_features[j].size(); k++) {
							float old = feature->getFeatureElement(k);
							featureDest->setFeatureElement(k, old);//setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//CF TI_MAX(stdev_features[j][k] , 0.00001)));
						}
					}

				}
			}	
		}
	}
		
	index_last_normalized = n-1;
}

void ACMediaLibrary::denormalizeFeatures() {
	cout << "denormalizing features" << endl;
	if ( isEmpty() || index_last_normalized<0) return; // Ju: added index_last_normalized<0 to avoid segmentation fault under linux, I believe caused by the fact i is declared as unsigned int while index_last_normalized is int, so it messed up in the for loop

	unsigned int j,k;
	int i;

	// XS denormalize only those that have been normalized (duh),
	// so n is NOT the full library size !

	for(i=0; i<= index_last_normalized; i++){
		ACMedia* item = media_library[i];
		if (item->getMediaType() == this->media_type){ //CF
			for(j=0; j<mean_features.size(); j++) {
				if (item->getFeaturesVector(j)->getNeedsNormalization()) {
					for(k=0; k<mean_features[j].size(); k++) {
						float old = item->getFeaturesVector(j)->getFeatureElement(k);
						item->getFeaturesVector(j)->setFeatureElement(k, old * stdev_features[j][k] + mean_features[j][k]);
					}
				}
			}
		}	
	}
	index_last_normalized = -1;
	cleanStats();
}

// returns the list of plugins that have been used to calculate the mediafeatures for media[0]
std::vector<std::string> ACMediaLibrary::getListOfActivePlugins(){
	std::vector<std::string> plugins_list;
	if (this->getSize() ==0)
		plugins_list.clear();
	else
		plugins_list = this->getMedia(0)->getListOfFeaturesPlugins();
	return plugins_list;
}

// -------------------------------------------------------------------------
// XS custom for Thomas Israel videos : features sorted, along with filename
void ACMediaLibrary::saveSorted(string output_file){
	//XS dirty -- should be generalized !
	ofstream out(output_file.c_str());

	const int nfeat=10;
	std::vector<std::pair<float, int> > f[nfeat];

	for (int i=0; i< this->getSize() ;i++) {
		out << i+1 << " : " << media_library[i]->getFileName() << endl;
		int nfeatv =  media_library[i]->getNumberOfFeaturesVectors();
		if (nfeatv <= nfeat) {
			cerr << "problem with custom saveSorted : " << nfeatv << "<=" << nfeat<< endl;
		}
		for (int j=0; j< nfeat-1 ;j++) { // XS TODO: this is custom, to skip 0
			// XS TODO : this will only take first dimension of feature...
			//int nfeat = media_library[i]->getFeaturesVector(j)->getSize();
			out << media_library[i]->getFeaturesVector(j+1)->getFeatureElement(0) << endl;
			f[j].push_back (std::pair<float, int> (media_library[i]->getFeaturesVector(j+1)->getFeatureElement(0), i));
		}
	}

	for (int j=0; j< nfeat-1 ;j++) {
		sort (f[j].begin(),f[j].end());
	}
	std::vector<std::pair<float, int> >::const_iterator itr;
	//	for (int j=0; j< nfeat ;j++) {
	////		cout << " -- " << j <<   " -- " << endl;
	////		for(itr = f[j].begin(); itr != f[j].end(); ++itr){
	////			std::cout << "Value = " << (*itr).first << ", original index = " << (*itr).second << endl;
	////		}
	//		for(itr = f[j].begin(); itr != f[j].end(); ++itr){
	//			out  << (*itr).first << "\t" << (*itr).second+1  << endl;		}
	//		out << endl;
	//	}
	for (int i=0; i<this->getSize() ;i++) {
		out << f[0][i].first << "\t" << f[0][i].second << "\t" <<
				f[1][i].first << "\t" << f[1][i].second << "\t" <<
				f[2][i].first << "\t" << f[2][i].second << "\t" <<
				f[3][i].first << "\t" << f[3][i].second << "\t" <<
				f[4][i].first << "\t" << f[4][i].second << "\t" <<
				f[5][i].first << "\t" << f[5][i].second << "\t" <<
				f[6][i].first << "\t" << f[6][i].second << "\t" <<
				f[7][i].first << "\t" << f[7][i].second << "\t" <<
				f[8][i].first << "\t" << f[8][i].second << "\t" <<
//				f[9][i].first << "\t" << f[9][i].second << "\t" <<
		endl;
	}
	out.close();
}

void ACMediaLibrary::setPreProcessPlugin(ACPlugin* acpl)
{
	if (((ACPlugin*)mPreProcessPlugin!=acpl)&&(mPreProcessInfo!=NULL))
		mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
	if (acpl==NULL&&mPreProcessPlugin!=NULL)
	{		
		mPreProcessPlugin=NULL;
		std::vector<ACMedia*>::iterator iter;
		for (iter=media_library.begin();iter!=media_library.end();iter++)
			(*iter)->defaultPreProcFeatureInit();		
	}
	if (acpl!=NULL)
		if (acpl->implementsPluginType(PLUGIN_TYPE_PREPROCESS))
			mPreProcessPlugin=dynamic_cast<ACPreProcessPlugin*> (acpl) ;
}
void ACMediaLibrary::setMediaReaderPlugin(ACPlugin* acpl){	
	if (acpl==NULL&&mReaderPlugin!=NULL)
	{		
		mReaderPlugin=NULL;
		
	}
	if (acpl!=NULL)
		if (acpl->implementsPluginType(PLUGIN_TYPE_MEDIAREADER))
			mReaderPlugin=dynamic_cast<ACMediaReaderPlugin*> (acpl) ;	
}

ACMediaType ACMediaLibrary::getActiveSubMediaType(){
	if (media_type!=MEDIA_TYPE_MIXED||media_library.size()==0)
		return media_type;
	else {
		return media_library[0]->getActiveSubMediaType();
	}

}

#ifdef SUPPORT_MULTIMEDIA
int ACMediaLibrary::setActiveMediaType(std::string mediaName){
	if (media_type!=MEDIA_TYPE_MIXED)
		return 0;
	this->cleanStats();
	std::vector<ACMedia*>::iterator iter;
	for (iter = media_library.begin(); iter != media_library.end(); iter++) {
		if ((*iter)->getMediaType()==MEDIA_TYPE_MIXED){
			ACMediaDocument *currMedia=static_cast<ACMediaDocument *> (*iter);
			if (currMedia!=0){
				currMedia->setActiveSubMedia(mediaName);
			}	
		}
		else {
			continue;
		}
	}	
}
#endif//def SUPPORT_MULTIMEDIA

// -------------------------------------------------------------------------
// test
//#if defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
//int ACMediaLibrary::testFFMPEG(std::string _filename){
//	//CF early check in video files for audio and video streams, towards ACMediaDocuments
//	// from http://www.inb.uni-luebeck.de/~boehme/using_libavcodec.html
//	// and http://www.inb.uni-luebeck.de/~boehme/libavcodec_update.html
//
//	string extension = fs::extension(_filename);
//	ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(extension);
//
//	if (media_type == MEDIA_TYPE_VIDEO || media_type == MEDIA_TYPE_AUDIO) {
//		if (fileMediaType == MEDIA_TYPE_VIDEO) {
//			AVFormatContext *pFormatCtx;
//			int             i, videoStreams, audioStreams;
//			/*
//			AVCodecContext  *pCodecCtx;
//			AVCodec         *pCodec;
//			AVFrame         *pFrame;
//			AVFrame         *pFrameRGB;
//			AVPacket        packet;
//			int             frameFinished;
//			int             numBytes;
//			uint8_t         *buffer;
//			*/
//			// Open video file
//			if(av_open_input_file(&pFormatCtx, _filename.c_str(), 0, 0, 0)!=0){
//				std::cout << "Couldn't open file" << std::endl;
//				return 0;
//			}
//
//			// Retrieve stream information
//			if(av_find_stream_info(pFormatCtx)<0){
//				std::cout << "Couldn't find stream information" << std::endl;
//				return 0;
//			}
//
//			// Dump information about file onto standard error
//			dump_format(pFormatCtx, 0, _filename.c_str(), false);
//
//			// Count video streams
//			videoStreams=0;
//			for(i=0; i<pFormatCtx->nb_streams; i++)
//				if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
//					videoStreams++;
//			if(videoStreams == 0)
//				std::cout << "Didn't find any video stream." << std::endl;
//			else
//				std::cout << "Found " << videoStreams << " video stream(s)." << std::endl;
//
//			// Count audio streams
//			audioStreams=0;
//			for(i=0; i<pFormatCtx->nb_streams; i++)
//				if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO)
//					audioStreams++;
//			if(audioStreams == 0)
//				std::cout << "Didn't find any audio stream" << std::endl;
//			else
//				std::cout << "Found " << audioStreams << " audio stream(s)." << std::endl;
//		}
//	}
//}
//#endif //defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
