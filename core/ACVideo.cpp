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

#if defined (SUPPORT_VIDEO)

#include "ACVideo.h"

#include <osg/ImageUtils>
#include <osgDB/ReaderWriter>
#include "boost/filesystem.hpp"

#include <iostream>
using namespace std;

// ----------- class constants
const int ACVideo:: default_thumbnail_width = 64;
const int ACVideo:: default_thumbnail_height = 64;
const int ACVideo:: default_thumbnail_area = 4096;

ACVideo::ACVideo() : ACMedia()
{
	this->init();
}	

void ACVideo::init()
{
	media_type = MEDIA_TYPE_VIDEO;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;
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
}

ACVideo::ACVideo(const ACVideo& m) : ACMedia(m) {
	this->init();	
	thumbnail = m.thumbnail;
	thumbnail_width = m.thumbnail_width;
	thumbnail_height = m.thumbnail_height;
	// Should I copy the thumbnail ?
}	

int ACVideo::computeThumbnail(int w, int h){
	this->computeThumbnailSize();
	//CvCapture* capture = data_ptr->getVideoData();
		
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
	
	// Loading the movie with OSG
	std::cout << boost::filesystem::extension(filename);
	/// prerequisites for loading OSG media files, 2 alternatives
	/// 1) standard procedure: checking for a plugin that can open the format of the media file
	//osgDB::ReaderWriter* readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(filename).substr(1));
	/// 2) hack: forcing the use of the ffmpeg plugin by checking the plugin that can open the ffmpeg format (most probably the ffmpeg plugin)
	osgDB::ReaderWriter* readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
	if (!readerWriter){
		cerr << "<ACVideo::computeThumbnail> problem loading file, no OSG plugin available" << endl;
		return -1;
	}

	osg::ref_ptr<osg::Image> thumbnail = osgDB::readImageFile(filename);
	//thumbnail->scaleImage(thumbnail_width,thumbnail_height,1);
	//thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
		cerr << "<ACVideo::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	
	// Saving the video as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);
	
	// Converting the video as preloaded stream to transmit the same instance to multiple recipient with unified playback controls
	image_stream = dynamic_cast<osg::ImageStream*>(thumbnail.get());
	image_stream->setLoopingMode(osg::ImageStream::LOOPING);
	
	// Hack to display a first valid frame, quite long!
	//while (thumbnail->isImageTranslucent())
	//	image_stream->play();
	//image_stream->pause();
	//image_stream->rewind();
	
	return 1;
}

CvCapture* ACVideo::getData()
{
	if (data == 0) {
		data = new ACMediaData(MEDIA_TYPE_VIDEO,filename);
	}	
	return data->getVideoData();
}

//ACMediaData* ACVideo::extractData(string _fname){
void ACVideo::extractData(string _fname){
	// XS todo : store the default header (16 below) size somewhere...
	//ACMediaData* video_data = new ACMediaData(MEDIA_TYPE_VIDEO,_fname);
	this->filename = _fname;
	CvCapture* capture = this->getData();
	width = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	height = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int fps     = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int nframes = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	start = 0.0;
	if (fps != 0) end = nframes * 1.0/fps;
	else end = nframes;
	this->computeThumbnailSize();
	computeThumbnail(16, 16);
}

void ACVideo::setData(CvCapture* _data){
	/*if (data->getMediaType()==MEDIA_TYPE_NONE)
		data = new ACMediaData(MEDIA_TYPE_VIDEO);	
	else
		data->setMediaType(MEDIA_TYPE_VIDEO);*/
	if (data == 0)
		data = new ACMediaData(MEDIA_TYPE_VIDEO);	
	data->setVideoData(_data);
	
	width = (int) cvGetCaptureProperty(_data, CV_CAP_PROP_FRAME_WIDTH);
	height = (int) cvGetCaptureProperty(_data, CV_CAP_PROP_FRAME_HEIGHT);
	this->computeThumbnailSize();
	
	int fps     = (int) cvGetCaptureProperty(_data, CV_CAP_PROP_FPS);
	int nframes = (int) cvGetCaptureProperty(_data,  CV_CAP_PROP_FRAME_COUNT);
	start = 0.0;
	if (fps != 0) end = nframes * 1.0/fps;
	else end = nframes;
}

void ACVideo::saveACLSpecific(ofstream &library_file) {

	library_file << filename_thumbnail << endl;
	library_file << this->getDuration() << endl;
	library_file << width << endl;
	library_file << height << endl;
}

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
	if ((width == 16)&&(height == 16)){
		CvCapture* tmp = cvCreateFileCapture(filename.c_str());	
		width = (int) cvGetCaptureProperty(tmp, CV_CAP_PROP_FRAME_WIDTH);
		height = (int) cvGetCaptureProperty(tmp, CV_CAP_PROP_FRAME_HEIGHT);
		cvReleaseCapture(&tmp);
		if ((width != 16)&&(height != 16))// if the image size isn't actually 64x64
			std::cout << "Please re-save your ACL library, old format with corrupted video size." << std::endl;
	}
	
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
}

