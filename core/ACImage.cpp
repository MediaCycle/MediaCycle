/*
 *  ACImage[loop].cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
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

#include "ACImage.h"
#include <fstream>

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;


//------------------------------------------------------------------

ACImage::ACImage() : ACMedia() {
	media_type = MEDIA_TYPE_IMAGE;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;	
}

ACImage::~ACImage() {
	delete thumbnail;
}

// XS: when we load from file, there is no need to have a pointer to the data passed to the plugin
// XS TODO check this
int ACImage::computeThumbnail(string _fname, int w, int h){
	if (w <=0 || h <=0){
		cerr << "<ACImage::computeThumbnail> dimensions should be positive: " << w << " x " << h << endl;
		return -1;
	}
	IplImage* imgp_full = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	thumbnail = cvCreateImage(cvSize (w, h), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);
	
	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	thumbnail_width = w;
	thumbnail_height = h;
	//	cvReleaseImage(&imgp_full);
	return 1;
}


int ACImage::computeThumbnail(ACMediaData* data_ptr, int w, int h){
	if (w <=0 || h <=0){
		cerr << "<ACImage::computeThumbnail> dimensions should be positive: " << w << " x " << h << endl;
		return -1;
	}
	IplImage* imgp_full = data_ptr->getImageData();
	thumbnail = cvCreateImage(cvSize (w, h), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);

	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	thumbnail_width = w;
	thumbnail_height = h;
//	cvReleaseImage(&imgp_full);
	return 1;
}

ACMediaData* ACImage::extractData(string fname){
	// XS todo : store the default header (16 below) size somewhere...
	ACMediaData* image_data = new ACMediaData(fname, MEDIA_TYPE_IMAGE);
	computeThumbnail(image_data, 64, 64);
	width = thumbnail_width;
	height = thumbnail_height;
	duration = 0;
	return image_data;
}

// C++ version
// writes in an existing (i.e. already opened) acl file
// works for binary too, the stream deals with it
void ACImage::saveACL(ofstream &library_file) {
	if (! library_file.is_open()) {
		cerr << "<ACImage::saveACL> : problem writing video in ACL file, it needs to be opened before" << endl;
	}	
	library_file << filename << endl;
//	library_file << filename_thumbnail << endl;
//	library_file << duration << endl;
//	library_file << mid << endl;
	library_file << width << endl;
	library_file << height << endl;
	int n_features = features_vectors.size();
	library_file << n_features << endl;
	for (int i=0; i<n_features;i++) {
		int n_features_elements = features_vectors[i]->getSize();
		library_file << features_vectors[i]->getName() << endl;
		library_file << n_features_elements << endl;
		for (int j=0; j<n_features_elements; j++) {
			library_file << features_vectors[i]->getFeatureElement(j) << "\t";
		}
		library_file << endl;
	}
}

// C++ version
// loads from an existing (i.e. already opened) acl file
int ACImage::loadACL(ifstream &library_file) {
	if (! library_file.is_open()) {
		cerr << "<ACImage::loadACL> : problem loading image from ACL file, it needs to be opened before" << endl;
	}		
	if (!library_file.good()){
		cerr << "<ACImage::loadACL> : bad library file" << endl;
	}
	library_file >> filename ;
//	library_file >> filename_thumbnail;
//	library_file >> duration;
//	library_file >> mid;	
	library_file >> width;
	library_file >> height;
	int n_features = 0;
	library_file >> n_features; 
	
	ACMediaFeatures* mediaFeatures;
	float local_feature;
	int n_features_elements = 0;
	
	for (int i=0; i<n_features;i++) {
		mediaFeatures = new ACMediaFeatures();
		features_vectors.push_back(mediaFeatures);
		features_vectors[i]->setComputed();
		library_file >> n_features_elements;
		features_vectors[i]->resize(n_features_elements);
		for (int j=0; j<n_features_elements; j++) {
			library_file >> local_feature;
			features_vectors[i]->setFeatureElement(j, local_feature);
		}
	}
	// XS TODO check if errors and return 0/1
}


void ACImage::save(FILE* library_file) {
	int i, j;
//	int path_size; // XS removed
	int n_features;
	int n_features_elements;
//	float value;  // XS removed
	
//	path_size = filename.size(); // XS TODO CHECK THIS // strlen(filename) + 1; // XS TODO: all string
	//fprintf(library_file, "%d\n", path_size);
	fprintf(library_file, "%s\n", filename.c_str());
	//fprintf(library_file, "%s\n", thumbnail_filename);
	
#ifdef SAVE_LOOP_BIN
	fwrite(&width,sizeof(int),1,library_file);
	fwrite(&height,sizeof(int),1,library_file);
	n_features = features_vectors.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i].size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
			fwrite(&value,sizeof(float),1,library_file);
		}
	}
#else
	fprintf(library_file, "%d\n", width);
	fprintf(library_file, "%d\n", height);
	n_features = features_vectors.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize(); // XS TODO: ACMediaFeatures don't have a size method
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%f\t", features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
		}
		fprintf(library_file, "\n");
	}
#endif
	
	/*
	 fprintf(library_file, "%d\n", thumbnail_width);
	 fprintf(library_file, "%d\n", thumbnail_height);
	 fwrite((void*)thumbnail->imageData, 3, thumbnail_width*thumbnail_height, library_file);
	 */
	//fprintf(library_file, "\n");
}

int ACImage::load(FILE* library_file) { // was loadLoop
	int i, j;
	int path_size;
	int n_features;
	int n_features_elements;
	
	int ret;
	char *retc;
	
	ACMediaFeatures* mediaFeatures;
	float local_feature;
	
	char audio_file_temp[1024];
	memset(audio_file_temp,0,1024);
	
	retc = fgets(audio_file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(audio_file_temp);
		filename = string(audio_file_temp, path_size-1);
		
		/*	memset(audio_file_temp,0,1024);
		 retc = fgets(audio_file_temp, 1024, library_file);
		 path_size = strlen(audio_file_temp);
		 thumbnail_filename = new char[path_size];
		 strncpy(thumbnail_filename, audio_file_temp, path_size-1);
		 thumbnail_filename[path_size-1] = 0;*/
		
		ret = fscanf(library_file, "%d", &width);
		ret = fscanf(library_file, "%d", &height);
		ret = fscanf(library_file, "%d", &n_features);
		// SD TODO - following wont't work
		for (i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			ret = fscanf(library_file, "%d", &n_features_elements);
			features_vectors[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
		}
		ret = fscanf(library_file, "\n");
		if (computeThumbnail(filename, height, width) != 1){
			cerr << "<ACImage::load> : problem computing thumbnail" << endl;
		}

//		ret = fscanf(library_file, "%d", &thumbnail_width);
//		ret = fscanf(library_file, "%d\n", &thumbnail_height);
//		thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), 8, 3);
//		thumbnail->widthStep = thumbnail_width * 3;
//		thumbnail->origin = 1;
//		strcpy(thumbnail->channelSeq,"RGB"); //XS TODO : or BGR ??
//		ret = fread ((void*)thumbnail->imageData, 3, thumbnail_width*thumbnail_height, library_file);
		//fscanf(library_file, "\n");
		return 1;
	}
	else {
		return 0;
	}
}
