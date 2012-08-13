/*
 *  ACVampDemoPlugin.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 08/06/10
 *  @copyright (c) 2010 – UMONS - Numediart
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

#include "ACVampDemoPlugin.h"
#include "vamp-plugin-interface.h"
#include "ACAudio.h"

#include <vector>
#include <string>

//class ACMedia;

ACVampDemoPlugin::ACVampDemoPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
   // this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Vamp";
    this->mDescription = "Vamp plugin";
    this->mId = "";
}

ACVampDemoPlugin::~ACVampDemoPlugin() {
}


std::vector<ACMediaFeatures*> ACVampDemoPlugin::calculate(ACMediaData* audio_data, ACMedia* theMedia, bool _save_timed_feat) {
	std::vector<ACMediaTimedFeature*> descmf;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	float* audioSamples = theAudio->getSamples();
	descmf.push_back(runPlugin("qm-vamp-plugins", "qm-chromagram", "0", 0, audioSamples, theAudio->getNFrames(), theAudio->getSampleRate(), theAudio->getChannels(), 0));
	//	descmf.push_back(runPlugin("qm-vamp-plugins", "qm-chromagram", "0", 0, theAudio->getFileName(), 0));
	delete [] audioSamples;

	for (int i=0; i<descmf.size(); i++)
		desc.push_back(descmf[i]->mean());
	return desc;
}

