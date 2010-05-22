/*
 *  ACImageColorMomentsPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 01/03/2010
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

#include "ACImageColorMomentsPlugin.h"

#include <vector>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

ACImageColorMomentsPlugin::ACImageColorMomentsPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Image Color Moments";
    this->mDescription = "Image Color Moments plugin";
    this->mId = "";
}

ACImageColorMomentsPlugin::~ACImageColorMomentsPlugin() {
}


std::vector<ACMediaFeatures*>  ACImageColorMomentsPlugin::calculate(std::string aFileName) {
	cout << "calculating Color Moments from histogram..." << endl;
	ACImageAnalysis* image = new ACImageAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allImageFeatures;

	ACMediaFeatures* imageColorFeatures = this->calculateColorFeatures(image);
	if (imageColorFeatures != NULL){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageColorMomentsPlugin::calculate> : NULL color feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageColorMomentsPlugin::calculate(ACMediaData* image_data) {
	cout << "calculating Color Moments from histogram..." << endl;
	IplImage* image_data_ptr = image_data->getImageData();
	ACImageAnalysis* image = new ACImageAnalysis(image_data_ptr);	
	std::vector<ACMediaFeatures*> allImageFeatures;
	
	ACMediaFeatures* imageColorFeatures = this->calculateColorFeatures(image);
	if (imageColorFeatures != NULL){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageColorMomentsPlugin::calculate> : NULL color feature" << endl;
	}
		
	delete image;
	// XS todo : this will destroy ACMediaData* and lead to a double free...
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageColorMomentsPlugin::calculate(ACMediaData* _aData, ACMedia* _theMedia){
	return this->calculate(_aData);
	// no need for ACMedia here...
};


ACMediaFeatures* ACImageColorMomentsPlugin::calculateColorFeatures(ACImageAnalysis* image){
	image->computeColorMoments(); // default n=4
	ACMediaFeatures* color_moments = new ACMediaFeatures(image->getColorMoments(), "Color");
	return color_moments;	
}
