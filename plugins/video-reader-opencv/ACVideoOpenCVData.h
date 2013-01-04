/**
 * @brief A class that provides a media data instance to parse and read videos using OpenCV.
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

#ifndef ACVideoOpenCVData_H
#define ACVideoOpenCVData_H

#include "ACVideoData.h"
#include "ACMediaTypes.h"
#include "ACOpenCVInclude.h"

class ACVideoOpenCVDataContainer : public ACMediaDataContainer, public ACSpatialData, public ACTemporalData {
public:
    ACVideoOpenCVDataContainer()
        : ACMediaDataContainer(), ACSpatialData(), ACTemporalData()
    {}
    virtual ~ACVideoOpenCVDataContainer(){
        if(!data.empty())
            data.deallocate();
    }
    void setData(cv::Mat _data){
        if(!data.empty())
            data.deallocate();
        data = _data;
    }
    cv::Mat getData(){return data;}
    virtual int getNumberOfFrames(){
        return 1;
    }
protected:
    cv::Mat data;
};

class ACVideoOpenCVData: public ACVideoData {
public:
    ACVideoOpenCVData():ACVideoData(),capture(0){}
    virtual ~ACVideoOpenCVData(){}

    virtual std::string getName(){return "OpenCV video";}

    virtual bool readData(std::string _fname);
    virtual bool closeFile();

    /// Function for spatial media
    virtual int getWidth();
    virtual int getHeight();

    /// Functions for temporal media
    virtual float getSampleRate();
    virtual int getNumberOfChannels();
    virtual int getNumberOfFrames();
    virtual ACMediaDataContainer* getBuffer(int start_frame, int number_of_frames, int channel);

protected:
    cv::VideoCapture* capture;
};

#endif // ACVideoOpenCVData_H
