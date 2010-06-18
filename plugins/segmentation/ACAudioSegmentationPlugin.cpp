/*
 *  ACAudioSegmentationPlugin.cpp
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 09/06/2010
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

#include "ACAudioSegmentationPlugin.h"
#include "ACAudio.h"

#include <vector>
#include <string>
#include "Armadillo-utils.h" 

ACAudioSegmentationPlugin::ACAudioSegmentationPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mPluginType = PLUGIN_TYPE_SEGMENTATION;
    this->mName = "AudioSegmentation";
    this->mDescription = "AudioSegmentation plugin";
    this->mId = "";
}

ACAudioSegmentationPlugin::~ACAudioSegmentationPlugin() {
}


std::vector<ACMedia*> ACAudioSegmentationPlugin::segment(ACMediaData* audio_data, ACMedia* theMedia) {
	int mfccNbChannels = 16;
	int mfccNb = 13;
	int windowSize = 512; 	
	bool extendSoundLimits = true;
	std::vector<ACMediaFeatures*> desc;
	//	int sr = ((ACAudio*)theMedia)->getSampleRate();
	ACAudio* theAudio = (ACAudio*) theMedia;
	ACMediaTimedFeature* desc_mf;
	float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];
	long index = 0;
	
	for (long i = theAudio->getSampleStart(); i< theAudio->getSampleEnd(); i++){
		for (long j = 0; j < theAudio->getChannels(); j++){
			data[index] = audio_data->getAudioData()[i*theAudio->getChannels()+j];
			index++;
		}
	}
	int method=1;
	icolvec peaks_v;
	fcolvec time_v;
	fcolvec desc_v;
	switch (method){
	case 0:{
		desc_mf = computeFeature(data, "Energy", theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024, extendSoundLimits);
		desc_v = conv_to<fcolvec>::from(-desc_mf->getValue());
		time_v = desc_mf->getTime();
		peaks_v = findpeaks(desc_v, 10);
		break;
	}
	case 1:{
		desc_mf = computeFeature(data, "Spectral Flux", theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024, extendSoundLimits);
		desc_v = conv_to<fcolvec>::from(desc_mf->getValue());
		time_v = desc_mf->getTime();
		peaks_v = findpeaks(desc_v, 10);		
		break;
	}
	default:
		std::cerr << "Error : Wrong method" << std::endl;
		exit(1);
	}
	icolvec zero_v = "0";
	icolvec tmp_v(1);
	tmp_v(0) = desc_v.n_elem-1;

	if (peaks_v.n_elem==0){
		peaks_v.set_size(1);
		peaks_v(0) = 0;
	}
	else{
		if ( peaks_v(0) != 0 ){
			peaks_v = join_cols(zero_v, peaks_v);
		}
	}
	if ( peaks_v(peaks_v.n_elem-1) != desc_v.n_elem)  {
		peaks_v = join_cols(peaks_v, tmp_v);
	}
	//	peaks_v.save("peak1.txt", arma_ascii);
	
	colvec peaks_sec_v(peaks_v.n_elem);
	for (int i=0; i<peaks_v.n_elem; i++){
		peaks_sec_v(i) = time_v(peaks_v(i));
	}
	
	//peaks_sec_v.save("peak_sec.txt", arma_ascii);

	mat seg_m(peaks_sec_v.n_elem-1, 2);
	seg_m.col(0) = peaks_sec_v.rows(0, peaks_sec_v.n_elem-2);
	seg_m.col(1) = peaks_sec_v.rows(1, peaks_sec_v.n_elem-1);

	//	seg_m.save("seg.txt", arma_ascii);
	
	vector<ACMedia*> segments;

	for (int i = 0; i < seg_m.n_rows; i++){
		ACMedia* media = ACMediaFactory::create(theAudio);
		media->setParentId(theMedia->getId());
		media->setStart(seg_m(i,0));
		media->setEnd(seg_m(i,1));
		segments.push_back(media);
	}
	// 	ofstream output("signal1.txt");
// 	for(int i=0; i < (long) theAudio->getNFrames() * theAudio->getChannels(); i++){
// 		output<<data[i]<<endl;
// 	}

	return segments;
}

