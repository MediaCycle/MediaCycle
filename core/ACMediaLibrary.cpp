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

// XS for sorting:
#include <algorithm>

#define VERBOSE

using namespace std;
// to save library items in binary format, uncomment the following line:
// #define SAVE_LOOP_BIN

// XS pthread needs a single argument to the function to be multithreaded
// so put them all in a struct
struct pthread_input
{
	ACMedia* media;
	string filename;
	int id;
	ACPluginManager *acpl;
};

void* p_importSingleFile(void *arg){
	struct pthread_input *data =(struct pthread_input *)arg;
	cout << data->filename << " ; " <<  data->id << " ; plugin size : " << data->acpl->getSize() << endl;
	data->media->import(data->filename, data->id, data->acpl);
}

ACMediaLibrary::ACMediaLibrary() {
	this->cleanLibrary();
	this->cleanStats();
	media_type = MEDIA_TYPE_NONE;
}

ACMediaLibrary::ACMediaLibrary(ACMediaType aMediaType) {
	this->cleanLibrary();
	this->cleanStats();
	media_type = aMediaType;
}

ACMediaLibrary::~ACMediaLibrary(){
	this->deleteAllMedia();
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
	media_library.resize(0);
	synthesisID = -1;
	mediaID = 0; // index of the next one to be inserted (= the number of media already imported); not -1
}

void ACMediaLibrary::cleanStats() {
	cout << "cleaning features mean and stdev"<< endl;
	mean_features.clear();
	stdev_features.clear();
}


// this same function is used to import a whole directory, a single file or a list of files.
// it uses scanDirectory to construct a vector (filenames) containing the files to be analyzed.
int ACMediaLibrary::importDirectory(std::string _path, int _recursive, ACPluginManager *acpl, bool forward_order, bool doSegment) {
	std::vector<string> filenames;

	scanDirectory(_path, _recursive, filenames);
	
	for (unsigned int i=0; i<filenames.size(); i++){
		int index = forward_order ? i : filenames.size()-1-i;//CF if reverse order (not forward_order), segments in subdirs are added to the library after the source recording
		this->importFile(filenames[index], acpl, doSegment );
		}
	std::cout << "Library size : " << this->getSize() << std::endl;
	return 1;
}

