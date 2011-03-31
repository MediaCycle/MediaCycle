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
    this->mName = "AudioFeatures";
    this->mDescription = "AudioFeatures plugin";
    this->mId = "";
	this->mPluginType = PLUGIN_TYPE_FEATURES; //mPluginType;
	this->mDescriptorsList.push_back("Spectral Centroid");
	this->mDescriptorsList.push_back("Spectral Spread");
	this->mDescriptorsList.push_back("Spectral Variation");
	this->mDescriptorsList.push_back("Spectral Flatness");
	this->mDescriptorsList.push_back("Spectral Flux");
	this->mDescriptorsList.push_back("Spectral Decrease");
	this->mDescriptorsList.push_back("MFCC");
	this->mDescriptorsList.push_back("DMFCC");
	this->mDescriptorsList.push_back("DDMFCC");
	this->mDescriptorsList.push_back("Zero Crossing Rate");
	this->mDescriptorsList.push_back("Energy");
	this->mDescriptorsList.push_back("Loudness");
	this->mDescriptorsList.push_back("Log Attack Time");
	this->mDescriptorsList.push_back("Energy Modulation Frequency");
	this->mDescriptorsList.push_back("Energy Modulation Amplitude");
	this->mtf_file_name = "";
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

// added by CF, temp
std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
	// from file
	std::cout << " ACAudioFeaturesPlugin::calculate: not implemented" << std::endl;
	return std::vector<ACMediaFeatures*>();
}

// CF
std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate(ACMediaData* audio_data, ACMedia* theMedia, bool _save_timed_feat) {
	// from MediaData
	return this->_calculate(audio_data->getFileName(),audio_data,theMedia,_save_timed_feat);
}

std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::_calculate(std::string aFileName, ACMediaData* audio_data, ACMedia* theMedia, bool _save_timed_feat){

	bool extendSoundLimits = true;
	std::vector<ACMediaTimedFeature*> descmf;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	
	float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];
	
	// SD replaced loop by more efficient memcpy
	memcpy(data, audio_data->getAudioData()+theAudio->getSampleStart()*theAudio->getChannels(),
		   (theAudio->getSampleEnd()-theAudio->getSampleStart())*theAudio->getChannels()*sizeof(float));
	/*
	long index = 0;
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

	// CF WARNING we save only the "Energy" feature that is expected for the AudioSegmentatinPlugin, until we can choose features associated to segmentation plugins
	if (_save_timed_feat) {
		// try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
		bool save_binary = true;
		string file_ext =  "_b.mtf";
		string aFileName_noext = theMedia->getFileName().substr(0,theMedia->getFileName().find_last_of('.'));
		mtf_file_name = aFileName_noext + "_" +this->mDescription + file_ext; // mName instead of mDescription due its the space char, just in case?
		// CF we're saving the Energy feature as it is used for method 0 in ACAudioSegmentationPlugin...
		descmf[10]->saveInFile(mtf_file_name, save_binary);//CF just the nth MediaTimeFeature [0] saved!
	}

	for (int i=0; i<descmf.size(); i++){
		delete descmf[i];
	}
	descmf.clear();
	delete [] data;
	return desc;
}


// the plugin should know internally where it saved the mtf
ACMediaTimedFeature* ACAudioFeaturesPlugin::getTimedFeatures(){
	if (mtf_file_name == ""){
        cout << "<ACAudioFeaturesPlugin::getTimedFeatures> : missing file name "<<endl;
		return 0;
	}
	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
		return 0;
	}
	return ps_mtf;
}


