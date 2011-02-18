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

#include "ACOpenCVInclude.h"
#include "ACMediaTypes.h"
#if defined (SUPPORT_AUDIO)
#include <sndfile.h>
#endif //defined (SUPPORT_AUDIO)
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>
#include <string>

class ACMediaData {
public:
	ACMediaData();
	ACMediaData(ACMediaType _type = MEDIA_TYPE_NONE,std::string _fname="");
	~ACMediaData();
	
	#if defined (SUPPORT_AUDIO)
	void readAudioData(std::string _fname);
	void setAudioData(float* _data, float _sample_number);
	float* getAudioData() {return audio_ptr;}
	float getAudioLength() {return audio_frames;}
	#endif //defined (SUPPORT_AUDIO)

	#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	void readImageData(std::string _fname);
	void setImageData(IplImage* _data);	
	IplImage* getImageData() {return image_ptr;}
	#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	
	#if defined (SUPPORT_VIDEO)
	void readVideoData(std::string _fname);
	void setVideoData(CvCapture* _data);
	CvCapture* getVideoData() {return video_ptr;}
	#endif //defined (SUPPORT_VIDEO)

	#if defined (SUPPORT_3DMODEL)
	void read3DModelData(std::string _fname);
	void set3DModelData(osg::ref_ptr< osg::Node > _data);
	osg::Node* get3DModelData(){return model_ptr;}
	#endif //defined (SUPPORT_3DMODEL)
	
	std::string getFileName() {return file_name;}
	void setFileName(std::string _fname);
	ACMediaType getMediaType(){return media_type;}
	void setMediaType(ACMediaType _media_type){media_type=_media_type;}
	bool copyData(ACMediaData* m);
	
private:
	ACMediaType media_type;
	std::string file_name;
	#if defined (SUPPORT_AUDIO)
	float* audio_ptr;
	float audio_frames;
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	IplImage* image_ptr;
	#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_VIDEO)
	CvCapture* video_ptr;
	#endif //defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_3DMODEL)
	osg::Node* model_ptr;
	#endif //defined (SUPPORT_3DMODEL)
};

#endif  // ACMEDIADATA_H
