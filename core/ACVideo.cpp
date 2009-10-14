/*
 *  ACVideo.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 25/05/09
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

#include "ACVideo.h"
#include <string>

using namespace std;

//#include "ACAnalysedVideo.h"
//#include "ACVideoFeaturesFactory.h"

ACVideo::ACVideo() : ACMedia() {
	_type = MEDIA_TYPE_VIDEO;
	features.resize(0);
	thumbnail = 0;
	width = 0;
	height = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
}	

ACVideo::~ACVideo() {
}

//void ACVideo::import(string _path) { 
// XS 23/09/09: now done in ACMedia.
//	if we need to add something here, call first ACMedia::import
//}

void ACVideo::save(FILE* library_file) { // was saveloop
	int i, j;
	int n_features;
	int n_features_elements;

	fprintf(library_file, "%s\n", filename.c_str());
	fprintf(library_file, "%s\n", filename_thumbnail.c_str());
	
#ifdef SAVE_LOOP_BIN
	fwrite(&mid,sizeof(int),1,library_file);
	fwrite(&width,sizeof(int),1,library_file);
	fwrite(&height,sizeof(int),1,library_file);
	n_features = features.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features.size();i++) {
		n_features_elements = features[i]->size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features[i]->getFeature(j)); // XS instead of [i][j]
			fwrite(&value,sizeof(float),1,library_file);
		}
	}
#else
	fprintf(library_file, "%d\n", mid);
	fprintf(library_file, "%d\n", width);
	fprintf(library_file, "%d\n", height);
	n_features = features.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features.size();i++) {
		n_features_elements = features[i]->size();
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%f\t", features[i]->getFeature(j)); // XS instead of [i][j]
		}
		fprintf(library_file, "\n");
	}
#endif
}

int ACVideo::load(FILE* library_file) { // was loadLoop
	int i, j;
	int path_size;
	int n_features;
	int n_features_elements;
	
	int ret;
	char *retc;
	
	ACMediaFeatures* mediaFeatures;
	float local_feature;
	
	char *file_temp;
	file_temp = new char[1024];
	memset(file_temp,0,1024);
	char *file_temp2;
	file_temp2 = new char[1024];
	memset(file_temp2,0,1024);
	
/*	char file_temp[1024];
	memset(file_temp,0,1024);
	char file_temp2[1024];
	memset(file_temp2,0,1024);
*/
	
	retc = fgets(file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(file_temp);
		filename = string(file_temp, path_size-1);
		
		retc = fgets(file_temp2, 1024, library_file);
		path_size = strlen(file_temp2);
		filename_thumbnail = string(file_temp2, path_size-1);
		
		ret = fscanf(library_file, "%d", &mid);
		ret = fscanf(library_file, "%d", &width);
		ret = fscanf(library_file, "%d", &height);
		ret = fscanf(library_file, "%d", &n_features);
		
		thumbnail_height = height;
		thumbnail_width = width;
		// SD TODO - following wont't work
		for (i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features.push_back(mediaFeatures);
			features[i]->setComputed();
			ret = fscanf(library_file, "%d", &n_features_elements);
			features[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features[i]->setFeature(j, local_feature);
			}
		}
		ret = fscanf(library_file, "\n");
		delete file_temp;
		delete file_temp2;
		return 1;
	}
	else {
		delete file_temp;
		delete file_temp2;
		return 0;
	}
}

// XS TODO: use C++ strings and streams instead of C-functions=======
