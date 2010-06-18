/*
 *  ACAudio.cpp
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 22/06/09
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

#include "ACAudio.h"
#include <sndfile.h>
//#include "ACAnalysedAudio.h"
//#include "ACAudioFeaturesFactory.h
#include <fstream>

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;

ACAudio::ACAudio() : ACMedia() {
    media_type = MEDIA_TYPE_AUDIO;
    features_vectors.resize(0);
    db = 0;
    bpm = 0;
    time_signature_num = 0;
    time_signature_den = 0;
    key = 0;
    acid_type = 0;
    sample_rate = 16000;
    channels = 0;
//     sample_start = 0;
//     sample_end = 0;
    waveformLength = 0;
		waveform = NULL;
}

ACAudio::ACAudio(const ACAudio& m) : ACMedia(m) {
	media_type = MEDIA_TYPE_AUDIO;
	db = m.db;
	bpm = m.bpm;
	time_signature_num = m.time_signature_num;
	time_signature_den = m.time_signature_den;
	key = m.key;
	acid_type = m.acid_type;
	sample_rate = m.sample_rate;
	channels = m.channels;
// 	sample_start = m.sample_start;
// 	sample_end = m.sample_end;
}

ACAudio::~ACAudio() {
	// XS added this on 4/11/2009 to clean up memory
	if (waveform) delete waveform;	
}

int ACAudio::getSampleStart(){
	int sampleStart;
	if (this->start < 0){
		sampleStart = -1;
	}
	else{
		sampleStart = (this->start * (float) this->sample_rate);
		sampleStart = floor(sampleStart+.5);
	}
	return sampleStart;
}

int ACAudio::getSampleEnd(){
	int sampleEnd;
	if (this->end < 0){
		sampleEnd = -1;
	}
	else{
		sampleEnd = (this->end * (float) this->sample_rate);
		sampleEnd = floor(sampleEnd + .5);
	}
	return sampleEnd;
}

// void ACAudio::save(FILE* library_file) {
// 	int i, j;
// 	int n_features;
// 	int n_features_elements;	
// 	int nn;
	
// 	fprintf(library_file, "%s\n", filename.c_str());
	
// #ifdef SAVE_LOOP_BIN
// 	fwrite(&mid,sizeof(int),1,library_file);
// 	fwrite(&sample_rate,sizeof(int),1,library_file);
// 	fwrite(&channels,sizeof(int),1,library_file);
// 	fwrite(&this->getSampleStart(),sizeof(int),1,library_file);
// 	fwrite(&sample_end,sizeof(int),1,library_file);
// 	fwrite(&waveformLength,sizeof(int),1,library_file);
// 	fwrite(waveform,sizeof(float),waveformLength,library_file);	// XS: was: waverform (typo)
// 	n_features = features_vectors.size();
// 	fwrite(&n_features,sizeof(int),1,library_file);
// 	for (i=0; i<features_vectors.size();i++) {
// 		n_features_elements = features_vectors[i].size();
// 		fwrite(&n_features_elements,sizeof(int),1,library_file);
// 		for (j=0; j<n_features_elements; j++) {
// 			value = features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
// 			fwrite(&value,sizeof(float),1,library_file);
// 		}
// 	}
// #else
// 	fprintf(library_file, "%d\n", mid);
// 	fprintf(library_file, "%d\n", sample_rate);
// 	fprintf(library_file, "%d\n", channels);
// 	fprintf(library_file, "%d\n", this->getSampleStart());
// 	fprintf(library_file, "%d\n", sample_end);
// 	fprintf(library_file, "%d\n", waveformLength);
// 	for (i=0; i<waveformLength; i++) {
// 		fprintf(library_file, "%2.6f\t", waveform[i]);
// 	}
// 	fprintf(library_file, "\n");
// 	n_features = features_vectors.size();
// 	fprintf(library_file, "%d\n", n_features);
// 	for (i=0; i<features_vectors.size();i++) {
// 		n_features_elements = features_vectors[i]->getSize();
// 		nn = features_vectors[i]->getNeedsNormalization();
// 		fprintf(library_file, "%s\n", features_vectors[i]->getName().c_str());
// 		fprintf(library_file, "%d\n", nn);
// 		fprintf(library_file, "%d\n", n_features_elements);
// 		for (j=0; j<n_features_elements; j++) {
// 			fprintf(library_file, "%0.15e\t", features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
// 		}
// 		fprintf(library_file, "\n");
// 	}
// #endif
// }

void ACAudio::saveACL(ofstream &library_file) {
	int i, j;
	int n_features;
	int n_features_elements;	
	int nn;
	
	library_file << filename << endl;

	library_file << mid << endl;
	library_file << sample_rate << endl;
	library_file << channels << endl;
	library_file << this->getSampleStart() << endl;
	library_file << getSampleEnd() << endl;
	library_file << waveformLength << endl;
	for (i=0; i<waveformLength; i++) {
		library_file << waveform[i] << " ";
	}
	library_file << endl;
	n_features = features_vectors.size();
	library_file << n_features << endl;
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize();
		nn = features_vectors[i]->getNeedsNormalization();
		library_file << features_vectors[i]->getName() << endl;
		library_file << nn << endl;
		library_file << n_features_elements << endl;
		for (j=0; j<n_features_elements; j++) {
			library_file << features_vectors[i]->getFeatureElement(j)  << "\t"; // XS instead of [i][j]
		}
		library_file << endl;
	}
}

void ACAudio::saveMCSL(ofstream &library_file) {
	int i, j;
	int n_features;
	int n_features_elements;	
	int nn;

	library_file << filename << endl;
	
	library_file << mid << endl;
	//CF DT  addition to saveACL
	library_file << parentid << endl; 
	library_file << this->getAllSegments().size() << endl;
	for (int i=0; i < this->getAllSegments().size(); i++){
		library_file << this->getSegment(i)->getId() << "\t";
	}
	library_file << endl;
	//CF DT until here
	library_file << sample_rate << endl;
	library_file << channels << endl;
	library_file << this->getSampleStart() << endl;
	library_file << getSampleEnd() << endl;
	library_file << waveformLength << endl;
	for (i=0; i<waveformLength; i++) {
		library_file << waveform[i] << " ";
	}
	library_file << endl;
	n_features = features_vectors.size();
	library_file << n_features << endl;
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize();
		nn = features_vectors[i]->getNeedsNormalization();
		library_file << features_vectors[i]->getName() << endl;
		library_file << nn << endl;
		library_file << n_features_elements << endl;
		for (j=0; j<n_features_elements; j++) {
			library_file << features_vectors[i]->getFeatureElement(j)  << "\t"; // XS instead of [i][j]
		}
		library_file << endl;
	}
}

int ACAudio::loadACL(ifstream &library_file) {
	if (! library_file.is_open()) {
		cerr << "<ACAudio::loadACL> : problem loading image from ACL file, it needs to be opened before" << endl;
		return 0;
	}		
	if (!library_file.good()){
		cerr << "<ACAudio::loadACL> : bad library file" << endl;
		return 0;
	}

	int i, j;
	int n_features;
	int n_features_elements = 0;	
	int nn;
	string tab;
	
	ACMediaFeatures* mediaFeatures;
	string featureName;
	float local_feature;
	int sample_start, sample_end;

	getline(library_file, filename, '\n');
	if (!filename.empty()){
		library_file >> mid;
		library_file >> sample_rate;
		library_file >> channels;
		library_file >> sample_start;
		this->setSampleStart(sample_start);
		library_file >> sample_end;
		this->setSampleEnd(sample_end);

		//library_file >> n_frames;
		//		library_file >> duration;
		library_file >> waveformLength;
		waveform = new float[waveformLength];
		for (i=0; i<waveformLength; i++) {
			library_file >> waveform[i];
		}
		getline(library_file, tab);
		library_file >> n_features;	
		getline(library_file, tab);
	
		for (int i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
//			getline(library_file, featureName, '\n');
			getline(library_file, featureName);
			features_vectors[i]->setName(featureName);
			library_file >> nn;
			features_vectors[i]->setNeedsNormalization(nn);
			library_file >> n_features_elements;
			features_vectors[i]->resize(n_features_elements);
			for (int j=0; j<n_features_elements; j++) {
				library_file >> local_feature;
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
			getline(library_file, tab);	
			std::cout << "read extra chars : \n" << tab << std::endl;
		}
		return 1;
	}
	else{
		return 0;
	}
}

int ACAudio::loadMCSL(ifstream &library_file) {
	if (! library_file.is_open()) {
		cerr << "<ACAudio::loadACL> : problem loading image from ACL file, it needs to be opened before" << endl;
		return 0;
	}		
	if (!library_file.good()){
		cerr << "<ACAudio::loadACL> : bad library file" << endl;
		return 0;
	}
	
	int n_features;
	int n_features_elements = 0;	
	int nn;
	string tab;
	
	ACMediaFeatures* mediaFeatures;
	string featureName;
	float local_feature;
	
	int sample_start, sample_end;
	int nbSegments;
	long segId;

	getline(library_file, filename, '\n');
	if (!filename.empty()){
		library_file >> mid;
		library_file >> parentid; //CF the only addition to saveACL!
		library_file >> nbSegments;
 		for (int i=0; i < nbSegments; i++){
			library_file >> segId;
			cout << segId << "\t";
		}                                                                           
		library_file >> sample_rate;
		library_file >> channels;
		library_file >> sample_start;
		this->setSampleStart(sample_start);
		library_file >> sample_end;
		this->setSampleEnd(sample_end);
		//library_file >> n_frames;
		//		library_file >> duration;
		library_file >> waveformLength;
		waveform = new float[waveformLength];
		for (int i=0; i<waveformLength; i++) {
			library_file >> waveform[i];
		}
		getline(library_file, tab);
		library_file >> n_features;	
		getline(library_file, tab);
		
		for (int i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			//			getline(library_file, featureName, '\n');
			getline(library_file, featureName);
			features_vectors[i]->setName(featureName);
			library_file >> nn;
			features_vectors[i]->setNeedsNormalization(nn);
			library_file >> n_features_elements;
			features_vectors[i]->resize(n_features_elements);
			for (int j=0; j<n_features_elements; j++) {
				library_file >> local_feature;
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
			getline(library_file, tab);	
		}
		return 1;
	}
	else{
		return 0;
	}
}

// int ACAudio::load(FILE* library_file) {
// 	int i, j;
// 	int path_size;
// 	int n_features;
// //	int f_type;
// 	int n_features_elements;
// 	char featureName[256];
// 	int nn;
	
// 	int ret;
// 	char *retc;
	
// 	//ACAudioFeaturesFactory* factory = new ACAudioFeaturesFactory(0);
// 	ACMediaFeatures* mediaFeatures;
// 	FeaturesVector all_features;
// 	float local_feature;
	
// 	char audio_file_temp[1024];
// 	memset(audio_file_temp,0,1024);
	
// 	retc = fgets(audio_file_temp, 1024, library_file);
	
// 	if (retc) {
// 		path_size = strlen(audio_file_temp);
// 		// filename = new char[path_size];
// 		//		strncpy(filename, audio_file_temp, path_size-1); // XS TODO all string
// 		// filename[path_size-1] = 0;
// 		filename = string(audio_file_temp, path_size-1); // -1 to remove "\n"
		
// 		/*	memset(audio_file_temp,0,1024);
// 		 retc = fgets(audio_file_temp, 1024, library_file);
// 		 path_size = strlen(audio_file_temp);
// 		 thumbnail_filename = new char[path_size];
// 		 strncpy(thumbnail_filename, audio_file_temp, path_size-1);
// 		 thumbnail_filename[path_size-1] = 0;*/
// 		ret = fscanf(library_file, "%d", &mid);
// 		ret = fscanf(library_file, "%d", &sample_rate);
// 		ret = fscanf(library_file, "%d", &channels);
// 		ret = fscanf(library_file, "%d", &sample_start);
// 		ret = fscanf(library_file, "%d", &sample_end);
// 		ret = fscanf(library_file, "%d", &n_frames);
// 		ret = fscanf(library_file, "%d", &waveformLength);
// 		waveform = new float[waveformLength];
// 		// load waveform
// 		for (i=0; i<waveformLength; i++) {
// 			ret = fscanf(library_file, "%f", &waveform[i]);
// 		}
// 		ret = fscanf(library_file, "%d\n", &n_features);
// 		for (i=0; i<n_features;i++) {
// 			mediaFeatures = new ACMediaFeatures();
// 			features_vectors.push_back(mediaFeatures);
// 			features_vectors[i]->setComputed();
// 			//ret = fscanf(library_file, "%s", featureName);
// 			fgets(featureName, 100, library_file);
// 			features_vectors[i]->setName(string(featureName));
// 			ret = fscanf(library_file, "%d", &nn);
// 			features_vectors[i]->setNeedsNormalization(nn);
// 			ret = fscanf(library_file, "%d", &n_features_elements);
// 			features_vectors[i]->resize(n_features_elements);
// 			for (j=0; j<n_features_elements; j++) {
// 				ret = fscanf(library_file, "%f", &(local_feature));
// 				features_vectors[i]->setFeatureElement(j, local_feature);
// 			}
// 			ret = fscanf(library_file, "\n");
// 			ret = fscanf(library_file, "\t");
// 		}
// 		ret = fscanf(library_file, "\n");
// 		return 1;
// 	}
// 	else {
// 		return 0;
// 	}	
// }

