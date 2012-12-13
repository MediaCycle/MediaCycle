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

#include <osg/ImageUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <boost/filesystem.hpp>

#include <iostream>
using namespace std;

// ----------- class constants
const int ACVideo:: default_thumbnail_area = 4096; // 64 * 64

ACVideo::ACVideo() : ACMedia() {
    this->init();
}

void ACVideo::init() {
    media_type = MEDIA_TYPE_VIDEO;
    thumbnail = 0;
    thumbnail_width = 0;
    thumbnail_height = 0;
    data = 0;
    fps = 0;
}

ACVideo::~ACVideo() {
	if (image_stream) image_stream->quit();	
	//cvReleaseImage(&thumbnail);
	/*if (image_stream){
		osg::ImageStream::StreamStatus streamStatus = image_stream->getStatus();
		switch (streamStatus) {
			case osg::ImageStream::INVALID:
				//std::cout << "Image stream invalid status" << std::endl;
				break;
			case osg::ImageStream::PLAYING:
				image_stream->pause();
				break;
			case osg::ImageStream::PAUSED:
				break;
			case osg::ImageStream::REWINDING:
				//std::cout << "Image stream rewinding" << std::endl;
				break;
			default:
				break;
		}
	}*/	
// XS TODO  pas 2x (déjà ds ACMedia)	
//	if (data) delete data;
    
	image_stream=0;	
	thumbnail=0;
    image_texture=0;
}

ACVideo::ACVideo(const ACVideo& m) :ACMedia()/* ACMedia(m)*/ {
	this->init();	
	thumbnail = m.thumbnail;
	thumbnail_width = m.thumbnail_width;
	thumbnail_height = m.thumbnail_height;
	// Should I copy the thumbnail ?
	fps = m.fps;
}	

bool ACVideo::computeThumbnail(int w, int h){
	if (!computeThumbnailSize(w, h)) return false;
	bool ok = true;
	// option 1 : use openCV
	//CvCapture* capture = data_ptr->getData();
		
	// take thumbnail in the middle of the video...
	/*int nframes = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, nframes/2); 	
	
	if(!cvGrabFrame(capture)){
		cerr << "<ACVideo::computeThumbnail> Could not find frame..." << endl;
		return -1;
	}
	
	//XS use cloneImage, otherwise thumbnail gets destroyed along with cvCapture
	IplImage* tmp = cvRetrieveFrame(capture);
	thumbnail = cvCloneImage(tmp);*/
	
	//CF we should compute the following in a separate thread
	
	// option 2 : Loading the movie with OSG
	//std::cout << boost::filesystem::extension(filename);
	/// prerequisites for loading OSG media files, 2 alternatives
	/// 1) standard procedure: checking for a plugin that can open the format of the media file
//	osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(filename).substr(1));
	/// 2) hack: forcing the use of the ffmpeg plugin by checking the plugin that can open the ffmpeg format (most probably the ffmpeg plugin)
	osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
	if (!readerWriter){
		cerr << "<ACVideo::computeThumbnail> (video id = " << this->getId() << ") : problem loading file, no OSG plugin available" << endl;
		return false;
	}
	else{
		cout <<"<ACVideo::computeThumbnail> (video id = " << this->getId() << ") using OSG plugin: "<< readerWriter->className() <<std::endl;
	}	

    thumbnail = osgDB::readImageFile(filename);
	readerWriter = 0;
	// XS TODO : needs rescaling !!
	//thumbnail->scaleImage(thumbnail_width,thumbnail_height,1);
	//thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
		cerr << "<ACVideo::computeThumbnail> problem creating thumbnail" << endl;
		return false;
	}
	
	// Saving the video as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);
	
	// Converting the video as preloaded stream to transmit the same instance to multiple recipient with unified playback controls
	image_stream = dynamic_cast<osg::ImageStream*>(thumbnail.get());
	image_stream->setLoopingMode(osg::ImageStream::LOOPING); // todo: this won't work for segments which may not loop back from the beginning of the file
	
	// Hack to display a first valid frame, quite long!
	//while (thumbnail->isImageTranslucent())
	//	image_stream->play();
	//image_stream->pause();
	//image_stream->rewind();
	
	image_stream->seek(this->start); // to start with the correct frame, especially for segments
	
	// CF: Adding these two blank the geodes under OSX 10.6.8
	//image_stream->play(); // to display a frame
	//image_stream->pause(); // to stop the playback once a frame is displayed
	
	return ok;
}

cv::VideoCapture* ACVideo::getData()
{
	if (data == 0) {
		data = new ACVideoData(filename);
	}	
	return static_cast<cv::VideoCapture*> (data->getData());
}

