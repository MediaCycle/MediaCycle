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

#if defined (USE_SNDFILE)
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<sndfile.h>
#endif

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <iostream>               // for std::cout
//using boost::filesystem;          // for ease of tutorial presentation;
namespace fs = boost::filesystem;

#include <string>

using namespace std;

// file_extensions are static and thus have to be initialized outside class
const filext::value_type _ini[] = {};
filext ACMediaFactory::available_file_extensions(_ini, _ini + sizeof _ini / sizeof *_ini);

// complement with remote MIME type check?
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
	filext::value_type(".3gp", MEDIA_TYPE_VIDEO), \
	filext::value_type(".avi", MEDIA_TYPE_VIDEO), \
	filext::value_type(".ffmpeg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".flv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".m4v", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mjpeg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mkv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mov", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mp4", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mpg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".mpv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".ogg", MEDIA_TYPE_VIDEO), \
	filext::value_type(".sav", MEDIA_TYPE_VIDEO), \
	filext::value_type(".sdp", MEDIA_TYPE_VIDEO), \
	filext::value_type(".swf", MEDIA_TYPE_VIDEO), \
	filext::value_type(".wmv", MEDIA_TYPE_VIDEO), \
	filext::value_type(".xine", MEDIA_TYPE_VIDEO), \
	
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
	filext::value_type(".xi", MEDIA_TYPE_AUDIO), \
};
filext ACMediaFactory::possible_file_extensions(_init, _init + sizeof _init / sizeof *_init);


ACMediaFactory::ACMediaFactory(){
#if defined (USE_SNDFILE)
	addSndFileExtensions();
#endif
	addOsgFileExtensions();
	listMediaExtensions();
}

ACMediaFactory::~ACMediaFactory(){
}

