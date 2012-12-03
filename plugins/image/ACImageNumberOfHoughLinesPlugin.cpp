/*
 *  ACImageNumberOfHoughLinesPlugin.cpp
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

#include "ACImageNumberOfHoughLinesPlugin.h"

#include <vector>
#include <string>
#include <iostream>
using namespace std;

ACImageNumberOfHoughLinesPlugin::ACImageNumberOfHoughLinesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_IMAGE;
    this->mName = "Number of Hough lines";
    this->mDescription = "Image Number of Hough lines plugin";
    this->mId = "";
	this->mDescriptorsList.push_back("NumberOfHoughLines");
}

ACImageNumberOfHoughLinesPlugin::~ACImageNumberOfHoughLinesPlugin() {
}

std::vector<ACMediaFeatures*> ACImageNumberOfHoughLinesPlugin::calculate(ACMediaData* image_data) {
	cout << "calculating Number of Hough lines from image..." << endl;
	std::vector<ACMediaFeatures*> allImageFeatures;

	ACColorImageAnalysis* image = new ACColorImageAnalysis(image_data);	
	
	// probabilistic computeHoughLinesP is better than computeHoughLines
	ACMediaFeatures* imageColorFeatures = this->calculateNumberOfHoughLinesP(image);
	if (imageColorFeatures != 0){
		allImageFeatures.push_back(imageColorFeatures);
	}
	else{
		cerr << "<ACImageNumberOfHoughLinesPlugin::calculate> : no color feature" << endl;
	}
		
	delete image;
	return allImageFeatures;
}

std::vector<ACMediaFeatures*> ACImageNumberOfHoughLinesPlugin::calculate(ACMediaData* _aData, ACMedia* _theMedia, bool _save_timed_feat){
	return this->calculate(_aData);
	// XS TODO no need for ACMedia here...
}

ACMediaFeatures* ACImageNumberOfHoughLinesPlugin::calculateNumberOfHoughLinesP(ACColorImageAnalysis* image){ 
	image->computeHoughLinesP(); 
	ACMediaFeatures* number_of_hough_linesP = new ACMediaFeatures(image->getNumberOfHoughLinesP(), "NumberOfHoughLinesP");
	return number_of_hough_linesP;
}

ACMediaFeatures* ACImageNumberOfHoughLinesPlugin::calculateNumberOfHoughLines(ACColorImageAnalysis* image){ 
	image->computeHoughLines();
	ACMediaFeatures* number_of_hough_lines = new ACMediaFeatures(image->getNumberOfHoughLines(), "NumberOfHoughLines");
	return number_of_hough_lines;
}
