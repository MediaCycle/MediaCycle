/*
 *  ACMedia.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 23/09/09
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

#include "ACMedia.h"

#include <iostream>
#include <algorithm>

#include "ACMediaFactory.h"

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;

ACMedia::ACMedia() { 
    mid = -1;
    parentid = -1;
    media_type = MEDIA_TYPE_NONE;
    width = 0;
    height = 0;
    filename="";
    filename_thumbnail="";
    label="";
    key="";
    text_tags="";
    hyper_links="";
    start = -1;
    end = -1;
    startInt = -1;
    endInt = -1;
    discarded = false;
    taggedClassId = -1;
}

ACMedia::ACMedia(const ACMedia& m){
    this->mid = m.mid;
    this->parentid = m.parentid;
    this->media_type = m.media_type;
    this->width = m.width;
    this->height = m.height;
    this->filename = m.filename;
    this->filename_thumbnail = m.filename_thumbnail;
    this->label = m.label;
    this->key = m.key;
    this->text_tags = m.text_tags;
    this->hyper_links = m.hyper_links;
    this->start = m.start;
    this->end = m.end;
    this->startInt = m.startInt;
    this->endInt = m.endInt;
    this->discarded = m.discarded;
    this->taggedClassId = m.taggedClassId;
}

ACMedia::~ACMedia() { 
	filename.clear();
	filename_thumbnail.clear();
    key.clear();
    for(std::vector<ACMediaThumbnail*>::iterator thumbnail = thumbnails.begin();thumbnail != thumbnails.end();thumbnail++)
        delete *thumbnail;
    thumbnails.clear();
	for (int i=0;i<preproc_features_vectors.size();i++)
	{
		if(preproc_features_vectors[i]) delete (preproc_features_vectors[i]);
		preproc_features_vectors[i]=0;
	}
	preproc_features_vectors.clear();
	for (int i=0;i<features_vectors.size();i++)
	{
		if(features_vectors[i]) delete (features_vectors[i]);
		features_vectors[i]=0;
	}
	features_vectors.clear();
/*	vector<ACMediaFeatures*> ::iterator iter;
	for (iter = features_vectors.begin(); iter != features_vectors.end(); iter++) {
		delete (*iter);//needed erase call destructor of pointer (i.e. none since it's just a pointer) not of pointee ACMediaFeatures
		//features_vectors.erase(iter); //will cause segfault. besides the vector is automatically emptied, no need to erase.
	}
	for (iter = preproc_features_vectors.begin(); iter != preproc_features_vectors.end(); iter++) {
		delete (*iter);//needed erase call destructor of pointer (i.e. none since it's just a pointer) not of pointee ACMediaFeatures
		//features_vectors.erase(iter); //will cause segfault. besides the vector is automatically emptied, no need to erase.
	}*/
	// XS TODO : why is this commented ?
	//if (data) delete data;
}

// C++ version
// writes in an existing (i.e. already opened) acl file
// works for binary too, the stream deals with it
void ACMedia::saveACL(ofstream &library_file, int mcsl) {
	
	int n_features;
	int n_features_elements;	
	int nn;
	
	if (! library_file.is_open()) {
		cerr << "<ACMedia::saveACL> : problem writing in ACL file, it needs to be opened before" << endl;
	}	
	
	library_file << filename << endl;
//	library_file << mid << endl;
	
	if (mcsl) {
		library_file << parentid << endl; 
		library_file << this->getAllSegments().size() << endl;
		for (unsigned int i=0; i < this->getAllSegments().size(); i++){
			library_file << this->getSegment(i)->getId() << "\t";
		}
		library_file << endl;
	}
	
	saveACLSpecific(library_file);
	
	n_features = features_vectors.size();
	library_file << n_features << endl;
	for (unsigned int i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize();
		nn = features_vectors[i]->getNeedsNormalization();
		library_file << features_vectors[i]->getName() << endl;
		library_file << nn << endl;
		library_file << n_features_elements << endl;
		for (int j=0; j<n_features_elements; j++) {
			library_file << features_vectors[i]->getFeatureElement(j)  << "\t"; // XS instead of [i][j]
		}
		library_file << endl;
	}	
}

