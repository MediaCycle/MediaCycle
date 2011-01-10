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
#include <sndfile.h>
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>
#include <string>

using std::string;

class ACMediaData {
public:
	ACMediaData();
	ACMediaData(ACMediaType _type = MEDIA_TYPE_NONE,string _fname="");
	~ACMediaData();
	
	void readAudioData(string _fname);
	void setAudioData(float* _data, float _sample_number);
	float* getAudioData() {return audio_ptr;}
	float getAudioLength() {return audio_frames;}
	
#if !defined (APPLE_IOS)
	void readImageData(string _fname);
	void setImageData(IplImage* _data);	
	IplImage* getImageData() {return image_ptr;}
	
	void readVideoData(string _fname);
	void setVideoData(CvCapture* _data);
	CvCapture* getVideoData() {return video_ptr;}
#endif	

	void read3DModelData(string _fname);
	void set3DModelData(osg::ref_ptr< osg::Node > _data);
	osg::Node* get3DModelData(){return model_ptr;}
	
	string getFileName() {return file_name;}
	void setFileName(string _fname);
	ACMediaType getMediaType(){return media_type;}
	void setMediaType(ACMediaType _media_type){media_type=_media_type;}
	bool copyData(ACMediaData* m);
	
private:
	ACMediaType media_type;
	string file_name;
	float* audio_ptr;
	float audio_frames;
#if !defined (APPLE_IOS)	
	IplImage* image_ptr;
	CvCapture* video_ptr;
#endif	
	osg::Node* model_ptr;
};

#endif  // ACMEDIADATA_H
