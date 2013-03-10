/*
 *  ACVideoPixelSpeedPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 22/09/09
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

//uses ACVideoAnalysis and converts the results into ACMediaFeatures

#include "ACVideoPixelSpeedPlugin.h"
#include "ACMedia.h"

#include<iostream>
using namespace std;

// note : this->mDescription will be used for mtf_file_name
ACVideoPixelSpeedPlugin::ACVideoPixelSpeedPlugin() : ACTimedFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mName = "Video Pixel Speed";
    this->mDescription = "Pixel_Speed";
    this->mId = "";

    //other vars
    this->videoAn = 0;
}

ACVideoPixelSpeedPlugin::~ACVideoPixelSpeedPlugin() {
    this->clean();
}

void ACVideoPixelSpeedPlugin::clean() {
    if (videoAn != 0)
        delete videoAn;
}

ACFeatureDimensions ACVideoPixelSpeedPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Pixel Speed"] = 1;
    return featureDimensions;
}

//std::vector<ACMediaFeatures*>  ACVideoPixelSpeedPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
//	this->clean();
//	videoAn = new ACVideoAnalysis(aFileName);
//	return this->_calculate(aFileName,_save_timed_feat);
//}

std::vector<ACMediaFeatures*> ACVideoPixelSpeedPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
	this->clean();
	std::vector<ACMediaFeatures*> videoFeatures;
#ifdef USE_DEBUG
        cout << "[ACVideoPixelSpeedPlugin::calculate] analysing from frame " << theMedia->getStartInt() << " to " << theMedia->getEndInt() << endl;
#endif //USE_DEBUG
        videoAn = new ACVideoAnalysis(theMedia, theMedia->getStartInt(), theMedia->getEndInt());
	videoAn->computeGlobalPixelsSpeed();
	vector<float> t = videoAn->getTimeStamps();
	vector<float> s = videoAn->getGlobalPixelsSpeeds();
    string aFileName= theMedia->getFileName();
	
    ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,s, "Pixel Speed");
	ACMediaFeatures* pixel_speed = ps_mtf->mean();
	//this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat); // by default : binary
	bool _binary=false;
	theMedia->addTimedFileNames(this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat,_binary)); // by default : binary
	
	delete ps_mtf;

	videoFeatures.push_back(pixel_speed);
	return videoFeatures;
}
