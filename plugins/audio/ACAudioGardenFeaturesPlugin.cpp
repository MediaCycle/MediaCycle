/*
 *  ACAudioGardenFeaturesPlugin.cpp
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

#include "ACAudioGardenFeaturesPlugin.h"
#include "ACAudio.h"

#include <vector>
#include <string>

//class ACMedia;

ACAudioGardenFeaturesPlugin::ACAudioGardenFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "AudioGardenFeatures";
    this->mDescription = "AudioGarden plugin";
    this->mId = "";
}

ACAudioGardenFeaturesPlugin::~ACAudioGardenFeaturesPlugin() {
}


std::vector<ACMediaFeatures*> ACAudioGardenFeaturesPlugin::calculate(ACMediaData* audio_data, ACMedia* theMedia) {
	int mfccNbChannels = 16;
	int mfccNb = 13;
	int windowSize = 1024; 	
	bool extendSoundLimits = true;
	std::vector<ACMediaTimedFeatures*> descmf;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	descmf = computeFeatures(audio_data->getAudioData(), theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024, extendSoundLimits);

	// 	for (int i=0; i<descmf.size(); i++)
	// 		desc.push_back(descmf[i]->mean());

	int nrgIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Energy")
			nrgIdx = i;
	}
	std::cout << "nrgIdx = " << nrgIdx << std::endl;

	int edIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Effective Duration")
			edIdx = i;
	}
	std::cout << "edIdx = " << edIdx << std::endl;

	int loudIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Loudness")
			loudIdx = i;
	}
	std::cout << "loudIdx = " << loudIdx << std::endl;

	int scIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Spectral Centroid")
			scIdx = i;
	}
	std::cout << "scIdx = " << scIdx << std::endl;

	int zcrIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Zero Crossing Rate")
			zcrIdx = i;
	}
	std::cout << "zcrIdx = " << zcrIdx << std::endl;

	int mfccIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "MFCC")
			mfccIdx = i;
	}
	std::cout << "mfccIdx = " << mfccIdx << std::endl;

	int emaIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Energy Modulation Amplitude")
			emaIdx = i;
	}
	std::cout << "emaIdx = " << emaIdx << std::endl;

	
	float start_sec = descmf[edIdx]->getValue(0, 1);;
	float stop_sec =  descmf[edIdx]->getValue(0, 2);

	//	desc.push_back(descmf[loudIdx]->temporalModel(start_sec, stop_sec));
	//desc.push_back(descmf[edIdx]->mean());
	
	desc.push_back(descmf[edIdx]->mean());
	desc.push_back(descmf[mfccIdx]->mean());
	desc.push_back(descmf[emaIdx]->mean());
	return desc;
}

