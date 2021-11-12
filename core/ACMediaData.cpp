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
#include "ACMediaData.h"
#include <iostream>

using std::cerr;
using std::endl;
using std::string;

ACMediaData::ACMediaData() { 
	this->init();
}

void ACMediaData::init() {
	media_type = MEDIA_TYPE_NONE;
	file_name = "";
}	

ACMediaData::~ACMediaData() {
//media-specific
}

//bool ACMediaData::copyData(ACMediaData* m){
//	bool success = false;
//	switch (media_type) {
//		case MEDIA_TYPE_AUDIO :
//			#if defined (SUPPORT_AUDIO)
//			audio_ptr = (float *)malloc( m->getAudioLength() * sizeof( float ));
//			memcpy(audio_ptr,m->getData(),m->getAudioLength()* sizeof( float ));
//			if( audio_ptr )
//				success=true;
//			#endif //defined (SUPPORT_AUDIO)
//			break;
//		case MEDIA_TYPE_IMAGE :
//			#if defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
//			cvCopy(m->getData(),image_ptr);
//			if( image_ptr )
//				success=true;
//			#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO)
//			break;
//		case MEDIA_TYPE_VIDEO :
//			#if defined (SUPPORT_VIDEO)
//			cvCopy(m->getData(),video_ptr);
//			if( video_ptr )
//				success=true;
//			#endif //defined (SUPPORT_VIDEO)
//			break;
//		case MEDIA_TYPE_PDF :
//			#if defined (SUPPORT_PDF)
//			//cvCopy(m->getData(),pdf_ptr);
//			//if( pdf_ptr )
//			//	success=true;
//			#endif //defined (SUPPORT_PDF)
//			break;
//		case MEDIA_TYPE_3DMODEL :
//			#if defined (SUPPORT_3DMODEL)
//			model_ptr = dynamic_cast<osg::Node*>( m->getData()->clone(osg::CopyOp::DEEP_COPY_ALL));
//			if( model_ptr )
//				success=true;
//			#endif //defined (SUPPORT_3DMODEL)
//			break;
//		default:
//			break;
//	}
//	return success;
//}	
