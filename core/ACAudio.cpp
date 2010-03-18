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
//#include "ACAudioFeaturesFactory.h"

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
    sample_start = 0;
    sample_end = 0;
    n_frames = 0;
    waveform = NULL;
}

ACAudio::~ACAudio() {
	// XS added this on 4/11/2009 to clean up memory
	if (waveform) delete waveform;	
}

void ACAudio::save(FILE* library_file) {
	int i, j;
	int n_features;
	int n_features_elements;	
	int nn;
	
	fprintf(library_file, "%s\n", filename.c_str());
	
#ifdef SAVE_LOOP_BIN
	fwrite(&mid,sizeof(int),1,library_file);
	fwrite(&sample_rate,sizeof(int),1,library_file);
	fwrite(&channels,sizeof(int),1,library_file);
	fwrite(&sample_start,sizeof(int),1,library_file);
	fwrite(&sample_end,sizeof(int),1,library_file);
	fwrite(&n_frames,sizeof(int),1,library_file);
	fwrite(waveform,sizeof(float),n_frames,library_file);	// XS: was: waverform (typo)
	n_features = features_vectors.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i].size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
			fwrite(&value,sizeof(float),1,library_file);
		}
	}
#else
	fprintf(library_file, "%d\n", mid);
	fprintf(library_file, "%d\n", sample_rate);
	fprintf(library_file, "%d\n", channels);
	fprintf(library_file, "%d\n", sample_start);
	fprintf(library_file, "%d\n", sample_end);
	fprintf(library_file, "%d\n", n_frames);
	for (i=0; i<n_frames; i++) {
		//fprintf(library_file, "%2.6f\t", waveform[i]);
	}
	fprintf(library_file, "\n");
	n_features = features_vectors.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize();
		nn = features_vectors[i]->getNeedsNormalization();
		fprintf(library_file, "%s\n", features_vectors[i]->getName().c_str());
		fprintf(library_file, "%d\n", nn);
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%0.15e\t", features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
		}
		fprintf(library_file, "\n");
	}
#endif
}

int gmid = 0;

