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

using namespace std;
// to save *acl library items in binary format, uncomment the following line:
// #define SAVE_ACL_BINARY

ACMediaLibrary::ACMediaLibrary() {
    this->cleanLibrary();
    media_type = MEDIA_TYPE_NONE;
    mPreProcessPlugin=0;
    mPreProcessInfo=0;
    mReaderPlugin=0;
    mLibraryReaderPlugin=0;
    mLibraryWriterPlugin=0;
    files_processed = 0;
    files_to_import = 0;
    media_at_import = 0;
    metadata = ACMediaLibraryMetadata();
    curator = ACUserProfile();
    segmenting = false;
}

ACMediaLibrary::ACMediaLibrary(ACMediaType aMediaType) {
    media_type = aMediaType;
    mPreProcessPlugin=0;
    mPreProcessInfo=0;
    mReaderPlugin=0;
    mLibraryReaderPlugin=0;
    mLibraryWriterPlugin=0;
    this->cleanLibrary();
    files_processed = 0;
    files_to_import = 0;
    media_at_import = 0;
    metadata = ACMediaLibraryMetadata();
    curator = ACUserProfile();
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
    for (ACMedias::iterator iter = media_library.begin(); iter != media_library.end(); iter++) {
        delete iter->second;
    }
    media_library.clear();
}

// this is the method to be called when re-initializing
void ACMediaLibrary::cleanLibrary() {
    cleanStats();
    deleteAllMedia();
    index_last_normalized = -1;
    synthesisID = -1;
    mediaID = -1; // index of the last one to be inserted
    media_path ="";
    // XS leave the media type as is, suppose we'll keep loading the same type of media
    // (not this:) media_type=MEDIA_TYPE_NONE;
    metadata = ACMediaLibraryMetadata();
    failed_imports.clear();

    if (mPreProcessPlugin!=0){
        if (mPreProcessInfo!=0)
            mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
        mPreProcessInfo=0;
    }
    total_ext_check_time = 0;
    checked_files = 0;
    files_processed = 0;
    files_to_import = 0;
#ifdef SUPPORT_MULTIMEDIA
    submediakey = "";
#endif
}

std::vector<std::string> ACMediaLibrary::getExtensionsFromMediaType(ACMediaType media_type)
{
    /*if (mReaderPlugin!=NULL){
        if (mReaderPlugin->mediaTypeSuitable(media_type)){
            return mReaderPlugin->getSupportedExtensions();
        }
    }*/
    return ACMediaFactory::getInstance().getExtensionsFromMediaType(media_type);
}


