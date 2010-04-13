/*
 *  ACImageTextureWaveletsMomentsPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 17/11/09
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

#include "ACImageTextureWaveletsMomentsPlugin.h"

#include <vector>
#include <string>

// to check if file exist
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
using std::cout;
using std::cerr;
using std::endl;

ACImageTextureWaveletsMomentsPlugin::ACImageTextureWaveletsMomentsPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Image Texture Moments";
    this->mDescription = "Image Texture Moments plugin";
    this->mId = "";
}

ACImageTextureWaveletsMomentsPlugin::~ACImageTextureWaveletsMomentsPlugin() {
}

std::vector<ACMediaFeatures*>  ACImageTextureWaveletsMomentsPlugin::calculate(std::string aFileName) {
	cout << "calculating Texture Moments from wavelets" << endl;
	ACImageAnalysis* image = new ACImageAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allImageFeatures;

	ACMediaFeatures* imageTextureFeatures = this->calculateHuMoments(image);
	if (imageTextureFeatures != NULL){
		allImageFeatures.push_back(imageTextureFeatures);
	}
	else{
		cerr << "<ACImageTextureWaveletsMomentsPlugin::calculate> : NULL texture feature" << endl;
	}
	
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageTextureWaveletsMomentsPlugin::calculate(ACMediaData* image_data) {
	cout << "calculating Texture Moments from wavelets" << endl;
	ACImageAnalysis* image = new ACImageAnalysis(image_data->getImageData());
	std::vector<ACMediaFeatures*> allImageFeatures;
	
	ACMediaFeatures* imageTextureFeatures = this->calculateHuMoments(image);
	if (imageTextureFeatures != NULL){
		allImageFeatures.push_back(imageTextureFeatures);
	}
	else{
		cerr << "<ACImageTextureWaveletsMomentsPlugin::calculate> : NULL texture feature" << endl;
	}
	
	delete image;
	// XS todo : this will destroy ACMediaData* and lead to a double free...
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageTextureWaveletsMomentsPlugin::calculate(ACMediaData* _aData, ACMedia* _theMedia){
	return this->calculate(_aData);
	// no need for ACMedia here...
};

ACMediaFeatures* ACImageTextureWaveletsMomentsPlugin::calculateHuMoments(ACImageAnalysis* image){
	image->computeGaborMoments();
	ACMediaFeatures* gabor_moments = new ACMediaFeatures(image->getGaborMoments(), "Texture");
	return gabor_moments;	
}

