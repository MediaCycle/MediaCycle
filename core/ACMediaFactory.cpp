/*
 *  ACMediaFactory.cpp
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

#include "ACMediaFactory.h"
#include "ACImage.h"
#include "ACAudio.h"
#include "ACVideo.h"
#include "AC3DModel.h"

#include <string>

using namespace std;

// file_extensions is static and thus has to be initialized outside class
// TODO: complete this...
const filext::value_type _ini[] = { filext::value_type(".wav", MEDIA_TYPE_AUDIO), \
filext::value_type(".aif", MEDIA_TYPE_AUDIO), \
filext::value_type(".mp3", MEDIA_TYPE_AUDIO), \
filext::value_type(".WAV", MEDIA_TYPE_AUDIO), \
filext::value_type(".AIF", MEDIA_TYPE_AUDIO), \
filext::value_type(".MP3", MEDIA_TYPE_AUDIO), \

filext::value_type(".jpg", MEDIA_TYPE_IMAGE), \
filext::value_type(".tiff", MEDIA_TYPE_IMAGE), \
filext::value_type(".tif", MEDIA_TYPE_IMAGE), \
filext::value_type(".png", MEDIA_TYPE_IMAGE), \
filext::value_type(".bmp", MEDIA_TYPE_IMAGE), \
filext::value_type(".gif", MEDIA_TYPE_IMAGE), \
filext::value_type(".JPG", MEDIA_TYPE_IMAGE), \
filext::value_type(".TIFF", MEDIA_TYPE_IMAGE), \
filext::value_type(".TIF", MEDIA_TYPE_IMAGE), \
filext::value_type(".PNG", MEDIA_TYPE_IMAGE), \
filext::value_type(".BMP", MEDIA_TYPE_IMAGE), \
filext::value_type(".GIF", MEDIA_TYPE_IMAGE), \

filext::value_type(".avi", MEDIA_TYPE_VIDEO), \
filext::value_type(".mov", MEDIA_TYPE_VIDEO), \
filext::value_type(".AVI", MEDIA_TYPE_VIDEO), \
filext::value_type(".MOV", MEDIA_TYPE_VIDEO), \

// SD TODO - these are the 3D formats supported by osgdb
//			Need to test that these are really behaving properly
//			Only .3ds tested at the moment
filext::value_type(".3dc", MEDIA_TYPE_3DMODEL), \
filext::value_type(".asc", MEDIA_TYPE_3DMODEL), \
filext::value_type(".3ds", MEDIA_TYPE_3DMODEL), \
filext::value_type(".ac", MEDIA_TYPE_3DMODEL), \
filext::value_type(".bsp", MEDIA_TYPE_3DMODEL), \
filext::value_type(".dae", MEDIA_TYPE_3DMODEL), \
filext::value_type(".dw", MEDIA_TYPE_3DMODEL), \
filext::value_type(".dxf", MEDIA_TYPE_3DMODEL), \
filext::value_type(".fbx", MEDIA_TYPE_3DMODEL), \
filext::value_type(".gem", MEDIA_TYPE_3DMODEL), \
filext::value_type(".geo", MEDIA_TYPE_3DMODEL), \
filext::value_type(".iv", MEDIA_TYPE_3DMODEL), \
filext::value_type(".wrl", MEDIA_TYPE_3DMODEL), \
filext::value_type(".ive", MEDIA_TYPE_3DMODEL), \
filext::value_type(".logo", MEDIA_TYPE_3DMODEL), \
filext::value_type(".lwo", MEDIA_TYPE_3DMODEL), \
filext::value_type(".lw", MEDIA_TYPE_3DMODEL), \
filext::value_type(".geo", MEDIA_TYPE_3DMODEL), \
filext::value_type(".lws", MEDIA_TYPE_3DMODEL), \
filext::value_type(".md2", MEDIA_TYPE_3DMODEL), \
filext::value_type(".obj", MEDIA_TYPE_3DMODEL), \
filext::value_type(".flt", MEDIA_TYPE_3DMODEL), \
filext::value_type(".osg", MEDIA_TYPE_3DMODEL), \
filext::value_type(".shp", MEDIA_TYPE_3DMODEL), \
filext::value_type(".stl", MEDIA_TYPE_3DMODEL), \
filext::value_type(".sta", MEDIA_TYPE_3DMODEL), \
filext::value_type(".wrl", MEDIA_TYPE_3DMODEL), \
filext::value_type(".x", MEDIA_TYPE_3DMODEL), \
filext::value_type(".3DC", MEDIA_TYPE_3DMODEL), \
filext::value_type(".ASC", MEDIA_TYPE_3DMODEL), \
filext::value_type(".3DS", MEDIA_TYPE_3DMODEL), \
filext::value_type(".AC", MEDIA_TYPE_3DMODEL), \
filext::value_type(".BSP", MEDIA_TYPE_3DMODEL), \
filext::value_type(".DAE", MEDIA_TYPE_3DMODEL), \
filext::value_type(".DW", MEDIA_TYPE_3DMODEL), \
filext::value_type(".DXF", MEDIA_TYPE_3DMODEL), \
filext::value_type(".FXB", MEDIA_TYPE_3DMODEL), \
filext::value_type(".GEM", MEDIA_TYPE_3DMODEL), \
filext::value_type(".GEO", MEDIA_TYPE_3DMODEL), \
filext::value_type(".IV", MEDIA_TYPE_3DMODEL), \
filext::value_type(".WRL", MEDIA_TYPE_3DMODEL), \
filext::value_type(".IVE", MEDIA_TYPE_3DMODEL), \
filext::value_type(".LOGO", MEDIA_TYPE_3DMODEL), \
filext::value_type(".LWO", MEDIA_TYPE_3DMODEL), \
filext::value_type(".LW", MEDIA_TYPE_3DMODEL), \
filext::value_type(".GEO", MEDIA_TYPE_3DMODEL), \
filext::value_type(".LWS", MEDIA_TYPE_3DMODEL), \
filext::value_type(".MD2", MEDIA_TYPE_3DMODEL), \
filext::value_type(".OBJ", MEDIA_TYPE_3DMODEL), \
filext::value_type(".FLT", MEDIA_TYPE_3DMODEL), \
filext::value_type(".OSG", MEDIA_TYPE_3DMODEL), \
filext::value_type(".SHP", MEDIA_TYPE_3DMODEL), \
filext::value_type(".STL", MEDIA_TYPE_3DMODEL), \
filext::value_type(".STA", MEDIA_TYPE_3DMODEL), \
filext::value_type(".WRL", MEDIA_TYPE_3DMODEL), \
filext::value_type(".X", MEDIA_TYPE_3DMODEL), \
};
filext ACMediaFactory::file_extensions(_ini, _ini + sizeof _ini / sizeof *_ini);

ACMediaFactory::ACMediaFactory(){
}

ACMediaFactory::~ACMediaFactory(){
}

ACMedia* ACMediaFactory::create(string file_ext){
	filext::iterator iter = file_extensions.find(file_ext);
	if( iter == file_extensions.end() ) {
		return NULL;
	}
	ACMediaType m = iter->second;
	return ACMediaFactory::create(m); 
}

ACMedia* ACMediaFactory::create(ACMediaType media_type){
	switch (media_type) {
		case MEDIA_TYPE_AUDIO:
			return new ACAudio;
			break;
		case MEDIA_TYPE_IMAGE:
#if !defined (APPLE_IOS)				
			return new ACImage;
#endif//CF APPLE_IOS			
			break;
		case MEDIA_TYPE_VIDEO:
#if !defined (APPLE_IOS)				
			return new ACVideo;
#endif//CF APPLE_IOS			
			break;
		case MEDIA_TYPE_3DMODEL:
			return new AC3DModel;
			break;
		default:
			return NULL;
			break;
	}
}

ACMedia* ACMediaFactory::create(ACMedia* media){
	switch (media->getMediaType()) {
		case MEDIA_TYPE_AUDIO:
		{
			ACAudio* audio = (ACAudio*) media;
			return new ACAudio(*audio,true);
			break;
		}
		case MEDIA_TYPE_IMAGE:
		{
#if !defined (APPLE_IOS)				
			ACImage* image = (ACImage*) media;
			return new ACImage(*image);
#endif//CF APPLE_IOS			
			break;
		}
		case MEDIA_TYPE_VIDEO:
		{
#if !defined (APPLE_IOS)				
			ACVideo* video = (ACVideo*) media;
			return new ACVideo(*video);
#endif//CF APPLE_IOS			
			break;
		}
		case MEDIA_TYPE_3DMODEL:
		{
			AC3DModel* model = (AC3DModel*) media;
			return new AC3DModel(*model);
			break;
		}
		default:
			return NULL;
			break;
	}
}
