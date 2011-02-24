/*
 *  ACImageShapeFourierPolarMomentsPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 24/08/10
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

#include "ACImageShapeFourierPolarMomentsPlugin.h"

#include <vector>
#include <string>

// to check if file exist
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
using std::cout;
using std::cerr;
using std::endl;

ACImageShapeFourierPolarMomentsPlugin::ACImageShapeFourierPolarMomentsPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Shape FP Moments";
    this->mDescription = "Image Shape Fourier Polar Moments plugin";
    this->mId = "";
	this->mDescriptorsList.push_back("Shape Fourier Polar Moments");

}

ACImageShapeFourierPolarMomentsPlugin::~ACImageShapeFourierPolarMomentsPlugin() {
}

std::vector<ACMediaFeatures*>  ACImageShapeFourierPolarMomentsPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
	cout << "calculating Shape (FourierPolar) Moments from file name..." << endl;
	ACColorImageAnalysis* image = new ACColorImageAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allImageFeatures;

	ACMediaFeatures* imageShapeFeatures = this->calculateFourierPolarMoments(image);
	if (imageShapeFeatures != 0){
		allImageFeatures.push_back(imageShapeFeatures);
	}
	else{
		cerr << "<ACImageShapeFourierPolarMomentsPlugin::calculate> : no shape feature" << endl;
	}
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageShapeFourierPolarMomentsPlugin::calculate(ACMediaData* image_data) {
	cout << "calculating Shape (FourierPolar) Moments from ACMediaData..." << endl;
	ACColorImageAnalysis* image = new ACColorImageAnalysis(image_data->getImageData());
	std::vector<ACMediaFeatures*> allImageFeatures;
	
	ACMediaFeatures* imageShapeFeatures = this->calculateFourierPolarMoments(image);
	if (imageShapeFeatures != 0){
		allImageFeatures.push_back(imageShapeFeatures);
	}
	else{
		cerr << "<ACImageShapeFourierPolarMomentsPlugin::calculate> : no shape feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageShapeFourierPolarMomentsPlugin::calculate(ACMediaData* _aData, ACMedia* _theMedia, bool _save_timed_feat){
	return this->calculate(_aData);
	// no need for ACMedia here...
};

ACMediaFeatures* ACImageShapeFourierPolarMomentsPlugin::calculateFourierPolarMoments(ACImageAnalysis* image){
	image->computeFourierPolarMoments();
	ACMediaFeatures* shape_moments = new ACMediaFeatures(image->getFourierPolarMoments(), "Shape");
	return shape_moments;	
}


