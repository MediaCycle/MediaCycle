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

#include <vector>

using std::cout;
using std::endl;
using std::cerr;

// note : this->mDescription will be used for mtf_file_name
ACVideoPixelSpeedPlugin::ACVideoPixelSpeedPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Video Pixel Speed";
    this->mDescription = "Pixel_Speed";
    this->mId = "";
	
	//other vars
	this->videoAn = NULL;
	this->mtf_file_name = "";
}

ACVideoPixelSpeedPlugin::~ACVideoPixelSpeedPlugin() {
	this->clean();
}

void ACVideoPixelSpeedPlugin::clean(){
	if (videoAn != NULL)
		delete videoAn;
}

std::vector<ACMediaFeatures*>  ACVideoPixelSpeedPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
	this->clean();
	videoAn = new ACVideoAnalysis(aFileName);
	return this->_calculate(aFileName,_save_timed_feat);
}

std::vector<ACMediaFeatures*> ACVideoPixelSpeedPlugin::calculate(ACMediaData* video_data, ACMedia* theMedia, bool _save_timed_feat) {
	this->clean();
	videoAn = new ACVideoAnalysis(video_data);
	return this->_calculate(video_data->getFileName(),_save_timed_feat);
}

std::vector<ACMediaFeatures*> ACVideoPixelSpeedPlugin::_calculate(std::string aFileName, bool _save_timed_feat){
	std::vector<ACMediaFeatures*> videoFeatures;
	videoAn->computeGlobalPixelSpeed();
	vector<float> t = videoAn->getGlobalTimeStamps();
	vector<float> s = videoAn->getGlobalPixelsSpeeds();
	
	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,s, "pixel speed");
	ACMediaFeatures* pixel_speed = ps_mtf->mean();
	if (_save_timed_feat) {
		// try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
		bool save_binary = true;
		string file_ext =  "_b.mtf";
		string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
		mtf_file_name = aFileName_noext + "_" +this->mDescription + file_ext;
		ps_mtf->saveInFile(mtf_file_name, save_binary);
	}
	
	delete ps_mtf;

	videoFeatures.push_back(pixel_speed);
	return videoFeatures;
}

// the plugin should know internally where it saved the mtf 
ACMediaTimedFeature* ACVideoPixelSpeedPlugin::getTimedFeatures(){
	if (mtf_file_name == ""){
        cout << "<ACVideoPixelSpeedPlugin::getTimedFeatures> : missing file name "<<endl;
		return NULL;
	}
	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
		return NULL;
	}
	return ps_mtf;
}

