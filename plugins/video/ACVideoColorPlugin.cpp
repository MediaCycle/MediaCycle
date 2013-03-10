/*
 *  ACVideoColorPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/04/11
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

#include "ACVideoColorPlugin.h"
#include "ACMedia.h"
#include<iostream>
using namespace std;

// note : this->mDescription will be used for mtf_file_name
ACVideoColorPlugin::ACVideoColorPlugin() : ACTimedFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mName = "Video Color";
    this->mDescription = "Color";
    this->mId = "";
    this->nMoments = 4;

    //other vars
    this->videoAn = 0;
}

ACVideoColorPlugin::~ACVideoColorPlugin() {
    this->clean();
}

ACFeatureDimensions ACVideoColorPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Color Moments"] = this->nMoments*3;
    return featureDimensions;
}

void ACVideoColorPlugin::clean() {
    if (videoAn != 0)
        delete videoAn;
}

// computes first 4 moments in HSV color space
std::vector<ACMediaFeatures*> ACVideoColorPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
	this->clean();
	std::vector<ACMediaFeatures*> videoFeatures;
#ifdef USE_DEBUG
        cout << "[ACVideoColorsPlugin::calculate] analysing from frame " << theMedia->getStartInt() << " to " << theMedia->getEndInt() << endl;
#endif //USE_DEBUG
        videoAn = new ACVideoAnalysis(theMedia, theMedia->getStartInt(), theMedia->getEndInt());
        //videoAn = new ACVideoAnalysis(video_data, theMedia->getStartInt(), theMedia->getEndInt());
    videoAn->computeColorMoments(this->nMoments, "HSV");
	vector<float> t = videoAn->getTimeStamps();
	std::vector<std::vector<float> > s = videoAn->getColorMoments();
    string aFileName= theMedia->getFileName();
    ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,s, "Color Moments");
	ACMediaFeatures* mean_color_moments = ps_mtf->mean();
	bool _binary=false;
	theMedia->addTimedFileNames(this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat,_binary)); // by default : binary
	
    //    this->saveTimedFeatures(ps_mtf, aFileName, _save_timed_feat); // by default : binary
	delete ps_mtf;
	
	videoFeatures.push_back(mean_color_moments);
	return videoFeatures;
}