int ACVideo::loadXMLSpecific(TiXmlElement* _pMediaNode){
	// XS TODO add checks
	filename_thumbnail = _pMediaNode->Attribute("thumbnailFileName");
	double t=0;
	_pMediaNode->QueryDoubleAttribute("Duration", &t);
	// XS TODO is duration always end-start ?
	int w=0;
	_pMediaNode->QueryIntAttribute("Width", &w);
	int h=0;
	_pMediaNode->QueryIntAttribute("Height", &h);
	this->width = w;
	this->height = h;
	
	data = new ACMediaData(MEDIA_TYPE_IMAGE,filename);
	
	if (computeThumbnail(width, height) != 1){
		cerr << "<ACVideo::loadXMLSpecific> : problem computing thumbnail" << endl;
		return 0;
	}
	return 1;	
}


void ACVideo::computeThumbnailSize(){
	if ((width !=0) && (height!=0)){
		float scale = sqrt((float)default_thumbnail_area/((float)width*(float)height));
		thumbnail_width = (int)(width*scale);
		thumbnail_height = (int)(height*scale);
	}
	else {
		std::cerr << "Video dimensions not set." << std::endl;
	}
}

/*
void ACVideo::save(FILE* library_file) {
	int i, j;
	int n_features;
	int n_features_elements;
	
	fprintf(library_file, "%s\n", filename.c_str());
	fprintf(library_file, "%s\n", filename_thumbnail.c_str());
	
#ifdef SAVE_LOOP_BIN
	fwrite(&mid,sizeof(int),1,library_file);
	fwrite(&width,sizeof(int),1,library_file);
	fwrite(&height,sizeof(int),1,library_file);
	n_features = features_vectors.size();
	fwrite(&n_features,sizeof(int),1,library_file);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->size();
		fwrite(&n_features_elements,sizeof(int),1,library_file);
		for (j=0; j<n_features_elements; j++) {
			value = features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
			fwrite(&value,sizeof(float),1,library_file);
		}
	}
#else
  fprintf(library_file, "%f\n", end);
	fprintf(library_file, "%d\n", mid);
	fprintf(library_file, "%d\n", width);
	fprintf(library_file, "%d\n", height);
	n_features = features_vectors.size();
	fprintf(library_file, "%d\n", n_features);
	for (i=0; i<features_vectors.size();i++) {
		n_features_elements = features_vectors[i]->getSize();
		fprintf(library_file, "%s\n", features_vectors[i]->getName().c_str());
		fprintf(library_file, "%d\n", n_features_elements);
		for (j=0; j<n_features_elements; j++) {
			fprintf(library_file, "%f\t", features_vectors[i]->getFeatureElement(j)); // XS instead of [i][j]
		}
		fprintf(library_file, "\n");
	}
#endif
}

int ACVideo::load(FILE* library_file) { // was loadLoop
	int i, j;
	int path_size;
	int n_features;
	int n_features_elements;
	char featureName[256];
	
	int ret;
	char *retc;
	
	ACMediaFeatures* mediaFeatures;
	float local_feature;
	
	char *file_temp;
	file_temp = new char[1024];
	memset(file_temp,0,1024);
	char *file_temp2;
	file_temp2 = new char[1024];
	memset(file_temp2,0,1024);
	
	//char file_temp[1024];
	//memset(file_temp,0,1024);
	//char file_temp2[1024];
	//memset(file_temp2,0,1024);
	
	retc = fgets(file_temp, 1024, library_file);
	
	if (retc) {
		path_size = strlen(file_temp);
		this->filename = string(file_temp, path_size-1);
		if (this->filename.size()==0){
			cerr << "ACVIdeo : Empty filename" << endl;
			exit(-1);
		}
		retc = fgets(file_temp2, 1024, library_file);
		path_size = strlen(file_temp2);
		this->filename_thumbnail = string(file_temp2, path_size-1);
		if (this->filename_thumbnail.size()==0){
			cerr << "ACVIdeo : Empty thumbnail filename" << endl;
			exit(-1);
		}
		
		ret = fscanf(library_file, "%f", &end);
		ret = fscanf(library_file, "%d", &mid);
		ret = fscanf(library_file, "%d", &width);
		ret = fscanf(library_file, "%d", &height);
		ret = fscanf(library_file, "%d", &n_features);
		
		thumbnail_height = 180/2;
		 _width = 320/2;
		width = 320/2;
		height = 180/2;	

		for (i=0; i<n_features;i++) {
			mediaFeatures = new ACMediaFeatures();
			features_vectors.push_back(mediaFeatures);
			features_vectors[i]->setComputed();
			ret = fscanf(library_file, "%s", featureName);
			features_vectors[i]->setName(string(featureName));
			ret = fscanf(library_file, "%d", &n_features_elements);
			features_vectors[i]->resize(n_features_elements);
			for (j=0; j<n_features_elements; j++) {
				ret = fscanf(library_file, "%f", &(local_feature));
				features_vectors[i]->setFeatureElement(j, local_feature);
			}
		}
		ret = fscanf(library_file, "\n");
		delete file_temp;
		delete file_temp2;
		return 1;
	}
	else {
		delete file_temp;
		delete file_temp2;
		return 0;
	}
}
*/
#endif //defined (SUPPORT_VIDEO)