ACMedia* ACMediaFactory::create(string file_ext){
	boost::to_lower(file_ext);
	filext::iterator iter = available_file_extensions.find(file_ext);
	if( iter == available_file_extensions.end() ) {
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

// returns the ACMediaType corresponding to a given file extension
// or MEDIA_TYPE_NONE if the extension is unknown
ACMediaType ACMediaFactory::getMediaTypeFromExtension(std::string file_ext){
	boost::to_lower(file_ext);
	filext::iterator iter = available_file_extensions.find(file_ext);
	if( iter == available_file_extensions.end() ) {
		return MEDIA_TYPE_NONE;
	}
	return (ACMediaType)(iter->second);
}

void ACMediaFactory::listMediaExtensions(){
	std::vector<std::string> audioExt,imageExt,videoExt,model3dExt, textExt;
	filext::iterator iter = available_file_extensions.begin();
	for(;iter!=available_file_extensions.end();++iter){
		switch (iter->second){
			case MEDIA_TYPE_AUDIO:
				audioExt.push_back(iter->first);
				break;
			case MEDIA_TYPE_IMAGE:
				imageExt.push_back(iter->first);
				break;
			case MEDIA_TYPE_VIDEO:
				videoExt.push_back(iter->first);
				break;
			case MEDIA_TYPE_3DMODEL:
				model3dExt.push_back(iter->first);
				break;
			case MEDIA_TYPE_TEXT:
				textExt.push_back(iter->first);
				break;
			default:
				break;
		}	
	}	
	std::cout << "Supported media types: " << std::endl;
	
	std::cout << "-- audio:";
	std::vector<std::string>::iterator audioIter = audioExt.begin();
	for(;audioIter!=audioExt.end();++audioIter)
		std::cout << " " << (*audioIter);
	std::cout << std::endl;
	
	std::cout << "-- image:";
	std::vector<std::string>::iterator imageIter = imageExt.begin();
	for(;imageIter!=imageExt.end();++imageIter)
		std::cout << " " << (*imageIter);
	std::cout << std::endl;
	
	std::cout << "-- video:";
	std::vector<std::string>::iterator videoIter = videoExt.begin();
	for(;videoIter!=videoExt.end();++videoIter)
		std::cout << " " << (*videoIter);
	std::cout << std::endl;
	
	std::cout << "-- 3D models:";
	std::vector<std::string>::iterator model3dIter = model3dExt.begin();
	for(;model3dIter!=model3dExt.end();++model3dIter)
		std::cout << " " << (*model3dIter);
	std::cout << std::endl;
	
	std::cout << "-- text:";
	std::vector<std::string>::iterator textIter = textExt.begin();
	for(;textIter!=textExt.end();++textIter)
		std::cout << " " << (*textIter);
	std::cout << std::endl;
}

std::vector<std::string> ACMediaFactory::getExtensionsFromMediaType(ACMediaType media_type){
	std::vector<std::string> mediaExt;
	filext::iterator iter = available_file_extensions.begin();
	for(;iter!=available_file_extensions.end();++iter)
		if (iter->second == media_type )
			mediaExt.push_back(iter->first);
	return mediaExt;
}	

bool ACMediaFactory::addFileExtensionSupport(std::string file_ext,ACMediaType media_type){
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

#if defined (USE_SNDFILE)
void ACMediaFactory::addSndFileExtensions(){
	//CF ripped from sndfile's examples/list_formats.c
	SF_FORMAT_INFO	info ;
	SF_INFO 		sfinfo ;
	char buffer [128] ;
	int format, major_count, subtype_count, m, s;
	
	memset (&sfinfo, 0, sizeof (sfinfo));
	buffer [0] = 0 ;
	sf_command (NULL, SFC_GET_LIB_VERSION, buffer, sizeof (buffer));
	if (strlen (buffer) < 1)
	{	printf ("sndfile line %d: could not retrieve lib version.\n", __LINE__);
		//exit (1);
	} ;
	//printf ("Adding libsnfile supported extension, version : %s\n\n", buffer);// CF enable for debug mode
	
	sf_command (NULL, SFC_GET_FORMAT_MAJOR_COUNT, &major_count, sizeof (int)) ;
	sf_command (NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &subtype_count, sizeof (int)) ;
	
	sfinfo.channels = 1 ;
	for (m = 0 ; m < major_count ; m++)
	{	
		info.format = m ;
		sf_command (NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof (info)) ;
		
		std::string ext =  std::string(info.extension);
		if (ext == "aif" || ext == "aiff"){
			addFileExtensionSupport(".aif",MEDIA_TYPE_AUDIO);
			addFileExtensionSupport(".aiff",MEDIA_TYPE_AUDIO);
		}	
		else 
			addFileExtensionSupport(std::string(".") + ext,MEDIA_TYPE_AUDIO);
		
		//CF the following could be used to add format metadata
		/*		
		printf ("%s  (extension \"%s\")\n", info.name, info.extension) ;
		format = info.format ;
		for (s = 0 ; s < subtype_count ; s++)
		{	info.format = s ;
			sf_command (NULL, SFC_GET_FORMAT_SUBTYPE, &info, sizeof (info)) ;
			
			format = (format & SF_FORMAT_TYPEMASK) | info.format ;
			
			sfinfo.format = format ;
			if (sf_format_check (&sfinfo))
				printf ("   %s\n", info.name) ;
		} ;
		*/ 
	} ;
}	
#endif

void ACMediaFactory::addOsgFileExtensions(){

	// Create a list of plugin names manually, since osgDB::listAllAvailablePlugins won't work (under Apple Snow Leopard):
	/*
	 osgDB::FileNameList pluginList = osgDB::listAllAvailablePlugins();
	 osgDB::FileNameList::iterator pluginIter = pluginList.begin();
	 std::cout << "OSG plugins: " << std::endl;
	 for(;pluginIter!=pluginList.end();++pluginIter){
	 std::cout << (*pluginIter) << " ";
	 //osgDB::outputPluginDetails(std::cout,*pluginIter);
	 }
	 */
	std::vector<std::string> pluginNames;
	osgDB::FilePathList libList = osgDB::Registry::instance()->getLibraryFilePathList();
	osgDB::FilePathList::iterator libIter = libList.begin();
	//std::cout << "OSG plugins: " << std::endl;// CF enable for debug mode
	for(;libIter!=libList.end();++libIter){
		//std::cout << "\t" << (*libIter) << " " << std::endl;
		if (fs::exists(*libIter)){
			fs::directory_iterator end_iter;
			for ( fs::directory_iterator dir_itr( *libIter );dir_itr != end_iter; ++dir_itr )
			{
				if ( fs::is_regular_file( dir_itr->status() ) )
				{
					std::string plugin = fs::basename(dir_itr->path().filename());
					std::string prefix = "osgdb_";// CF check if used in all platforms for OSG
					size_t found = plugin.find(prefix);

					if (found!=string::npos){
						plugin.replace(plugin.find(prefix),prefix.length(),"");
						//std::cout << plugin << std::endl;
						pluginNames.push_back(plugin);
					}	
					//std::cout << fs::basename(dir_itr->path().filename()) << "\n";
				}
			}
		}
		//osgDB::outputPluginDetails(std::cout,*pluginIter);
	}
	
	//CF force-load each OSG plugin 
	//std::cout << "OSG supports the following extensions:" << std::endl;// CF enable for debug mode
	std::vector<std::string>::iterator pluginIter = pluginNames.begin();
	osgDB::Registry::instance()->closeAllLibraries();
	for(;pluginIter!=pluginNames.end();++pluginIter){
		std::string pluginInstance = osgDB::Registry::instance()->createLibraryNameForExtension(*pluginIter); 
		osgDB::Registry::LoadStatus pluginStatus = osgDB::Registry::instance()->loadLibrary(pluginInstance);
		
		if (pluginStatus != osgDB::Registry::NOT_LOADED) {
			//std::cout << "\t " << (*pluginIter) << " plugin:" << std::endl;// CF enable for debug mode
			osgDB::Registry::ReaderWriterList readerWriterList = osgDB::Registry::instance()->getReaderWriterList();
			for (int r=0; r<readerWriterList.size();r++)
			{
				// TODO deal with osgbd_pfb 
				
				osgDB::ReaderWriter::FormatDescriptionMap fDM = readerWriterList[r]->supportedExtensions();
				osgDB::ReaderWriter::FormatDescriptionMap::iterator iter = fDM.begin();
				for(;iter!=fDM.end();++iter){
					//std::cout << "\t\t -- (*." << iter->first << ") "<< iter->second << std::endl;// CF enable for debug mode
					std::string ext = "."+ iter->first;
					
					// First check if already listed in available file extensions:
					filext::iterator available_iter = available_file_extensions.find(ext);
					if( available_iter == available_file_extensions.end() ) {
						
						// Second check if already listed in available file extensions:
						filext::iterator possible_iter = possible_file_extensions.find(ext);
						if( possible_iter != possible_file_extensions.end() ) {
							available_file_extensions.insert(available_file_extensions.end(),filext::value_type(ext, possible_iter->second));
						}	
					}
				}	
					
			}	
		}
		if (*pluginIter == "ffmpeg" || *pluginIter == "qt" || *pluginIter == "imageio")
			std::cout << "Using " << *pluginIter << " plugin for OSG" << std::endl;
		else
			osgDB::Registry::instance()->closeLibrary(pluginInstance);
	}
}