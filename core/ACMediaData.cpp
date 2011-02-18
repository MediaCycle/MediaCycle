/*
 *  ACMediaData.cpp
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

// This class provides a container for the data belonging to each media
// One of the goals is to avoid ugly void* pointers
#include "ACMediaData.h"
#include <iostream>

using std::cerr;
using std::endl;
using std::string;

// ACMediaData is not base class, it works for all media types
// XS note: this does not seem optimal, because we need to change many things upong adding one new media type

ACMediaData::ACMediaData() { 
	#if defined (SUPPORT_AUDIO)
	audio_ptr = NULL;
	audio_frames = 0;
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	image_ptr = NULL;
	#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_VIDEO)
	video_ptr = NULL;
	#endif //defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_3DMODEL)
	model_ptr = NULL;
	#endif //defined (SUPPORT_3DMODEL)
}

ACMediaData::ACMediaData(ACMediaType _type, std::string _fname) {
	#if defined (SUPPORT_AUDIO)
	audio_ptr = NULL;
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	image_ptr = NULL;
	#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_VIDEO)
	video_ptr = NULL;
	#endif //defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_3DMODEL)
	model_ptr = NULL;
	#endif //defined (SUPPORT_3DMODEL)
	media_type = _type;
	if(_fname!=""){
		file_name=_fname;
		switch (_type) {
			case MEDIA_TYPE_AUDIO :
				#if defined (SUPPORT_AUDIO)
				readAudioData(_fname);
				#endif //defined (SUPPORT_AUDIO)
				break;
			case MEDIA_TYPE_IMAGE :
				#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
				readImageData(_fname);
				#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
				break;
			case MEDIA_TYPE_VIDEO :
				#if defined (SUPPORT_VIDEO)
				readVideoData(_fname);
				#endif //defined (SUPPORT_VIDEO)
				break;
			case MEDIA_TYPE_3DMODEL :
				#if defined (SUPPORT_3DMODEL)
				read3DModelData(_fname);
				#endif //defined (SUPPORT_3DMODEL)
				break;
			default:
				break;
		}
	}	
}

ACMediaData::~ACMediaData() {
#if defined (SUPPORT_AUDIO)
	if (audio_ptr != NULL) delete [] audio_ptr;
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	if (image_ptr != NULL) cvReleaseImage(&image_ptr);
#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
#if defined (SUPPORT_VIDEO)
	if (video_ptr != NULL) cvReleaseCapture(&video_ptr);
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_3DMODEL)
	if (model_ptr != NULL) { model_ptr->unref(); model_ptr=0; }
#endif //defined (SUPPORT_3DMODEL)
}

#if defined (SUPPORT_AUDIO)
void ACMediaData::readAudioData(string _fname){ 
	SF_INFO sfinfo;
	SNDFILE* testFile;
	if (! (testFile = sf_open (_fname.c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s.\n", _fname.c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		exit(1);
	}
	audio_ptr = new float[(long) sfinfo.frames * sfinfo.channels];
	audio_frames = sfinfo.frames;
	sf_readf_float(testFile, audio_ptr, sfinfo.frames);
	sf_close(testFile);
}

void ACMediaData::setAudioData(float* _data, float _sample_number){
	audio_frames = _sample_number;
	audio_ptr = (float *)malloc( _sample_number * sizeof( float ));
	memcpy(audio_ptr,_data,audio_frames* sizeof( float ));
	if( !audio_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACMediaData::setAudioData> Could not set data" << endl;
	}
}
#endif //defined (SUPPORT_AUDIO)

#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
void ACMediaData::readImageData(string _fname){ 
	image_ptr = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	try {
		if (!image_ptr) {
			cerr << "Check file name : " << _fname << endl;
			throw(string(" <ACMediaData::readImageData> CV_LOAD_IMAGE_COLOR : not a color image !"));
		}
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
	}
}

void ACMediaData::setImageData(IplImage* _data){
	cvCopy(_data,image_ptr);		
	if( !image_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACMediaData::setImageData> Could not set data" << endl;
	}
	
}
#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)

#if defined (SUPPORT_VIDEO)
void ACMediaData::readVideoData(string _fname){
	video_ptr = cvCreateFileCapture(_fname.c_str());		
	if( !video_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACMediaData::readImageData> Could not initialize capturing from file " << _fname << endl;
	}

}

void ACMediaData::setVideoData(CvCapture* _data){
	cvCopy(_data,video_ptr);		
	if( !video_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACMediaData::setVideoData> Could not set data" << endl;
	}
	
}
#endif //defined (SUPPORT_VIDEO)

#if defined (SUPPORT_3DMODEL)
void ACMediaData::read3DModelData(string _fname){ 
	
	if (model_ptr != NULL) { model_ptr->unref(); model_ptr=0; }
	
	model_ptr = osgDB::readNodeFile(_fname);
	model_ptr->ref();
    if( model_ptr == NULL ) {
		cerr << "<ACMediaData::read3DModelData> file can not be read !" << endl;
     }
}

void ACMediaData::set3DModelData(osg::ref_ptr< osg::Node > _data)
{
	model_ptr = dynamic_cast<osg::Node*>( _data->clone(osg::CopyOp::DEEP_COPY_ALL));		
	if( !model_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACMediaData::set3DModelData> Could not set data" << endl;
	}
	
}
#endif //defined (SUPPORT_3DMODEL)

bool ACMediaData::copyData(ACMediaData* m){
	bool success = false;
	switch (media_type) {
		case MEDIA_TYPE_AUDIO :
			#if defined (SUPPORT_AUDIO)
			audio_ptr = (float *)malloc( m->getAudioLength() * sizeof( float ));
			memcpy(audio_ptr,m->getAudioData(),m->getAudioLength()* sizeof( float ));
			if( audio_ptr )
				success=true;
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE :
			#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
			cvCopy(m->getImageData(),image_ptr);
			if( image_ptr )
				success=true;
			#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_VIDEO :
			#if defined (SUPPORT_VIDEO)
			cvCopy(m->getVideoData(),video_ptr);
			if( video_ptr )
				success=true;
			#endif //defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_3DMODEL :
			#if defined (SUPPORT_3DMODEL)
			model_ptr = dynamic_cast<osg::Node*>( m->get3DModelData()->clone(osg::CopyOp::DEEP_COPY_ALL));
			if( model_ptr )
				success=true;
			#endif //defined (SUPPORT_3DMODEL)
			break;
		default:
			break;
	}
	return success;
}	
