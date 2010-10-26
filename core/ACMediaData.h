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

#include "ACMediaTypes.h"
#include <sndfile.h>
#include "ACOpenCVInclude.h"
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>
#include <string>

using std::string;

class ACMediaData {
public:
	ACMediaData();
	ACMediaData(string _fname, ACMediaType _type = MEDIA_TYPE_NONE);
	~ACMediaData();
	
	float* getAudioData() {return audio_ptr;}
	
#if !defined (APPLE_IOS)	
	IplImage* getImageData() {return image_ptr;}
	CvCapture* getVideoData() {return video_ptr;}
#endif	
	osg::Node* get3DModelData();
	
	void setAudioData(float* _data);
#if !defined (APPLE_IOS)
	void setImageData(IplImage* _data);
	void setVideoData(CvCapture* _data);
#endif	
	void set3DModelData(osg::ref_ptr< osg::Node > _data);
					  
	void readAudioData(string _fname);
#if !defined (APPLE_IOS)	
	void readImageData(string _fname);
	void readVideoData(string _fname);
#endif	
	void read3DModelData(string _fname);
	
	string getFileName() {return file_name;}
	void setFileName(string _fname);
	
private:
	ACMediaType media_type;
	string file_name;
	float* audio_ptr;
#if !defined (APPLE_IOS)	
	IplImage* image_ptr;
	CvCapture* video_ptr;
#endif	
	osg::Node* model_ptr;
};

#endif  // ACMEDIADATA_H
