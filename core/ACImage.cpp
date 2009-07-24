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
//#include "ACAnalysedImage.h"
//#include "ACImageFeaturesFactory.h"

using namespace std;
//------------------------------------------------------------------

ACImage::ACImage() : ACMedia() {
	_type = IMAGE;
	
	// XS I defined a resize method for ACMediaFeatures, but it is necessary ?
	features.resize(0); //FEATURES_NUM_EL);
//	features[0].resize(IMAGE_LOOP_COLOR_NUM_EL);
//	features[1].resize(IMAGE_LOOP_SHAPE_NUM_EL);
//	features[2].resize(IMAGE_LOOP_TEXTURE_NUM_EL);	
}

ACImage::~ACImage() {
}

void ACImage::save(FILE* library_file) { // was saveloop
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
	n_features = features.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features.size();i++) {
		n_features_elements = features[i].size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features[i]->getFeature(j)); // XS instead of [i][j]
			fwrite(&value,sizeof(float),1,library_file);
		}
	}
#else
	fprintf(library_file, "%d\n", width);
	fprintf(library_file, "%d\n", height);
	n_features = features.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features.size();i++) {
		n_features_elements = features[i]->size(); // XS TODO: ACMediaFeatures don't have a size method
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%f\t", features[i]->getFeature(j)); // XS instead of [i][j]
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
	
	char audio_file_temp[1024];
	memset(audio_file_temp,0,1024);
	
	retc = fgets(audio_file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(audio_file_temp);
		filename = new char[path_size];
//		strncpy(filename, audio_file_temp, path_size-1); // XS TODO all string
		filename[path_size-1] = 0;
		
		/*	memset(audio_file_temp,0,1024);
		 retc = fgets(audio_file_temp, 1024, library_file);
		 path_size = strlen(audio_file_temp);
		 thumbnail_filename = new char[path_size];
		 strncpy(thumbnail_filename, audio_file_temp, path_size-1);
		 thumbnail_filename[path_size-1] = 0;*/
		
		ret = fscanf(library_file, "%d", &width);
		ret = fscanf(library_file, "%d", &height);
		ret = fscanf(library_file, "%d", &n_features);
		features.resize(n_features);
		// SD TODO - following wont't work
		for (i=0; i<n_features;i++) {
			ret = fscanf(library_file, "%d", &n_features_elements);
			features[i]->resize(n_features_elements); // XS TODO: define resize for ACMediaFeatures
			for (j=0; j<n_features_elements; j++) {
                            float tmp;
                            //ret = fscanf(library_file, "%f", &(features[i][j]));
                            ret = fscanf(library_file, "%f", &tmp);
                            features[i]->setFeature(j,tmp);
			}
		}
		ret = fscanf(library_file, "\n");
		ret = fscanf(library_file, "%d", &thumbnail_width);
		ret = fscanf(library_file, "%d\n", &thumbnail_height);
		thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), 8, 3);
		thumbnail->widthStep = thumbnail_width * 3;
		thumbnail->origin = 1;
		strcpy(thumbnail->channelSeq,"RGB"); //XS TODO : or BGR ??
		ret = fread ((void*)thumbnail->imageData, 3, thumbnail_width*thumbnail_height, library_file);
		//fscanf(library_file, "\n");
		return 1;
	}
	else {
		return 0;
	}
}

void ACImage::import(string _path) { // was importFile
	
	cout << "importing..." << _path << endl;
	filename=_path;

/*	ACAnalysedImage* full_image = new ACAnalysedImage(_path);
	
	// TODO check if image has been imported properly ..
	ACImageFeaturesFactory* factory = new ACImageFeaturesFactory(full_image);

	// XS do we need a new factory here ?
	// XS is this clean enough ?
	//features.push_back(factory->calculateFeature("Gabor"));
	features.push_back(factory->calculateFeature("Shape")); // XS TODO fichier
	//features.push_back(factory->calculateFeature("Color"));
	
	// SD TODO - following have to be called because full image will be deleted
//	setThumbnail(full_image->getThumbnail());
//	setHeight(full_image->getHeight());
//	setWidth(full_image->getWidth());
	
	delete factory;
	delete full_image;
     */
}