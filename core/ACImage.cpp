/*
 *  ACImage[loop].cpp
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

#if !defined (APPLE_IOS)

#include "ACImage.h"
#include <fstream>

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;

// ----------- class constants
const int ACImage:: default_thumbnail_width = 128;
const int ACImage:: default_thumbnail_height = 128;
const int ACImage:: default_thumbnail_area = 16384;

//------------------------------------------------------------------

ACImage::ACImage() : ACMedia() {
	media_type = MEDIA_TYPE_IMAGE;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;	
}

ACImage::~ACImage() {
	if (thumbnail) {
		//cvReleaseImage(&thumbnail);
		thumbnail = 0;
	}
}

//osg::Image* ACImage::Convert_OpenCV_TO_OSG_IMAGE(IplImage* cvImg)
void ACImage::Convert_OpenCV_TO_OSG_IMAGE(IplImage* cvImg)
{
	
	// XS uncomment the following 4 lines for visual debug (e.g., thumbnail)	
	//	cvNamedWindow("T", CV_WINDOW_AUTOSIZE);
	//	cvShowImage("T", cvImg);
	//	cvWaitKey(0);
	//	cvDestroyWindow("T");	
	
	
	if(cvImg->nChannels == 3)
	{
		// Flip image from top-left to bottom-left origin
		if(cvImg->origin == 0) {
			cvConvertImage(cvImg , cvImg, CV_CVTIMG_FLIP);
			cvImg->origin = 1;
		}
		
		// Convert from BGR to RGB color format
		//printf("Color format %s\n",cvImg->colorModel);
		if ( !strcmp(cvImg->channelSeq,"BGR") ) {
			cvCvtColor( cvImg, cvImg, CV_BGR2RGB );
		}
		
		//osg::Image* osgImg = new osg::Image();
		thumbnail = new osg::Image();
		
		/*temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
		 for (i=0;i<cvImg->height;i++) {
		 memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
		 }*/
		
		//osgImg->setImage(
		thumbnail->setImage(
						 cvImg->width, //s
						 cvImg->height, //t
						 1, //r //CF needs to be 1 otherwise scaleImage can't be used
						 3, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
						 6407, // GLenum pixelFormat, (GL_RGB, 0x1907)
						 5121, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
						 (unsigned char*)(cvImg->imageData), // unsigned char* data
						 //temp_data,
						 osg::Image::NO_DELETE, // AllocationMode mode (shallow copy)
						 1);//int packing=1); (???)
		
		//printf("Conversion completed\n");
		//return osgImg;
	}
	// XS TODO : what happens with BW images ?
	else {
		cerr << "Unrecognized image type" << endl;
		//printf("Unrecognized image type");
		//return 0;
	}
	
}

// XS: when we load from file, there is no need to have a pointer to the data passed to the plugin
// XS TODO check this; could combine the following 2 methods...
int ACImage::computeThumbnail(string _fname, int w, int h){
	this->computeThumbnailSize();

	IplImage* cvImg = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	/*thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);*/
	
	//thumbnail = Convert_OpenCV_TO_OSG_IMAGE(cvImg);
	Convert_OpenCV_TO_OSG_IMAGE(cvImg);
	thumbnail->scaleImage(thumbnail_width,thumbnail_height, 1);
	thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	
	// Saving the video as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);
	
	//cvReleaseImage(&cvImg); // because  cvLoadImage == new()
	return 1;
}


int ACImage::computeThumbnail(ACMediaData* data_ptr, int w, int h){
	this->computeThumbnailSize();

	IplImage* cvImg = data_ptr->getImageData();
	/*thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);
*/
	
	//thumbnail = Convert_OpenCV_TO_OSG_IMAGE(cvImg);
	Convert_OpenCV_TO_OSG_IMAGE(cvImg);
	thumbnail->scaleImage(thumbnail_width,thumbnail_height, 1);
	thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	
	// Saving the video as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);

	// *NOT* cvReleaseImage(&cvImg); // because there is no new, we just access data_ptr->getImageData();
	return 1;
}

int ACImage::checkWidth(int w){
	if (w < 0){
		cerr << "<ACImage::checkWidth> width should be positive: " << w << endl;
		return -1;
	}
	else if (w == 0){
		w = default_thumbnail_width;
		cout << "<ACImage::checkWidth> using default width: " << w << endl;
	}
	return w;
}

int ACImage::checkHeight(int h){
	if (h < 0){
		cerr << "<ACImage::checkHeight> height should be positive: " << h << endl;
		return -1;
	}
	else if (h == 0){
		h = default_thumbnail_height;
		cout << "<ACImage::checkHeight> using default height: " << h << endl;
	}
	return h;
}

void ACImage::computeThumbnailSize(){
	if ((width !=0) && (height!=0)){
		float scale = sqrt((float)default_thumbnail_area/((float)width*(float)height));
		thumbnail_width = (int)(width*scale);
		thumbnail_height = (int)(height*scale);
	}
	else {
		std::cerr << "Image dimensions not set." << std::endl;
	}
}	

// returns a pointer to the data contained in the image
// AND computes a thumbnail at the same time.
//ACMediaData* ACImage::extractData(string fname){
void ACImage::extractData(string fname){
	data = new ACMediaData(MEDIA_TYPE_IMAGE,fname);
	width = data->getImageData()->width;
	height = data->getImageData()->height;
	this->computeThumbnailSize();
	computeThumbnail(data, thumbnail_width , thumbnail_height);
	//return data;
}

void ACImage::setData(IplImage* _data){
	if (data->getMediaType()==MEDIA_TYPE_NONE)
		data = new ACMediaData(MEDIA_TYPE_IMAGE);	
	else
		data->setMediaType(MEDIA_TYPE_IMAGE);
	data->setImageData(_data);
	this->height = _data->height;
	this->width = _data->width;
	this->computeThumbnailSize();
}

void ACImage::saveACLSpecific(ofstream &library_file) {
	// XS  TODO : or thumbnail width ??
	library_file << width << endl;
	library_file << height << endl;
}

int ACImage::loadACLSpecific(ifstream &library_file) {
	library_file >> width;
	library_file >> height;
	
	// Old bug with image size set to thumbnail size
	if ((width == 64)&&(height == 64)){
		IplImage* tmp = cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_COLOR);	
		width=tmp->width;
		height=tmp->height;
		cvReleaseImage(&tmp);
		if ((width != 64)&&(height != 64))// if the image size isn't actually 64x64
			std::cout << "Please re-save your ACL library, old format with corrupted image size." << std::endl;
	}

	data = new ACMediaData(MEDIA_TYPE_IMAGE,filename);
	
	this->computeThumbnailSize();
	if (computeThumbnail(data, thumbnail_width , thumbnail_height) != 1){
		cerr << "<ACImage::loadACLSpecific> : problem computing thumbnail" << endl;
		return 0;
	}
	return 1;
}
#endif//CF APPLE_IOS APPLE_IOS