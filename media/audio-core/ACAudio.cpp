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
    features_vectors.resize(0);
	preproc_features_vectors.resize(0);
    data=0;
}

ACAudio::ACAudio(const ACAudio& m, bool reduce) : ACMedia(m){
    this->init();
    sample_rate = m.sample_rate;
    channels = m.channels;
    this->data = m.data;
}

ACAudio::~ACAudio() {
    // data is deleted by ACMedia
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

    getline(library_file, tab);

    return 1;
}

void ACAudio::saveXMLSpecific(TiXmlElement* _media){
    _media->SetAttribute("SamplingRate", sample_rate);
    _media->SetAttribute("Channels", channels);
    _media->SetAttribute("SampleStart",this->getSampleStart());
    _media->SetAttribute("SampleEnd",this->getSampleEnd());
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
    return 1;
}

bool ACAudio::extractData(string fname) {
    ACAudioData* audio_data = dynamic_cast<ACAudioData*>(this->getMediaData());
    if(!audio_data)
    {
        std::cerr << "ACAudio::extractData: no audio data set" << std::endl;
        return 0;
    }

    sample_rate = audio_data->getSampleRate();
    channels = audio_data->getNumberOfChannels();
    int frames = audio_data->getNumberOfFrames();

    if (this->getSampleStart() < 0)
        this->setSampleStart(0);
    if (this->getSampleEnd() < 0)
        this->setSampleEnd(frames);

    std::string seg_level;
    if (getParentId()==-1) //CF before a proper way exists to check if an ACMedia is a file or segment
        seg_level = "file";
    else
        seg_level = "segment";
    std::cout << "Duration of the "<< seg_level <<" : " << this->getDuration() << std::endl;
    std::cout << "Number of frames of the "<< seg_level <<" : " << this->getNFrames() << std::endl;
    return true;
}

// To deprecate, will crash on long files

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sndfile.hh>

#include <stdint.h>
#include <stdarg.h>

#define SF_COUNT_TO_LONG(x)	((long) (x))
#define	ARRAY_LEN(x)		((int) (sizeof (x)) / (sizeof ((x) [0])))
#define SIGNED_SIZEOF(x)	((int64_t) (sizeof (x)))
#define	NOT(x)				(! (x))

#define	PIPE_INDEX(x)	((x) + 500)
#define	PIPE_TEST_LEN	12345

float* ACAudio::getSamples(){
    if(!this->data){
        std::cerr << "ACAudio::getSamples: no media data set" << std::endl;
        return 0;
    }
    ACAudioData* audio_data = dynamic_cast<ACAudioData*>(this->getMediaData());
    if(!audio_data)
    {
        std::cerr << "ACAudio::getSamples: no audio data set" << std::endl;
        return 0;
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

        float* _data = new float[(long) this->getNFrames() * this->getChannels()];

        sf_seek(testFile, this->getSampleStart(), SEEK_SET);
        sf_readf_float(testFile, _data, this->getNFrames());
        sf_close(testFile);
        return _data;
    }
}

// To deprecate, will crash on long files
float* ACAudio::getMonoSamples(){
    if (data){
        float* _data = new float[(long) this->getNFrames()];
        return 0;
        float* _tmpdata = 0;
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
        //std::cout << "i file" << i << std::endl;
        return _data;
    }
}
