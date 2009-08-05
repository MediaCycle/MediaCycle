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
//#include "ACAnalysedAudio.h"
//#include "ACAudioFeaturesFactory.h"

ACAudio::ACAudio() : ACMedia() {
	_type = MEDIA_TYPE_AUDIO;
	features.resize(0);
	db = 0;
	bpm = 0;
	time_signature_num = 0;
	time_signature_den = 0;
	key = 0;
	acid_type = 0;
}

ACAudio::~ACAudio() {
}

void ACAudio::save(FILE* library_file) {
	int i, j;
	int n_features;
	int n_features_elements;	
	fprintf(library_file, "%s\n", filename.c_str());
	
#ifdef SAVE_LOOP_BIN
	fwrite(&mid,sizeof(int),1,library_file);
	fwrite(&sample_rate,sizeof(int),1,library_file);
	fwrite(&channels,sizeof(int),1,library_file);
	fwrite(&sample_start,sizeof(int),1,library_file);
	fwrite(&sample_end,sizeof(int),1,library_file);
	fwrite(&n_frames,sizeof(int),1,library_file);
	fwrite(waverform,sizeof(float),n_frames,library_file);	
	n_features = features.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features.size();i++) {
		n_features_elements = features[i].size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features[i]->getFeature(j)); // XS instead of [i][j]
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
		fprintf(library_file, "%2.6f\t", waveform[i]);
	}
	fprintf(library_file, "\n");
	n_features = features.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features.size();i++) {
		fprintf(library_file, "%d\n", features[i]->getType());
		n_features_elements = features[i]->size(); // XS TODO: ACMediaFeatures don't have a size method
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%f\t", features[i]->getFeature(j)); // XS instead of [i][j]
		}
		fprintf(library_file, "\n");
	}
#endif
}

int ACAudio::load(FILE* library_file) {
	int i, j;
	int path_size;
	int n_features;
	int f_type;
	int n_features_elements;
	
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
		ret = fscanf(library_file, "%d", &mid);
		ret = fscanf(library_file, "%d", &sample_rate);
		ret = fscanf(library_file, "%d", &channels);
		ret = fscanf(library_file, "%d", &sample_start);
		ret = fscanf(library_file, "%d", &sample_end);
		ret = fscanf(library_file, "%d", &n_frames);
		waveform = new float[n_frames];
                cout << "nframes : " << n_frames << endl;
		for (i=0; i<n_frames; i++) {
			ret = fscanf(library_file, "%f", &waveform[i]);
		}
		ret = fscanf(library_file, "%d", &n_features);
		for (i=0; i<n_features;i++) {
			ret = fscanf(library_file, "%d", &f_type);
			mediaFeatures = new ACMediaFeatures();
			features.push_back(mediaFeatures);
			features[i]->setComputed();
			ret = fscanf(library_file, "%d", &n_features_elements);
			features[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features[i]->setFeature(j, local_feature);
			}
		}
		ret = fscanf(library_file, "\n");
		return 1;
	}
	else {
		return 0;
	}	
}

void ACAudio::import(std::string _path) {
//This should just add an audio file to the list of media
//creating/adding features should somehow be done by calling the plugin manager (here or somewhere else)
	// DT : Maybe try to put those lines in ACMedia
	cout << "importing..." << _path << endl;
	filename=_path;

//creating features : where should this go ?
/*	ACAnalysedAudio* full_audio = new ACAnalysedAudio(_path);
	ACAudioFeaturesFactory* factory = new ACAudioFeaturesFactory(full_audio);
	features.push_back(factory->calculateFeature("Timbre")); // XS TODO fichier
	//features.push_back(factory->calculateFeature("LaughRhythm")); 
	
	sample_rate = full_audio->getSampleRate();
	channels = full_audio->getNbChannels();
	sample_start = 0;
	sample_end = full_audio->getLength();
	if (sample_end) {
		full_audio->getWaveform(&n_frames, &waveform);
		filename_thumbnail = _path + ".thumb";
		saveThumbnail(filename_thumbnail);
	}

	delete factory;
	delete full_audio;
  // DT:  until here
    */
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
