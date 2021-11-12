/*
 *  ACAudio.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 22/06/09
 *  @author XS
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

#ifndef ACAUDIO_H
#define ACAUDIO_H

#include "ACMedia.h"
#include "ACAudioData.h"
#include <string>
#include <cstring>
#include <sys/stat.h>

class ACAudio : public ACMedia {
    // contains the *minimal* information about an audio
    // is this too much already ?
public:
    ACAudio();
    ACAudio(const ACAudio&, bool reduce = true);
    ~ACAudio();

private:
    void init();

public:
    void saveACLSpecific(std::ofstream &library_file);
    int loadACLSpecific(std::ifstream &library_file);
    void saveXMLSpecific(TiXmlElement* _media);
    int loadXMLSpecific(TiXmlElement* _pMediaNode);

    //void import(std::string _path); // XS 240210 : migrated to ACMedia
    void saveThumbnail(std::string _path);
    void* getThumbnailPtr() { return (void*)waveform; }
    int getThumbnailWidth() {return waveformLength;} // width in thumbnail frames, not samples
    int getThumbnailHeight() {return 0;} // width in thumbnail frames, not samples
    int getWidth() {return getSampleEnd();}
    int getHeight() {return 0;}

    float* getData(){return static_cast<float*> (data->getData());}
    void setData(float* _data,float _sample_number=0 ,int _sr=44100,int _channels=1);
    virtual ACMediaData* getMediaData(){return data;}
    bool extractData(std::string fname);
    virtual void deleteData();

    void setSampleRate(int _sample_rate) { sample_rate = _sample_rate; }
    int getSampleRate() { return sample_rate; }
    void setChannels(int _channels) { channels = _channels; }
    int getChannels() { return channels; }

    long getNFrames() { return getSampleEnd() - getSampleStart() + 1; }
    float getDuration() {return (float)getNFrames()/(float) sample_rate;}

    int getSampleStart();
    int getSampleEnd();

    void setSampleStart(int st){this->start = (float) st / (float) sample_rate; };
    void setSampleEnd(int en){this->end = (float) en / (float) sample_rate; };

    void setWaveformLength(int _waveformLength) { waveformLength = _waveformLength; }
    int getWaveformLength() { return waveformLength; }
    void setWaveform(float* _waveform) { waveform = _waveform; }
    float* getWaveform() { return waveform; }

    float* computeWaveform(int& length, float start_ratio = 0.0f, float end_ratio = 1.0f);

    float* getSamples();
    float* getMonoSamples();

private:
    int sample_rate;
    int channels;
    //float time_stamp_start;	// seconds
    //float time_stamp_end;
    int waveformLength;
    float* waveform; // typically one value every 10ms
    // following are specific to loops or music
    float db;
    float bpm;
    int time_signature_num;
    int time_signature_den;
    int key;
    int acid_type;
    ACAudioData* data;
};


#endif // ACAUDIO_H
#endif //defined (SUPPORT_AUDIO)
