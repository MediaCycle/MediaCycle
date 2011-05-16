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
#if defined (SUPPORT_AUDIO)
#include "ACAudio.h"
#include <sndfile.h>
//#include "ACAnalysedAudio.h"
//#include "ACAudioFeaturesFactory.h
#include <fstream>

using std::vector;
using std::string;
using std::ofstream;
using std::ifstream;
using std::endl;
using std::cout;


ACAudio::ACAudio() : ACMedia() {
	this->init();
}

void ACAudio::init() {
	media_type = MEDIA_TYPE_AUDIO;
    channels = 0;
	sample_rate = 0;	
	waveformLength = 0;
	waveform = 0;
    features_vectors.resize(0);
    db = 0.0f;
    bpm = 0.0f;
    time_signature_num = 0;
    time_signature_den = 0;
    key = 0;
    acid_type = 0;
	data=0;
}

ACAudio::ACAudio(const ACAudio& m, bool reduce) : ACMedia(m){
	this->init();
	db = m.db;
	bpm = m.bpm;
	time_signature_num = m.time_signature_num;
	time_signature_den = m.time_signature_den;
	key = m.key;
	acid_type = m.acid_type;
	sample_rate = m.sample_rate;
	channels = m.channels;
	if (!reduce){
		waveformLength = m.waveformLength;
		waveform = new float[waveformLength];
		for (int i=0; i<waveformLength; i++){
			waveform[i] = m.waveform[i];
		}
	}
}

ACAudio::~ACAudio() {
	if (waveform) delete [] waveform;
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

void ACAudio::saveACLSpecific(ofstream &library_file) {
	
	int i;
	
	library_file << sample_rate << endl;
	library_file << channels << endl;
	library_file << this->getSampleStart() << endl;
	library_file << this->getSampleEnd() << endl;
	library_file << waveformLength << endl;
	for (i=0; i<waveformLength; i++) {
		library_file << waveform[i] << " ";
	}
	library_file << endl;
}

int ACAudio::loadACLSpecific(ifstream &library_file) {
	
	int i;
	string tab;
	int sample_start, sample_end;
	
	library_file >> sample_rate;
	library_file >> channels;
	library_file >> sample_start;
	this->setSampleStart(sample_start);
	library_file >> sample_end;
	this->setSampleEnd(sample_end);
	
	library_file >> waveformLength;
	waveform = new float[waveformLength];
	for (i=0; i<waveformLength; i++) {
		library_file >> waveform[i];
	}
	getline(library_file, tab);
	
	return 1;
}

void ACAudio::saveXMLSpecific(TiXmlElement* _media){
	_media->SetAttribute("SamplingRate", sample_rate);
	_media->SetAttribute("Channels", channels);
	_media->SetAttribute("SampleStart",this->getSampleStart());
	_media->SetAttribute("SampleEnd",this->getSampleEnd());
	_media->SetAttribute("waveformLength",waveformLength);
	// XS TODO add tests

	// waveform
	if (waveform ) {
		TiXmlElement* mediawf = new TiXmlElement( "Waveform" );  
		std::string s;
		std::stringstream tmp;
		for (int j=0; j<waveformLength; j++) {
			tmp << waveform[j] << " " ;
		}
		s = tmp.str();
		TiXmlText* mediawft = new TiXmlText(s.c_str());
		mediawf->LinkEndChild( mediawft );  
		_media->LinkEndChild( mediawf );
	}						  
}

int ACAudio::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int sample_start, sample_end;
	// XS TODO add checks
	_pMediaNode->QueryIntAttribute("SamplingRate", &this->sample_rate);
	_pMediaNode->QueryIntAttribute("Channels", &this->channels);

	_pMediaNode->QueryIntAttribute("SampleStart",&sample_start);
	_pMediaNode->QueryIntAttribute("SampleEnd",&sample_end);
	
	this->setSampleStart(sample_start);
	this->setSampleEnd(sample_end);
	string wfs = "";
	
	_pMediaNode->QueryIntAttribute("waveformLength",&waveformLength);
	
	TiXmlElement* waveformElement = _pMediaNode->FirstChildElement( "Waveform" );
	if (waveformElement) {
		TiXmlText*  waveformElementsAsText = waveformElement->FirstChild()->ToText();
		wfs = waveformElementsAsText->ValueStr();		
		waveform = new float[waveformLength];
		if (wfs.size() > 0) {
			std::stringstream wfss;
			wfss << wfs;
			for (int j=0; j<waveformLength; j++) {
				// XS TODO add test
				wfss >> waveform[j];
			}
		}
	}	

// XS TODO note : for image we have this:
// data = new ACMediaData(MEDIA_TYPE_IMAGE,filename);
// should we read the file ?
// here already read the waveform	
	return 1;
	
}

