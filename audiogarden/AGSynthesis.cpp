/**
 * @brief AGSynthesis.cpp
 * @author Damien Tardieu
 * @date 29/06/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#include "AGSynthesis.h"
#include "ACAudio.h"
#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include "ACAudioFeatures.h"
#include <samplerate.h>
#include <iostream>
#include "Armadillo-utils.h"
#include <map>
#include "MediaCycle.h"


using namespace arma;


void AGSynthesis(MediaCycle* mc, long targetId, vector<long> grainIds, float** syn, long &length){

	ACMediaLibrary* lib = mc->getLibrary();
	vector<string> featureList;
	featureList.push_back("Mean of MFCC");
	featureList.push_back("Mean of Spectral Flatness");
	featureList.push_back("Interpolated Energy");

	int durationT = lib->getMedia(targetId)->getDuration();

	mat descG_m = extractDescMatrix(lib, featureList, grainIds);
	
	vector<ACMedia*> targetSegment1_v = lib->getMedia(targetId)-> getAllSegments();
	vector<ACAudio*> targetSegment_v;

	for (int i=0; i<targetSegment1_v.size(); i++){
		targetSegment_v.push_back((ACAudio*) targetSegment1_v[i]);
	}
	
	// if the target has no segment then the segment is the target itself
	if (targetSegment_v.size() == 0){
		targetSegment_v.push_back((ACAudio*)lib->getMedia(targetId));
	}

	vector<long> targetSegmentIds;
	for (int i=0; i < targetSegment_v.size(); i++){
		targetSegmentIds.push_back(targetSegment_v[i]->getId());
	}

	// extract durations
	colvec durationTS_v(targetSegmentIds.size());
	colvec durationG_v(grainIds.size());
	
	for (int i=0; i < targetSegmentIds.size(); i++){
		durationTS_v(i) = targetSegment_v[i]->getDuration();
	}
	for (int i=0; i < grainIds.size(); i++){
		durationG_v(i) = lib->getMedia(grainIds[i])->getDuration();
	}

	// features of the target segments
	mat descTS_m = extractDescMatrix(lib, featureList, targetSegmentIds);
	
	descG_m = zscore(descG_m);
	descTS_m = zscore(descTS_m);
	
	descG_m.save("descG.txt", arma_ascii);
	descTS_m.save("descTS.txt", arma_ascii);
	mat dist_m = euclideanDistance(descTS_m, descG_m);
	dist_m.save("dist.txt", arma_ascii);

	ucolvec mp_v(dist_m.n_rows);
	for (int i=0; i<dist_m.n_rows; i++){
		mp_v(i) = min_index(conv_to<rowvec>::from(dist_m.row(i)));
	}
	
	for (int i=0; i < mp_v.n_elem; i++){
		std::cout << grainIds[mp_v(i)] << " ";		
	}

	colvec seg_samp_start_v(targetSegment_v.size()); 
	colvec seg_samp_end_v(targetSegment_v.size());
	colvec seg_samp_duration_v(targetSegment_v.size());
	for (int i=0; i<targetSegment_v.size(); i++){
		seg_samp_start_v(i) = targetSegment_v[i]->getSampleStart();
		seg_samp_end_v(i) = targetSegment_v[i]->getSampleEnd();
		seg_samp_duration_v(i) = targetSegment_v[i]->getNFrames();
	}

	ACAudio* audioGrain;

	// compuation of the synthesized sound duration
	long durationSyn = 0;
	for (int i=0; i<targetSegment_v.size(); i++){
		if (seg_samp_start_v(i) + ((ACAudio*)lib->getMedia(grainIds[mp_v(i)]))->getNFrames() > durationSyn){
			durationSyn = seg_samp_start_v(i) + ((ACAudio*)lib->getMedia(grainIds[mp_v(i)]))->getNFrames();
		}
	}
	

	float* audioSamples;
	colvec syn_v;
	syn_v.zeros(durationSyn);
	colvec win_v;
	
	for (int i=0; i < targetSegment_v.size(); i++){
		audioGrain = (ACAudio*) lib->getMedia(grainIds[mp_v(i)]);
		audioSamples = audioGrain->getSamples();
		// TODO resample
		if (audioGrain->getSampleRate() != 44100){
			std::cerr << "Wrong sampling rate" << std::endl;
			exit(1);
		}

		colvec selG_v(audioGrain->getNFrames());
		
		for (long i = 0; i< audioGrain->getNFrames(); i++){
			selG_v[i] = audioSamples[i*audioGrain->getChannels()];
		}
		delete [] audioSamples;
		win_v = tukeywin(audioGrain->getNFrames(), .01);
// 		std::cout << "Start = " << seg_samp_start_v(i) << std::endl;
// 		std::cout << "NFrames = " << audioGrain->getNFrames()-1 << std::endl;		
		if (seg_samp_start_v(i)+audioGrain->getNFrames() - 1 < syn_v.n_elem){
			syn_v.rows(seg_samp_start_v(i), seg_samp_start_v(i)+audioGrain->getNFrames()-1) = selG_v % win_v;
		}
		else{
			std::cout << "----------------" << std::endl;
			std::cout << syn_v.n_elem << std::endl;
			std::cout << seg_samp_start_v(i)+audioGrain->getNFrames()-1 << std::endl;
			std::cerr << "Wrong synthesis vector size" << std::endl;
		}
	}
	
	*syn = new float[durationSyn];
	for (int i=0; i < durationSyn; i++)
		(*syn)[i] = syn_v(i);
	length = durationSyn;
	return;
}

mat extractDescMatrix(ACMediaLibrary* lib, vector<string> featureList, vector<long> mediaIds){
	mat desc_m;
	mat tmpDesc_m;
	
	desc_m = extractDescMatrix(lib, featureList[0], mediaIds);
	for (int i=1; i<featureList.size(); i++){
		tmpDesc_m = extractDescMatrix(lib, featureList[i], mediaIds);
		desc_m = join_rows(desc_m, tmpDesc_m);
	}
	return desc_m;
}

mat extractDescMatrix(ACMediaLibrary* lib, string featureName, vector<long> mediaIds){
  vector<ACMedia*> loops = lib->getAllMedia();
  int nbMedia = loops.size(); 
	int featDim;
	int totalDim = 0;
	int featureId = 0;
	int featureSize = 0;

	int nbFeature = loops.back()->getNumberOfFeaturesVectors();

	for(int f=0; f< nbFeature; f++){
		if (loops.back()->getFeaturesVector(f)->getName() == featureName){
			featureId = f;
		}	
	}

	featureSize = loops.back()->getFeaturesVector(featureId)->getSize();

  mat desc_m(mediaIds.size(),featureSize);
  
	mat pos_m(nbMedia,2);
  
  for(int i=0; i<mediaIds.size(); i++) {    
		for(int d=0; d < featureSize; d++){
			desc_m(i, d) = loops[mediaIds[i]]->getFeaturesVector(featureId)->getFeatureElement(d);
		}
  }
	return desc_m;
}
