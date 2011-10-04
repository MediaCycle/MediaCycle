/**
 * @brief ACAudioAcidPlugin.cpp
 * @author Xavier Siebert
 * @date 04/10/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include "ACAudioAcidPlugin.h"
#include "ACAudio.h"
#include <vector>
#include <string>
#include <libgen.h>

ACAudioAcidPlugin::ACAudioAcidPlugin() : ACTimedFeaturesPlugin() {
	//vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "AudioAcid";
    this->mDescription = "AudioAcid plugin";
    this->mId = "";
	this->mDescriptorsList.push_back("Acid Header Features");	
}

ACAudioAcidPlugin::~ACAudioAcidPlugin() {
	
}

float ACAudioAcidPlugin::getBPM(int acid_type, int _nbeats, int nsamples, int sample_rate) {
	
	int lowest_BPM=60;
	int highest_BPM=180;
	float BPM;
	int nbeats = _nbeats;
	
	BPM = (float)(nbeats*sample_rate*60)/(float)(nsamples);
	
	if (BPM<lowest_BPM) {
		nbeats = (int)(nsamples*lowest_BPM/(60*sample_rate));
		BPM = (float)(nbeats*sample_rate*60)/(float)(nsamples);
		if(BPM<lowest_BPM) {
			nbeats++;
		}
	}
	if (BPM>highest_BPM) {
		nbeats=(int)(nsamples*highest_BPM/(60*sample_rate));
	}
	BPM = (float)(nbeats*sample_rate*60)/(float)nsamples;
	if (BPM<lowest_BPM) {
		BPM = lowest_BPM;
	}
	else if (BPM>highest_BPM) {
		BPM = highest_BPM;
	}
	return BPM;
}

void ACAudioAcidPlugin::extractDataWavNotAcid(string fname, int nsamples, int sample_rate) {

	string bpmfilename;
	FILE *bpmfile;
	
	bpmfilename = dirname(strdup(fname.c_str()));
	bpmfilename = bpmfilename + "/bpm.txt";
	
	bpmfile = fopen(bpmfilename.c_str(),"r");
	if (bpmfile) {
		fscanf(bpmfile, "%f", &acid_bpm);
		acid_type = 2;
                fclose(bpmfile);
	}
	else {
		acid_bpm = 0;
	}
	
	if (acid_bpm) {
		acid_key = 0;
	}	
}

void ACAudioAcidPlugin::extractDataWavAcid(string fname, int nsamples, int sample_rate) {
	
	CwavesAcid *	pWaveLoader = NULL;
	WAVEID			WaveID;
	/*
	unsigned long	ulDataSize = 0; 
	unsigned long	ulFrequency = 0; 
	unsigned long	ulFormat = 0;
	unsigned long	ulBytesWritten;
	void *			pData = NULL;
	float			beat_interval;
	*/
	
	// Create instance of WaveLoader class
	pWaveLoader = new CwavesAcid();
	
	if (pWaveLoader) {
		if (pWaveLoader->OpenWaveFile(fname.c_str(), &WaveID)>=0) {
			// this is just used to read to acid header if it is there //
			pWaveLoader->GetAcidKey(WaveID, &acid_key);
			pWaveLoader->GetAcidType(WaveID, &acid_type);
			pWaveLoader->GetAcidNbeats(WaveID, &acid_nbeats);
			// actual reading of data is done using sndfile elsewhere //
			/*
			 pWaveLoader->GetWaveSize(WaveID, &ulDataSize);		// in bytes
			 pWaveLoader->GetWaveFrequency(WaveID, &ulFrequency); 
			 pWaveLoader->GetNchannels(WaveID, &nchannels);			 
			 pData = malloc(ulDataSize);
			 data = pData;
			 pWaveLoader->SetWaveDataOffset(WaveID, 0);
			 pWaveLoader->ReadWaveData(WaveID, pData, ulDataSize, &ulBytesWritten);
			 size = ulDataSize;
			 if (nchannels==1) {
			 format = AL_FORMAT_MONO16;
			 }
			 else if (nchannels==2) {
			 format = AL_FORMAT_STEREO16;
			 }
			 freq = ulFrequency;
			 */
			
			if (acid_type != 65535) {
				
				acid_bpm = getBPM(acid_type, acid_nbeats, nsamples, sample_rate);
				
			}
		}	
	}	
}

std::vector<ACMediaFeatures*> ACAudioAcidPlugin::calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat) {
	
	std::vector<ACMediaFeatures*> desc;
	ACMediaFeatures* feat;
	ACAudio* theAudio = 0;
	
	try {
		theAudio = static_cast <ACAudio*> (theMedia);
		if(!theAudio) 
			throw runtime_error("<ACAudioAcidPlugin::_calculate> problem with ACAudio cast");
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return desc;
	}
	
	// acid_type = 65535;
	
	extractDataWavAcid(theAudio->getFileName(), theAudio->getNFrames(), theAudio->getSampleRate());
	
	if (acid_type == 65535) {

		extractDataWavNotAcid(theAudio->getFileName(), theAudio->getNFrames(), theAudio->getSampleRate());
		
		if (acid_bpm == 0) {
			acid_key = 0;
			acid_nbeats = 0;
			acid_bpm = 0;
		}
	}
	
	feat = new ACMediaFeatures(); 
	feat->setName("acid_type");
	feat->addFeatureElement(acid_type);
	feat->setNeedsNormalization(0);
	desc.push_back(feat);
	feat = new ACMediaFeatures(); 
	feat->setName("acid_key");
	feat->addFeatureElement(acid_key);
	feat->setNeedsNormalization(0);
	desc.push_back(feat);
	feat = new ACMediaFeatures(); 
	feat->setName("acid_nbeats");
	feat->addFeatureElement(acid_nbeats);
	feat->setNeedsNormalization(0);
	desc.push_back(feat);
	feat = new ACMediaFeatures(); 
	feat->setName("acid_bpm");
	feat->addFeatureElement(acid_bpm);
	feat->setNeedsNormalization(0);
	desc.push_back(feat);
	
	return desc;
}

//ACMediaTimedFeature* ACAudioAcidPlugin::getTimedFeatures() {
//	
//	return 0;
//}