void ACMedia::saveXML(TiXmlElement* media){
	//if ( features_saved_xml || _medias == NULL) return; 
    /*if (_medias == NULL) return;
    TiXmlElement* media;
    #ifdef SUPPORT_MULTIMEDIA
    if(this->media_type == MEDIA_TYPE_MIXED)
        media = new TiXmlElement( "MediaDocument" );
    else
    #endif
        media = new TiXmlElement( "Media" );
    _medias->LinkEndChild( media );*/

    if (media == NULL) return;

	media->SetAttribute("MediaID", mid);
	media->SetAttribute("MediaType", media_type);
	
	media->SetAttribute("FileName", filename);
	media->SetAttribute("Label", label);

    if(key!="")
        media->SetAttribute("Key", key);
	
	saveXMLSpecific(media);
	
	int n_features = features_vectors.size();
	TiXmlElement* features = new TiXmlElement( "Features" );  
	features->SetAttribute("NumberOfFeatures", n_features);
    media->LinkEndChild( features );

    TiXmlElement* _thumbnails = new TiXmlElement( "Thumbnails" );
    media->LinkEndChild( _thumbnails );
    _thumbnails->SetAttribute("NumberOfThumbnails", this->getNumberOfThumbnails());

    // saves info about thumbnails
    for(std::vector<ACMediaThumbnail*>::iterator thumbnail = thumbnails.begin();thumbnail != thumbnails.end();thumbnail++)
        (*thumbnail)->saveXML(_thumbnails);

	for (int i=0; i<n_features; i++) {
		int n_features_elements = features_vectors[i]->getSize();
		int nn = features_vectors[i]->getNeedsNormalization();
		TiXmlElement* mediaf = new TiXmlElement( "Feature" );  
		features->LinkEndChild( mediaf );  
		mediaf->SetAttribute("FeatureName", features_vectors[i]->getName());
		mediaf->SetAttribute("NeedsNormalization", nn);
		mediaf->SetAttribute("NumberOfFeatureElements",n_features_elements);

		// keep feature elements separated by a " "
		std::string s;
		std::stringstream tmp;
		for (int j=0; j<n_features_elements; j++) {
			tmp << features_vectors[i]->getFeatureElement(j) << " " ;
		}
		s = tmp.str();
		TiXmlText* mediafe = new TiXmlText(s.c_str());
		mediaf->LinkEndChild( mediafe );  		
	}
	if (taggedClassId!=-1){
		TiXmlElement* taggedClassIdField = new TiXmlElement( "TagId" );
		std::stringstream tmp;
        tmp<<taggedClassId;
		TiXmlText* tagText = new TiXmlText(tmp.str().c_str());
		taggedClassIdField->LinkEndChild(tagText);
        media->LinkEndChild( taggedClassIdField );
        
    }
	TiXmlElement* segments = new TiXmlElement( "Segments" );  
	media->LinkEndChild( segments );  
	segments->SetAttribute("NumberOfSegments", this->getNumberOfSegments());

	// saves info about segments (if any) : beginning, end, ID
	// the parent ID of the segment is the ID of the current media
	/*for (int i=0; i<this->getNumberOfSegments();i++) {
		TiXmlElement* seg = new TiXmlElement( "Segment" );  
		segments->LinkEndChild( seg );  
		//cout << " START " << this->getSegment(i)->getStart() << endl;
		//cout << " END " << this->getSegment(i)->getEnd() << endl;
		seg->SetDoubleAttribute("Start", this->getSegment(i)->getStart()); //CPL	
		seg->SetDoubleAttribute("End", this->getSegment(i)->getEnd()); //CPL
		std::string s;
		std::stringstream tmp;
		tmp << this->getSegment(i)->getId();
		s = tmp.str();
		TiXmlText* segID = new TiXmlText(s.c_str());
		seg->LinkEndChild( segID );  		
	}*/
    for (int i=0; i<this->getNumberOfSegments();i++) {
        ACMedia* locMedia=this->getSegment(i);
        if (locMedia==0)
            continue;
		//cout << " START " << this->getSegment(i)->getStart() << endl;
		//cout << " END " << this->getSegment(i)->getEnd() << endl;
        TiXmlElement* seg = new TiXmlElement( "Segment" );
		segments->LinkEndChild( seg );
        locMedia->saveXML( seg);
	}
	//features_saved_xml = true;
}

