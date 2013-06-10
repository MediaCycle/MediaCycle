/*
 *  ACImageNumberOfFacesPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 
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

#include "ACImageNumberOfFacesPlugin.h"
#include "MediaCycle.h"

#include <vector>
#include <string>

//#include <boost/filesystem/operations.hpp>
//namespace fs = boost::filesystem;

using namespace std;

ACImageNumberOfFacesPlugin::ACImageNumberOfFacesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
    this->mName = "Number of faces";
    this->mDescription = "Image Number of faces plugin";
    this->mId = "";
}

ACImageNumberOfFacesPlugin::~ACImageNumberOfFacesPlugin() {
}

ACFeatureDimensions ACImageNumberOfFacesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Number of Faces"] = 1;
    return featureDimensions;
}

std::vector<ACMediaFeatures*> ACImageNumberOfFacesPlugin::calculate(ACMedia* _theMedia, bool _save_timed_feat){
	cout << "calculating Number of faces from image..." << endl;
	std::vector<ACMediaFeatures*> allImageFeatures;

    ACColorImageAnalysis* image = new ACColorImageAnalysis(_theMedia->getFileName());
	
	ACMediaFeatures* imageColorFeatures = this->calculateNumberOfFaces(image);
	if (imageColorFeatures != 0){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageNumberOfFacesPlugin::calculate> : no color feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

ACMediaFeatures* ACImageNumberOfFacesPlugin::calculateNumberOfFaces(ACColorImageAnalysis* image){
    std::string haar_cascade_file(haar_cascade);

    #ifdef __MINGW32__
    string slash = "\\";
    if(!media_cycle){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: mediacycle instance not set, will try reload the dataflow later" << std::endl;
        return 0;
    }
    std::string _path = media_cycle->getLibraryPathFromPlugin(this->mName);
    if(_path==""){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: plugin path not available" << std::endl;
        return 0;
    }
    
    boost::filesystem::path haar_cascade_path ( _path + slash + haar_cascade);
    haar_cascade_file = haar_cascade_path.string();
    #endif
    
    image->computeNumberOfFaces(haar_cascade_file);
    ACMediaFeatures* number_of_faces = new ACMediaFeatures(image->getNumberOfFaces(), "Number of Faces");
	return number_of_faces;	
}
