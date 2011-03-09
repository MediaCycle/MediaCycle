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
#if defined (SUPPORT_AUDIO)
#include "ACAudio.h"
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_IMAGE)
#include "ACImage.h"
#endif //defined (SUPPORT_IMAGE)
#if defined (SUPPORT_VIDEO)
#include "ACVideo.h"
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_3DMODEL)
#include "AC3DModel.h"
#endif //defined (SUPPORT_3DMODEL)

#if defined (SUPPORT_AUDIO)
#if defined (USE_SNDFILE)
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<sndfile.h>
#endif //defined (USE_SNDFILE)
#endif //defined (SUPPORT_AUDIO)

#if defined (SUPPORT_IMAGE) || defined(SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)
#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>
#endif //defined (SUPPORT_IMAGE) || defined(SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <iostream>               // for std::cout
//using boost::filesystem;          // for ease of tutorial presentation;
namespace fs = boost::filesystem;

#include <string>

using namespace std;

// ----------- uncomment this parse OSG plugins in verbose mode
//#define PARSE_OSG_PLUGINS_VERBOSE

// filext members are static and thus have to be initialized outside class
const filext::value_type _ini[] = {};

#if defined USE_DEBUG && defined __APPLE__
#include "ACMediaFactoryDebug.cpp"
#else
/// List of media file extensions available on the current running system and MediaCycle distribution.
const filext::value_type _mini[] = {};
filext ACMediaFactory::available_file_extensions(_mini, _mini + sizeof _mini / sizeof *_mini);

/// List of unchecked media file extensions, available on the current running system and MediaCycle distribution,
/// but not listed on the possible extensions so far.
const filext::value_type _uini[] = {};
filext ACMediaFactory::unchecked_file_extensions(_uini, _uini + sizeof _uini / sizeof *_uini);
#endif

/// List of media file extensions possibly in use by MediaCycle
filext ACMediaFactory::used_file_extensions(_ini, _ini + sizeof _ini / sizeof *_ini);

