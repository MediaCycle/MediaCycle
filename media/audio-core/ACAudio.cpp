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
	preproc_features_vectors.resize(0);
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
    if (waveform)
        delete [] waveform;
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
    if (waveform) {
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

bool ACAudio::extractData(string fname) {

    SF_INFO sfinfo;
    SNDFILE* testFile;
    if (! (testFile = sf_open (fname.c_str(), SFM_READ, &sfinfo))){
        // Open failed so print an error message.
        printf ("Not able to open input file %s.\n", fname.c_str()) ;
        // Print the error message from libsndfile.
        puts (sf_strerror (0)) ;
        return false;
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
    //data->setFileName(fname);//CF
    if (!data->readData(fname)) return false;
    float hop = 0.02f;
    this->waveformLength = ((float)getNFrames() / (float)sample_rate) / hop;
    this->waveformLength--;
    this->waveform = this->computeWaveform(this->waveformLength);
    waveformLength *= 2;
    return true;
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
        //std::cout << "i file" << i << std::endl;
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

float* ACAudio::computeWaveform(int& length, float start_ratio, float end_ratio) {
    //CF to do: waveform for segments
    float* _waveform = 0;
    //if (this->parentid == -1){
        //int i, j, k;
        int n_samples_hop;
        float hop = 0.02f;
        int minWaveformLength = 200;

        if(length==0){
            std::cerr << "ACAudio::computeWaveform() no width specified" << std::endl;
            return _waveform;
        }

        int progress = 0.0f;
        // Adapted from sndfile-to-text.c: fixed buffer
        const char *infilename = filename.c_str();
        SNDFILE *infile = NULL;
        SF_INFO sfinfo;
        int bufsize = 1024;

        if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
        {
            std::cerr << "Not able to open input file " << infilename << std::endl;
            return _waveform;
        }

        float n_frames = sfinfo.frames;

        int last_frame = n_frames-1;
        if(this->start<0){
            std::cerr << "ACAudio::computeWaveform() start not set" << std::endl;
            return _waveform;
        }
        else if(this->start >0){
            int current_frame = sf_seek(infile, (int)(this->start*sample_rate), SEEK_SET) ;
            std::cout << "ACAudio::computeWaveform() seeked at " << current_frame << " while " << (int)(this->start*sample_rate) << " was asked." << std::endl;
        }

        if(this->end<0){
            std::cerr << "ACAudio::computeWaveform() end not set" << std::endl;
            return _waveform;
        }
        else if(int(this->end * sample_rate) > n_frames){
            std::cerr << "ACAudio::computeWaveform() end beyond sample size" << std::endl;
            return _waveform;
        }
        else{
            last_frame = (int)((sample_rate)*(this->end-this->start))-1;
        }

        /*waveformLength = ((float)getNFrames() / (float)sample_rate) / (hop);
                waveformLength--;*/
        if (length < minWaveformLength){
            length = minWaveformLength;
            hop =  (float)(this->end-this->start) / (float)(length);

        }

        n_samples_hop = hop * sample_rate;

        float buf [channels * bufsize];
        int k, m, readcount ;
        _waveform = new float[2 * length];
        for (int t=0;t<2 * length;t++)
            _waveform[t]=0.0f;
        double waveform_in = getTime();

        //std::cout << "Testing new sndfile bufferized waveforming on " << sfinfo.frames << " samples with "<< (float)sfinfo.frames/(float)n_samples_hop << " times a hop of " << n_samples_hop << std::endl;
        //int t = getTime();
        int i = -2;// thumbnail index
        int s = 0;// source index
        while ( ( (readcount = sf_readf_float (infile, buf, bufsize)) > 0 ) && s<last_frame )
        {
            //std::cout << " i " << i << " readcount " << readcount << std::endl;
            for (k = 0 ; k < readcount ; k++)
            {
                int m = 0;
                //for (m = 0 ; m < channels ; m++)
                //...
                if(s % n_samples_hop == 0){
                    //if(i>-1)
                    //    std::cout << "Thumbnail["<<i<<"]="<<_waveform[i];
                    ++i;
                    ++i;
                    //std::cout << " hop s " << s << " / " << sfinfo.frames << std::endl;
                    _waveform[i] = buf[k * channels + m];//0;
                    _waveform[i+1] = buf[k * channels + m];//0;
                }
                else{

                    if ( buf[k * channels + m] < _waveform[i] ) {
                        _waveform[i] = buf[k * channels + m];
                    }
                    if ((buf[k * channels + m])> _waveform[i+1]) {
                        _waveform[i+1] = buf[k * channels + m];
                        //if (_waveform[i]<0)
                        //	 std::cout << "Error not positive" << std::endl;
                    }
                    //if (_waveform[i]>0)
                    //	 std::cout << "Error not negative" << std::endl;
                }
                //   if (_waveform[i] < _waveform[i+width])
                //       std::cerr << "Mismatch at " << i << std::endl;
                s++;
                progress = (float)i/(float)length;
            }
        }
        std::cout << "Done computing waveform " << filename << " in " << getTime()-waveform_in << " sec." << std::endl;
        //if(buf)
        //    delete[] buf;
        //std::cout << "Thumbnail with " << i << " elements / " << 2 * length << std::endl;
        //std::cout << "Testing new sndfile bufferized waveforming on " << sfinfo.frames << " samples with "<< (float)sfinfo.frames/(float)n_samples_hop << " times a hop of " << n_samples_hop << std::endl;
        //std::cout << "Computed in " << getTime()-t << std::endl;
        sf_close (infile) ;
        progress = 1.0f;
    //}
    //std::cout << "Thumbnail size " << sizeof(&_waveform)/sizeof(float) << std::endl;
    return _waveform;
}
