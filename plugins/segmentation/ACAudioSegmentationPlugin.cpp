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
	int method=0;
	icolvec peaks_v;
	fcolvec time_v;
	fcolvec desc_v;
	fmat desc_m;
	switch (method){
	case 0:{
		desc_mf = computeFeature(data, "Energy", theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024, extendSoundLimits);
		desc_v = conv_to<fcolvec>::from(-desc_mf->getValue());
		time_v = desc_mf->getTime();
		peaks_v = findpeaks(desc_v, 10);
		break;
	}
	case 1:{
		desc_mf = computeFeature(data, "Spectral Flux", theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024*2, extendSoundLimits);
		desc_v = conv_to<fcolvec>::from(desc_mf->delta()->getValue());
		time_v = desc_mf->getTime();
		peaks_v = findpeaks(desc_v, min((unsigned int) 10, desc_v.n_elem-1));		
		break;
	}
        case 2:{ //FASTBIC
                desc_mf = computeFeature(data, "MFCC", theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 16, 13, 1024*2, false);
                peaks_v=FastBIC(desc_mf->getValue(), 2, theAudio->getSampleRate());
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
	delete[] data;
	return segments;
}

icolvec FastBIC(fmat audiofeatures_m, float lambda, int samplerate)
{
    int nwindows=audiofeatures_m.n_cols;
    int nfeatures=audiofeatures_m.n_rows;
    icolvec segment_v;
    
    /// ALGORITHM PARAMETERS ///
    int delta_h=1; // high resolution: linked to to accuracy of the estimation
    int delta_l=10*delta_h; // low resolution, to obtain a first estimation of the segmentation point. Must be a multiple of delta_h.
    float Wmin=0.05; /// in seconds, minimum length of the search window 
    int Nmin=(int)(Wmin*samplerate/delta_l+0.5)*delta_l; // in samples, minimum window length. Must be a multiple of delta_l
    float Wmax=0.2; // in seconds, maximum length of the search window
    int Nmax=(int)(Wmax*samplerate/delta_l+0.5)*delta_l; // in samples, maximum length of the window. Must be a multiple of delta_l.
    int Nsecond=Nmin; // length of the window when refining the segment position. Must be a multiple of delta_h
    int delta_Ngrow=2*delta_l; // increase of the window when no candidate segmentation is found. Must be a multiple of delta_l
    int delta_Nshift=4*delta_l; // shift of the window when no candidate segmentation is found and window size=Nmax. Must be a multiple of delta_l;
    int delta_Nmargin=2*delta_l; // size of the border windows on which deltaBIC is not computed. Must be a multiple of delta_l
    
    
    
    
    return segment_v;
}

