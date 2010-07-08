/**
 * @brief AGSynthesis.cpp
 * @author Cecile Picard
 * @date 08/07/2010
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


using namespace arma;


AGSynthesis::AGSynthesis(){
	method = AG_METHOD_SIMPLE;
	mapping = AG_MAPPING_MEANVAR;
	randomness = 0;
	threshold = 1;
}

bool AGSynthesis::compute(long targetId, set<int> selectedNodes){
	vector<long> grainIds;
	for (set<int>::const_iterator iter = selectedNodes.begin();iter != selectedNodes.end();++iter){
		grainIds.push_back(*iter);
	}
	return compute(targetId, grainIds);
}

bool AGSynthesis::compute(long targetId, vector<long> grainIds){

	ACMediaLibrary* lib = mediacycle->getLibrary();
	vector<string> featureList;
	featureList.push_back("Mean of MFCC");
	featureList.push_back("Mean of Spectral Flatness");
	featureList.push_back("Interpolated Energy");

	int durationT = lib->getMedia(targetId)->getDuration();

	mat descG_m = extractDescMatrix(lib, featureList, grainIds);
	mat enerG_m = extractDescMatrix(lib, "Mean of Energy", grainIds);
	mat specG_m = extractDescMatrix(lib, "Mean of Spectral Centroid", grainIds);
	
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
	mat enerTS_m = extractDescMatrix(lib, "Mean of Energy", targetSegmentIds);

	// Mapping choice
	switch (this->getMapping()) {
	case AG_MAPPING_NONE:{
		break;
	}
	case AG_MAPPING_MEAN:{
		descG_m = descG_m - repmat(mean(descG_m, 1), 1, descG_m.n_cols);
		descTS_m = descTS_m - repmat(mean(descTS_m, 1), 1, descTS_m.n_cols);
		break;
	}
	case AG_MAPPING_MEANVAR:{
		descG_m = zscore(descG_m);
		descTS_m = zscore(descTS_m);
		break;
	}
	}
	
	// distance computation between all target (pattern) segments and all grain
	mat dist_m = euclideanDistance(descTS_m, descG_m);

	// sorting the dist matrix to find the closest grains
	umat sp_m(dist_m.n_rows, dist_m.n_cols);
	umat tmp_v; 	// actually a vector
	ucolvec perm_v;
	std::cout << "Random = " << this->getRandomness() << std::endl;
	for (int i=0; i<dist_m.n_rows; i++){
		sp_m.row(i) = sort_index(conv_to<rowvec>::from(dist_m.row(i)));
		perm_v = randperm(max((int) round(sp_m.n_cols * this->getRandomness()),1));
		std::cout << "perm_v =  " << perm_v << std::endl;
		std::cout << "O sp_m.row(i) : "  << sp_m.row(i) << std::endl;
		tmp_v = sp_m.submat(i, 0, i, perm_v.n_elem-1);
		for (int j=0; j<perm_v.n_elem; j++){
			sp_m(i,j) = tmp_v(perm_v(j));
		}
		std::cout << "M sp_m.row(i) : "  << sp_m.row(i) << std::endl;
	}

	for (int i=0; i < sp_m.n_rows; i++){
		std::cout << grainIds[sp_m(i,0)] << " ";		
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
		if (seg_samp_start_v(i) + ((ACAudio*)lib->getMedia(grainIds[sp_m(i,0)]))->getNFrames() > durationSyn){
			durationSyn = seg_samp_start_v(i) + ((ACAudio*)lib->getMedia(grainIds[sp_m(i,0)]))->getNFrames();
		}
	}
	
	long maxGrainLength = 0;
	for (int i=0; i<grainIds.size(); i++){
		if (((ACAudio*)lib->getMedia(grainIds[i]))->getNFrames() > maxGrainLength){
			maxGrainLength = ((ACAudio*)lib->getMedia(grainIds[i]))->getNFrames();
		}
	}
	durationSyn += maxGrainLength;
	
	colvec syn_v;
	syn_v.zeros(durationSyn);
	colvec win_v;

	long ind0 = 0;

	for (int i=0; i < targetSegment_v.size(); i++){
		audioGrain = (ACAudio*) lib->getMedia(grainIds[sp_m(i,0)]);
		colvec selG_v = extractSamples(audioGrain);
		colvec targetSample_v = extractSamples(targetSegment_v[i]);
		colvec selGS_v = zeros<colvec>(selG_v.n_rows); colvec targetSampleS_v = zeros<colvec>(targetSample_v.n_rows);
		//targetSample_v = pow(targetSample_v,2)
		//Amplitude factor to take into account amplitude evolution of the signal model (target)
		float ampFactor1 = ((enerTS_m(i,0)) / (enerG_m(sp_m(i,0),0)));
		for (int j=0; j < targetSampleS_v.n_rows; j++){
			targetSampleS_v[j] = targetSample_v[j] * targetSample_v[j];
		}
		for (int j=0; j < selGS_v.n_rows; j++){
			selGS_v[j] = selG_v[j] * selG_v[j];
		}
		float ampFactor2 = sqrt(sum(targetSampleS_v) / sum(selGS_v));
		if (ampFactor2 > 5.0)
			ampFactor2 = 1;
			
		win_v = tukeywin(audioGrain->getNFrames(), .01);
		
		switch (this->getMethod()) {
		case AG_METHOD_SIMPLE:{
			if (seg_samp_start_v(i)+audioGrain->getNFrames() - 1 < syn_v.n_elem){
				syn_v.rows(seg_samp_start_v(i), seg_samp_start_v(i)+audioGrain->getNFrames()-1) += ampFactor2*selG_v % win_v;
				//syn_v = syn_v * ampFactor2;
				std::cout << " ampFactor " << grainIds[sp_m(i,0)] << " " << ampFactor1 << " " << ampFactor2;
			}
			else{
				std::cout << "----------------" << std::endl;
				std::cout << syn_v.n_elem << std::endl;
				std::cout << seg_samp_start_v(i)+audioGrain->getNFrames()-1 << std::endl;
				std::cerr << "Wrong synthesis vector size" << std::endl;
			}
			break;
		}
		case AG_METHOD_SQUEEZED:{
			if (ind0+audioGrain->getNFrames() - 1 < syn_v.n_elem){
				syn_v.rows(ind0, ind0+audioGrain->getNFrames()-1) = ampFactor2 * selG_v % win_v;
				//syn_v = syn_v * ampFactor2;
				ind0 = ind0+audioGrain->getNFrames();
				std::cout << " ampFactor " << grainIds[sp_m(i,0)] << " " << ampFactor1 << " " << ampFactor2;
				
			}
			else{
				std::cout << "----------------" << std::endl;
				std::cout << syn_v.n_elem << std::endl;
				std::cout << ind0+audioGrain->getNFrames()-1 << std::endl;
				std::cerr << "Wrong synthesis vector size" << std::endl;
			}
			break;
		}
		case AG_METHOD_PADDED:{
			if (seg_samp_start_v(i)+audioGrain->getNFrames() - 1 < syn_v.n_elem){
				syn_v.rows(seg_samp_start_v(i), seg_samp_start_v(i)+audioGrain->getNFrames()-1) = ampFactor2 * selG_v % win_v;
				//syn_v = syn_v * ampFactor2;
				std::cout << " ampFactor " << ampFactor2;
			}
			else{
				std::cout << "----------------" << std::endl;
				std::cout << syn_v.n_elem << std::endl;
				std::cout << seg_samp_start_v(i)+audioGrain->getNFrames()-1 << std::endl;
				std::cerr << "Wrong synthesis vector size" << std::endl;
			}

			ind0 = ind0+audioGrain->getNFrames();
			int round = 1;
			while (ind0 < seg_samp_end_v(i) & round < grainIds.size()){
				std::cout << "Plus! " << round << std::endl;
				int sGrainId = grainIds[sp_m(i,round)];
				audioGrain = (ACAudio*) lib->getMedia(sGrainId);
				selG_v = extractSamples(audioGrain);
				colvec targetSample_v = extractSamples(targetSegment_v[i]);
				colvec selGS_v = zeros<colvec>(selG_v.n_rows); colvec targetSampleS_v = zeros<colvec>(targetSample_v.n_rows);
				//Amplitude factor to take into account amplitude evolution of the signal model (target)
				float ampFactor1 = sqrt((enerTS_m(i,0)) / (enerG_m(sp_m(i,0),0)));
				for (int j=0; j < targetSampleS_v.n_rows; j++){
					targetSampleS_v[j] = targetSample_v[j] * targetSample_v[j];
				}
				for (int j=0; j < selGS_v.n_rows; j++){
					selGS_v[j] = selG_v[j] * selG_v[j];
				}
				float ampFactor2 = sqrt(sum(targetSampleS_v) / sum(selGS_v));
				if (ampFactor2 > 5.0)
					ampFactor2 = 1;
				win_v = tukeywin(audioGrain->getNFrames(), .01);
				if (ind0+audioGrain->getNFrames() - 1 < syn_v.n_elem){
					syn_v.rows(ind0, ind0+audioGrain->getNFrames()-1) = ampFactor2 * selG_v % win_v;
					//syn_v = syn_v * ampFactor2;
					std::cout << " ampFactor " << ampFactor2;
				}
				else{
					std::cout << "----------------" << std::endl;
					std::cout << syn_v.n_elem << std::endl;
					std::cout << ind0+audioGrain->getNFrames()-1 << std::endl;
					std::cerr << "Wrong synthesis vector size" << std::endl;
				}
				ind0 += audioGrain->getNFrames();
				round++;
			}
		}
		}

	}
	if (max(syn_v) > 1){
		syn_v = (syn_v/max(syn_v))*.99;
	}
	
	if (this->synthesisLength > 0){
		delete [] synthesisSound;
	}
	
	synthesisSound = new float[durationSyn];
	for (int i=0; i < durationSyn; i++)
		synthesisSound[i] = syn_v(i);
	this->synthesisLength = durationSyn;
	return true;
}
	
mat AGSynthesis::extractDescMatrix(ACMediaLibrary* lib, vector<string> featureList, vector<long> mediaIds){
	mat desc_m;
	mat tmpDesc_m;
	
	desc_m = extractDescMatrix(lib, featureList[0], mediaIds);
	for (int i=1; i<featureList.size(); i++){
		tmpDesc_m = extractDescMatrix(lib, featureList[i], mediaIds);
		desc_m = join_rows(desc_m, tmpDesc_m);
	}
	return desc_m;
}

mat AGSynthesis::extractDescMatrix(ACMediaLibrary* lib, string featureName, vector<long> mediaIds){
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
 
colvec AGSynthesis::extractSamples(ACAudio* audioGrain){
	//	audioGrain = (ACAudio*) lib->getMedia(mediaId);
	float* audioSamples = audioGrain->getSamples();
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
	return selG_v;
}

bool AGSynthesis::saveAsWav(string path){
	SF_INFO sfinfo;
	SNDFILE* testFile;
	sfinfo.samplerate = 44100;
	sfinfo.channels = 1;
	sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	
	if (! (testFile = sf_open (path.c_str(), SFM_WRITE, &sfinfo))){  
		printf ("Not able to open input file %s.\n", "synthesis.wav") ;
		puts (sf_strerror (NULL)) ;
		return false;
	}
	sf_writef_float  (testFile, synthesisSound, synthesisLength);
	sf_close(testFile);
	return true;
}
