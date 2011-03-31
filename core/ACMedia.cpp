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

using namespace std;
ACMedia::ACMedia() { 
	this->init();
}

void ACMedia::init() { 
	mid = -1;
	parentid = -1;	
	width = 0;
	height = 0;
	start = -1;
	end = -1;
	features_vectors.resize(0);
	persistent_data = false;
	data = 0; // new ACMediaData(MEDIA_TYPE_NONE);
	features_saved_xml = false;
}

ACMedia::ACMedia(const ACMedia& m, bool reduce) {
	this->init();	
	media_type = m.media_type;
	width = m.width;
	height = m.height;
	filename = m.filename;
	filename_thumbnail = m.filename_thumbnail;
	start = m.start;
	end = m.end;
	persistent_data = !reduce;
	if (persistent_data){
		if (m.media_type == 0){ //if (m.media_type != MEDIA_TYPE_NONE){
			if( m.data != 0){//if( m.data->getMediaType() != MEDIA_TYPE_NONE){// if m.data contains data
				data->copyData(m.data);
			}
			else {
				data = new ACMediaData(m.media_type,m.filename);
			}
		}
		//else we dont have any data to copy
	}
	//else
	//	data = new ACMediaData(MEDIA_TYPE_NONE); // already through init();
}

ACMedia::~ACMedia() { 
	filename.clear();
	filename_thumbnail.clear();
	vector<ACMediaFeatures*> ::iterator iter;
	for (iter = features_vectors.begin(); iter != features_vectors.end(); iter++) {
		delete *iter;//needed erase call destructor of pointer (i.e. none since it's just a pointer) not of pointee ACMediaFeatures
		//features_vectors.erase(iter); //will cause segfault. besides the vector is automatically emptied, no need to erase.
	}
	// XS TODO : why is this commented ?
	//if (data) delete data;
}

