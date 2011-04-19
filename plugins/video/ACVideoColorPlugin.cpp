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

#include<iostream>
using namespace std;

// note : this->mDescription will be used for mtf_file_name
ACVideoColorPlugin::ACVideoColorPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    //this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Video Color";
    this->mDescription = "Color";
    this->mId = "";
	this->mDescriptorsList.push_back("Color");
	
	//other vars
	this->videoAn = 0;
//	this->mtf_file_name = "";
}

ACVideoColorPlugin::~ACVideoColorPlugin() {
	this->clean();
}

void ACVideoColorPlugin::clean(){
	if (videoAn != 0)
		delete videoAn;
}

std::vector<ACMediaFeatures*>  ACVideoColorPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
}

std::vector<ACMediaFeatures*> ACVideoColorPlugin::calculate(ACMediaData* video_data, ACMedia* theMedia, bool _save_timed_feat) {
}

std::vector<ACMediaFeatures*> ACVideoColorPlugin::_calculate(std::string aFileName, bool _save_timed_feat){
}