void ACAudio::setData(float* _data,float _sample_number, int _sr,int _channels) {
	if (data == 0)
		data = new ACAudioData();
	data->setData(_data,_sample_number);
	this->channels = _channels;
	this->sample_rate = _sr;
	
	this->start=0;
	this->end=_sample_number/(float) _sr/(float) _channels;
}

void ACAudio::extractData(string fname) {
	
	SF_INFO sfinfo;
	SNDFILE* testFile;
	if (! (testFile = sf_open (fname.c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s.\n", fname.c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (0)) ;
		//return  0;
	}
	sample_rate = sfinfo.samplerate;
	channels = sfinfo.channels;
	if (this->getSampleStart() < 0)
		this->setSampleStart(0);
	if (this->getSampleEnd() < 0)
		this->setSampleEnd(sfinfo.frames);	

	std::string seg_level;
	if (getParentId()==-1) //CF before a proper way exists to check if an ACMedia is a file or segment
		seg_level = "file";
	else
		seg_level = "segment";
	std::cout << "Duration of the "<< seg_level <<" : " << this->getDuration() << std::endl;
	std::cout << "Number of frames of the "<< seg_level <<" : " << this->getNFrames() << std::endl;
// 	sf_readf_float(testFile, data, sfinfo.frames);
	if (data) delete data; // XS TODO deleteData
	data = new ACAudioData();
	data->readData(fname);
 	this->computeWaveform(this->getData());
	sf_close(testFile);
}

void ACAudio::deleteData(){
	if (data)
		delete data;
	data=0;
}

float* ACAudio::getSamples(){
	if (data){
		return this->getData();
	}	
	else{
		SF_INFO sfinfo;
		SNDFILE* testFile;
		cout << this->getFileName() << endl;
		if (! (testFile = sf_open (this->getFileName().c_str(), SFM_READ, &sfinfo))){  
			/* Open failed so print an error message. */
			printf ("Not able to open input file %s.\n", this->getFileName().c_str()) ;
			/* Print the error message from libsndfile. */
			puts (sf_strerror (0)) ;
			return  0;
		}
		sample_rate = sfinfo.samplerate;
		channels = sfinfo.channels;
		if (this->getSampleStart() < 0)
			this->setSampleStart(0);
		if (this->getSampleEnd() < 0)
			this->setSampleEnd(sfinfo.frames);

		float* _data = new float[(long) this->getNFrames() * this->getChannels()];

		sf_seek(testFile, this->getSampleStart(), SEEK_SET);
		sf_readf_float(testFile, _data, this->getNFrames());
		sf_close(testFile);
		return _data;
	}	
}

float* ACAudio::getMonoSamples(){
	if (data){
		float* _data = new float[(long) this->getNFrames()];
		float* _tmpdata  = this->getData();
		long i;
		for (i = 0; i< this->getNFrames(); i++){
			_data[i] = _tmpdata[i*this->getChannels()];
		}
		std::cout << "i data" << i << std::endl;
		return _data;
	}	
	else{	
		SF_INFO sfinfo;
		SNDFILE* testFile;
		if (! (testFile = sf_open (this->getFileName().c_str(), SFM_READ, &sfinfo))){  
			/* Open failed so print an error message. */
			printf ("Not able to open input file %s.\n", this->getFileName().c_str()) ;
			/* Print the error message from libsndfile. */
			puts (sf_strerror (0)) ;
			return  0;
		}
		
		sample_rate = sfinfo.samplerate;
		channels = sfinfo.channels;
		if (this->getSampleStart() < 0)
			this->setSampleStart(0);
		if (this->getSampleEnd() < 0)
			this->setSampleEnd(sfinfo.frames);
		
		float* tmpdata = new float[(long) this->getNFrames() * this->getChannels()];
		float* _data = new float[(long) this->getNFrames()];
		
		sf_seek(testFile, this->getSampleStart(), SEEK_SET);
		sf_readf_float(testFile, tmpdata, this->getNFrames());
		sf_close(testFile);
				
		long i;
		for (i = 0; i< this->getNFrames(); i++){
			_data[i] = tmpdata[i*this->getChannels()];
		}
		delete[] tmpdata;
		std::cout << "i file" << i << std::endl;
		return _data;
	}	
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
	//CF to do: waveform for segments
	if (this->parentid == -1){
		int i, j, k;
		int n_samples_hop;
		float hop = 0.02f;
		int minWaveformLength = 200;
		
		waveformLength = ((float)getNFrames() / (float)sample_rate) / (hop);
		waveformLength--;
		
		if (waveformLength < minWaveformLength){
			waveformLength = minWaveformLength;
			hop =  ((float)getNFrames() / (float)sample_rate) / (float)(waveformLength);
		}
		n_samples_hop = hop * sample_rate;
		
		waveform = new float[2 * waveformLength];
		k = 0;
		for (i=0; i< 2*waveformLength-1; i=i+2) {
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
}
#endif //defined (SUPPORT_AUDIO)