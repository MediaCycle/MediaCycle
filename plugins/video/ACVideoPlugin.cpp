/*
 *  ACVideoPlugin.cpp
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

#include "ACVideoPlugin.h"

#include <vector>
#include <string>

ACVideoPlugin::ACVideoPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Video";
    this->mDescription = "Video plugin";
    this->mId = "";
	
    //local vars
}

ACVideoPlugin::~ACVideoPlugin() {
}

// the following two HAVE to be defined to make the class non-abstract
int ACVideoPlugin::initialize(){
}

ACMediaFeatures* ACVideoPlugin::calculate(){
}



// uses ACVideoAnalysis and converts the results into ACMediaFeatures
ACMediaFeatures* ACVideoPlugin::calculate(std::string aFileName) {
	// TODO: test if file exists
	ACVideoAnalysis* video = new ACVideoAnalysis(aFileName);
	
//	ACMediaFeatures* mMediaFeatures = new ACMediaFeatures();
//  check how to store the features...
	
	//XS TODO: make this more modular
	this->calculateTrajectory(video);
	delete video;

}

void ACVideoPlugin::calculateTrajectory(ACVideoAnalysis* video){
	video->computeMergedBlobsTrajectory(0);
	std::vector<blob_center> trajectory = video->getMergedBlobsTrajectory();
	std::vector<float> time_stamps = video->getDummyTimeStamps();
	ACMediaTimedFeatures *mediaTimedFeatures = new ACMediaTimedFeatures(time_stamps, trajectory, "trajectory");

}
