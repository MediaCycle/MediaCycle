/*
 *  ACImage.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
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

#if defined (SUPPORT_IMAGE) 

#include "ACImage.h"
#include <fstream>
#include <osg/ImageUtils>
#include <boost/filesystem.hpp>

using namespace std;

// ----------- class constants
const int ACImage:: default_thumbnail_area = 16384; // 128*128

//------------------------------------------------------------------

ACImage::ACImage() : ACMedia() {
	this->init();
}

void ACImage::init() {
	media_type = MEDIA_TYPE_IMAGE;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
	data=0;
}	

// copy-constructor, used for example to generate fragments
ACImage::ACImage(const ACImage& m, bool reduce) : ACMedia(m) {
	this->init();
	// ... XS TODO
}

ACImage::~ACImage() {
	this->deleteData();
	// the osg pointers should be automatically deleted (ref_ptr)
}

// used when we load from XML file
// so there is no need to compute a pointer to the data passed to the plugin
// in this case w and h can be read from XML file
bool ACImage::computeThumbnail(string _fname, int w, int h){
	bool ok = true;
	if (_fname == "")
		ok=false;
		//throw runtime_error("corrupted XML file, no filename");
	else {
//		fixWhiteSpace(_fname);
		this->setThumbnailFileName(_fname);
	}
	IplImage* cvImg;
	cvImg = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	if (!cvImg) ok = false;
	else {
		ok = this->computeThumbnail(cvImg, w, h);	
		cvReleaseImage(&cvImg); // because  cvLoadImage == new()
	}
	return ok;
}

// used in extractData (when features are computed on-the-fly, not read from XML)
// in this case w=0 and h=0 (generally)
bool ACImage::computeThumbnail(ACImageData* data_ptr, int w, int h){
	bool ok = true;
	IplImage* cvImg = static_cast<IplImage*>(data_ptr->getData());
		
	if (!cvImg) ok = false;
	else 
		ok = this->computeThumbnail(cvImg, w, h);
	
	// *NOT* cvReleaseImage(&cvImg); // because there is no new, we just access data_ptr->getData();
	return ok;
}

bool ACImage::computeThumbnail(IplImage* img, int w, int h){
	if (!img) return false;
	if (!computeThumbnailSize(w, h)) return false;
	bool ok = true;

    // option 1) using only OpenCV:
	//thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), imgp_full->depth, imgp_full->nChannels);
	//cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);
	
    // option 2) converting from OpenCV to OSG
    // CF: crashes with imageCLEF
    //this->thumbnail = this->openCVToOSG(img,thumbnail_width,thumbnail_height);

    // option 3) using OSG -- with ref_ptr to ensure proper garbage collection
    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(filename).substr(1));

    if (!readerWriter){
        cerr << "<ACImage::computeThumbnail> (image id = " << this->getId() << ") : problem loading file, no OSG plugin available" << endl;
        return false;
    }
    else{
        cout <<"<ACImage::computeThumbnail> (image id = " << this->getId() << ") using OSG plugin: "<< readerWriter->className() <<std::endl;
    }

    thumbnail = osgDB::readImageFile(filename);
    readerWriter = 0;

	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem converting thumbnail to osg" << endl;
		ok= false;
	}
	else{
		image_texture = new osg::Texture2D;
		image_texture->setImage(thumbnail);
	}
	return ok;
}

bool ACImage::computeThumbnailSize(int w_, int h_){
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
		std::cerr << "Image dimensions not set." << std::endl;
		ok = false;
	}
	return ok;
}	

// 1) extracts data 
// - IplImage
// - height, width
// 2) computes thumbnail
bool ACImage::extractData(string fname){
	// XS TODO return error message if it did not work
	if (data) delete data;
	data = new ACImageData(fname);
	
	if (!data) {
		cerr << "<ACImage::extractData> no data in file: " << fname << endl;
		return false;
	}
	IplImage* tmp_im = this->getData();
	
	// XS TODO FIX THIS COPYCAT
	if (!data) {
		cerr << "<ACImage::extractData> can't extract data from file: " << fname << endl;
		return false;
	}
	// original image width
	width = tmp_im->width;
	height = tmp_im->height;
	// thumbnail_width and thumbnail_height are almost always zero
	return computeThumbnail(data, thumbnail_width , thumbnail_height);
}

void ACImage::setData(IplImage* _data){
	if (data == 0)
		data = new ACImageData();
	data->setData(_data);
	this->height = _data->height;
	this->width = _data->width;
}

void ACImage::deleteData(){
	if (data)
		delete data;
	data=0;
}

// obsolete + confusing with thumbnail width/height
void ACImage::saveACLSpecific(ofstream &library_file) {
	library_file << width << endl;
	library_file << height << endl;
}

// obsolete + confusing with thumbnail width/height
int ACImage::loadACLSpecific(ifstream &library_file) {
	library_file >> width;
	library_file >> height;
	data = new ACImageData(filename);
	if (computeThumbnail(data, thumbnail_width , thumbnail_height) != 1){
		cerr << "<ACImage::loadACLSpecific> : problem computing thumbnail" << endl;
		delete data;
		return 0;
	}
	delete data;
	return 1;
}

void ACImage::saveXMLSpecific(TiXmlElement* _media){
	_media->SetAttribute("Width", width);
	_media->SetAttribute("Height", height);
	_media->SetAttribute("ThumbnailWidth", thumbnail_width);
	_media->SetAttribute("ThumbnailHeight", thumbnail_height);
}


int ACImage::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int w=-1;
	int h=-1;
	int t_w=-1;
	int t_h=-1;

	_pMediaNode->QueryIntAttribute("Width", &w);
	if (w < 0)
		throw runtime_error("corrupted XML file, wrong image width");
	else
		this->width = w;

	_pMediaNode->QueryIntAttribute("Height", &h);
	if (h < 0)
		throw runtime_error("corrupted XML file, wrong image height");
	else
		this->height = h;
	
	// try to read thumbnail size
	// if undefined or missing attribute, t_w or t_h will be < 0
	// and computeThumbnailSize will take care of it
	
	_pMediaNode->QueryIntAttribute("ThumbnailWidth", &t_w);
	_pMediaNode->QueryIntAttribute("ThumbnailHeight", &t_h);
	
	if (!computeThumbnail(filename, t_w , t_h))
		throw runtime_error("<ACImage::loadXMLSpecific> : problem computing thumbnail");

	return 1;	
}

osg::ref_ptr<osg::Image> ACImage::openCVToOSG(IplImage* cvImg, int sx, int sy) {		
	if(cvImg->nChannels == 3) {		
		osg::ref_ptr<osg::Image> osgImg = new osg::Image();
		
		// "clean" treatment of cvImg->widthStep
		unsigned char *temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
		for (int i=0;i<cvImg->height;i++) {
			memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
		}
		
		// GL_UNSIGNED_BYTE = IPL_DEPTH_8U
		// GL_BGR = how openCV loaded the image (even if it says RGB...)	
		int pixel_format = GL_BGR;
		osgImg->setImage(
						 cvImg->width, //s
						 cvImg->height, //t
						 1, //r //CF needs to be 1 otherwise scaleImage can't be used
						 3, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
						 pixel_format, // // GLenum pixelFormat, (GL_RGB, 0x1907)
						 GL_UNSIGNED_BYTE, //5121, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
						 //(unsigned char*)(cvImg->imageData), // unsigned char* data
						 temp_data,
						 osg::Image::NO_DELETE, // AllocationMode mode (shallow copy)
						 1);//int packing=1); (???)
		
		if (cvImg->origin == IPL_ORIGIN_TL) {
			// usually this is the case
			osgImg->setOrigin(osg::Image::TOP_LEFT);
			osgImg->flipVertical();
		}
		else
			osgImg->setOrigin(osg::Image::BOTTOM_LEFT);
		
		if (osgImg->valid() && sx>0 && sy >0){
			osgImg->scaleImage(sx,sy, 1);
		}
		// XS uncomment the following 4 lines for visual debug (e.g., thumbnail)	
//		cvNamedWindow("5", CV_WINDOW_AUTOSIZE);
//		cvShowImage("5", cvImg);
//		cvWaitKey(0);
//		cvDestroyWindow("5");	
		
		return osgImg;
	}
	// XS TODO : what happens with BW images (or alpha channel) ?
	else {
		cerr << "Unrecognized image type : needs 3 channels" << endl;
		return 0;
	}
	
}

#endif //defined (SUPPORT_IMAGE)
