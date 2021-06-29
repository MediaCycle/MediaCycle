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

#include "ACVideoOpenCVData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

// creates a pointer (video_ptr) to the beginning of the video
bool ACVideoOpenCVData::readData(string _fname){
	if(_fname=="") return false;
    capture = new cv::VideoCapture(_fname.c_str());
	// check if video successfully opened 
    if (!capture->isOpened()){
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
        cerr << "<ACVideoOpenCVData::readData> Could not initialize capturing from file " << _fname << endl;
        delete capture;
        capture = 0;
		return false;
	}	
	return true;
}

// XS TODO check this copy 2.2
/*void ACVideoOpenCVData::setData(cv::VideoCapture* _data){
	cvCopy(_data,video_ptr);		
	if( !video_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACVideoData::setData> Could not set data" << endl;
	}	
}*/

bool ACVideoOpenCVData::closeFile(){
    return false;
}

ACMediaDataContainer* ACVideoOpenCVData::getBuffer(int start_frame, int number_of_frames, int channel)
{
    if(start_frame < 0){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid start frame " << start_frame << " (<0)" << std::endl;
        return 0;
    }

    if(number_of_frames < 0){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid number of frames " << number_of_frames << " (<0)" << std::endl;
        return 0;
    }

    if(channel < 0){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid channel " << channel << " (<0)" << std::endl;
        return 0;
    }

    if(!capture)
        this->readData(this->file_name);
    if(!capture){
        std::cerr << "ACVideoOpenCVData::getBuffer: couldn't load file '" << this->file_name << "'" << std::endl;
        return 0;
    }

    if(start_frame > this->getNumberOfFrames()){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid start frame " << start_frame << " beyond " << this->getNumberOfFrames() << std::endl;
        return 0;
    }

    if(start_frame+number_of_frames > this->getNumberOfFrames()){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid number of frames " << number_of_frames << " goes beyond " << this->getNumberOfFrames() << " with start frame " << start_frame << std::endl;
        return 0;
    }

    if(channel > this->getNumberOfChannels()){
        std::cerr << "ACVideoOpenCVData::getBuffer: invalid channel " << channel << " beyond " << this->getNumberOfChannels() << std::endl;
        return 0;
    }

    capture->set(cv::CAP_PROP_POS_FRAMES, start_frame);
    int current_frame = (int)(capture->get(cv::CAP_PROP_POS_FRAMES));
    if(current_frame != start_frame){
        std::cerr << "ACVideoOpenCVData::getBuffer: couldn't seek to frame " << start_frame << ", got " << current_frame << " instead" << std::endl;
        return 0;
    }

    cv::Mat img;
    *capture >> img;
    ACVideoOpenCVDataContainer* video_data = new ACVideoOpenCVDataContainer();
    video_data->setData(img);
    //video_data->setNumberOfFrames(1);
    return video_data;
}

float ACVideoOpenCVData::getSampleRate()
{
    if(capture)
        return (float)(capture->get(cv::CAP_PROP_FPS));
    else
        return 0.0f;
}

int ACVideoOpenCVData::getNumberOfChannels()
{
    if(capture)
        return 1;
    else
        return 0;
}

int ACVideoOpenCVData::getNumberOfFrames()
{
    if(capture)
        return (int)(capture->get(cv::CAP_PROP_FRAME_COUNT)) -1;  // XS -1 seems necessary in OpenCV 2.3
    else
        return 0;
}

int ACVideoOpenCVData::getWidth()
{
    if(capture)
        return (int)(capture->get(cv::CAP_PROP_FRAME_WIDTH));
    else
        return 0;
}

int ACVideoOpenCVData::getHeight()
{
    if(capture)
        return (int)(capture->get(cv::CAP_PROP_FRAME_HEIGHT));
    else
        return 0;
}