//fills in data about the video
// - capture = pointer to the beginning
// - height, width
// - fps = frames per second
// - nframes = total number of frames in the video
bool ACVideo::extractData(string _fname){
	// XS todo : store the default header (16 below) size somewhere...
	this->filename = _fname;
	cv::VideoCapture* capture = this->getData();
	width = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
	height = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
	fps     = (float) capture->get(CV_CAP_PROP_FPS);
	int nframes = (int) capture->get(CV_CAP_PROP_FRAME_COUNT);
	start = 0.0;
	if (fps != 0) end = (nframes-1) * 1.0/fps;
        else end = nframes-1; // time code of last frame
        // test if start and end have been set from outside
        if (startInt == -1)
            startInt = 0;
        if (endInt == -1)
            endInt = nframes-1; // index of last frame
	// XS TODO : do this here ??
	return computeThumbnail();
}

void ACVideo::deleteData(){
	if (data)
		delete data;
	data=0;
}

void ACVideo::setData(cv::VideoCapture* _data){
	if (data == 0)
		data = new ACVideoData();	
	data->setData(_data);
	
	width = (int) _data->get(CV_CAP_PROP_FRAME_WIDTH);
	height = (int) _data->get(CV_CAP_PROP_FRAME_HEIGHT);
//	this->computeThumbnailSize();
	
	fps     = (int) _data->get(CV_CAP_PROP_FPS);
	int nframes = (int) _data->get(CV_CAP_PROP_FRAME_COUNT);
	start = 0.0;
	if (fps != 0) end = nframes * 1.0/fps;
	else end = nframes;
}

// obsolete + confusing with thumbnail width/height
void ACVideo::saveACLSpecific(ofstream &library_file) {
	library_file << filename_thumbnail << endl;
	library_file << this->getDuration() << endl;
	library_file << width << endl;
	library_file << height << endl;
}

// obsolete + confusing with thumbnail width/height
int ACVideo::loadACLSpecific(ifstream &library_file) {
	std::string ghost;//CF somebody please explain me why this is required!
	getline(library_file, ghost, '\n');//CF somebody please explain me why this is required!
	getline(library_file, filename_thumbnail, '\n');
	library_file >> end;
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
	
	if (computeThumbnail(height, width) != 1){
		cerr << "<ACVideo::loadACLSpecific> : problem computing thumbnail" << endl;
		return 0;
	}
	return 1;
}

void ACVideo::saveXMLSpecific(TiXmlElement* _media){
	_media->SetAttribute("thumbnailFileName", filename_thumbnail);
	_media->SetDoubleAttribute("Duration", this->getDuration());
	_media->SetAttribute("Width", width);
	_media->SetAttribute("Height", height);
	_media->SetAttribute("FrameRate", fps);
	_media->SetAttribute("ThumbnailWidth", thumbnail_width);
	_media->SetAttribute("ThumbnailHeight", thumbnail_height);
}

int ACVideo::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int w=-1;
	int h=-1;
	int t_w=-1;
	int t_h=-1;
	float _fps=-1;

	// XS TODO is this one necessary ?
	filename_thumbnail = _pMediaNode->Attribute("thumbnailFileName");
	
	double t=0;
	_pMediaNode->QueryDoubleAttribute("Duration", &t);
	// XS TODO is duration always end-start ?
	this->start = 0;
	if (t < 0){
		//throw runtime_error("corrupted XML file, wrong video frame rate");
		cv::VideoCapture* capture = this->getData();
		this->end = (double) capture->get(CV_CAP_PROP_FRAME_COUNT) * 1.0/(float) capture->get(CV_CAP_PROP_FPS);
	}	
	else{
		this->end = t;
	}
	
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
		cv::VideoCapture* capture = this->getData();
		this->fps = (float) capture->get(CV_CAP_PROP_FPS);
	}	
	else
		this->fps = _fps;
	
	_pMediaNode->QueryIntAttribute("ThumbnailWidth", &t_w);
	_pMediaNode->QueryIntAttribute("ThumbnailHeight", &t_h);
	
	if (computeThumbnail (t_w , t_h) != 1)
		throw runtime_error("<ACVideo::loadXMLSpecific> : problem computing thumbnail");

	// not necessary
	//	data = new ACVideoData(filename);
	
	return 1;	
}

// cut and paste from ACImage
// XS TODO should put this in ACSpatialMedia
bool ACVideo::computeThumbnailSize(int w_, int h_){
	// we really want a specific (positive) thumbnail size
	bool ok = true;
	if ((w_ > 0) && (h_ > 0)) {
		thumbnail_width = w_;
		thumbnail_height = h_;
	}
	// we just scale the original width and height to the default thumbnail area
        else if ((width !=0) && (height!=0)){
		float scale = sqrt((float)default_thumbnail_area/((float)width*(float)height));
		thumbnail_width = (int)(width*scale);
		thumbnail_height = (int)(height*scale);
	}
	else {
		std::cerr << "Video dimensions not set." << std::endl;
		ok = false;
	}
	return ok;
}