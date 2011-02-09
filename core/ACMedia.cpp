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
#include "ACMediaFactory.h"

using namespace std;
ACMedia::ACMedia() { 
	mid = -1;
	parentid = -1;	
	width = 0;
	height = 0;
	start = -1;
	end = -1;
	features_vectors.resize(0);
	persistent_data = false;
	data = new ACMediaData(MEDIA_TYPE_NONE);
}

ACMedia::ACMedia(const ACMedia& m, bool reduce){
	media_type = m.media_type;
	mid = -1;
	width = m.width;
	height = m.height;
	filename = m.filename;
	filename_thumbnail = m.filename_thumbnail;
	start = m.start;
	end = m.end;
	features_vectors.resize(0);	
	persistent_data = !reduce;
	if (persistent_data){
		if (m.media_type != MEDIA_TYPE_NONE){
			if( m.data->getMediaType() != MEDIA_TYPE_NONE){// if m.data contains data
				data->copyData(m.data);
			}
			else {
				data = new ACMediaData(m.media_type,m.filename);
			}
		}
		//else we dont have any data to copy
	}
	else
		data = new ACMediaData(MEDIA_TYPE_NONE);
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
	TiXmlElement* media;
	media = new TiXmlElement( "Window" );  
	_medias->LinkEndChild( media );  
	media->SetAttribute("FileName", filename);
	media->SetAttribute("MediaID", mid);
	
	saveXMLSpecific(media);
	
	int n_features = features_vectors.size();
	media->SetAttribute("NumberOfFeatures", n_features);
	
	for (unsigned int i=0; i<features_vectors.size();i++) {
		int n_features_elements = features_vectors[i]->getSize();
		int nn = features_vectors[i]->getNeedsNormalization();
		TiXmlElement* mediaf = new TiXmlElement( "Feature" );  
		media->LinkEndChild( mediaf );  
		mediaf->SetAttribute("FeatureName", features_vectors[i]->getName());
		mediaf->SetAttribute("NeedsNormalization", nn);
		mediaf->SetAttribute("NumberOfFeatureElements",n_features_elements);
		for (int j=0; j<n_features_elements; j++) {
			TiXmlElement* mediafe = new TiXmlElement( "Element" );  
			mediaf->LinkEndChild( mediafe );  
			std::string s;
			std::stringstream tmp;
			tmp << j;
			s = tmp.str();
			
			mediafe->SetDoubleAttribute(s.c_str(), features_vectors[i]->getFeatureElement(j));
		}
		
	}
}

// C++ version
// loads from an existing (i.e. already opened) acl file
// returns 0 if error (trying to open empty file, failed making thumbnail, ...)
// returns 1 if fine
// return value is used in ACMediaLibrary::openACLLibrary
int ACMedia::loadACL(std::string media_path, ifstream &library_file, int mcsl) {
	
	int n_features;
	int n_features_elements = 0;	
	int nn;
	string tab;
	
	ACMediaFeatures* mediaFeatures;
	string featureName;
	float local_feature;
	
	int nbSegments, segId;
	
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

ACMediaFeatures* ACMedia::getFeaturesVector(int i){ 
	if (i < int(features_vectors.size()) )
		return features_vectors[i]; 
	else {
		std::cerr << "ACMedia::getFeaturesVector : out of bounds " << i << " > " << features_vectors.size() << std::endl;
	}
	return NULL;
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

// Calls the plugins and fills in info such as width, height, ...
// Implemented in ACMedia.cpp, since it is the same for all media
// Returns 1 if it worked, 0 if it failed
int ACMedia::import(std::string _path, int _mid, ACPluginManager *acpl, bool _save_timed_feat ) {
	std::cout << "importing..." << _path << std::endl;
	this->filename = _path;
	this->filename_thumbnail = _path;
	int import_ok = 0;
	
	if (_mid>=0) this->setId(_mid);
	
	// get info about width, height, thumbnail, ...
	// and return a pointer to the data (ACMediaData*)
	// data will be used by the plugin to compute features
	this->extractData(this->getFileName());
	if (data==NULL){
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
					vector<ACMediaFeatures*> afv = acpl->getPluginLibrary(i)->getPlugin(j)->calculate(data, this, _save_timed_feat);
					
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

	//delete data;
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
	if (data==NULL){
		cerr << "<ACMedia::segment> failed accessing data for media number: " << this->getId() << endl;
		return -1;
	}
	
	if (acpl==NULL){
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
	ACMediaTimedFeature* ft_from_disk;
	
	if (_saved_timed_features) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if ( acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->getType()){
					if ( acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
						cout << "Collecting saved features using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;

						features_plugins_count ++;
						if (features_plugins_count ==1) {
							// first plugin : get (implicit "new" done in getTimedFeatures, so you need to delete)
							ft_from_disk = acpl->getPluginLibrary(i)->getPlugin(j)->getTimedFeatures();
						}
						else {
							// next plugins (hence i=1) : append
							ft_from_disk->appendTimedFeature(acpl->getPluginLibrary(i)->getPlugin(j)->getTimedFeatures());
						}
					}
				}
			}
		}
		
		// DEBUG
		ft_from_disk->dump();
		
		// should not use all segmentation plugins -- choose one using menu !!	
		// XS TODO: check that ft_from_disk is not empty
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_SEGMENTATION) {
					cout << "Segmenting features using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;

					segmentation_plugins_count ++;
					vector<ACMedia*> afv = acpl->getPluginLibrary(i)->getPlugin(j)->segment(ft_from_disk, this);
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
			
	// i.e., not _saved_timed_features		
	else {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_SEGMENTATION) {
					segmentation_plugins_count ++;
					cout << "Segmenting media using plugin : " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;
					vector<ACMedia*> afv = acpl->getPluginLibrary(i)->getPlugin(j)->segment(data, this);
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
// 		ACMedia* media = ACMediaFactory::create(this);
// 		media->setParentId(this->mid);
// 		media->setStart(this->start + ((float)i/4.0) * this->getDuration());
// 		media->setEnd(this->start + ((float)(i+1)/4.0) * this->getDuration()-.01);
// 		this->addSegment(media);
// 	}
// }