int ACMediaLibrary::importFile(std::string _filename, ACPluginManager *acpl, bool doSegment) {
	string extension;
	std::vector<ACMedia*> mediaSegments;

	extension = fs::extension(_filename);
	ACMedia* media = ACMediaFactory::create(extension);
	cout << "extension:" << extension << endl; 		
	if (media == NULL) {
		cout << "extension unknown, skipping " << _filename << " ... " << endl;
	}
	else {
		// This has to be done before segmentation to have proper id
		if (media->import(_filename, this->getMediaID(), acpl)){
			this->addMedia(media);
#ifdef VERBOSE
			cout << "imported " << _filename << " with mid = " <<  this->getMediaID() << endl;
#endif // VERBOSE
			this->incrementMediaID();
		}
		if (doSegment){
			// segments are created without id 
			media->segment(acpl);
			mediaSegments = media->getAllSegments();
			for (unsigned int i = 0; i < mediaSegments.size(); i++){
				if (mediaSegments[i]->import(_filename, this->getMediaID(), acpl)){
					this->addMedia(mediaSegments[i]);
					this->incrementMediaID();
				}
			}
		}
	}
	
}
// construct a vector (filenames) containing the files in a given directory (_path).
// when "recursive" is turned on, it will scan subdirectories too 
int ACMediaLibrary::scanDirectory(std::string _path, int _recursive, std::vector<string>& filenames) {
	
	fs::path full_path( fs::initial_path<fs::path>() );
	
	full_path = fs::system_complete( fs::path( _path, fs::native ) );
	
	if ( !fs::exists( full_path ) )	{
		printf("File or directory not found: %s\n", full_path.native_file_string().c_str());
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
	else { 
		// encounter a file in the directory
		// put it in the vector of files to be analyzed
		filenames.push_back(_path);
	}
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
		local_media = ACMediaFactory::create(media_type);
		if (local_media != NULL) {
			ret = local_media->loadACL(library_file);
			if (ret) {
				//std::cout << "Media Library Size : " << this->getSize() << std::endl;//CF free the console
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
		local_media = ACMediaFactory::create(media_type);
		if (local_media != NULL) {
			ret = local_media->loadMCSL(library_file);
			if (ret) {
				//std::cout << "Media Library Size : " << this->getSize() << std::endl;//CF free the console
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

// C++ version
int ACMediaLibrary::saveACLLibrary(std::string _path){
#ifdef SAVE_LOOP_BIN
	ofstream library_file (_path.c_str(), ios::binary);
#else
	ofstream library_file (_path.c_str());
#endif //SAVE_LOOP_BIN
	cout << "saving ACL file: " << _path << endl;
	
	int n_loops = this->getSize();
	// we save UNnormalized features
	denormalizeFeatures();
	for(int i=0; i<n_loops; i++) {
		media_library[i]->saveACL(library_file);
	}
	library_file.close();
	normalizeFeatures();
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
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

/* SD 2010 sep discontinued
int ACMediaLibrary::openLibrary(std::string _path, bool aInitLib){
	// this does not re-initialize the media_library
	// but appends new media to it.
	// except if aInitLib is set to true
	int ret, file_count=0;
	
	FILE *library_file = fopen(_path.c_str(),"r");
	setlocale(LC_NUMERIC, "C");//correct problem with fscanf caused by Qt (cannot read floating point string without it)
	//if the file exists
	if (library_file) {
		ACMedia* local_media;
		// --TODO-- ???  how does it know which type of media ?
		// have to be set up  at some point using setMediaType()
		if (aInitLib) {
			cleanLibrary();
		}
		//media_library.resize(0); //no reason to be here if no reset asked. resize to 0 when calling cleanLibrary()
		do {
			local_media = ACMediaFactory::create(media_type);
			if (local_media != NULL) {
				ret = local_media->load(library_file);
				if (ret) {
					// problem if id is -1 (==default value) --> this corrects the problem (used temporarily in avlaughtercycle where all ids were -1
					//if (local_media->getId() < 0)
					//	local_media->setId(media_library.size());//this is not reliable if the library file lists a mix of audio files w/ and w/o an id. maybe we should set the id only here
					//std::cout << "Media Library Size : " << this->getSize() << std::endl;//CF free the console
					media_library.push_back(local_media);
					file_count++;
					
				}
			}
			else {
				std::cout<<"openLibrary : Wrong Media Type" <<std::endl;
			}
			
		}
		while (ret>0);
		
		fclose(library_file);
	}
	
	return file_count;
}
*/

/* void ACMediaLibrary::saveAsLibrary(string _path) {
	
	int n_loops = this->getSize();
	
	FILE *library_file = fopen(_path.c_str(),"w");
	
	// we save UNnormalized features
	denormalizeFeatures();
	for(int i=0; i<n_loops; i++) {
		// --TODO-- ???  how does it know which type of media ?
		// have to be set up  at some point using setMediaType()
		media_library[i]->save(library_file);
	}
	fclose(library_file);
	normalizeFeatures();
}
 */

int ACMediaLibrary::addMedia(ACMedia *aMedia) {
    //TODO remove media_type check
    // mediacycle should be able to manage a mix of any media
    // instead of only medias of one type
	if (aMedia != NULL){
		if (aMedia->getType() == this->media_type) {
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
		return NULL;
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

std::string ACMediaLibrary::getThumbnailFileName(int id) {
	int i;
	for (i=0;i<this->getSize();i++) {
		if (id==media_library[i]->getId()) {
			return media_library[i]->getThumbnailFileName();
		}
	}
	return "";
}

bool ACMediaLibrary::isEmpty() {
	if (this->getSize() <= 0) {
		cout << "empty library" << endl;
		return true;
	}
	
	// XS assumes each item has same number of features
	// and each feature has the same number of floats in its vector
	
	int number_of_features = media_library[0]->getNumberOfFeaturesVectors();
	if (number_of_features <= 0) {
		cout << "no features in vector [0]" << endl;
		return true;
	}
	return false;
}

void ACMediaLibrary::calculateStats() {
	if ( isEmpty() ) return;
	int n = this->getSize() ;
	int number_of_features = media_library[0]->getNumberOfFeaturesVectors();
	
	// initialize to zero
	int i,j,k;
	for (i=0; i< number_of_features; i++) {
		vector<double> tmp_vect;
		for (j=0; j< media_library[0]->getFeaturesVector(i)->getSize(); j++) {
			tmp_vect.push_back(0.0);
		}
		mean_features.push_back(tmp_vect);
		stdev_features.push_back(tmp_vect);
	}
	
	// computing sums
	for(i=0; i<n; i++) {
		ACMedia* item = media_library[i];		
		for(j=0; j<(int)mean_features.size(); j++){
			for(k=0; k<(int)mean_features[j].size(); k++){
				double val = item->getFeaturesVector(j)->getFeatureElement(k);
				
				mean_features[j][k] += val;
				stdev_features[j][k] += val * val;
				
			}
		}
	}
	
	// before: divide by N --> biased variance estimator
	// now : divide by (n-1) -- unless n=1
	int nn;
	if (n==1) nn = n;
	else nn = n-1;
	
	for(j=0; j<(int)mean_features.size(); j++) {
		printf("feature %d\n", j);
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

void ACMediaLibrary::normalizeFeatures() {
	
	ACMediaFeatures* feature;
	cout << "normalizing features" << endl;
	if ( isEmpty() )  return;
	
	if (index_last_normalized < 0) {
		// *first* normalization
		calculateStats();
	}
	else {
		// *subsequent* normalization
		denormalizeFeatures();
		calculateStats();
	}
	
	unsigned int i,j,k;
	unsigned int n = this->getSize() ;
	
	for(i=0; i<n; i++){
		ACMedia* item = media_library[i];
		for(j=0; j<mean_features.size(); j++) {
			feature = item->getFeaturesVector(j);
			if (feature->getNeedsNormalization()) {
				for(k=0; k<mean_features[j].size(); k++) {
					float old = feature->getFeatureElement(k);
					feature->setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//CF TI_MAX(stdev_features[j][k] , 0.00001)));
				}
			}
		}
	}
	index_last_normalized = n-1;
}

void ACMediaLibrary::denormalizeFeatures() {
	cout << "denormalizing features" << endl;
	if ( isEmpty() || index_last_normalized<0) return; // Ju: added index_last_normalized<0 to avoid segmentation fault under linux, I believe caused by the fact i is declared as unsigned int while index_last_normalized is int, so it messed up in the for loop
	
	unsigned int i,j,k;
	
	// XS denormalize only those that have been normalized (duh),
	// so n is NOT the full library size !
	
	for(i=0; i<= index_last_normalized; i++){
		ACMedia* item = media_library[i];
		for(j=0; j<mean_features.size(); j++) {
			if (item->getFeaturesVector(j)->getNeedsNormalization()) {
				for(k=0; k<mean_features[j].size(); k++) {
					float old = item->getFeaturesVector(j)->getFeatureElement(k);
					item->getFeaturesVector(j)->setFeatureElement(k, old * stdev_features[j][k] + mean_features[j][k]);
				}
			}
		}
	}
	index_last_normalized = -1;
	cleanStats();
}

// -------------------------------------------------------------------------
// XS custom for Thomas Isreal videos : features sorted, along with filename
void ACMediaLibrary::saveSorted(string output_file){
	//XS dirty, supposes you save feature number 0 -- should be generalized !
	ofstream out(output_file.c_str()); 
	
	const int nfeat=1;
	
	std::vector<std::pair<float, int> > f[nfeat];
	
	for (int i=0; i< this->getSize() ;i++) {
		out << i << " : " << media_library[i]->getFileName() << endl;
		for (int j=0; j< nfeat ;j++) {
			out << media_library[i]->getFeaturesVector(0)->getFeatureElement(j) << endl;
			f[j].push_back (std::pair<float, int> (media_library[i]->getFeaturesVector(0)->getFeatureElement(j), i));
		}
	}
	
	for (int j=0; j< nfeat ;j++) {
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
		//		f[1][i].first << "\t" << f[1][i].second << "\t" <<
		//		f[2][i].first << "\t" << f[2][i].second << "\t" << 
		//		f[3][i].first << "\t" << f[3][i].second << "\t" << 
		//		f[4][i].first << "\t" << f[4][i].second << "\t" << 
		//		f[5][i].first << "\t" << f[5][i].second << "\t" <<  
		endl;
	}
	out.close();
}

