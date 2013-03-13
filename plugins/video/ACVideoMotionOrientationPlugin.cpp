/*
 *  ACVideoMotionOrientationPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 15/07/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACVideoMotionOrientationPlugin.h"
#include "ACMedia.h"
#if CV_MIN_VERSION_REQUIRED(2,3,0)

#include<iostream>
using namespace std;

// note : this->mDescription will be used for mtf_file_name
ACVideoMotionOrientationPlugin::ACVideoMotionOrientationPlugin() : ACTimedFeaturesPlugin() {
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mName = "Video Motion Orientation";
    this->mDescription = "Motion Orientation";
    this->mId = "";
    this->videoAn = 0;
}

ACVideoMotionOrientationPlugin::~ACVideoMotionOrientationPlugin() {
	this->clean();
}

void ACVideoMotionOrientationPlugin::clean(){
	if (videoAn != 0)
		delete videoAn;
}

ACFeatureDimensions ACVideoMotionOrientationPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Motion Orientation: Mean"] = 1;
    return featureDimensions;
}

// computes global orientation
// watch out : you can't just do mean() on angles ! (0+360)/2 would give 180
std::vector<ACMediaFeatures*> ACVideoMotionOrientationPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
	this->clean();
	std::vector<ACMediaFeatures*> videoFeatures;
#ifdef USE_DEBUG
        cout << "[ACVideoMotionOrientationPlugin::calculate] analysing from frame " << theMedia->getStartInt() << " to " << theMedia->getEndInt() << endl;
#endif //USE_DEBUG
        videoAn = new ACVideoAnalysis(theMedia, theMedia->getStartInt(), theMedia->getEndInt());
	videoAn->computeGlobalOrientation();
	vector<float> t = videoAn->getTimeStamps();
	std::vector<float> angles = videoAn->getGlobalOrientations();
    ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,angles, "Motion Orientation");

	//XS TODO: this could be angular_mean in ACMediaTimedFeatures
	// special mean for angles !
	float average_angle=0;
	float xx=0, yy=0;
	for (unsigned int i=0; i<angles.size(); i++){
		float angle = angles[i]*CV_PI/180;
		xx+=cos(angle);
		yy+=sin(angle);
	}
	if (!(xx==0 && yy==0)) // atan2(0,0) undefined, global_orientation set to 0 in this case
		average_angle=atan2(yy,xx)*180/CV_PI; // in degrees
	
	ACMediaFeatures* mean_mf = new ACMediaFeatures();  
    mean_mf->setName("Global Orientations");
	mean_mf=ps_mtf->mean();
	videoFeatures.push_back(mean_mf);

    string aFileName= theMedia->getFileName();
     //   this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat); // by default : binary
	bool _binary=false;
	theMedia->addTimedFileNames(this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat,_binary)); // by default : binary
	
	delete ps_mtf;
		
	return videoFeatures;
	
}
#endif //CV_MIN_VERSION_REQUIRED(2,3,0)
