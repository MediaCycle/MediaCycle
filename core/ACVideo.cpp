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
}	

ACVideo::~ACVideo() {
}

void ACVideo::import(string _path) { 
	cout << "importing..." << _path << endl;
	filename=_path;
/*	ACAnalysedVideo* full_video = new ACAnalysedVideo(_path);
	ACVideoFeaturesFactory* factory = new ACVideoFeaturesFactory(full_video);
	features.push_back(factory->calculateFeature("Speed")); // XS TODO fichier

	delete factory;
	delete full_video;
*/
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
	
	char file_temp[1024];
	memset(file_temp,0,1024);
	
	retc = fgets(file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(file_temp);
		filename = string(file_temp, path_size-1);
		
		retc = fgets(file_temp, 1024, library_file);
		path_size = strlen(file_temp);
		filename_thumbnail = string(file_temp, path_size-1);
		
		ret = fscanf(library_file, "%d", &mid);
		ret = fscanf(library_file, "%d", &width);
		ret = fscanf(library_file, "%d", &height);
		ret = fscanf(library_file, "%d", &n_features);
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
		
		return 1;
	}
	else {
		return 0;
	}
}


// TODO : complete this (Load/Save)