void ACMedia::deleteData(){
	if (!persistent_data){
		delete data;
		data = 0;
	}
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
	library_file << mid << endl;
	
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

void ACMedia::saveXML(TiXmlElement* _medias){
	if ( features_saved_xml || _medias == NULL) return; 
	TiXmlElement* media;
	media = new TiXmlElement( "Media" );  
	_medias->LinkEndChild( media );  
	media->SetAttribute("MediaID", mid);
	media->SetAttribute("MediaType", media_type);
	
	media->SetAttribute("FileName", filename);
	
	saveXMLSpecific(media);
	
	int n_features = features_vectors.size();
	TiXmlElement* features = new TiXmlElement( "Features" );  
	features->SetAttribute("NumberOfFeatures", n_features);
	media->LinkEndChild( features );  

	for (unsigned int i=0; i<n_features; i++) {
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
	
	TiXmlElement* segments = new TiXmlElement( "Segments" );  
	media->LinkEndChild( segments );  
	segments->SetAttribute("NumberOfSegments", this->getNumberOfSegments());

	// saves info about segments (if any) : beginning, end, ID
	// the parent ID of the segment is the ID of the current media
	for (int i=0; i<this->getNumberOfSegments();i++) {
		TiXmlElement* seg = new TiXmlElement( "Segment" );  
		segments->LinkEndChild( seg );  
		seg->SetAttribute("Start", this->getSegment(i)->getStart());
		seg->SetAttribute("End", this->getSegment(i)->getEnd());
		std::string s;
		std::stringstream tmp;
		tmp << this->getSegment(i)->getId();
		s = tmp.str();
		TiXmlText* segID = new TiXmlText(s.c_str());
		seg->LinkEndChild( segID );  		
	}
	features_saved_xml = true;
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
			temp += "\\ ";
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
		fixWhiteSpace(pName);
		this->setFileName(pName);
	}
	
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
	
	int count_f = 0;
	TiXmlElement* featuresElement = _pMediaNodeHandle.FirstChild( "Features" ).Element();
	if (!featuresElement)
		throw runtime_error("corrupted XML file, no features");	
	int nf=-1;
	featuresElement->QueryIntAttribute("NumberOfFeatures", &nf);
	
	if (nf < 0)
		throw runtime_error("corrupted XML file, wrong number of features");
	else if (nf ==0)
		// XS TODO could this happen without it being an error?
		// if not, put <= in the test above
		cout << "loading media with no features" << endl;
	
	TiXmlElement* featureElement = _pMediaNodeHandle.FirstChild( "Features" ).FirstChild( "Feature" ).Element();
	if (!featureElement)
		throw runtime_error("corrupted XML file, error reading features");
	TiXmlText* featureElementsAsText = 0;
	ACMediaFeatures* mediaFeatures;
	for( featureElement; featureElement; featureElement = featureElement->NextSiblingElement() ) {
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
				// XS TODO add test
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
		features_vectors.push_back(mediaFeatures);
		count_f++;
	}
	
	// consistency check for features
	if (count_f != nf) 
		throw runtime_error("<ACMedia::loadXML> inconsistent number of features");

	// --- segments --- 

	// allow an XML without segments.
	TiXmlElement* segmentsElement = _pMediaNodeHandle.FirstChild( "Segments" ).Element();
	if (featuresElement) {
		int ns = -1;
		segmentsElement->QueryIntAttribute("NumberOfSegments", &ns);
		if (ns < 0)
			throw runtime_error("corrupted XML file, <segments> present, but no segments");

		TiXmlElement* segmentElement = _pMediaNodeHandle.FirstChild( "Segments" ).FirstChild( "Segment" ).Element();
		TiXmlText* segmentIDElementsAsText = 0;
		int count_s = 0;
		
		for( segmentElement; segmentElement; segmentElement = segmentElement->NextSiblingElement() ) {
			ACMedia* segment_media = ACMediaFactory::getInstance().create(this->getMediaType());
			int n_start=-1;
			int n_end=-1;
			segmentElement->QueryIntAttribute("Start", &n_start);
			if (n_start < 0) {
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment start");
			}
			segment_media->setStart(n_start);
			
			segmentElement->QueryIntAttribute("End", &n_end);
			if (n_end < 0){
				delete segment_media;
				throw runtime_error("corrupted XML file, wrong segment end");
			}
			segment_media->setEnd(n_end);
			
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
			count_s++;
		}
		// consistency check for segments
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
	std::cerr << "ACMedia::getFeaturesVector : not found feature named " << feature_name << std::endl;
	return 0;
}

std::vector<std::string> ACMedia::getListOfFeaturesPlugins(){
	std::vector<std::string> plugins_list;
	for (int i=0; i<getNumberOfFeaturesVectors(); i++){
		plugins_list.push_back(features_vectors[i]->getName());
	}
	return plugins_list;
}

// Calls the plugins and fills in info such as width, height, ...
// Implemented in ACMedia.cpp, since it is the same for all media
// Returns 1 if it worked, 0 if it failed
int ACMedia::import(std::string _path, int _mid, ACPluginManager *acpl, bool _save_timed_feat) {
	std::cout << "importing..." << _path << std::endl;
	this->filename = _path;
	this->filename_thumbnail = _path;
	int import_ok = 0;
	if (_mid>=0) this->setId(_mid);
	
	// get info about width, height, thumbnail, ...
	// and return a pointer to the data (ACMediaData*)
	// data will be used by the plugin to compute features
	this->extractData(this->getFileName());
	if (data==0){
		import_ok = 0;
		cerr << "<ACMedia::import> failed accessing data for media number: " << _mid << endl;
		return 0;
	}
	
	//compute features with available plugins
	// XS TODO config file
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->getType()
					&& acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
					
					cout << "Computing features using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;
					//plugin->start();
					ACFeaturesPlugin* localPlugin=dynamic_cast<ACFeaturesPlugin*>( acpl->getPluginLibrary(i)->getPlugin(j));
					vector<ACMediaFeatures*> afv;
					if (localPlugin!=NULL)
						afv =localPlugin->calculate(data, this, _save_timed_feat);
					else {
						cerr << "<ACMedia::import> failed plugin access failed "<< acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
					}

					if (afv.size()==0){
						import_ok = 0;
						cerr << "<ACMedia::import> failed importing feature from plugin: " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
					}
					else {
						for (unsigned int Iafv=0; Iafv< afv.size() ; Iafv++)
							this->addFeaturesVector(afv[Iafv]);
						import_ok = 1;
					}
					//plugin->stop();
				}
				else {
					//cerr << "Plugin " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << " not used here." << endl;
					//cerr << "acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType(): " << acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType()  << endl;
					//cerr << "this->getType(): " << this->getType() << endl;
					//cerr << "acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType(): " << acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() << endl;
					//cerr << "PLUGIN_TYPE_FEATURES: "<< PLUGIN_TYPE_FEATURES << endl;
				}
			}
		}
		std::cout << "Features calculated" << std::endl;
	}
	else {
		cerr << "<ACMedia::import> no features imported -- no plugin manager for media number: " << _mid << endl;
		import_ok = 0;
	}
		
	//delete data; <--- this is managed from outside (media->deleteData)
	return import_ok;
}

