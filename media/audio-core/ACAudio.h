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
    virtual ~ACAudio();

private:
    void init();

public:
    void saveACLSpecific(std::ofstream &library_file);
    int loadACLSpecific(std::ifstream &library_file);
    void saveXMLSpecific(TiXmlElement* _media);
    int loadXMLSpecific(TiXmlElement* _pMediaNode);

    int getWidth() {return getSampleEnd();}
    int getHeight() {return 0;}

    bool extractData(std::string fname);

    void setSampleRate(int _sample_rate) { sample_rate = _sample_rate; }
    int getSampleRate() { return sample_rate; }
    void setChannels(int _channels) { channels = _channels; }
    int getChannels() { return channels; }

    long getNFrames() { return getSampleEnd() - getSampleStart() + 1; }
    float getDuration() {return (float)getNFrames()/(float) sample_rate;}

    int getSampleStart();
    int getSampleEnd();

    void setSampleStart(int st){this->start = (float) st / (float) sample_rate;}
    void setSampleEnd(int en){this->end = (float) en / (float) sample_rate;}

    // To deprecate, will crash on long files
    float* getSamples();
    float* getMonoSamples();

private:
    int sample_rate;
    int channels;
};

#endif // ACAUDIO_H