/// List of known media file extensions associated to MediaCycle media types.
/// We can complement them by checking the list of unchecked media file extensions
const filext::value_type _init[] = {
	// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	filext::value_type(".3dc", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".3ds", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".ac", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".asc", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".bsp", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".dae", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".dw", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".dxf", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".fbx", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".flt", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".geo", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".gem", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".iv", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".ive", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".logo", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".lw", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".lwo", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".lws", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".md2", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".obj", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".ogr", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".osg", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".shp", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".sta", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".stl", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".wrl", MEDIA_TYPE_3DMODEL), \
	filext::value_type(".x", MEDIA_TYPE_3DMODEL), \
	
	// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	filext::value_type(".bmp", MEDIA_TYPE_IMAGE), \
	filext::value_type(".bmpf", MEDIA_TYPE_IMAGE), \
	filext::value_type(".bw", MEDIA_TYPE_IMAGE), \
	filext::value_type(".cr2", MEDIA_TYPE_IMAGE), \
	filext::value_type(".crw", MEDIA_TYPE_IMAGE), \
	filext::value_type(".cur", MEDIA_TYPE_IMAGE), \
	filext::value_type(".dcr", MEDIA_TYPE_IMAGE), \
	filext::value_type(".dds", MEDIA_TYPE_IMAGE), \
	filext::value_type(".dng", MEDIA_TYPE_IMAGE), \
	filext::value_type(".epi", MEDIA_TYPE_IMAGE), \
	filext::value_type(".eps", MEDIA_TYPE_IMAGE), \
	filext::value_type(".epsf", MEDIA_TYPE_IMAGE), \
	filext::value_type(".epsi", MEDIA_TYPE_IMAGE), \
	filext::value_type(".exr", MEDIA_TYPE_IMAGE), \
	filext::value_type(".fpx", MEDIA_TYPE_IMAGE), \
	filext::value_type(".fpxi", MEDIA_TYPE_IMAGE), \
	filext::value_type(".gif", MEDIA_TYPE_IMAGE), \
	filext::value_type(".hdr", MEDIA_TYPE_IMAGE), \
	filext::value_type(".icns", MEDIA_TYPE_IMAGE), \
	filext::value_type(".ico", MEDIA_TYPE_IMAGE), \
	filext::value_type(".int", MEDIA_TYPE_IMAGE), \
	filext::value_type(".inta", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jp2", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jpc", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jpe", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jpeg", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jpg", MEDIA_TYPE_IMAGE), \
	filext::value_type(".jps", MEDIA_TYPE_IMAGE), \
	filext::value_type(".mac", MEDIA_TYPE_IMAGE), \
	filext::value_type(".mrw", MEDIA_TYPE_IMAGE), \
	filext::value_type(".nef", MEDIA_TYPE_IMAGE), \
	filext::value_type(".orf", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pct", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pic", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pict", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pbm", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pgm", MEDIA_TYPE_IMAGE), \
	filext::value_type(".png", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pnm", MEDIA_TYPE_IMAGE), \
	filext::value_type(".pnt", MEDIA_TYPE_IMAGE), \
	filext::value_type(".ppm", MEDIA_TYPE_IMAGE), \
	filext::value_type(".psd", MEDIA_TYPE_IMAGE), \
	filext::value_type(".ptng", MEDIA_TYPE_IMAGE), \
	filext::value_type(".qti", MEDIA_TYPE_IMAGE), \
	filext::value_type(".qtif", MEDIA_TYPE_IMAGE), \
	filext::value_type(".raf", MEDIA_TYPE_IMAGE), \
	filext::value_type(".raw", MEDIA_TYPE_IMAGE), \
	filext::value_type(".rgb", MEDIA_TYPE_IMAGE), \
	filext::value_type(".rgba", MEDIA_TYPE_IMAGE), \
	filext::value_type(".sgi", MEDIA_TYPE_IMAGE), \
	filext::value_type(".srf", MEDIA_TYPE_IMAGE), \
	filext::value_type(".targa", MEDIA_TYPE_IMAGE), \
	filext::value_type(".tga", MEDIA_TYPE_IMAGE), \
	filext::value_type(".tif", MEDIA_TYPE_IMAGE), \
	filext::value_type(".tiff", MEDIA_TYPE_IMAGE), \
	filext::value_type(".xbm", MEDIA_TYPE_IMAGE), \
	
	// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	// and OSG 2.9.11
	filext::value_type(".3gp", MEDIA_TYPE_VIDEO), \
	filext::value_type(".avi", MEDIA_TYPE_VIDEO), \
	filext::value_type(".ffmpeg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".flv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".m4v", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mjpeg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mkv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mov", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mp4", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mpeg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mpg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mpv", MEDIA_TYPE_VIDEO), \
//	filext::value_type(".ogg", MEDIA_TYPE_VIDEO), \ //can be, but *.ogg audio files are more frequent
	filext::value_type(".sav", MEDIA_TYPE_VIDEO), \
	filext::value_type(".sdp", MEDIA_TYPE_VIDEO), \
	filext::value_type(".swf", MEDIA_TYPE_VIDEO), \
	filext::value_type(".wmv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".xine", MEDIA_TYPE_VIDEO), \
	filext::value_type(".xvid", MEDIA_TYPE_VIDEO), \
	
	// from libsndfile 1.0.21
	filext::value_type(".aif", MEDIA_TYPE_AUDIO), \
	filext::value_type(".aiff", MEDIA_TYPE_AUDIO), \
	filext::value_type(".au", MEDIA_TYPE_AUDIO), \
	filext::value_type(".avr", MEDIA_TYPE_AUDIO), \
	filext::value_type(".caf", MEDIA_TYPE_AUDIO), \
	filext::value_type(".flac", MEDIA_TYPE_AUDIO), \
	filext::value_type(".htk", MEDIA_TYPE_AUDIO), \
	filext::value_type(".iff", MEDIA_TYPE_AUDIO), \
	filext::value_type(".mat", MEDIA_TYPE_AUDIO), \
	filext::value_type(".mpc", MEDIA_TYPE_AUDIO), \
	filext::value_type(".oga", MEDIA_TYPE_AUDIO), \
	filext::value_type(".ogg", MEDIA_TYPE_AUDIO), \
	filext::value_type(".paf", MEDIA_TYPE_AUDIO), \
	filext::value_type(".pvf", MEDIA_TYPE_AUDIO), \
	filext::value_type(".raw", MEDIA_TYPE_AUDIO), \
	filext::value_type(".rf64", MEDIA_TYPE_AUDIO), \
	filext::value_type(".sd2", MEDIA_TYPE_AUDIO), \
	filext::value_type(".sds", MEDIA_TYPE_AUDIO), \
	filext::value_type(".sf", MEDIA_TYPE_AUDIO), \
	filext::value_type(".voc", MEDIA_TYPE_AUDIO), \
	filext::value_type(".w64", MEDIA_TYPE_AUDIO), \
	filext::value_type(".wav", MEDIA_TYPE_AUDIO), \
	filext::value_type(".wve", MEDIA_TYPE_AUDIO), \
	filext::value_type(".xi", MEDIA_TYPE_AUDIO)
};
filext ACMediaFactory::known_file_extensions(_init, _init + sizeof _init / sizeof *_init);

ACMediaFactory* ACMediaFactory::getInstance()
{
    static ACMediaFactory* instance = new ACMediaFactory;
    return instance;
}

ACMediaFactory::ACMediaFactory(){
	if (available_file_extensions.size()==0){
		checkAvailableFileExtensions();
	}	
	useAvailableFileExtensions();
	//useKnownFileExtensions(); //use this instead of the above if library extensions parsing doesn't work
	listSupportedMediaExtensions();
	#ifdef USE_DEBUG
	listUncheckedMediaExtensions();
	#endif //def USE_DEBUG
}

ACMediaFactory::~ACMediaFactory(){
}

ACMedia* ACMediaFactory::create(string file_ext){
	boost::to_lower(file_ext);
	filext::iterator iter = used_file_extensions.find(file_ext);
	if( iter == used_file_extensions.end() ) {
		return 0;
	}
	ACMediaType m = iter->second;
	return ACMediaFactory::create(m); 
}

ACMedia* ACMediaFactory::create(ACMediaType media_type){
	switch (media_type) {
		case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
			return new ACAudio();
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
			#if defined (SUPPORT_IMAGE)
			return new ACImage();
			#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
			return new ACVideo();
			#endif //defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_3DMODEL:
			#if defined (SUPPORT_3DMODEL)
			return new AC3DModel();
			#endif //defined (SUPPORT_3DMODEL)
			break;
		default:
			return 0;
			break;
	}
}

ACMedia* ACMediaFactory::create(ACMedia* media){
	switch (media->getMediaType()) {
		case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
			return new ACAudio(*((ACAudio*) media));
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
			#if defined (SUPPORT_IMAGE)
			return new ACImage(*((ACImage*) media));
			#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
			return new ACVideo(*((ACVideo*) media));
			#endif //defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_3DMODEL:
			#if defined (SUPPORT_3DMODEL)
			return new AC3DModel(*((AC3DModel*) media));
			#endif //defined (SUPPORT_3DMODEL)
			break;
		default:
			return 0;
			break;
	}
	/*
	switch (media->getMediaType()) {
		#if defined (SUPPORT_AUDIO)
		case MEDIA_TYPE_AUDIO:
			
				ACAudio* audio = (ACAudio*) media;
				return new ACAudio(*audio,true);
			
			break;
		#endif //defined (SUPPORT_AUDIO)
		#if defined (SUPPORT_IMAGE)	
		case MEDIA_TYPE_IMAGE:
			
				ACImage* image = (ACImage*) media;
				return new ACImage(*image);
			
				break;
		#endif //defined (SUPPORT_IMAGE)	
		#if defined (SUPPORT_VIDEO)	
		case MEDIA_TYPE_VIDEO:
			
				ACVideo* video = (ACVideo*) media;
				return new ACVideo(*video);
			
				break;
		#endif //defined (SUPPORT_VIDEO)	
		#if defined (SUPPORT_3DMODEL)
		case MEDIA_TYPE_3DMODEL:
			AC3DModel* model = (AC3DModel*) media;
			return new AC3DModel(*model);
			break;
		#endif //defined (SUPPORT_3DMODEL)
		default:
			return 0;
			break;
	}
	 */
}

// returns the ACMediaType corresponding to a given file extension
// or MEDIA_TYPE_NONE if the extension is unknown
ACMediaType ACMediaFactory::getMediaTypeFromExtension(std::string file_ext){
	boost::to_lower(file_ext);
	filext::iterator iter = used_file_extensions.find(file_ext);
	if( iter == used_file_extensions.end() ) {
		return MEDIA_TYPE_NONE;
	}
	return (ACMediaType)(iter->second);
}

void ACMediaFactory::listSupportedMediaExtensions(){
	std::cout << "Supported media types: " << std::endl;
	listMediaExtensions(used_file_extensions);
}

void ACMediaFactory::listUncheckedMediaExtensions(){
	std::cout << "Unchecked media types: " << std::endl;
	listMediaExtensions(unchecked_file_extensions);
}

void ACMediaFactory::listMediaExtensions(filext _extensions){
	std::vector<std::string> audioExt,imageExt,videoExt,model3dExt,textExt,otherExt;
	filext::iterator iter = _extensions.begin();
	for(;iter!=_extensions.end();++iter){
		switch (iter->second){
			case MEDIA_TYPE_AUDIO:
				#if defined (SUPPORT_AUDIO)
				audioExt.push_back(iter->first);
				#endif //defined (SUPPORT_AUDIO)
				break;
			case MEDIA_TYPE_IMAGE:
				#if defined (SUPPORT_IMAGE)
				imageExt.push_back(iter->first);
				#endif //defined (SUPPORT_IMAGE)
				break;
			case MEDIA_TYPE_VIDEO:
				#if defined (SUPPORT_VIDEO)
				videoExt.push_back(iter->first);
				#endif //defined (SUPPORT_VIDEO)
				break;
			case MEDIA_TYPE_3DMODEL:
				#if defined (SUPPORT_3DMODEL)
				model3dExt.push_back(iter->first);
				#endif //defined (SUPPORT_3DMODEL)
				break;
			case MEDIA_TYPE_TEXT:
				#if defined (SUPPORT_TEXT)
				textExt.push_back(iter->first);
				#endif //defined (SUPPORT_TEXT)
				break;
			case MEDIA_TYPE_NONE:
				otherExt.push_back(iter->first);
				break;
			default:
				break;
		}	
	}	

	#if defined (SUPPORT_AUDIO)
	if (audioExt.size()>0){
		std::cout << "-- audio:";
		std::vector<std::string>::iterator audioIter = audioExt.begin();
		for(;audioIter!=audioExt.end();++audioIter)
			std::cout << " " << (*audioIter);
		std::cout << std::endl;
	}	
	#endif //defined (SUPPORT_AUDIO)
	
	#if defined (SUPPORT_IMAGE)
	if (imageExt.size()>0){
		std::cout << "-- image:";
		std::vector<std::string>::iterator imageIter = imageExt.begin();
		for(;imageIter!=imageExt.end();++imageIter)
			std::cout << " " << (*imageIter);
		std::cout << std::endl;
	}	
	#endif //defined (SUPPORT_IMAGE)
	
	#if defined (SUPPORT_VIDEO)
	if (videoExt.size()>0){
		std::cout << "-- video:";
		std::vector<std::string>::iterator videoIter = videoExt.begin();
		for(;videoIter!=videoExt.end();++videoIter)
			std::cout << " " << (*videoIter);
		std::cout << std::endl;
	}	
	#endif //defined (SUPPORT_VIDEO)
	
	#if defined (SUPPORT_3DMODEL)
	if (model3dExt.size()>0){
		std::cout << "-- 3D models:";
		std::vector<std::string>::iterator model3dIter = model3dExt.begin();
		for(;model3dIter!=model3dExt.end();++model3dIter)
			std::cout << " " << (*model3dIter);
		std::cout << std::endl;
	}	
	#endif //defined (SUPPORT_3DMODEL)
	
	#if defined (SUPPORT_TEXT)
	if (textExt.size()>0){
		std::cout << "-- text:";
		std::vector<std::string>::iterator textIter = textExt.begin();
		for(;textIter!=textExt.end();++textIter)
			std::cout << " " << (*textIter);
		std::cout << std::endl;
	}	
	#endif //defined (SUPPORT_TEXT)
	
	if (otherExt.size()>0){
		std::cout << "-- undetermined:";
		std::vector<std::string>::iterator otherIter = otherExt.begin();
		for(;otherIter!=otherExt.end();++otherIter)
			std::cout << " " << (*otherIter);
		std::cout << std::endl;
	}	
}

bool ACMediaFactory::useAvailableFileExtensions(){
	if (available_file_extensions.size()>0){
		used_file_extensions.clear();
		used_file_extensions=available_file_extensions;
		return true;
	}	
	else {
		return false;
	}

}

void ACMediaFactory::useKnownFileExtensions(){
	if (known_file_extensions.size()>0){
		used_file_extensions.clear();
		used_file_extensions=known_file_extensions;
	}	
		
	/*
	filext::iterator iter = known_file_extensions.begin();
	for(;iter!=known_file_extensions.end();++iter){
		switch (iter->second){
			case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
				this->addAvailableFileExtensionSupport(iter->first,iter->second);
			#endif //defined (SUPPORT_AUDIO)
				break;
			case MEDIA_TYPE_IMAGE:
			#if defined (SUPPORT_IMAGE)
				this->addAvailableFileExtensionSupport(iter->first,iter->second);
			#endif //defined (SUPPORT_IMAGE)
				break;
			case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
				this->addAvailableFileExtensionSupport(iter->first,iter->second);
			#endif //defined (SUPPORT_VIDEO)
				break;
			case MEDIA_TYPE_3DMODEL:
			#if defined (SUPPORT_3DMODEL)
				this->addAvailableFileExtensionSupport(iter->first,iter->second);
			#endif //defined (SUPPORT_3DMODEL)
				break;
			case MEDIA_TYPE_TEXT:
			#if defined (SUPPORT_TEXT)
				this->addAvailableFileExtensionSupport(iter->first,iter->second);
			#endif //defined (SUPPORT_TEXT)
				break;
			default:
				break;
		}	
	}
	 */
}

void ACMediaFactory::checkAvailableFileExtensions(){
	#if defined (SUPPORT_AUDIO)
		addAvailableSndFileExtensions();
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE) || defined(SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)
		addAvailableOsgFileExtensions();
	#endif //defined (SUPPORT_IMAGE OR SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)
}	

std::vector<std::string> ACMediaFactory::getExtensionsFromMediaType(ACMediaType media_type){
	std::vector<std::string> mediaExt;
	filext::iterator iter = available_file_extensions.begin();
	for(;iter!=available_file_extensions.end();++iter)
		if (iter->second == media_type )
			mediaExt.push_back(iter->first);
	return mediaExt;
}	

bool ACMediaFactory::addAvailableFileExtensionSupport(std::string file_ext,ACMediaType media_type){
	boost::to_lower(file_ext);
	filext::iterator iter = available_file_extensions.find(file_ext);
	if( iter == available_file_extensions.end() ) {
		available_file_extensions.insert(available_file_extensions.end(),filext::value_type(file_ext, media_type));
		return true; // added
	}
	else {
		return false; // already present
	}
}

void ACMediaFactory::checkAvailableFileExtensionSupport(std::string file_ext,ACMediaType media_type){
	// First check if already listed in available file extensions:
	filext::iterator available_iter = available_file_extensions.find(file_ext);
	if( available_iter == available_file_extensions.end() ) {
		
		// Second check if already listed in possible file extensions:
		filext::iterator possible_iter = known_file_extensions.find(file_ext);
		if( possible_iter != known_file_extensions.end() ) {
			available_file_extensions.insert(available_file_extensions.end(),filext::value_type(file_ext, possible_iter->second));
		}
		else {
			// add to the list of unchecked file extensions:
			filext::iterator unchecked_iter = unchecked_file_extensions.find(file_ext);
			if( unchecked_iter == unchecked_file_extensions.end() ) {
				unchecked_file_extensions.insert(unchecked_file_extensions.end(),filext::value_type(file_ext, media_type));
			}
		}
	}
}

#if defined (SUPPORT_AUDIO)
void ACMediaFactory::addAvailableSndFileExtensions(){
	//CF ripped from sndfile's examples/list_formats.c
	SF_FORMAT_INFO	info ;
	SF_INFO 		sfinfo ;
	char buffer [128] ;
	int format, major_count, subtype_count, m, s;
	
	memset (&sfinfo, 0, sizeof (sfinfo));
	buffer [0] = 0 ;
	sf_command (0, SFC_GET_LIB_VERSION, buffer, sizeof (buffer));
	if (strlen (buffer) < 1)
	{	printf ("sndfile line %d: could not retrieve lib version.\n", __LINE__);
		//exit (1);
	} ;
	//printf ("Adding libsnfile supported extension, version : %s\n\n", buffer);// CF enable for debug mode
	
	sf_command (0, SFC_GET_FORMAT_MAJOR_COUNT, &major_count, sizeof (int)) ;
	sf_command (0, SFC_GET_FORMAT_SUBTYPE_COUNT, &subtype_count, sizeof (int)) ;
	
	sfinfo.channels = 1 ;
	for (m = 0 ; m < major_count ; m++)
	{	
		info.format = m ;
		sf_command (0, SFC_GET_FORMAT_MAJOR, &info, sizeof (info)) ;
		
		std::string ext =  std::string(".") + std::string(info.extension);

		if (ext == ".aif" || ext == ".aiff"){
			addAvailableFileExtensionSupport(".aif",MEDIA_TYPE_AUDIO);
			addAvailableFileExtensionSupport(".aiff",MEDIA_TYPE_AUDIO);
		}
		else if (ext == ".oga"){
			addAvailableFileExtensionSupport(".oga",MEDIA_TYPE_AUDIO);
			addAvailableFileExtensionSupport(".ogg",MEDIA_TYPE_AUDIO);
		}	
		else 
			checkAvailableFileExtensionSupport(ext,MEDIA_TYPE_AUDIO);
		
		//CF the following could be used to add format metadata
		/*		
		printf ("%s  (extension \"%s\")\n", info.name, info.extension) ;
		format = info.format ;
		for (s = 0 ; s < subtype_count ; s++)
		{	info.format = s ;
			sf_command (0, SFC_GET_FORMAT_SUBTYPE, &info, sizeof (info)) ;
			
			format = (format & SF_FORMAT_TYPEMASK) | info.format ;
			
			sfinfo.format = format ;
			if (sf_format_check (&sfinfo))
				printf ("   %s\n", info.name) ;
		} ;
		*/ 
	} ;
}	
#endif //defined (SUPPORT_AUDIO)

#if defined (SUPPORT_IMAGE) || defined(SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)
void ACMediaFactory::addAvailableOsgFileExtensions(){
	#ifdef PARSE_OSG_PLUGINS_VERBOSE
	std::cout << "Gathering media file extensions from OSG plugins..." << std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE
	
	// Ripped from applications/osgconv, method for "--formats"
	osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
	for(osgDB::FileNameList::iterator itr = plugins.begin();
		itr != plugins.end();
		++itr)
	{
		#ifdef PARSE_OSG_PLUGINS_VERBOSE
		std::cout<<"-- Plugin "<<*itr<<std::endl;
		#endif//def PARSE_OSG_PLUGINS_VERBOSE;
		// Ripped from osgDB/PluginQuery.cpp, osgDB::outputPluginDetails
		osgDB::ReaderWriterInfoList infoList;
		if (osgDB::queryPlugin(*itr, infoList))
		{
			for(osgDB::ReaderWriterInfoList::iterator rwi_itr = infoList.begin();
				rwi_itr != infoList.end();
				++rwi_itr)
			{
				osgDB::ReaderWriterInfo& info = *(*rwi_itr);
				osgDB::ReaderWriter::FormatDescriptionMap::iterator fdm_itr;				
				for(fdm_itr = info.extensions.begin();
					fdm_itr != info.extensions.end();
					++fdm_itr)
				{
					#ifdef PARSE_OSG_PLUGINS_VERBOSE
					std::cout<<"    extensions : ."<<fdm_itr->first<<" ("<<fdm_itr->second<<")"<<std::endl;
					#endif//def PARSE_OSG_PLUGINS_VERBOSE
					std::string ext = std::string(".") + fdm_itr->first;
					checkAvailableFileExtensionSupport(ext,MEDIA_TYPE_NONE);
				}
			}
		}	
	}
	#ifdef PARSE_OSG_PLUGINS_VERBOSE
	std::cout << "Gathering media file extensions from OSG plugins... done" << std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE
}
#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL)
