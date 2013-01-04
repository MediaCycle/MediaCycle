/**
 * @brief A class that provides a media data instance to parse and read audio using libsndfile.
 * @author Christian Frisson
 * @date 14/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACAudioSndfileData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

ACAudioSndfileData::ACAudioSndfileData() : ACAudioData() {
    this->init();
}

void ACAudioSndfileData::init() {
    /*audio_ptr = 0;
    audio_frames = 0;*/
    sfinfo = 0;
    audio_file = 0;
}

ACAudioSndfileData::ACAudioSndfileData(std::string _fname) : ACAudioData(){
	this->init();
	file_name=_fname;
	this->readData(_fname);
}


ACAudioSndfileData::~ACAudioSndfileData() {
    /*if (audio_ptr != 0)
        delete [] audio_ptr;*/
}

bool ACAudioSndfileData::readData(std::string _filename){
    if(_filename == "")
        _filename = this->file_name;
    if(audio_file){
        std::cout << "ACAudioSndfileData::readData: file " << _filename << " already opened" << std::endl;
        return true;
    }
    if(!sfinfo)
        sfinfo = new SF_INFO;
    if (! (audio_file = sf_open (_filename.c_str(), SFM_READ, sfinfo))){
        // Open failed so print an error message.
        std::cout << "ACAudioSndfileData::readData: not able to open input file " <<  _filename << std::endl;
        // Print the error message from libsndfile.
        return false;
    }

    std::cout << "ACAudioSndfileData::readData: file " << _filename << " with " << sfinfo->channels << " channel(s)" << std::endl;
    std::cout << "ACAudioSndfileData::readData: file " << _filename << " of format ";
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_S8)
        std::cout << "-> Signed 8 bit data "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16)
        std::cout << "-> Signed 16 bit data "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_24)
        std::cout << "-> Signed 24 bit data "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32)
        std::cout << "-> Signed 32 bit data "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_U8)
        std::cout << "-> Unsigned 8 bit data (WAV and RAW only) "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_FLOAT)
        std::cout << "-> 32 bit float data "<< std::endl;
    if((sfinfo->format & SF_FORMAT_SUBMASK) == SF_FORMAT_DOUBLE)
        std::cout << "-> 64 bit float data "<< std::endl;
    //std::cout << "ACAudioSndfileData::readData: file " << _filename << " with submask " << (int)(sfinfo->format & SF_FORMAT_SUBMASK) << std::endl;
    //std::cout << "ACAudioSndfileData::readData: file " << _filename << " with typemask " << (int)(sfinfo->format & SF_FORMAT_TYPEMASK) << std::endl;
    return true;
}

float ACAudioSndfileData::getSampleRate()
{
    if(sfinfo)
        return sfinfo->samplerate;
    else
        return 0.0f;
}

int ACAudioSndfileData::getNumberOfChannels()
{
    if(sfinfo)
        return sfinfo->channels;
    else
        return 1;
}

int ACAudioSndfileData::getNumberOfFrames()
{
    if(sfinfo)
        return sfinfo->frames;
    else
        return 1;
}

bool ACAudioSndfileData::closeFile()
{
    if(audio_file){
        sf_close (audio_file);
        return true;
    }
    else
        return false;
}

ACMediaDataContainer* ACAudioSndfileData::getBuffer(int start_frame, int number_of_frames, int channel)
{
    if(start_frame < 0){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid start frame " << start_frame << " (<0)" << std::endl;
        return 0;
    }

    if(number_of_frames < 0){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid number of frames " << number_of_frames << " (<0)" << std::endl;
        return 0;
    }

    if(channel < 0){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid channel " << channel << " (<0)" << std::endl;
        return 0;
    }

    if(!audio_file)
        this->readData(this->file_name);

    if(start_frame > this->getNumberOfFrames()){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid start frame " << start_frame << " beyond " << this->getNumberOfFrames() << std::endl;
        return 0;
    }

    if(start_frame+number_of_frames > this->getNumberOfFrames()){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid number of frames " << number_of_frames << " goes beyond " << this->getNumberOfFrames() << " with start frame " << start_frame << std::endl;
        return 0;
    }

    if(channel > this->getNumberOfChannels()){
        std::cerr << "ACAudioSndfileData::getBuffer: invalid channel " << channel << " beyond " << this->getNumberOfChannels() << std::endl;
        return 0;
    }

    int current_frame = sf_seek(audio_file, start_frame, SEEK_SET);
    if(current_frame != start_frame){
        std::cerr << "ACAudioSndfileData::getBuffer: couldn't seek to frame " << start_frame << ", got " << current_frame << " instead" << std::endl;
        return 0;
    }

    float* buf = new float[this->getNumberOfChannels() * number_of_frames];

    int readcount = sf_readf_float (audio_file, buf, number_of_frames);
    if(readcount != number_of_frames){
        std::cerr << "ACAudioSndfileData::getBuffer: couldn't read " << number_of_frames << " frames, got instead " << readcount << " frames" << std::endl;
        return 0;
    }
    ACAudioFloatPtrDataContainer* audio_pointer = new ACAudioFloatPtrDataContainer();
    audio_pointer->setData(buf);
    audio_pointer->setNumberOfFrames(readcount);
    return audio_pointer;
}
