/* 
 * File:   ACVideoOpticalFlowPlugin.cpp
 * Author: xavier
 * 
 * @date November 30, 2011
 * @copyright (c) 2011 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#include "ACVideoOpticalFlowPlugin.h"
#include "ACMedia.h"
#include<iostream>
using namespace std;

ACVideoOpticalFlowPlugin::ACVideoOpticalFlowPlugin() : ACTimedFeaturesPlugin() {
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mName = "Video OpticalFlow";
    this->mDescription = "OpticalFlow";
    this->mId = "";
    this->mDescriptorsList.push_back("OpticalFlow");
    this->videoAn = 0;
}

ACVideoOpticalFlowPlugin::~ACVideoOpticalFlowPlugin() {
	this->clean();
}

void ACVideoOpticalFlowPlugin::clean(){
	if (videoAn != 0)
		delete videoAn;
}

std::vector<ACMediaFeatures*> ACVideoOpticalFlowPlugin::calculate(ACMediaData* video_data, ACMedia* theMedia, bool _save_timed_feat) {
	this->clean();
	std::vector<ACMediaFeatures*> videoFeatures;
#ifdef USE_DEBUG
        cout << "[ACVideoOpticalFlowPlugin::calculate] analysing from frame " << theMedia->getStartInt() << " to " << theMedia->getEndInt() << endl;
#endif //USE_DEBUG
        videoAn = new ACVideoAnalysis(video_data, theMedia->getStartInt(), theMedia->getEndInt());
	videoAn->computeOpticalFlow();
	vector<float> t = videoAn->getTimeStamps();
	std::vector<std::vector<float> > s = videoAn->getOpticalFlow();
	string aFileName= video_data->getFileName();

	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,s, "optical flow");
	ACMediaFeatures* optical_flow = ps_mtf->mean();
	this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat); // by default : binary
	delete ps_mtf;

	videoFeatures.push_back(optical_flow);
	return videoFeatures;
}