ACMediaData* ACAudio::extractData(string fname){
	SF_INFO sfinfo;
	SNDFILE* testFile;
	if (! (testFile = sf_open (fname.c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s.\n", fname.c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		return  NULL;
	}
	sample_rate = sfinfo.samplerate;
	channels = sfinfo.channels;
	if (this->getSampleStart() < 0)
		this->setSampleStart(0);
	if (this->getSampleEnd() < 0)
		this->setSampleEnd(sfinfo.frames);	

	float* data = new float[(long) sfinfo.frames * sfinfo.channels];
	std::cout << "Duration of the segment : " << this->getDuration() << std::endl;
	std::cout << "Number of frame of the segment : " << this->getNFrames() << std::endl;
	sf_readf_float(testFile, data, sfinfo.frames);
	this->computeWaveform(data);
	ACMediaData* audio_data = new ACMediaData(fname, MEDIA_TYPE_AUDIO);
	audio_data->setAudioData(data);
	sf_close(testFile);
	return audio_data;
}

float* ACAudio::getSamples(){
	SF_INFO sfinfo;
	SNDFILE* testFile;
	if (! (testFile = sf_open (this->getFileName().c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s.\n", this->getFileName().c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		return  NULL;
	}

	float* data = new float[(long) this->getNFrames() * this->getChannels()];

	sf_seek(testFile, this->getSampleStart(), SEEK_SET);
	sf_readf_float(testFile, data, this->getNFrames());
	sf_close(testFile);
	return data;
}


void ACAudio::saveThumbnail(std::string _path) {
	int i;
	FILE *thumbnail_file;
	if (_path.length()) {
		thumbnail_file = fopen(_path.c_str(),"w");
		fprintf(thumbnail_file, "<?xml version=\"1.0\" encoding=\"utf-8\"?> ");
		fprintf(thumbnail_file, "<waveform> ");
		fprintf(thumbnail_file, "<name>%s</name> ", filename.c_str());
		fprintf(thumbnail_file, "<data> ");
		fprintf(thumbnail_file, "<points> ");
		//fprintf(thumbnail_file, "%d ", n_frames);
		for (i=0;i<waveformLength;i++) {
			fprintf(thumbnail_file, "%2.6f ", waveform[i]);
		}
		//fprintf(thumbnail_file, "\n");
		fprintf(thumbnail_file, "</points>");
		fprintf(thumbnail_file, "</data>");
		fprintf(thumbnail_file, "</waveform>\n");
		fclose(thumbnail_file);	
	}
}


void ACAudio::computeWaveform(const float* samples_v) {
	int i, j, k;
	int n_samples_hop;
	float hop = .02;
	int minWaveformLength = 60;
	
	waveformLength = ((float)getNFrames() / (float)sample_rate) / (hop);
	waveformLength--;
	
	if (waveformLength < minWaveformLength){
		waveformLength = minWaveformLength;
		hop =  ((float)getNFrames() / (float)sample_rate) / (float)(waveformLength);
	}
	n_samples_hop = hop * sample_rate;
	
	waveform = new float[2 * waveformLength];
	k = 0;
	for (i=0; i<2 * waveformLength; i=i+2) {
		waveform[i] = 0;
		waveform[i+1] = 0;
		for (j=k;j<k+n_samples_hop;j++) {
 			if ((samples_v[channels*j])< waveform[i]) {
				waveform[i] = samples_v[channels*j];
			}
			if ( samples_v[channels*j] > waveform[i+1] ) {
				waveform[i+1] = samples_v[channels*j];
			}
		}
		k += n_samples_hop;
	}
	waveformLength *= 2;
}
