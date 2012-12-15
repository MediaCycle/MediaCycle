/**
 * @brief ACAudioHaitsmaFingerprintPlugin.cpp
 * @author Thierry Ravet
 * @date 15/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACAudioHaitsmaFingerprintPlugin.h"
#include "ACAudio.h"

#include <vector>
#include <string>

//class ACMedia;

ACAudioHaitsmaFingerprintPlugin::ACAudioHaitsmaFingerprintPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Haitsma Fingerprint";
    this->mDescription = "Audio Haitsma Fingerprint plugin";
    this->mId = "";
	//this->mPluginType=mPluginType;
	this->mDescriptorsList.push_back("Haitsma Audio Sub-Fingerprint");
	
	audio_fingerprint = new ACAudioFingerprint();
	audio_fingerprint->setParameters();
	/*
	 void setParameters(int analysisSampleRate = 5512.5,
	 int windowSize = 2048,
	 int windowShift = 64,
	 int windowType = WINDOW_TYPE_HANNING,
	 int minFreq = 200,
	 int maxFreq = 2000,
	 int bandNbr = 33,
	 int filterShape = FILTER_SHAPE_RECTANGLE,
	 int freqScale = FREQ_SCALE_LOG);
	 */
}

ACAudioHaitsmaFingerprintPlugin::~ACAudioHaitsmaFingerprintPlugin() {
	
	delete audio_fingerprint;
}


std::vector<ACMediaFeatures*> ACAudioHaitsmaFingerprintPlugin::calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat) {
    
    cout<<"ACAudioHaitsmaFingerprintPlugin::calculate"<<endl;

	std::vector<ACMediaTimedFeature*> descmf;
	std::vector<ACMediaFeatures*> desc;
	ACAudio* theAudio = 0;

	try {
		theAudio = static_cast <ACAudio*> (theMedia);
		if(!theAudio) 
			throw runtime_error("<ACAudioHaitsmaFingerprintPlugin::_calculate> problem with ACAudio cast");
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return desc;
	}
	
	float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];
	memcpy(data, static_cast<float*>(aData->getData())+theAudio->getSampleStart()*theAudio->getChannels(),
		   (theAudio->getSampleEnd()-theAudio->getSampleStart())*theAudio->getChannels()*sizeof(float));
	
	audio_fingerprint->setSampleRate(theAudio->getSampleRate(), theAudio->getChannels());
	descmf = audio_fingerprint->compute(data, theAudio->getNFrames());
	//descmf = computeFeatures(data, theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 32, 13, 1024, true);
	
	for (int i=0; i<descmf.size(); i++){
		desc.push_back(descmf[i]->mean());
	}
	
	if (_save_timed_feat) {
		// try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
		bool save_binary = false;
		string mtf_file_name; // file(s) in which feature(s) will be saved
		string file_ext =  "_b.mtf";
		string aFileName = theMedia->getFileName();
		string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
		for (unsigned int i=0; i<descmf.size(); i++){
			mtf_file_name = aFileName_noext + "_" + descmf[i]->getName() + file_ext;
			descmf[i]->saveInFile(mtf_file_name, save_binary);
			theMedia->addTimedFileNames(mtf_file_name);            
			//mtf_file_names.push_back(mtf_file_name); // keep track of saved features
		}
	}
	
	for (unsigned int i=0; i<descmf.size(); i++){
		delete descmf[i];
	}
	descmf.clear();
	delete [] data;
	return desc;
}

// XS TODO there is a mtf_file_nameS with "S" here ???

//ACMediaTimedFeature* ACAudioHaitsmaFingerprintPlugin::getTimedFeatures() {
//	if (mtf_file_name == ""){
//        cout << "<ACAudioHaitsmaFingerprintPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}