int ACAudio::load_v1(FILE* library_file) {
	
	float dump;
	int s;
	
	int i, j;
	int path_size;
	int n_features;
	int f_type;
	int n_features_elements;
	char featureName[256];
	int nn;
	
	int ret;
	char *retc;
	
	//ACAudioFeaturesFactory* factory = new ACAudioFeaturesFactory(0);
	ACMediaFeatures* mediaFeatures;
	FeaturesVector all_features;
	float local_feature;
	
	char audio_file_temp[1024];
	memset(audio_file_temp,0,1024);
	
	retc = fgets(audio_file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(audio_file_temp);
		// filename = new char[path_size];
		//		strncpy(filename, audio_file_temp, path_size-1); // XS TODO all string
		// filename[path_size-1] = 0;
		filename = string(audio_file_temp, path_size-1);
		
		/*	memset(audio_file_temp,0,1024);
		 retc = fgets(audio_file_temp, 1024, library_file);
		 path_size = strlen(audio_file_temp);
		 thumbnail_filename = new char[path_size];
		 strncpy(thumbnail_filename, audio_file_temp, path_size-1);
		 thumbnail_filename[path_size-1] = 0;*/
		//ret = fscanf(library_file, "%d", &mid);
		mid = gmid; gmid++;
		ret = fscanf(library_file, "%d", &sample_rate);
		ret = fscanf(library_file, "%d", &channels);
		ret = fscanf(library_file, "%d", &sample_start);
		ret = fscanf(library_file, "%d", &sample_end);
		ret = fscanf(library_file, "%f", &dump);
		ret = fscanf(library_file, "%f", &dump);
		ret = fscanf(library_file, "%d", &n_frames);
		waveform = new float[n_frames];
		cout << "nframes : " << n_frames << endl;
		/* for (i=0; i<n_frames; i++) {
			ret = fscanf(library_file, "%f", &waveform[i]);
			s = i % 2;
			if (s==0) {s=-1;} else {s=1;}
			waveform[i] *= s;
		} */
		ret = fscanf(library_file, "%d", &n_features);
		for (i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			if (i==0) { strcpy(featureName, "rhythm"); nn=1; }
			if (i==1) { strcpy(featureName, "timbre"); nn=1; }
			if (i==2) { strcpy(featureName, "harmony"); nn=1; }
			features_vectors[i]->setName(string(featureName));
			features_vectors[i]->setNeedsNormalization(nn);
			ret = fscanf(library_file, "%d", &n_features_elements);
			features_vectors[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
		}
		n_features += 5;
		for (i=i; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			if (i==3) { strcpy(featureName, "db"); nn=0; n_features_elements = 1;}
			if (i==4) { strcpy(featureName, "bpm"); nn=0; n_features_elements = 1;}
			if (i==5) { strcpy(featureName, "time_signature"); nn=0; n_features_elements = 2; }
			if (i==6) { strcpy(featureName, "key"); nn=0; n_features_elements = 1;}
			if (i==7) { strcpy(featureName, "acid_type"); nn=0; n_features_elements = 1;}
			features_vectors[i]->setName(string(featureName));
			features_vectors[i]->setNeedsNormalization(nn);
			features_vectors[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
		}
		
		ret = fscanf(library_file, "\n");
		return 1;
	}
	else {
		return 0;
	}	
}

int ACAudio::load(FILE* library_file) {
	int i, j;
	int path_size;
	int n_features;
	int f_type;
	int n_features_elements;
	char featureName[256];
	int nn;
	
	int ret;
	char *retc;
	
	//ACAudioFeaturesFactory* factory = new ACAudioFeaturesFactory(0);
	ACMediaFeatures* mediaFeatures;
	FeaturesVector all_features;
	float local_feature;
	
	char audio_file_temp[1024];
	memset(audio_file_temp,0,1024);
	
	retc = fgets(audio_file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(audio_file_temp);
		// filename = new char[path_size];
		//		strncpy(filename, audio_file_temp, path_size-1); // XS TODO all string
		// filename[path_size-1] = 0;
		filename = string(audio_file_temp, path_size-1); // -1 to remove "\n"
		
		/*	memset(audio_file_temp,0,1024);
		 retc = fgets(audio_file_temp, 1024, library_file);
		 path_size = strlen(audio_file_temp);
		 thumbnail_filename = new char[path_size];
		 strncpy(thumbnail_filename, audio_file_temp, path_size-1);
		 thumbnail_filename[path_size-1] = 0;*/
		ret = fscanf(library_file, "%d", &mid);
		ret = fscanf(library_file, "%d", &sample_rate);
		ret = fscanf(library_file, "%d", &channels);
		ret = fscanf(library_file, "%d", &sample_start);
		ret = fscanf(library_file, "%d", &sample_end);
		ret = fscanf(library_file, "%d", &n_frames);
		waveform = new float[n_frames];
		cout << "nframes : " << n_frames << endl;
		/* for (i=0; i<n_frames; i++) {
			ret = fscanf(library_file, "%f", &waveform[i]);
		} */
		ret = fscanf(library_file, "%d\n", &n_features);
		for (i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			//ret = fscanf(library_file, "%s", featureName);
			fgets(featureName, 100, library_file);
			features_vectors[i]->setName(string(featureName));
			ret = fscanf(library_file, "%d", &nn);
			features_vectors[i]->setNeedsNormalization(nn);
			ret = fscanf(library_file, "%d", &n_features_elements);
			features_vectors[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
			ret = fscanf(library_file, "\n");
			ret = fscanf(library_file, "\t");
		}
		ret = fscanf(library_file, "\n");
		return 1;
	}
	else {
		return 0;
	}	
}

ACMediaData* ACAudio::extractData(string fname){
	// XS/SD todo : extraire ce qu'il faut passer au plugin.
	// SD : help me with this one !!!
	
	// ça devrait ressembler à:
	//	ACAnalysedAudio* full_audio = new ACAnalysedAudio(_path);
//	sample_rate = full_audio->getSampleRate();
//	channels = full_audio->getNbChannels();
//	sample_start = 0;
//	sample_end = full_audio->getLength();
//	if (sample_end) {
//		full_audio->getWaveform(&n_frames, &waveform);
//		filename_thumbnail = _path + ".thumb";
//		saveThumbnail(filename_thumbnail);
//	}
//	delete full_audio;
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
	sample_start = 0;
	n_frames = sfinfo.frames;
	sample_end = sfinfo.frames;
	float* data = new float[(long) sfinfo.frames];
	sf_read_float(testFile, data, sfinfo.frames);
	ACMediaData* audio_data = new ACMediaData(fname, MEDIA_TYPE_AUDIO);
	audio_data->setAudioData(data);
	sf_close(testFile);
	return audio_data;
}

// void ACAudio::import(std::string _path)
// migrated to ACMedia::import.

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
		for (i=0;i<n_frames;i++) {
			fprintf(thumbnail_file, "%2.6f ", waveform[i]);
		}
		//fprintf(thumbnail_file, "\n");
		fprintf(thumbnail_file, "</points>");
		fprintf(thumbnail_file, "</data>");
		fprintf(thumbnail_file, "</waveform>\n");
		fclose(thumbnail_file);	
	}
}
