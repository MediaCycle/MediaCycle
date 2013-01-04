/*
 *  ACMediaData.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/12/09
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

#ifndef _ACMEDIADATA_H
#define _ACMEDIADATA_H

#include <string>
#include "ACMediaTypes.h"

#include "ACMediaNode.h"  // this contains ACPoint

#include<iostream>
#include <boost/any.hpp>

class ACMediaDataContainer {
public:
    ACMediaDataContainer(){}
    virtual ~ACMediaDataContainer(){}
};

class ACSpatialData {
public:
    ACSpatialData(){}
    virtual ~ACSpatialData(){}
};

class ACTemporalData {
public:
    ACTemporalData(){}
    virtual ~ACTemporalData(){}
    virtual int getNumberOfFrames()=0;
};

class ACMediaData {
public:
    ACMediaData();
    virtual ~ACMediaData();

    virtual std::string getName()=0;
    virtual bool readData(std::string _fname)=0;//{return false;}
    virtual bool closeFile(){return false;} //=0;

    bool isTemporal();
    bool isSpatial();

    ACMediaDataContainer* getData(){return data;}
    void setData(ACMediaDataContainer* _data){data = _data;}

    std::string getFileName() {return file_name;}
    void setFileName(std::string _fname);
    ACMediaType getMediaType(){return media_type;}
    void setMediaType(ACMediaType _media_type){media_type=_media_type;}
    void setLabel(std::string _label){label = _label;}
    std::string getLabel(void){return label;}

protected:
    ACMediaType media_type;
    std::string file_name;
    std::string label;
    ACMediaDataContainer* data;

public:

    /// Functions for temporal media
    virtual ACMediaDataContainer* getBuffer(int start_frame, int number_of_frames, int channel){return 0;} //=0;
    virtual bool rewind(){return false;} //=0;
    virtual float getSampleRate(){return 0.0f;}//=0;
    virtual int getNumberOfChannels(){return 0;}//=0;
    virtual int getNumberOfFrames(){return 0;}//=0;

    /// Functions for spatial media
    virtual ACMediaDataContainer* getRectRegion(ACPoint top_left, ACPoint bottom_right){return 0;}
    virtual ACMediaDataContainer* getCircRegion(ACPoint center, double radius){return 0;}
    virtual ACMediaDataContainer* getPolygonRegion(std::vector<ACPoint> points){return 0;}
    virtual int getWidth(){return 0;}//=0;
    virtual int getHeight(){return 0;}//=0;
    virtual int getDepth(){return 0;}//=0;
};

#endif  // ACMEDIADATA_H
