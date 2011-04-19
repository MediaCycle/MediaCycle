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

using namespace std;

ACImageColorMomentsPlugin::ACImageColorMomentsPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
  //  this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Color Moments";
    this->mDescription = "Image Color Moments plugin";
    this->mId = "";
	this->mDescriptorsList.push_back("Color Moments");

}

ACImageColorMomentsPlugin::~ACImageColorMomentsPlugin() {
}


std::vector<ACMediaFeatures*>  ACImageColorMomentsPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
	cout << "calculating Color Moments from histogram..." << endl;
	ACColorImageAnalysis* image = new ACColorImageAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allImageFeatures;

	ACMediaFeatures* imageColorFeatures = this->calculateColorFeatures(image);
	if (imageColorFeatures != 0){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageColorMomentsPlugin::calculate> : no color feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageColorMomentsPlugin::calculate(ACMediaData* image_data) {
	cout << "calculating Color Moments from histogram..." << endl;
	std::vector<ACMediaFeatures*> allImageFeatures;
//	ACImageData* local_image_data ;
//	try{
//		local_image_data = static_cast <ACImageData*> (image_data);
//		if(! local_image_data) 
//			throw runtime_error("<ACImageColorMomentsPlugin::calculate> problem with mediaData cast");
//	}catch (const exception& e) {
//		cerr << e.what() << endl;
//		return allImageFeatures;
//	}
//	
//	IplImage* image_data_ptr = local_image_data->getData();

	// XS TODO: which color model ?
	ACColorImageAnalysis* image = new ACColorImageAnalysis(image_data);	
	
	ACMediaFeatures* imageColorFeatures = this->calculateColorFeatures(image);
	if (imageColorFeatures != 0){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageColorMomentsPlugin::calculate> : no color feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageColorMomentsPlugin::calculate(ACMediaData* _aData, ACMedia* _theMedia, bool _save_timed_feat){
	return this->calculate(_aData);
	// no need for ACMedia here...
};


ACMediaFeatures* ACImageColorMomentsPlugin::calculateColorFeatures(ACColorImageAnalysis* image){
	image->computeColorMoments(); // default n=4
	ACMediaFeatures* color_moments = new ACMediaFeatures(image->getColorMoments(), "Color");
	return color_moments;	
}
