/*
 *  ACAudioFeaturesPlugin.cpp
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

#include "ACAudioFeaturesPlugin.h"
#include "ACAudio.h"

#include <vector>
#include <string>

//class ACMedia;

ACAudioFeaturesPlugin::ACAudioFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "AudioFeatures";
    this->mDescription = "AudioFeatures plugin";
    this->mId = "";
}

ACAudioFeaturesPlugin::~ACAudioFeaturesPlugin() {
}


std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate(ACMediaData* audio_data, ACMedia* theMedia) {
	int mfccNbChannels = 16;
	int mfccNb = 13;
	int windowSize = 512; 	
	bool extendSoundLimits = true;
	std::vector<ACMediaTimedFeature*> descmf;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	
	float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];
	long index = 0;
	
	for (long i = theAudio->getSampleStart(); i< theAudio->getSampleEnd(); i++){
		for (long j = 0; j < theAudio->getChannels(); j++){
			data[index] = audio_data->getAudioData()[i*theAudio->getChannels()+j];
			index++;
		}
	}

// 	ofstream output("signal1.txt");
// 	for(int i=0; i < (long) theAudio->getNFrames() * theAudio->getChannels(); i++){
// 		output<<data[i]<<endl;
// 	}

	descmf = computeFeatures(data, theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(),16, 13, 1024, extendSoundLimits);
	
	for (int i=0; i<descmf.size(); i++)
		desc.push_back(descmf[i]->mean());
	
	int nrgIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Energy")
			nrgIdx = i;
	}
	std::cout << "nrgIdx = " << nrgIdx << std::endl;
	
	desc.push_back(descmf[nrgIdx]->interpN(10)->toMediaFeatures());

	for (int i=0; i<descmf.size(); i++){
		delete descmf[i];
	}
	descmf.clear();
	delete [] data;
	return desc;
}

