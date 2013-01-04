/**
 * @brief Base audio data and float* / std::vector<float> containers
 * @author Xavier Siebert, Christian Frisson
 * @date 7/04/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#ifndef ACAUDIODATA_H
#define ACAUDIODATA_H

#include "ACMediaData.h"
#include<iostream>

class ACAudioFloatVecDataContainer : public ACMediaDataContainer, public ACTemporalData {
public:
    ACAudioFloatVecDataContainer() : ACMediaDataContainer(), ACTemporalData() {}
    virtual ~ACAudioFloatVecDataContainer(){
        data.clear();
    }
    void setData(std::vector<float> _data){
        data.clear();
        this->data = _data;
    }
    std::vector<float> getData(){return data;}
    virtual int getNumberOfFrames(){
        return data.size();
    }

protected:
    std::vector<float> data;
};

class ACAudioFloatPtrDataContainer : public ACMediaDataContainer, public ACTemporalData {
public:
    ACAudioFloatPtrDataContainer() : ACMediaDataContainer(), ACTemporalData() {
        data=0;
        nframes=0;
    }
    virtual ~ACAudioFloatPtrDataContainer(){
        if(data)
            delete data;
    }
    void setData(float* _data){
        if(data)
            delete data;
        this->data = _data;
    }
    float* getData(){return data;}
    void setNumberOfFrames(int _number){
        this->nframes = _number;
    }
    virtual int getNumberOfFrames(){
        return nframes;
    }
protected:
    float* data;
    int nframes;
};

class ACAudioData: public ACMediaData {
public:
    ACAudioData() : ACMediaData()
    {
        media_type = MEDIA_TYPE_AUDIO;
    }
    virtual ~ACAudioData(){}

    virtual float getSampleRate(){return 0.0f;}
    virtual int getNumberOfChannels(){return 1;}
    virtual int getNumberOfFrames(){return 1;}
};

#endif // ACAUDIODATA_H