// this same function is used to import a whole directory, a single file or a list of files.
// it uses scanDirectory to construct a vector (filenames) containing the files to be analyzed.
// return values:
//	-1 if error
//  0 if empty directory (or no media of the required type found)
//	> 0 if no error (returns the number of files found)
int ACMediaLibrary::importDirectory(std::string _path, int _recursive, ACPluginManager *acpl, bool forward_order, bool doSegment, bool _save_timed_feat){ //, TiXmlElement* _medias) {
    this->segmenting = doSegment;
    std::vector<string> filenames;
    total_ext_check_time = 0;
    checked_files = 0;
    int nf = scanDirectory(_path, _recursive, filenames);
    std::cout << "ACMediaLibrary::scanDirectory: took " << total_ext_check_time << "s to check file extensions for " << checked_files << " files of which " << filenames.size() << " are of chosen media type." << std::endl;

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

    files_to_import += filenames.size();
    std::vector<int> ret;
    for (unsigned int i=0; i<filenames.size(); i++){
        //CF files_processed++; // done in importFile
        int index = forward_order ? i : filenames.size()-1-i;//CF if reverse order (not forward_order), segments in subdirs are added to the library after the source recording
        std::vector<int> media_ids = this->importFile(filenames[index], acpl, doSegment, _save_timed_feat); //, _medias );
        for (std::vector<int>::iterator it=media_ids.begin();it!=media_ids.end();it++){
            ret.push_back(*it);
        }
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


int ACMediaLibrary::importFiles(std::vector<std::string> filenames, ACPluginManager *acpl, bool doSegment, bool _save_timed_feat){ //, TiXmlElement* _medias) {
    this->segmenting = doSegment;
    std::vector<int> ret;
    total_ext_check_time = 0;
    checked_files = 0;

    files_to_import += filenames.size();
    for (unsigned int i=0; i<filenames.size(); i++){
        std::vector<int> media_ids = this->importFile(filenames[i], acpl, doSegment, _save_timed_feat); //, _medias );
        for (std::vector<int>::iterator it=media_ids.begin();it!=media_ids.end();it++){
            ret.push_back(*it);
        }
    }
    return this->getSize();
}

// import single file :
// computes features (= "import" media) using FEATURES plugins available in the plugin Manager
// doSegment = true : uses SEGMENTATION plugins on-the-fly
// save_timedfeat = true : save the timedFeatures on the disk
// returns the media id of the imported file
std::vector<int> ACMediaLibrary::importFile(std::string _filename, ACPluginManager *acpl, bool doSegment, bool _save_timed_feat){ //, TiXmlElement* _medias) {
    this->segmenting = doSegment;

    std::vector<int> ret;

    // check if file has already been imported.
    // XS TODO: isn't this going to be too heavy when library size gets large ?
    // may be add a flag to (in)activate the test ?
    int is_present = this->getMediaIndex(_filename);
    if (is_present >= 0) {
        cout << "<ACMediaLibrary::importFile> skipping "<< _filename << " : media already present at position " << is_present << endl;
        // Even if the file can't be imported, we have to notify the progress the file has been processed
        files_processed++;
        failed_imports.push_back(_filename);
        return ret;
    }

    string extension = fs::extension(_filename);

    ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(extension);
    ACMedia* media=0;
#if defined (SUPPORT_MULTIMEDIA) 
    if (media_type==MEDIA_TYPE_MIXED){
        if (mReaderPlugin!=0){
            media=mReaderPlugin->mediaFactory(media_type);
            if(media){
                ACMediaData* _media_data = mReaderPlugin->mediaReader(media_type);
                if(!_media_data){
                    std::cerr << "ACMediaFactory::create: plugin "<< mReaderPlugin->getName() << " didn't set a media data" << std::endl;
                    // Even if the file can't be imported, we have to notify the progress the file has been processed
                    files_processed++;
                    failed_imports.push_back(_filename);
                    return ret;
                }
                media->setMediaData(_media_data);
            }
        }
        else
            std::cerr << "<ACMediaLibrary::importFile> no media document reader plugin set, trying with the media factory"<< std::endl;
        if (media==0)
            media = ACMediaFactory::getInstance().create(extension,MEDIA_TYPE_MIXED);
        /*if (media==0)
            media=new ACMediaDocument();*/
        if (media==0){
            std::cerr << "<ACMediaLibrary::importFile> couldn't create the required media document with the media factory, skipping "<< std::endl;
            // Even if the file can't be imported, we have to notify the progress the file has been processed
            files_processed++;
            failed_imports.push_back(_filename);
            return ret;
        }
        else {
            media_at_import = media;
            int nbMedia = media->import(_filename, this->getAvailableMediaID(), acpl);
            if (nbMedia == 0){
                // Even if the file can't be imported, we have to notify the progress the file has been processed
                files_processed++;
                failed_imports.push_back(_filename);
                return ret;
            }
            this->addMedia(media);
            ret.push_back(media->getId());
            ACMediaContainer medias = (static_cast<ACMediaDocument*> (media))->getContainer();
            ACMediaContainer::iterator iter;
            for ( iter=medias.begin() ; iter!=medias.end(); ++iter ){
                this->addMedia(iter->second);
                iter->second->setParentId(media->getId());
                ret.push_back(iter->second->getId());
            }
            this->setActiveMediaType(((ACMediaDocument*)media)->getActiveMediaKey(), acpl);
            files_processed++;
            //return ret;
        }
    }
    else{
#endif	
        // XS TODO: do we want to check the media type of the whole library (= impose a unique one)?

        if (media_type == fileMediaType){
            media = ACMediaFactory::getInstance().create(extension);
            //cout << "extension:" << extension << endl;
        }
        else {
            cout << "<ACMediaLibrary::importFile> other media type, skipping " << _filename << " ... " << endl;
            cout << "-> media_type " << media_type << " ... " << endl;
            cout << "-> fileMediaType " << fileMediaType << " ... " << endl;
            // Even if the file can't be imported, we have to notify the progress the file has been processed
            files_processed++;
            failed_imports.push_back(_filename);
            return ret;
        }

        if (media == 0) {
            cout << "<ACMediaLibrary::importFile> extension unknown, skipping " << _filename << " ... " << endl;
            // Even if the file can't be imported, we have to notify the progress the file has been processed
            files_processed++;
            failed_imports.push_back(_filename);
            return ret;
        }
        // import has to be done before segmentation to have proper id.
        media_at_import = media;
        if (media->import(_filename, this->getAvailableMediaID(), acpl, _save_timed_feat)){
            this->addMedia(media);
            ret.push_back(media->getId());
#ifdef VERBOSE
            cout << "imported " << _filename << " with mid = " <<  media->getId() << endl;
#endif // VERBOSE

#if defined (SUPPORT_MULTIMEDIA)
        }
    }
#endif

        if (doSegment){
            // segments are created without id
            media->segment(acpl, _save_timed_feat);

            //			if(this->media_type == MEDIA_TYPE_VIDEO)//CF
            //				std::cout << "\n\nWARNING! Video segments not added to the library to test segmentation until features are not re-calculated from each segment!\n\n" << std::endl;
            //			else{
            std::vector<ACMedia*> mediaSegments;
            mediaSegments = media->getAllSegments();
            std::cout << "ACMediaLibrary::importFile found " << mediaSegments.size() << " segments." << std::endl;
            std::vector<ACMedia*> importedSegment;
            files_to_import += mediaSegments.size();
            for (unsigned int i = 0; i < mediaSegments.size(); i++){
                // for the segments we do not save (again) timedFeatures
                // XS TODO but we should not re-calculate them either !=> TR I put the mtf files names of the media parent in all his segments
                media_at_import = mediaSegments[i];
                std::string _segmentfilename = mediaSegments[i]->getFileName();
                if(_segmentfilename == "")
                    _segmentfilename = _filename;
                if (mediaSegments[i]->import(_segmentfilename, this->getAvailableMediaID(), acpl)){
                    this->addMedia(mediaSegments[i]);
                    mediaSegments[i]->setParentId(media->getId());
                    importedSegment.push_back(mediaSegments[i]);
                    ret.push_back(mediaSegments[i]->getId());
                    mediaSegments[i]->deleteData();//TR TODO verify that we must delete this data
                }
                else{//impossible to import the segment
                    mediaSegments[i]->deleteData();//TR TODO verify that we must delete this data
                    media_at_import = 0;
                    delete mediaSegments[i];
                }
                files_processed++; // each segment is processed
                media->setAllSegments(importedSegment);
            }
        }
        files_processed++; // the media is processed
#if ! defined (SUPPORT_MULTIMEDIA)
    }
#endif
    /*else {
        media_at_import = 0;
        cerr << "<ACMediaLibrary::importFile> problem importing file : " << _filename << " ... " << endl;
        failed_imports.push_back(_filename);
        // Even if the file can't be imported, we have to notify the progress the file has been processed
        files_processed++;
        return ret;

    }*/
    // XS TODO this was an attempt to save on-the-fly each media
    // but it does not work well.

    // appending current media (if imported properly) to the project's XML file
    //media->saveXML(_medias);

    // deleting the data that have been used for analysis, keep media small.
    if(media->getMediaData())
        media->getMediaData()->closeFile();

    if(files_processed == files_to_import){
        files_processed = files_to_import = 0;
        media_at_import = 0;
    }

    return ret;
}

float ACMediaLibrary::getImportProgress(){
    float progress = 1.0f;
    if(files_to_import != 0){
        progress = (float)(files_processed)/(float)(files_to_import);
        if(media_at_import){
            progress += media_at_import->getImportProgress(this->segmenting)/(float)(files_to_import);
        }
    }
    return progress;
}

int ACMediaLibrary::scanDirectories(std::vector<string> _paths, int _recursive, std::vector<string>& filenames) {
    int cnt = 0;
    total_ext_check_time = 0;
    checked_files = 0;
    for (unsigned int i=0; i<_paths.size(); i++) {
        int c = scanDirectory(_paths[i], _recursive, filenames) ; // could be -1 if error
        if (c > 0) cnt += c;
    }
    std::cout << "ACMediaLibrary::scanDirectory: took " << total_ext_check_time << "s to check file extensions for " << checked_files << " files of which " << filenames.size() << " are of chosen media type." << std::endl;
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
        string extension = fs::extension(_path);
        double single_pre_check_time = getTime();
        checked_files++;
        if( (this->media_type == MEDIA_TYPE_MIXED && extension == ".xml") || this->media_type == ACMediaFactory::getInstance().getMediaTypeFromExtension(extension)){
            total_ext_check_time += getTime() - single_pre_check_time;
            filenames.push_back(_path);
        }
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
                this->addMedia(local_media);
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
                this->addMedia(local_media);
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
        TiXmlElement* media_element = this->openCoreXMLLibrary(rootHandle);
        while(  media_element != 0 ){
            media_element = this->openNextMediaFromXMLLibrary(media_element);
        }

    }
    catch (const exception& e) {
        cout << e.what( ) << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

TiXmlElement* ACMediaLibrary::openCoreXMLLibrary(TiXmlHandle _rootHandle){

    // Distance Types are read from the XML if they exist, so that they can then be used to set the distance type while loading the features in openNextMediaFromXMLLibrary
    mDistanceTypes.clear();
    mFeatureNames.clear();
    TiXmlText* nLibraryDistanceTypes=_rootHandle.FirstChild( "DistanceTypes" ).FirstChild().Text();
    TiXmlElement* nLibraryDistanceTypesNode=_rootHandle.FirstChild( "DistanceTypes" ).Element();
    if(nLibraryDistanceTypesNode){
        if(nLibraryDistanceTypesNode->Attribute("NumberOfFeatures")) {
            string Nfeat = nLibraryDistanceTypesNode->Attribute("NumberOfFeatures");
            stringstream tmp2;
            tmp2 << Nfeat;
                int nFeatures;
                tmp2 >> nFeatures;
                string distance_types = nLibraryDistanceTypes->ValueStr();
                size_t pos;
                int dis;
                while ((pos = distance_types.find(' ')) != std::string::npos) {
                    std::stringstream ss(distance_types.substr(0, pos));
                    ss >> dis;
                    mDistanceTypes.push_back(dis);
                    distance_types.erase(0, pos + 1);
                }
            std::stringstream ss(distance_types.substr(0, pos));
            ss >> dis;
            mDistanceTypes.push_back(dis);
            
            if(mDistanceTypes.size()-nFeatures)
            {
                std::cout << "Problem in reading distance types. Expected: " << nFeatures << " Actually read: " << mDistanceTypes.size() << std::endl;
            }
        }
    }
    
    // Library Metadata
    TiXmlText* nLibraryMetadata=_rootHandle.FirstChild( "LibraryMetadata" ).FirstChild().Text();
    TiXmlElement* nLibraryMetadataNode=_rootHandle.FirstChild( "LibraryMetadata" ).Element();
    if(nLibraryMetadataNode){
        if(nLibraryMetadataNode->Attribute("Title"))
            this->metadata.title = nLibraryMetadataNode->Attribute("Title");
        if(nLibraryMetadataNode->Attribute("Author"))
            this->metadata.author = nLibraryMetadataNode->Attribute("Author");
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
                this->curator.name = nLibraryCuratorNode->Attribute("Name");
            if(nLibraryCuratorNode->Attribute("Email"))
                this->curator.email = nLibraryCuratorNode->Attribute("Email");
            if(nLibraryCuratorNode->Attribute("Website"))
                this->curator.website = nLibraryCuratorNode->Attribute("Website");
            if(nLibraryCuratorNode->Attribute("Location"))
                this->curator.location = nLibraryCuratorNode->Attribute("Location");
            if(nLibraryCuratorNode->Attribute("Picture"))
                this->curator.picture = nLibraryCuratorNode->Attribute("Picture");
        }
    }

    TiXmlText* nMediaText=_rootHandle.FirstChild( "NumberOfMedia" ).FirstChild().Text();
    std::stringstream tmp;
    int n_medias=0;

    if (!nMediaText)
        throw runtime_error("corrupted XML file, no number of media");
    else {
        string nof = nMediaText->ValueStr();
        tmp << nof;
        tmp >> n_medias;
    }
    files_to_import = n_medias;
    TiXmlElement* pMediaNode=_rootHandle.FirstChild( "Medias" ).FirstChild().Element();
#ifdef SUPPORT_MULTIMEDIA
    if (!pMediaNode)
        pMediaNode=_rootHandle.FirstChild( "MediaDocuments" ).FirstChild().Element();
#endif
    return pMediaNode;
}

// has to be called right after ACMediaLibrary::openCoreXMLLibrary using its return TiXmlElement* is input
TiXmlElement* ACMediaLibrary::openNextMediaFromXMLLibrary(TiXmlElement* pMediaNode, bool with_thumbnails){
    /*if (!pMediaNode)
        throw runtime_error("corrupted XML file, no medias");
    else {
        // loop over all medias
        for( pMediaNode; pMediaNode; pMediaNode=pMediaNode->NextSiblingElement()) {*/
    ACMediaType typ;
    int typi = -1;
    pMediaNode->QueryIntAttribute("MediaType", &typi);
    files_processed++;
    if (typi < 0)
        throw runtime_error("corrupted XML file, wrong media type");
    else {
        typ = (ACMediaType) typi;
        ACMedia* local_media = ACMediaFactory::getInstance().create(typ);
        if(!local_media){
            throw runtime_error("Couldn't create the media, no media reader available");
        }
        local_media->loadXML(pMediaNode,mDistanceTypes,&mFeatureNames,with_thumbnails);
        if (mPreProcessPlugin==NULL)
            local_media->defaultPreProcFeatureInit();
        this->addMedia(local_media);
        if (local_media->getNumberOfSegments()>0){
            std::vector<ACMedia*> seg=local_media->getAllSegments();
            for (std::vector<ACMedia*>::iterator it=seg.begin();it!=seg.end();it++){
                this->addMedia((*it));
                (*it)->setParentId(local_media->getId());

            }
        }

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
                    if (mPreProcessPlugin==NULL)
                        local_submedia->defaultPreProcFeatureInit();
                    this->addMedia(local_submedia);
                    local_submedia->setParentId(local_media->getId());
                    local_submedia->loadXML(mediaElement,with_thumbnails);

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

    }
    pMediaNode=pMediaNode->NextSiblingElement();

    /*}
        // consistency check (XS TODO quits the app -> exceptions )
        //if (cnt != n_medias){
        //    cerr << "<ACMediaLibrary::openXMLLibrary>: inconsistent number of media"<< endl;
        //    cerr << "n_medias = " << n_medias << "; cnt = " << cnt << endl;
        //    return -1;
        //}
    /*}
    return n_medias;*/
    // if(files_processed == files_to_import){
    //     files_processed = files_to_import = 0;
    // }
    return pMediaNode;
}

// C++ version
// XS TODO return value
int ACMediaLibrary::saveACLLibrary(std::string _path){
#ifdef SAVE_ACL_BINARY
    ofstream library_file (_path.c_str(), ios::binary);
#else
    ofstream library_file (_path.c_str());
#endif //SAVE_ACL_BINARY
    cout << "saving ACL file: " << _path << endl;

    // we save UNnormalized features
    //denormalizeFeatures();
    for(ACMedias::iterator media = media_library.begin(); media != media_library.end();media++){
        media->second->saveACL(library_file);
    }
    library_file.close();
    //normalizeFeatures();
}

void ACMediaLibrary::getDistanceTypes()
{
    mDistanceTypes.clear();
    if(media_library.size()>0){
        ACMedia *firstMedia=this->getFirstMedia();
        int n_features = firstMedia->getNumberOfFeaturesVectors();//features_vec.size();
        if(n_features){
            std::vector<ACMediaFeatures*> features_vec=firstMedia->getAllFeaturesVectors();
            
            for (int i=0; i<n_features; i++) {
                mDistanceTypes.push_back(features_vec[i]->getDistanceType());
                //cout << "Feature : " << features_vec[i]->getName() << " Distance: " << features_vec[i]->getDistanceType() << endl;
            }
        }
    }
    
}

int ACMediaLibrary::getFeatureIndex(std::string FeatureName)
{
    int i;
    for(i=0;i<mFeatureNames.size();i++)
    {
        if(!mFeatureNames[i].compare(FeatureName))
        {
            return i;
        }
    }
    return -1;
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

    int result = this->saveCoreXMLLibrary(MC_e_root, MC_e_medias);

    // normalize features again
    //normalizeFeatures();
    return MC_doc.SaveFile( _path.c_str());
}

// no headers, just media information
// can be called from MediaCycle's saveXML or from the app via mediaCycle
// JU added storage of distance types
int ACMediaLibrary::saveCoreXMLLibrary( TiXmlElement* _MC_e_root, TiXmlElement* _MC_e_medias){
    // JU: add FeaturesWeights AND DistanceTypes;
         // a) impossible as it would require to pass a _mediaBrowser as argument.
        // b) Distance Types
        TiXmlElement* MC_e_distance_types = new TiXmlElement("DistanceTypes");
        _MC_e_root->LinkEndChild( MC_e_distance_types );
        getDistanceTypes();
        MC_e_distance_types->SetAttribute("NumberOfFeatures", mDistanceTypes.size());

        // concatenate feature weights separated by a " "
        std::string sfw2;
        std::stringstream tmp2;
        for (unsigned int j=0; j<mDistanceTypes.size(); j++) {
            tmp2 << mDistanceTypes[j]<< " " ;
        }
        sfw2 = tmp2.str();
        TiXmlText* MC_t_distance_types = new TiXmlText(sfw2.c_str());
        MC_e_distance_types->LinkEndChild( MC_t_distance_types );
    // library metadata
    TiXmlElement* MC_e_library_metadata = new TiXmlElement("LibraryMetadata");
    _MC_e_root->LinkEndChild(  MC_e_library_metadata );
    MC_e_library_metadata->SetAttribute("Title", this->metadata.title);
    MC_e_library_metadata->SetAttribute("Author", this->metadata.author);
    MC_e_library_metadata->SetAttribute("Year", this->metadata.year);
    MC_e_library_metadata->SetAttribute("Publisher", this->metadata.publisher);
    MC_e_library_metadata->SetAttribute("License", this->metadata.license);
    MC_e_library_metadata->SetAttribute("Website", this->metadata.website);
    MC_e_library_metadata->SetAttribute("Cover", this->metadata.cover);

    TiXmlElement* MC_e_library_curator = new TiXmlElement("Curator");
    MC_e_library_metadata->LinkEndChild(  MC_e_library_curator );
    MC_e_library_curator->SetAttribute("Name", this->curator.name);
    MC_e_library_curator->SetAttribute("Email", this->curator.email);
    MC_e_library_curator->SetAttribute("Website", this->curator.website);
    MC_e_library_curator->SetAttribute("Location", this->curator.location);
    MC_e_library_curator->SetAttribute("Picture", this->curator.picture);

    // "medias and features"
    TiXmlElement* MC_e_number_of_medias = new TiXmlElement( "NumberOfMedia" );
    _MC_e_root->LinkEndChild( MC_e_number_of_medias );
    int n_medias = this->getSize();
    std::string s_medias;
    std::stringstream tmp;
    for(ACMedias::iterator m = media_library.begin(); m != media_library.end();m++)
        if(m->second->isDiscarded())
            n_medias--;
    tmp << n_medias;
    s_medias = tmp.str();
    int a=0;
    TiXmlText* MC_t_nm = new TiXmlText( s_medias );
    MC_e_number_of_medias->LinkEndChild( MC_t_nm );

    // XS TODO iterator
    std::vector<TiXmlElement*> medias;

    for(ACMedias::iterator m = media_library.begin(); m != media_library.end();m++){
        if((!m->second->isDiscarded())&&(m->second->getParentId()==-1)){ // don't save medias discarded by the user. Segments and submedias are saved with their parent
            if(this->media_type != MEDIA_TYPE_MIXED){
                TiXmlElement* media = new TiXmlElement( "Media" );
                _MC_e_medias->LinkEndChild( media );
                m->second->saveXML( media);
            }
            else{
                if(m->second->getMediaType() == MEDIA_TYPE_MIXED){
                    TiXmlElement* doc = new TiXmlElement( "MediaDocument" );
                    _MC_e_medias->LinkEndChild( doc );
                    m->second->saveXML( doc );

                    medias.push_back( new TiXmlElement( "Medias" ) );
                    doc->LinkEndChild( medias.back() );

                }
                else{
                    TiXmlElement* media = new TiXmlElement( "Media" );
                    medias.back()->LinkEndChild( media );
                    m->second->saveXML( media );
                }
            }
        }
    }
    return 1;
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
// XS TODO return value
int ACMediaLibrary::saveMCSLLibrary(std::string _path){
#ifdef SAVE_ACL_BINARY
    ofstream library_file (_path.c_str(), ios::binary);
#else
    ofstream library_file (_path.c_str());
#endif //SAVE_ACL_BINARY
    cout << "saving MCSL file: " << _path << endl;

    //CF Header
    library_file << "MediaCycle Segmented Library" << endl;
    library_file << 0.1 << endl; //CF v. 0.1 on 31/05/2010, adding parentid to ACL v0.1

    int n_medias = this->getSize();
    // we save UNnormalized features
    denormalizeFeatures();
    for(ACMedias::iterator m = media_library.begin(); m != media_library.end();m++){
        m->second->saveMCSL(library_file);
    }
    library_file.close();
    normalizeFeatures();
}


/// These must be try/catch'd, errors are thrown
void ACMediaLibrary::importLibrary(std::string _path, ACMediaLibraryReaderPlugin* _plugin){
    if(_path==""){
        throw runtime_error("Couldn't open library from an empty path");
    }
    if(!_plugin){
        throw runtime_error("Couldn't open library '" + _path + "', no media library reader plugin set");
    }
    if(_plugin->openLibrary(_path)==false){
        throw runtime_error("Couldn't open library file '" + _path + "' for writing");
    }
    std::vector<std::string> filenames;
    try{
        filenames = this->filenamesToOpen(_path,_plugin);
    }
    catch(std::exception& e){
        try{
            _plugin->closeLibrary(_path);
        }catch(...){}
        throw runtime_error(e.what());
    }
    for(std::vector<std::string>::iterator filename = filenames.begin(); filename != filenames.end(); filename++){
        try{
            this->importFile(*filename);
        }catch(...){
            try{
                _plugin->closeLibrary(_path);
            }catch(...){}
            throw runtime_error("Error when opening media " + *filename + " from library file " + _path);
        }
    }
    bool libraryClosed = false;
    try{
        libraryClosed = _plugin->closeLibrary(_path);
    }catch(...){}
    if(libraryClosed==false){
        throw runtime_error("Couldn't close library file '" + _path + "'");
    }
}

std::vector<std::string> ACMediaLibrary::filenamesToOpen(std::string _path, ACMediaLibraryReaderPlugin* _plugin){
    std::vector<std::string> filenames;
    if(_plugin){
        filenames = _plugin->filenamesToOpen();
    }
    else
        throw runtime_error("Library '" + _path + "' is empty.");
    return filenames;
}

void ACMediaLibrary::saveLibrary(std::string _path, ACMediaLibraryWriterPlugin* _plugin){
    if(_path==""){
        throw runtime_error("Couldn't save library with an empty path");
    }
    if(!_plugin){
        throw runtime_error("Couldn't save library '" + _path + "', no media library writer plugin set");
    }
    if(_plugin->openLibrary(_path)==false){
        throw runtime_error("Couldn't open library file '" + _path + "' for writing");
    }
    if(_plugin->saveLibraryMetadata()==false){
        throw runtime_error("Couldn't save library metadata for file '" + _path + "'");
    }
    for(ACMedias::iterator _media = media_library.begin(); _media != media_library.end(); _media++){
        if(_media->second != 0 && _media->first >=0 ){
            try{
                _plugin->saveMedia(_media->second);
            }catch(...){
                try{
                    _plugin->closeLibrary(_path);
                }catch(...){}
                throw runtime_error("Error when saving media " + _media->second->getFileName() + " in library file " + _path);
            }
        }
        //else{}
        // silencing ghost media
    }
    bool libraryClosed = false;
    try{
        libraryClosed = _plugin->closeLibrary(_path);
    }catch(...){}
    if(libraryClosed==false){
        throw runtime_error("Couldn't close library file '" + _path + "'");
    }
}

std::vector<long> ACMediaLibrary::getAllMediaIds() {
    std::vector<long> ids;
    for(ACMedias::iterator media = media_library.begin(); media != media_library.end(); media++) {
        ids.push_back(media->first);
    }
    return ids;
}

// Increment the media id of the newest media
void ACMediaLibrary::incrementMediaID(){
    //CF we assume media ids are incremental (but not necessarily incremented of 1)
    //CF so increment the highest id, which is the id of the last element of the map (auto-ordered by id)
    if(media_library.size()>0)
        mediaID = media_library.rbegin()->first;
    //CF otherwise mediaID is already initialized/clean to 0;
    mediaID++;
}

int ACMediaLibrary::addMedia(ACMedia *aMedia) {
    // XS TODO remove media_type check
    // mediacycle should be able to manage a mix of any media
    // instead of only medias of one type
    if (aMedia != 0){
        if (aMedia->getType() == this->media_type || this->media_type == MEDIA_TYPE_MIXED) {
            if (mPreProcessPlugin==NULL)
                aMedia->defaultPreProcFeatureInit();
            this->incrementMediaID();
            media_library[mediaID] = aMedia;
            aMedia->setId(mediaID);
            return mediaID;
        } else {
            return -1;
        }
    }
    else {
        return -1;
    }
}

ACMedia* ACMediaLibrary::getMedia(int i){
    if (media_library.find(i) != media_library.end()){
        return media_library[i];
    }
    else {
        cerr << "<ACMediaLibrary::getMedia> index out of bounds: " << i << endl;
        return 0;
    }
}

int ACMediaLibrary::deleteMedia(int i){
    if (media_library.find(i) != media_library.end()){
        this->checkForLastNormalizedId(i);
        media_library.erase(i);
        return 1;
    }
    else {
        cerr << "<ACMediaLibrary::deleteMedia> index out of bounds: " << i << endl;
        return -1;
    }
}

void ACMediaLibrary::checkForLastNormalizedId(int i){
    // If we are deleting the last normalized media, find the penultimate normalized media index
    if(i == index_last_normalized){
        ACMedias::iterator last_normalized = media_library.find(i);
        ACMedias::reverse_iterator penultimate_normalized(last_normalized);
        penultimate_normalized++;
        if(penultimate_normalized != media_library.rend() )
            index_last_normalized = penultimate_normalized->first;
        else
            index_last_normalized = -1;
    }
}

void ACMediaLibrary::deleteAllSegments(){
    for(ACMedias::iterator media = media_library.begin(); media!=media_library.end();media++){
        std::vector<ACMedia*> segments = media->second->getAllSegments();
        for(std::vector<ACMedia*>::iterator segment = segments.begin(); segment!=segments.end();segment++){
            this->checkForLastNormalizedId((*segment)->getId());
            media_library.erase((*segment)->getId());
            delete (*segment);
            (*segment) = 0;
        }
        media->second->deleteAllSegments();
    }
}

int ACMediaLibrary::getMediaIndex(std::string media_file_name){
    for(ACMedias::iterator media = media_library.begin(); media!=media_library.end();media++){
        if (media_file_name==media->second->getFileName()) {
            return media->first;
        }
    }
    return -1;

}

std::string ACMediaLibrary::getThumbnailFileName(int id) {
    std::string filename("");
    ACMedias::iterator media = media_library.find(id);
    if(media!=media_library.end())
        filename = media->second->getThumbnailFileName();
    return filename;
}

std::vector<int> ACMediaLibrary::getParentIds(void){
    std::vector<int> ret;
    for(ACMedias::iterator media = media_library.begin(); media!=media_library.end();media++){
        if (media->second->getParentId()==-1 ) {
            ret.push_back(media->first);
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
    int number_of_features = this->getFirstMedia()->getNumberOfFeaturesVectors();
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
    int number_of_features = this->getFirstMedia()->getNumberOfFeaturesVectors();
    int mt = 0; // number of media files of the library that have the same media type as the library (might be different in case of multimedia documents)

    // initialize to zero
    int i,j,k;
    for (i=0; i< number_of_features; i++) {
        if (media_type != MEDIA_TYPE_MIXED || (media_type == MEDIA_TYPE_MIXED && this->getFirstMedia()->getMediaType() == media_type)){ //CF
            vector<double> tmp_vect;
            for (j=0; j< this->getFirstMedia()->getFeaturesVector(i)->getSize(); j++) {
                tmp_vect.push_back(0.0);
            }
            mean_features.push_back(tmp_vect);
            stdev_features.push_back(tmp_vect);
            mt++; //CF
        }
    }

    // computing sums
    for(ACMedias::iterator media = media_library.begin(); media!=media_library.end();media++){
        ACMedia* item = media->second;
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

    // int start = 0;

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

    //vector<int> currId=this->getParentIds();
    //TR normalize just Parent Nodes
    //unsigned int n = currId.size();

    //unsigned int n = this->getSize() ;

    ACMedias::iterator start;
    
    if (needsNormalize) {
        start = media_library.begin();
    }
    else {
        // Starting with the next index to be normalized
        //vector<int>::iterator last_normalized = std::find(currId.begin(),currId.end(),index_last_normalized);
        //if(last_normalized != currId.end()){
        //    start = std::distance(currId.begin(),last_normalized)+1;
        //}
        start = media_library.find(index_last_normalized);
        
    }
    if (mPreProcessPlugin!=NULL){
        for(ACMedias::iterator it=start; it!=media_library.end(); it++){
            ACMedia* item = it->second;
            //if (item->getMediaType() == this->media_type){
                item->cleanPreProcFeaturesVector();
                std::vector<ACMediaFeatures*> tempFeatVect;
#ifdef SUPPORT_MULTIMEDIA
                if (item->getMediaType()==MEDIA_TYPE_MIXED)
                    tempFeatVect=mPreProcessPlugin->apply(mPreProcessInfo,((ACMediaDocument*)item)->getActiveMedia());
                else
#endif//def SUPPORT_MULTIMEDIA
                    tempFeatVect=mPreProcessPlugin->apply(mPreProcessInfo,item);
                for (int k=0;k<tempFeatVect.size();k++)
                    item->getAllPreProcFeaturesVectors().push_back(tempFeatVect[k]);
                tempFeatVect.clear();
            //}
        }
    }
    else {
        for(ACMedias::iterator it=start; it!=media_library.end(); it++){
            ACMedia* item = it->second;
            //if (item->getMediaType() == this->media_type){ //CF
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
            //}
        }
    }
    if (media_library.end()!=media_library.begin())
        index_last_normalized = (media_library.rbegin())->first;
    else
        index_last_normalized=0;
}

void ACMediaLibrary::denormalizeFeatures() {
    cout << "denormalizing features" << endl;
    if ( isEmpty() || index_last_normalized<0) return; // Ju: added index_last_normalized<0 to avoid segmentation fault under linux, I believe caused by the fact i is declared as unsigned int while index_last_normalized is int, so it messed up in the for loop

    unsigned int j,k;
    int i;

    // XS denormalize only those that have been normalized (duh),

    ACMedias::iterator last_normalized = media_library.find(index_last_normalized);

    for(ACMedias::iterator media = media_library.begin();media==last_normalized;media++){
        ACMedia* item = media->second;
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
        plugins_list = this->getFirstMedia()->getListOfPreProcFeaturesPlugins();
    return plugins_list;
}

// -------------------------------------------------------------------------
// XS custom for Thomas Israel videos : features sorted, along with filename
void ACMediaLibrary::saveSorted(string output_file){
    //XS dirty -- should be generalized !
    ofstream out(output_file.c_str());

    const int nfeat=10;
    std::vector<std::pair<float, int> > f[nfeat];

    for(ACMedias::iterator media = media_library.begin();media!=media_library.end();media++){
        out << media->first << " : " << media->second->getFileName() << endl;
        int nfeatv =  media->second->getNumberOfFeaturesVectors();
        if (nfeatv <= nfeat) {
            cerr << "problem with custom saveSorted : " << nfeatv << "<=" << nfeat<< endl;
        }
        for (int j=0; j< nfeat-1 ;j++) { // XS TODO: this is custom, to skip 0
            // XS TODO : this will only take first dimension of feature...
            //int nfeat = media_library[i]->getFeaturesVector(j)->getSize();
            out << media->second->getFeaturesVector(j+1)->getFeatureElement(0) << endl;
            f[j].push_back (std::pair<float, int> (media->second->getFeaturesVector(j+1)->getFeatureElement(0), media->first));
        }
    }

    for (int j=0; j< nfeat-1 ;j++) {
        sort (f[j].begin(),f[j].end());
    }
    //std::vector<std::pair<float, int> >::const_iterator itr;
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
    if (mPreProcessPlugin)
        if (mPreProcessPlugin!=dynamic_cast<ACPreProcessPlugin*>(acpl))
            if (mPreProcessInfo!=NULL)
                mPreProcessPlugin->freePreProcessInfo(mPreProcessInfo);
    if (acpl==NULL&&mPreProcessPlugin!=NULL)
    {
        mPreProcessPlugin=NULL;
        ACMedias::iterator iter;
        for (iter=media_library.begin();iter!=media_library.end();iter++)
            iter->second->defaultPreProcFeatureInit();
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
        return this->getFirstMedia()->getActiveSubMediaType();
    }

}

#ifdef SUPPORT_MULTIMEDIA

string ACMediaLibrary::getActiveSubMediaKey(){
    if (media_type!=MEDIA_TYPE_MIXED||media_library.size()==0)
        return string("");
    else {
        return submediakey;
        //return ((ACMediaDocument*)this->getFirstMedia())->getActiveMediaKey();
    }

}

int ACMediaLibrary::setActiveMediaType(std::string mediaName, ACPluginManager *acpl){
    if (media_type!=MEDIA_TYPE_MIXED)
        return 0;
    this->cleanStats();
    submediakey = mediaName;
    ACMedias::iterator iter;
    for (iter = media_library.begin(); iter != media_library.end(); iter++) {
        if (iter->second->getMediaType()==MEDIA_TYPE_MIXED){
            ACMediaDocument *currMedia=static_cast<ACMediaDocument *> (iter->second);
            if (currMedia!=0){
                currMedia->setActiveSubMedia(mediaName);
            }
        }
        else {
            continue;
        }
    }
    ACMediaType aMediaType=this->getActiveSubMediaType();
    ACPreProcessPlugin* preProcessPlugin=acpl->getPreProcessPlugin(aMediaType);
    if (preProcessPlugin&&preProcessPlugin->mediaTypeSuitable(aMediaType)) {
        this->setPreProcessPlugin(preProcessPlugin);
    }
    else
        this->setPreProcessPlugin(0);
    if (media_library.size()>1&&(static_cast<ACMediaDocument*> (this->getFirstMedia()))->getActiveMediaKey()==mediaName)
        return 1;
    return 0;
}

std::string ACMediaLibrary::getMediaDocumentIdentifier(){
    std::string identifier("");
    if (media_type!=MEDIA_TYPE_MIXED){
        std::cerr << "ACMediaLibrary::getMediaDocumentIdentifier: library not of media document type" << std::endl;
        return identifier;
    }
    if(this->getSize()==0)
    {
        std::cerr << "ACMediaLibrary::getMediaDocumentIdentifier: no media document in library" << std::endl;
        return identifier;
    }
    ACMedia* media(0);
    media = this->getFirstMedia();
    if(!media)
    {
        std::cerr << "ACMediaLibrary::getMediaDocumentIdentifier: coulnd't get first media" << std::endl;
        return identifier;
    }
    ACMediaDocument* doc(0);
    doc =static_cast<ACMediaDocument *> (media);
    if(!doc){
        std::cerr << "ACMediaLibrary::getMediaDocumentIdentifier: the first media of the library is not a media document"<< std::endl;
        return identifier;
    }
    identifier = doc->getIdentifier();
    return identifier;
}

bool ACMediaLibrary::containsMediaDocumentsOfIdentifier(std::string identifier){
    std::string _identifier = this->getMediaDocumentIdentifier();
    if(_identifier == ""){
        std::cerr << "ACMediaLibrary::containsMediaDocumentsOfIdentifier: library document identifier is empty " << std::endl;
        return false;
    }
    if(identifier == ""){
        std::cerr << "ACMediaLibrary::containsMediaDocumentsOfIdentifier: identifier to check is empty " << std::endl;
        return false;
    }
    boost::to_lower(identifier);
    boost::to_lower(_identifier);
    return (identifier == _identifier);
}
#endif//def SUPPORT_MULTIMEDIA

void ACMediaLibrary::setMediaTaggedClassId(int mediaId,int pId){
    cout<<"ACMediaLibrary::setMediaTaggedClassId mediaId:"<<mediaId<<" class:"<<pId<<endl;
    if (media_library.find(mediaId)==media_library.end())
        return;
    if (media_library[mediaId])
        media_library[mediaId]->setTaggedClassId(pId);
}

int ACMediaLibrary::getMediaTaggedClassId(int mediaId){
    if (media_library.find(mediaId)==media_library.end())
        return -1;
    if (media_library[mediaId])
        return media_library[mediaId]->getTaggedClassId();
    else
        return -1;
}
