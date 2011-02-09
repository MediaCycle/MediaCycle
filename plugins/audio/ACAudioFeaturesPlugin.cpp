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

/*
static starpu_codelet cl = {
	.where = STARPU_CPU,
	.cpu_func = cpu_func,
	.nbuffers = 2
}

static void cpu_func(void _*descr[], void *cl_args) {
	(ACAudioFeaturesPlugin*)cl_args.plugin->calculate(audio_data, theMedia, returndescriptor);
}

std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate_starpu(vector<ACMediaData*> audio_data, vector<ACMedia*> theMedia) {
	// create job list
	for () {
	}
 
	// return vector of decriptors
 
	// check that calculate is thread safe
}
*/

std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate(ACMediaData* audio_data, ACMedia* theMedia, bool _save_timed_feat) {

	bool extendSoundLimits = true;
	std::vector<ACMediaTimedFeature*> descmf;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	
	float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];
//	long index = 0;
	
	// SD replaced loop by more efficient memcpy
	memcpy(data, audio_data->getAudioData()+theAudio->getSampleStart()*theAudio->getChannels(),
		   (theAudio->getSampleEnd()-theAudio->getSampleStart())*theAudio->getChannels()*sizeof(float));
	/*
	for (long i = theAudio->getSampleStart(); i< theAudio->getSampleEnd(); i++){
		for (long j = 0; j < theAudio->getChannels(); j++){
			data[index] = audio_data->getAudioData()[i*theAudio->getChannels()+j];
			index++;
		}
	}
	 */
	
// 	ofstream output("signal1.txt");
// 	for(int i=0; i < (long) theAudio->getNFrames() * theAudio->getChannels(); i++){
// 		output<<data[i]<<endl;
// 	}
	
	descmf = computeFeatures(data, theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(),32, 13, 1024, extendSoundLimits);
	
	
	int nrgIdx = 0;
	for (int i=0; i<descmf.size(); i++){
		if (descmf[i]->getName() == "Energy")
			nrgIdx = i;
	}
	std::cout << "nrgIdx = " << nrgIdx << std::endl;
	
	for (int i=0; i<descmf.size(); i++){
		desc.push_back(descmf[i]->mean());
		if (i==nrgIdx){
			desc[i]->setNeedsNormalization(0);
		}
	}
	
	desc.push_back(descmf[nrgIdx]->interpN(10)->toMediaFeatures());
	
	for (int i=0; i<descmf.size(); i++){
		delete descmf[i];
	}
	descmf.clear();
	delete [] data;
	return desc;
}

