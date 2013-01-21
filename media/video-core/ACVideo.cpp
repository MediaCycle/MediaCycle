/*
 *  ACVideo.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 25/05/09
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

#include "ACVideo.h"

#include <boost/filesystem.hpp>

#include <iostream>
using namespace std;


ACVideo::ACVideo() : ACMedia() {
    this->init();
}

void ACVideo::init() {
    media_type = MEDIA_TYPE_VIDEO;
    fps = 0;
}

ACVideo::~ACVideo() {
}

ACVideo::ACVideo(const ACVideo& m) :ACMedia()/* ACMedia(m)*/ {
	this->init();	
	fps = m.fps;
}	

/*cv::VideoCapture* ACVideo::getData()
{
	if (data == 0) {
		data = new ACVideoData(filename);
	}	
	return static_cast<cv::VideoCapture*> (data->getData());
}*/

//fills in data about the video
// - capture = pointer to the beginning
// - height, width
// - fps = frames per second
// - nframes = total number of frames in the video
bool ACVideo::extractData(string _fname){
	// XS todo : store the default header (16 below) size somewhere...
    width = this->getMediaData()->getWidth();
    height = this->getMediaData()->getHeight();
    fps = this->getMediaData()->getSampleRate();
    int nframes = this->getMediaData()->getNumberOfFrames();

    // test if start and end have been set from outside (segments)
    if(start == -1)
        start = 0;
    if (end == -1){
        if (fps != 0)
            end = (nframes-1) * 1.0/fps;
        else
            end = nframes-1; // time code of last frame
    }
    if (startInt == -1)
        startInt = 0;
    if (endInt == -1)
        endInt = nframes-1; // index of last frame
    return true;
}

// obsolete + confusing with thumbnail width/height
void ACVideo::saveACLSpecific(ofstream &library_file) {
    /*library_file << filename_thumbnail << endl;*/
	library_file << this->getDuration() << endl;
	library_file << width << endl;
    library_file << height << endl;
}

// obsolete + confusing with thumbnail width/height
int ACVideo::loadACLSpecific(ifstream &library_file) {
	std::string ghost;//CF somebody please explain me why this is required!
	getline(library_file, ghost, '\n');//CF somebody please explain me why this is required!
    /*getline(library_file, filename_thumbnail, '\n');
    library_file >> end;*/
	library_file >> width;
    library_file >> height;
	//int n_features = 0;//CF done in ACMedia
	//library_file >> n_features;//CF done in ACMedia

	// Old bug with image size set to thumbnail size
	// XS removed this !
//	if ((width == 16)&&(height == 16)){
//		cv::VideoCapture* tmp(filename.c_str());	
//		width = (int) tmp.get(CV_CAP_PROP_FRAME_WIDTH);
//		height = (int) tmp.get(CV_CAP_PROP_FRAME_HEIGHT);
//		tmp.release();
//		if ((width != 16)&&(height != 16))// if the image size isn't actually 64x64
//			std::cout << "Please re-save your ACL library, old format with corrupted video size." << std::endl;
//	}
	
    /*if (computeThumbnail(height, width) != 1){
		cerr << "<ACVideo::loadACLSpecific> : problem computing thumbnail" << endl;
		return 0;
    }*/
	return 1;
}

void ACVideo::saveXMLSpecific(TiXmlElement* _media){
    //_media->SetAttribute("thumbnailFileName", filename_thumbnail);
	_media->SetDoubleAttribute("Duration", this->getDuration());
	_media->SetAttribute("Width", width);
	_media->SetAttribute("Height", height);
	_media->SetAttribute("FrameRate", fps);
    _media->SetAttribute("FrameStart",this->getStartInt());
    _media->SetAttribute("FrameEnd",this->getEndInt());
}

int ACVideo::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int w=-1;
	int h=-1;
	int t_w=-1;
	int t_h=-1;
	float _fps=-1;

	// XS TODO is this one necessary ?
    //filename_thumbnail = _pMediaNode->Attribute("thumbnailFileName");
	
	double t=0;
	_pMediaNode->QueryDoubleAttribute("Duration", &t);

	if (t < 0){
		//throw runtime_error("corrupted XML file, wrong video frame rate");
        //cv::VideoCapture* capture = this->getData();
        //this->end = (double) capture->get(CV_CAP_PROP_FRAME_COUNT) * 1.0/(float) capture->get(CV_CAP_PROP_FPS);
        if(this->getMediaData()==0){
            std::cerr << "ACVideo::loadXMLSpecific: no media data set" << std::endl;
            return false;
        }
        if(this->getMediaData()->readData(this->filename)==false){
            std::cerr << "ACVideo::loadXMLSpecific: couldn't load file" << this->filename << std::endl;
            return false;
        }
        t = (double) this->getMediaData()->getNumberOfFrames() * 1.0/(float) this->getMediaData()->getSampleRate();
        //this->end = (double) this->getMediaData()->getNumberOfFrames() * 1.0/(float) this->getMediaData()->getSampleRate();
	}	
//	else{
//		this->end = t;
//	}
	
	_pMediaNode->QueryIntAttribute("Width", &w);
	if (w < 0)
		throw runtime_error("corrupted XML file, wrong video width");
	else
		this->width = w;
	
	_pMediaNode->QueryIntAttribute("Height", &h);
	if (h < 0)
		throw runtime_error("corrupted XML file, wrong video height");
	else
		this->height = h;
	
	_pMediaNode->QueryFloatAttribute("FrameRate", &_fps);
	if (_fps < 0){
		//throw runtime_error("corrupted XML file, wrong video frame rate");
        /*cv::VideoCapture* capture = this->getData();
        this->fps = (float) capture->get(CV_CAP_PROP_FPS);*/
        if(this->getMediaData()==0){
            std::cerr << "ACVideo::loadXMLSpecific: no media data set" << std::endl;
            return false;
        }
        if(this->getMediaData()->readData(this->filename)==false){
            std::cerr << "ACVideo::loadXMLSpecific: couldn't load file" << this->filename << std::endl;
            return false;
        }
        this->fps = (float) this->getMediaData()->getSampleRate();
	}	
	else
		this->fps = _fps;
	
    int frame_start(-1), frame_end(-1);
    _pMediaNode->QueryIntAttribute("FrameStart",&frame_start);
    _pMediaNode->QueryIntAttribute("FrameEnd",&frame_end);

    // test if start and end have been set from outside (segments)
    if(frame_start == -1){
        this->start = 0.0f;
        this->startInt = 0;
    }
    else{
        this->startInt = frame_start;
        this->start = (float)frame_start* 1.0f/fps;
    }
    if (frame_end == -1){
        if (this->fps != 0){
            this->endInt = t*this->fps;
        }
        else{
            this->endInt = t; // time code of last frame
        }
        this->end = t;
    }
    else{
        this->end = frame_end* 1.0f/fps;
        this->endInt = frame_end;
    }
	return 1;	
}