// C++ version
// loads from an existing (i.e. already opened) acl file
// returns 0 if error (trying to open empty file, failed making thumbnail, ...)
// returns 1 if fine
// return value is used in ACMediaLibrary::openACLLibrary
int ACMedia::loadACL(std::string media_path, ifstream &library_file, int mcsl) {
	
	int n_features = 0;
	int n_features_elements = 0;	
	int nn = 0;
	string tab = "";
	
	ACMediaFeatures* mediaFeatures = 0;
	string featureName  = "";
	float local_feature = 0;
	
	int nbSegments(0), segId(0);
	
	if (! library_file.is_open()) {
		cerr << "<ACMedia::loadACL> : problem loading image from ACL file, it needs to be opened before" << endl;
		return 0;
	}		
	if (!library_file.good()){
		cerr << "<ACMedia::loadACL> : bad library file" << endl;
		return 0;
	}
	
	getline(library_file, filename, '\n');
	if (!media_path.empty() && !filename.empty()) {
		filename = media_path + "/" + filename;
	}
	
	if (!filename.empty()) {
		
		library_file >> mid;	
		
		if (mcsl) {
			library_file >> parentid; 
			library_file >> nbSegments;
			for (int i=0; i < nbSegments; i++){
				library_file >> segId;
				cout << segId << "\t";
			} 
		}
		
		if (!loadACLSpecific(library_file)) {
			return 0;
		}		
		
		library_file >> n_features;	
		getline(library_file, tab);
		
		// XS TODO are all these [i], [j] safe ?
		// seems so but could write it differently
		for (int i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			//			getline(library_file, featureName, '\n');
			getline(library_file, featureName);
			features_vectors[i]->setName(featureName);
			library_file >> nn;
			features_vectors[i]->setNeedsNormalization(nn);
			library_file >> n_features_elements;
			features_vectors[i]->resize(n_features_elements);
			for (int j=0; j<n_features_elements; j++) {
				library_file >> local_feature;
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
			getline(library_file, tab);	
			//std::cout << "read extra chars : \n" << tab << std::endl;//CF
		}
		return 1;
	}
	else {
		return 0;
	}
}

void ACMedia::saveMCSL(ofstream &library_file) {
	saveACL(library_file, 1);
}

int ACMedia::loadMCSL(ifstream &library_file) {
	return loadACL("", library_file, 1);
}

// converts white spaces in string : " " -> "\ "
// so that the file name is read properly afterwards
void ACMedia::fixWhiteSpace (std::string &str) {
    std::string temp;
    for (unsigned int i = 0; i < str.length(); i++){
        if (str[i] == ' ') 
			temp.append("\\ ");
        else
			temp += str[i];
	}
    str = temp;
}

void ACMedia::loadXML(TiXmlElement* _pMediaNode){
//  const char *pName=_pMediaNode->Attribute("FileName");
//  XS: no need for char*; string should work since we defined TIXML_USE_STL
	if (!_pMediaNode)
		throw runtime_error("corrupted XML file");

	string pName ="";
	pName = _pMediaNode->Attribute("FileName");
	if (pName == "")
		throw runtime_error("corrupted XML file, no filename");
    else {
        // #ifdef __APPLE__ //added by CF, white spaces are needed under Ubuntu!
        // fixWhiteSpace(pName);
        // #endif
        fs::path p( pName.c_str());
        if ( !fs::exists( p ) )
        {
            throw runtime_error("corrupted XML file, can't locate file '" + pName + "'");
        }
        if ( !fs::is_regular( p ) )
        {
            throw runtime_error("corrupted XML file, file '" + pName + "' is corrupted");
        }
        this->setFileName(pName);
    }

    string pLabel("");
    if(_pMediaNode->Attribute("Label"))
        pLabel = _pMediaNode->Attribute("Label");
    if (pLabel != "")
        this->setLabel(pLabel);

	int mid=-1;
	_pMediaNode->QueryIntAttribute("MediaID", &mid); // If this fails, original value is left as-is
	if (mid < 0)
		throw runtime_error("corrupted XML file, wrong media ID");
	else
		this->setId(mid);
	
	// loadXMLSpecific should throw an exception if a problem occured 
	// but to make sure we'll throw one as well if return value is wrong
	if (!loadXMLSpecific(_pMediaNode)) {
		throw runtime_error("corrupted XML file, problem with loadXMLSpecific");
	}		
			
	TiXmlHandle _pMediaNodeHandle(_pMediaNode);

    // allow an XML without thumbnails
    TiXmlElement* thumbnailsElement = _pMediaNodeHandle.FirstChild( "Thumbnails" ).Element();
    if (thumbnailsElement) {
        int nt = -1;
        thumbnailsElement->QueryIntAttribute("NumberOfThumbnails", &nt);
        if (nt < 0)
            throw runtime_error("corrupted XML file, <Thumbnails> present, but no thumbnails");

        TiXmlElement* thumbnailElement = _pMediaNodeHandle.FirstChild( "Thumbnails" ).FirstChild( "Thumbnail" ).Element();
        TiXmlText* thumbnailIDElementsAsText = 0;
        int count_s = 0;

        for( thumbnailElement; thumbnailElement; thumbnailElement = thumbnailElement->NextSiblingElement() ) {
            ACMediaThumbnail* thumbnail = new ACMediaThumbnail();
            thumbnail->loadXML(thumbnailsElement);
        }
    }
	
	int count_f = 0;
	TiXmlElement* featuresElement = _pMediaNodeHandle.FirstChild( "Features" ).Element();
	if (!featuresElement)
		throw runtime_error("corrupted XML file, no features");	
	int nf=-1;
	featuresElement->QueryIntAttribute("NumberOfFeatures", &nf);
	
	if (nf < 0)
		throw runtime_error("corrupted XML file, wrong number of features");
	else if (nf ==0) // XS could happen without it being an error, for mediadocuments
		cout << "loading media with no features" << endl;
	
	TiXmlElement* featureElement = _pMediaNodeHandle.FirstChild( "Features" ).FirstChild( "Feature" ).Element();
    if (!featureElement && nf>0)
        throw runtime_error("corrupted XML file, error reading features");
    if(featureElement){
        TiXmlText* featureElementsAsText = 0;
        ACMediaFeatures* mediaFeatures;
        for(featureElement; featureElement; featureElement = featureElement->NextSiblingElement() ) {
            mediaFeatures = new ACMediaFeatures();
            int nfe=-1;
            int nno=-1;
            featureElement->QueryIntAttribute("NumberOfFeatureElements", &nfe);
            if (nfe < 0)
                throw runtime_error("corrupted XML file, wrong number of feature elements");
            featureElement->QueryIntAttribute("NeedsNormalization", &nno);
            if (nno < 0)
                throw runtime_error("corrupted XML file, wrong normalization flag");
            string featureName = "";
            featureName = featureElement->Attribute("FeatureName");
            if (featureName == "")
                throw runtime_error("corrupted XML file, empty feature name");
            if (!featureElement->FirstChild())
                throw runtime_error("corrupted XML file, empty feature value");
            featureElementsAsText=featureElement->FirstChild()->ToText();
            if (!featureElementsAsText)
                throw runtime_error("corrupted XML file, error reading feature elements");
            string fes = "";
            fes = featureElementsAsText->ValueStr();
            if (fes == "")
                throw runtime_error("corrupted XML file, error reading feature elements");
            std::stringstream fess;
            fess << fes;
            try {
                for (int j=0; j<nfe; j++) {
                    // XS TODO add test on nne
                    float f;
                    fess >> f;
                    mediaFeatures->addFeatureElement(f);
                }
            }
            catch (...) {
                // attempt to catch potential problems and group them
                throw runtime_error("corrupted XML file, error reading feature elements");
            }
            mediaFeatures->setComputed();
            mediaFeatures->setName(featureName);
            mediaFeatures->setNeedsNormalization(nno);
            features_vectors.push_back(mediaFeatures);
            count_f++;
        }
    }
	// consistency check for features
	if (count_f != nf)
		throw runtime_error("<ACMedia::loadXML> inconsistent number of features");

    
	TiXmlElement* tagElement = _pMediaNodeHandle.FirstChild( "TagId" ).Element();
    if (tagElement) {
        TiXmlText* tagElementsAsText=tagElement->FirstChild()->ToText();
        if (!tagElementsAsText)
            throw runtime_error("corrupted XML file, error reading tag elements");
        std::stringstream fess;
        fess<< tagElementsAsText->ValueStr();
        int temp =-1;
        fess>>temp;
        taggedClassId=temp;
    }
    else
        taggedClassId=-1;
    
	// --- segments --- 

	// allow an XML without segments.
	TiXmlElement* segmentsElement = _pMediaNodeHandle.FirstChild( "Segments" ).Element();
    if (segmentsElement) {
		int ns = -1;
		segmentsElement->QueryIntAttribute("NumberOfSegments", &ns);
		if (ns < 0)
			throw runtime_error("corrupted XML file, <segments> present, but no segments");
        if( ns == 0)
            return;

		TiXmlElement* segmentElement = _pMediaNodeHandle.FirstChild( "Segments" ).FirstChild( "Segment" ).Element();
		TiXmlText* segmentIDElementsAsText = 0;
		int count_s = 0;
		/*
		for( segmentElement; segmentElement; segmentElement = segmentElement->NextSiblingElement() ) {
			ACMedia* segment_media = ACMediaFactory::getInstance().create(this->getMediaType());
			double n_start=-1;
			double n_end=-1;
			segmentElement->QueryDoubleAttribute("Start", &n_start); //CPL: start & end are float!!
			if (n_start < 0) {
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment start");
			}
			segment_media->setStart(n_start);
			
			segmentElement->QueryDoubleAttribute("End", &n_end);// CPL
			if (n_end < 0){
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment end");
			}
			segment_media->setEnd(n_end);
			
			if (!segmentElement->FirstChild()){
				delete segment_media;
				throw runtime_error("corrupted XML file, no segment ID");
			}
			
			segmentIDElementsAsText=segmentElement->FirstChild()->ToText();
			if (!segmentIDElementsAsText){
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment ID");
			}
			string mid_s = "";
			mid_s = segmentIDElementsAsText->ValueStr();
			std::stringstream mid_ss;
			mid_ss << mid_s;
			int midi = -1;
			mid_ss >> midi;
			if (midi <0){
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment ID");
			}
			segment_media->setId(midi);
			segment_media->setParentId(mid);
			this->addSegment(segment_media);
			count_s++;
		}*/
		// consistency check for segments
        for( segmentElement; segmentElement; segmentElement = segmentElement->NextSiblingElement() ) {
            ACMedia* segment_media = ACMediaFactory::getInstance().create(this->getMediaType());
			segment_media->loadXML(segmentElement);
			segment_media->setParentId(mid);
			this->addSegment(segment_media);
			count_s++;
        }
		if (count_s != ns)
			throw runtime_error("<ACMedia::loadXML> inconsistent number of segments");
	}
        
}


ACMediaFeatures* ACMedia::getFeaturesVector(int i){ 
	if (i < int(features_vectors.size()) )
		return features_vectors[i]; 
	else {
		std::cerr << "ACMedia::getFeaturesVector : out of bounds " << i << " > " << features_vectors.size() << std::endl;
	}
	return 0;
}

ACMediaFeatures* ACMedia::getFeaturesVector(string feature_name) { 
	int i;
	for (i=0;i<int(features_vectors.size());i++) {
		if (!(feature_name.compare(features_vectors[i]->getName()))) {
			return features_vectors[i];
		}
	}
	#ifdef USE_DEBUG // use debug message levels instead
	std::cerr << "ACMedia::getFeaturesVector : not found feature named " << feature_name << std::endl;
	#endif
	return 0;
}

std::vector<std::string> ACMedia::getListOfFeaturesPlugins(){
	std::vector<std::string> plugins_list;
	for (int i=0; i<getNumberOfFeaturesVectors(); i++){
		plugins_list.push_back(features_vectors[i]->getName());
	}
	return plugins_list;
}


ACMediaFeatures* ACMedia::getPreProcFeaturesVector(int i){ 
	if (i < int(preproc_features_vectors.size()) )
		return preproc_features_vectors[i]; 
	else {
		std::cerr << "ACMedia::getPreProcFeaturesVector : out of bounds " << i << " > " << preproc_features_vectors.size() << std::endl;
	}
	return 0;
}

ACMediaFeatures* ACMedia::getPreProcFeaturesVector(string feature_name) { 
	int i;
	for (i=0;i<int(preproc_features_vectors.size());i++) {
		if (!(feature_name.compare(preproc_features_vectors[i]->getName()))) {
			return preproc_features_vectors[i];
		}
	}
	#ifdef USE_DEBUG // use debug message levels instead
	std::cerr << "ACMedia::getPreProcFeaturesVector : not found feature named " << feature_name << std::endl;
	#endif
	return 0;
}

std::vector<std::string> ACMedia::getListOfPreProcFeaturesPlugins(){
	std::vector<std::string> plugins_list;
	for (int i=0; i<getNumberOfPreProcFeaturesVectors(); i++){
		plugins_list.push_back(preproc_features_vectors[i]->getName());
	}
	return plugins_list;
}

int ACMedia::replacePreProcFeatures(std::vector<ACMediaFeatures*> newFeatures){
	
	if (newFeatures.size()!=preproc_features_vectors.size())
		return 0;
	for (int i=0;i<newFeatures.size();i++){
		ACMediaFeatures* tempPtr=preproc_features_vectors[i];
		preproc_features_vectors[i]=newFeatures[i];
		if (tempPtr!=0) {
			delete tempPtr;
			tempPtr=0;
		}
	}
	return 1;
}
void ACMedia::cleanPreProcFeaturesVector(void){
	
	std::vector<ACMediaFeatures*>::iterator iter;
	for (iter=preproc_features_vectors.begin();iter!=preproc_features_vectors.end();iter++){
		if ((*iter)!=NULL){
			//cout << *iter << endl; //ccl
			//preproc_features_vectors.erase(iter); //ccl
			delete (*iter);
			(*iter)=NULL;
		}
	}
	preproc_features_vectors.clear();
}

void ACMedia::defaultPreProcFeatureInit(void){
	cleanPreProcFeaturesVector();
	std::vector<ACMediaFeatures*>::iterator iter;
	for (iter=features_vectors.begin(); iter!=features_vectors.end(); iter++) {
		ACMediaFeatures *tempFeat=new ACMediaFeatures;
		tempFeat->setComputed();
		tempFeat->setNeedsNormalization((*iter)->getNeedsNormalization());
		tempFeat->setName((*iter)->getName());
		tempFeat->resize((*iter)->getSize());
		preproc_features_vectors.push_back(tempFeat);
	}
}


// Calls the plugins and fills in info such as width, height, ...
// Implemented in ACMedia.cpp, since it is the same for all media
// Returns 1 if it worked, 0 if it failed
int ACMedia::import(std::string _path, int _mid, ACPluginManager *acpl, bool _save_timed_feat) {
	std::cout << "importing..." << _path << std::endl;
	this->filename = _path;
	this->filename_thumbnail = _path; // XS TODO make real separate thumbnail option
	int import_ok = 0;

	// get info about width, height, mediaData
	// computes thumbnail, ...
	// mediaData will be used by the plugin to compute features
    if (!this->extractData(this->getFileName()))
        return 0;

	if (this->getMediaData()==0){
		import_ok = 0;
		cerr << "<ACMedia::import> failed accessing data for media number: " << _mid << endl;
		return 0;
	}

    //compute thumbnails with available plugins that doesn't require feature extraction or segmentation
    this->computeThumbnails(acpl,false, false);

	//compute features with available plugins
    if (!this->extractFeatures(acpl,_save_timed_feat))
        return 0;
    import_ok=1;

    //compute thumbnails with available plugins that require feature extraction but not segmentation
    this->computeThumbnails(acpl,true, false);

    // Assigning media ids should be done once feature extraction is successful since media ids are incremental
    // If we used non-incremental media ids (md5/sha sums), this wouldn't be a problem anymore
    if (_mid>=0)
        this->setId(_mid);

    // Require at least one thumbnail
    /*if(this->getNumberOfThumbnails()==0){
        std::cerr << "ACMedia::import: no thumbnail available" << std::endl;
        return 0;
    }*/

    //delete data; <--- this is managed from outside (media->deleteData)
	return import_ok;
}


ACMediaThumbnail* ACMedia::getThumbnail(std::string name){
    for(std::vector<ACMediaThumbnail*>::iterator thumbnail = thumbnails.begin();thumbnail != thumbnails.end();thumbnail++){
        if((*thumbnail)->getName() == name)
            return *thumbnail;
    }
    return 0;
}

std::string ACMedia::getThumbnailFileName(std::string name){
    std::string filename("");
    ACMediaThumbnail* _thumbnail(0);
    _thumbnail = this->getThumbnail(name);
    if(_thumbnail)
        filename = _thumbnail->getFileName();
    return filename;
}

int ACMedia::computeThumbnails(ACPluginManager *acpl, bool feature_extracted, bool segmentation_done){
    int thumbnail_number = 0;
    if (acpl) {
        ACMediaData* local_media_data=dynamic_cast<ACMediaData*>(this->getMediaData());
        std::vector<ACMediaThumbnail*> local_thumbnails = acpl->getAvailableThumbnailerPlugins()->summarize(local_media_data, this, feature_extracted, segmentation_done);
        for(std::vector<ACMediaThumbnail*>::iterator local_thumbnail = local_thumbnails.begin();local_thumbnail != local_thumbnails.end();local_thumbnail++)
            this->thumbnails.push_back(*local_thumbnail);
        thumbnail_number = local_thumbnails.size();
    }
    else{
        cerr << "<ACMedia::computeThumbnail> no plugin manager for media";
    }
    return thumbnail_number;
}

int ACMedia::extractFeatures(ACPluginManager *acpl, bool _save_timed_feat) {
    int extract_feat_ok = 0;
    if (acpl) {
        //TR : new implementation to calculate the features
        ACMediaData* local_media_data=dynamic_cast<ACMediaData*>(this->getMediaData());
        this->features_vectors=acpl->getAvailableFeaturesPlugins()->calculate(local_media_data, this, _save_timed_feat);
                //acpl->dump();//CPL
        
		// Checking if any of the media features is empty:
        std::vector<ACMediaFeatures*>::iterator features_vector;
        for (features_vector = features_vectors.begin(); features_vector != features_vectors.end(); features_vector++){
            if((*features_vector)->getSize()==0)
                return 0;
        }

        std::cout << "ACMedia: " << this->features_vectors.size() << " features." <<std::endl;
        if (this->features_vectors.size()>0)
            extract_feat_ok = 1;
    }
    else {
        cerr << "<ACMedia::extractFeatures> no features imported -- no plugin manager for media";
        if(!filename.empty()) cerr << " file '" << filename << "'";
        else if (!label.empty()) cerr << " of label '" << label << "'";
        cerr << endl;
        extract_feat_ok = 0;
    }
    return extract_feat_ok;
}

// segment data (after import !)
// XS TODO the generic method provided here implies reading MediaTimedFeatures from disk
// ... but all media don't have timedFeatures

// if _saved_timed_features : we can read them on disk
// otherwise : hope the plugin will handle it 
//      ex: in audio : plugin extracts features and then segments
int ACMedia::segment(ACPluginManager *acpl, bool _saved_timed_features ) {	
	// check if data have been extracted properly by the import method
	if (this->getMediaData()==0){
		cerr << "<ACMedia::segment> failed accessing data for media number: " << this->getId() << endl;
		return -1;
	}
	
	if (acpl==0){
		cerr << "<ACMedia::segment> missing plugin manager for media number " << this->getId() << endl;
		return -1;	
	}
	
	int segment_ok = 1;
	int features_plugins_count = 0;
	int segmentation_plugins_count = 0;
		
	// XS TODO add sanity checks
	ACMediaTimedFeature* ft_from_disk = 0;
	vector<ACMedia*> afv;
	if (_saved_timed_features) {
		ft_from_disk=this->getTimedFeatures();
		#ifdef USE_DEBUG
		if(ft_from_disk)
			//ft_from_disk->dump(); //CPL commented
		#endif
		
		// should not use all segmentation plugins -- choose one using menu !!	
		// XS TODO: check that ft_from_disk is not empty
                afv=acpl->getActiveSegmentPlugins()->segment(ft_from_disk,this);
	}
	
	// XS TODO change me!!
	// this is very spefic to the audio segmentation plugin...
	// i.e., not _saved_timed_features		
	else {
                afv=acpl->getActiveSegmentPlugins()->segment(this->getMediaData(), this);
	}
	for (unsigned int Iafv=0; Iafv<afv.size(); Iafv++){
            cout << "segment " << Iafv << " - id = " << afv[Iafv]->getId() << endl;
            this->addSegment(afv[Iafv]);
	}
	delete ft_from_disk;

    //compute thumbnails with available plugins that require feature extraction and segmentation
    this->computeThumbnails(acpl,true, true);

	return segment_ok;
}

// int ACMedia::segment(){
// 	for (int i = 0; i < 4; i++){
// 		ACMedia* media = ACMediaFactory::getInstance()->create(this);
// 		media->setParentId(this->mid);
// 		media->setStart(this->start + ((float)i/4.0) * this->getDuration());
// 		media->setEnd(this->start + ((float)(i+1)/4.0) * this->getDuration()-.01);
// 		this->addSegment(media);
// 	}
// }


ACMediaTimedFeature* ACMedia::getTimedFeatures() {
	bool _binary=false;//true
	ACMediaType mediaType=this->getType();
    ACMediaTimedFeature* output = 0;
	std::vector<std::string> mtf_files_names=this->getTimedFileNames();
	
	//    vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin();
	//  if (iter_vec != mCurrPlugin[mediaType].end()) 
	std::vector<std::string>::iterator iter_vec=mtf_files_names.begin();
    if (iter_vec!=mtf_files_names.end())
	{	
		output = new ACMediaTimedFeature();
		output->loadFromFile(*iter_vec,_binary);
        size_t n2= iter_vec->find_last_of ( string("_") )-1;
        size_t n1=iter_vec->find_last_of ( string("_"),n2 );
        string locFeatureName=iter_vec->substr(n1+1,n2-n1);
            output->setName(locFeatureName);
        
        if (output != 0) {
            iter_vec++;
            for (; iter_vec != mtf_files_names.end(); iter_vec++) {
				ACMediaTimedFeature* temp=new ACMediaTimedFeature();
				temp->loadFromFile(*iter_vec,false);
                n2= iter_vec->find_last_of ( string("_") )-1;
                n1=iter_vec->find_last_of ( string("_"),n2 );
                locFeatureName=iter_vec->substr(n1+1,n2-n1);
                temp->setName(locFeatureName);
				output->appendTimedFeature(temp);
				delete temp;
            }
        }
    }
    return output;
}


// CPL 25/06
// get only one TimedFeature
ACMediaTimedFeature* ACMedia::getTimedFeatures(string feature_name) {
    bool _binary=false;//true
    ACMediaType mediaType=this->getType();
    ACMediaTimedFeature* mtf_from_file = 0;
    ACMediaTimedFeature* output = 0;
    std::vector<std::string> mtf_files_names=this->getTimedFileNames();

    // First try by name:
    std::vector<std::string>::iterator iter_vec;
    for (iter_vec=mtf_files_names.begin(); iter_vec != mtf_files_names.end(); iter_vec++) {
        size_t n2= iter_vec->find_last_of ( string("_") )-1;
        size_t n1=iter_vec->find_last_of ( string("_"),n2 );
        string locFeatureName=iter_vec->substr(n1+1,n2-n1);
        if(feature_name != locFeatureName) {
            std::cout << "ACMedia::getTimedFeatures: found matching mtf by filename from file " << *iter_vec << std::endl;
            break;
        }
    }
    if(iter_vec != mtf_files_names.end()){
        mtf_from_file=new ACMediaTimedFeature();
        mtf_from_file->loadFromFile(*iter_vec,_binary);

        if(feature_name != mtf_from_file->getName()){
            std::cerr << "ACMedia::getTimedFeatures: file " << *iter_vec << " doesn't list the expected feature named " << feature_name << std::endl;
        }

        if( mtf_from_file->getDistinctNames().size()!=1){
            std::cerr << "ACMedia::getTimedFeatures: file " << *iter_vec << " should contain just one distinct feature name matching " << feature_name << std::endl;
        }

        output = new ACMediaTimedFeature();
        output->setName(feature_name);
        output->setTime(mtf_from_file->getTime());
        output->setValue(mtf_from_file->getValue());
        //output->appendTimedFeature(temp);
        delete mtf_from_file;
        mtf_from_file = 0;
        return output;
    }

    // Otherwise try to load all files and check their contents:
    for (iter_vec=mtf_files_names.begin(); iter_vec != mtf_files_names.end(); iter_vec++) {
        std::cout << "ACMedia::getTimedFeatures: checking feature file " << *iter_vec << std::endl;
        mtf_from_file=new ACMediaTimedFeature();
        mtf_from_file->loadFromFile(*iter_vec,_binary);
        std::vector<std::string> names = mtf_from_file->getDistinctNames();
        std::vector<std::string>::iterator name = std::find(names.begin(),names.end(),feature_name);
        if(name != names.end()){
            int start(-1),end(-1);
            for(std::vector<std::string>::iterator name = names.begin();name != names.end();++name){
                if(*name==feature_name){
                    if(start==-1)
                        start = std::distance(names.begin(),name);
                    end = std::distance(names.begin(),name);
                }
            }
            std::cout << "ACMedia::getTimedFeatures: feature spans from columns " << start << " to " << end << std::endl;
            if(start == -1 && end == -1){
                std::cerr << "ACMedia::getTimedFeatures: wrong feature columns " << start << ".." << end << std::endl;
                delete mtf_from_file;
                mtf_from_file = 0;
                return 0;
            }

            output = new ACMediaTimedFeature();
            output->setName(feature_name);
            output->setTime(mtf_from_file->getTime());
            output->setValue(mtf_from_file->getValue().rows(start,end));
            delete mtf_from_file;
            mtf_from_file = 0;
            return output;
        }
        delete mtf_from_file;
        mtf_from_file = 0;
    }
    return 0;
}