// segment data (after import !)
// XS TODO the generic method provided here implies reading MediaTimedFeatures from disk
// ... but all media don't have timedFeatures

// if _saved_timed_features : we can read them on disk
// otherwise : hope the plugin will handle it 
//      ex: in audio : plugin extracts features and then segments
int ACMedia::segment(ACPluginManager *acpl, bool _saved_timed_features ) {	
	// check if data have been extracted properly by the import method
	if (data==0){
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
	// XS TODO 2 : the double "for" loops are heavy but necessary since we don't know the type of plugin in advance
	// if we do something like
	// vector <ACPlugin*> features_plugins
	// features_plugins.push_back( acpl->getPluginLibrary(i)->getPlugin(j));
	// then features_plugins[i]->segment() does not call the right segment method...
	// unless we make plugin a virtual class ?
	ACMediaTimedFeature* ft_from_disk = 0;
	
	if (_saved_timed_features) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if ( acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->getType()){
					if ( acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
						cout << "Collecting saved features using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;

						features_plugins_count ++;
						if (features_plugins_count ==1) {
							ACFeaturesPlugin* localPlugin=dynamic_cast<ACFeaturesPlugin*>( acpl->getPluginLibrary(i)->getPlugin(j));
							if (localPlugin!=NULL)
								ft_from_disk = localPlugin->getTimedFeatures();
							else 
							{
								cerr << "<ACMedia::import> failed plugin access failed "<< acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
							}
							// first plugin : get (implicit "new" done in getTimedFeatures, so you need to delete)
						}
						else {
							// next plugins (hence i=1) : append
							ACFeaturesPlugin* localPlugin=dynamic_cast<ACFeaturesPlugin*>( acpl->getPluginLibrary(i)->getPlugin(j));
							if (localPlugin!=NULL)
								ft_from_disk->appendTimedFeature(localPlugin->getTimedFeatures());
							else 
							{
								cerr << "<ACMedia::import> failed plugin access failed "<< acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
							}
						}
					}
				}
			}
		}
		
		// DEBUG
		if(ft_from_disk)
			ft_from_disk->dump();
		
		// should not use all segmentation plugins -- choose one using menu !!	
		// XS TODO: check that ft_from_disk is not empty
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_SEGMENTATION) {
					cout << "Segmenting features using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;

					segmentation_plugins_count ++;
					vector<ACMedia*> afv = dynamic_cast<ACSegmentationPlugin*>(acpl->getPluginLibrary(i)->getPlugin(j))->segment(ft_from_disk, this);
					if (afv.size()==0){
						segment_ok = 0;
						cerr << "<ACMedia::segment> failed importing segments from plugin: " <<  acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
					}
					else {
						for	(unsigned int Iafv=0; Iafv<afv.size(); Iafv++){
							this->addSegment(afv[Iafv]);
						}
						segment_ok = 1;
					}
				}
			}
		}
	}
	
	// XS TODO change me!!
	// this is very spefic to the audio segmentation plugin...
	// i.e., not _saved_timed_features		
	else {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_SEGMENTATION) {
					segmentation_plugins_count ++;
					cout << "Segmenting media using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;
					vector<ACMedia*> afv =dynamic_cast<ACSegmentationPlugin*>( acpl->getPluginLibrary(i)->getPlugin(j))->segment(data, this);
					// the audio plugin does features extraction AND segmentation
					if (afv.size()==0){
						segment_ok = 0;
						cerr << "<ACMedia::segment> failed importing segments from plugin: " <<  acpl->getPluginLibrary(i)->getPlugin(j)->getName() << endl;
					}
					else {
						for (unsigned int Iafv=0; Iafv<afv.size(); Iafv++){
							this->addSegment(afv[Iafv]);
						}
						segment_ok = 1;
					}
				}
			}
		}
	}
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

