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
#if defined (SUPPORT_TEXT)
#include "ACText.h"
#endif //defined (SUPPORT_TEXT)
#if defined (SUPPORT_PDF)
#include "ACPDF.h"
#endif //defined (SUPPORT_PDF)

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

// ----------- uncomment this to parse OSG plugins in verbose mode
//#define PARSE_OSG_PLUGINS_VERBOSE

// ----------- uncomment this to parse sndfile formats in verbose mode
//#define PARSE_SNDFILE_FORMATS_VERBOSE

#include "ACMediaFactory.h"

boost::once_flag ACMediaFactory::once_flag = BOOST_ONCE_INIT;

ACMediaFactory::ACMediaFactory(){

	if (known_file_extensions.size()==0){
		createKnownFileExtensions();
	}
	if (available_file_extensions.size()==0){
		checkAvailableFileExtensions();
	}
	useAvailableFileExtensions();
	//useKnownFileExtensions(); //TRY (but don't commit) this instead of the above if library extensions parsing doesn't work, for debugging
	//listSupportedMediaExtensions();
	#ifdef USE_DEBUG
	//listUncheckedMediaExtensions();
	#endif //def USE_DEBUG
	
}

ACMediaFactory::~ACMediaFactory(){
}

/// List of known media file extensions associated to MediaCycle media types.
/// We can complement them by checking the list of unchecked media file extensions
void ACMediaFactory::createKnownFileExtensions() {
// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	known_file_extensions[".3dc"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".3ds"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".ac"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".asc"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".bsp"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".dae"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".dw"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".dxf"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".fbx"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".flt"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".geo"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".gem"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".iv"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".ive"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".logo"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".lw"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".lwo"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".lws"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".md2"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".obj"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".ogr"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".osg"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".shp"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".sta"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".stl"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".wrl"] = MEDIA_TYPE_3DMODEL;
	known_file_extensions[".x"] = MEDIA_TYPE_3DMODEL;

	// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	known_file_extensions[".bmp"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".bmpf"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".bw"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".cr2"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".crw"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".cur"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".dcr"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".dds"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".dng"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".epi"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".eps"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".epsf"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".epsi"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".exr"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".fpx"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".fpxi"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".gif"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".hdr"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".icns"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".ico"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".int"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".inta"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jp2"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jpc"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jpe"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jpeg"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jpg"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".jps"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".mac"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".mrw"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".nef"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".orf"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pct"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pic"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pict"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pbm"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pgm"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".png"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pnm"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".pnt"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".ppm"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".psd"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".ptng"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".qti"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".qtif"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".raf"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".raw"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".rgb"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".rgba"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".sgi"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".srf"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".targa"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".tga"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".tif"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".tiff"] = MEDIA_TYPE_IMAGE;
	known_file_extensions[".xbm"] = MEDIA_TYPE_IMAGE;

	// from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
	// and OSG 2.9.11
	known_file_extensions[".3gp"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".avi"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".ffmpeg"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".flv"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".m4v"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mjpeg"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mkv"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mov"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mp4"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mpeg"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mpg"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".mpv"] = MEDIA_TYPE_VIDEO;
//	known_file_extensions[".ogg"] = MEDIA_TYPE_VIDEO; //can be, but *.ogg audio files are more frequent
	known_file_extensions[".sav"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".sdp"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".swf"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".wmv"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".xine"] = MEDIA_TYPE_VIDEO;
	known_file_extensions[".xvid"] = MEDIA_TYPE_VIDEO;

	// from libsndfile 1.0.21
	known_file_extensions[".aif"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".aiff"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".au"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".avr"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".caf"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".flac"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".htk"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".iff"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".mat"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".mpc"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".oga"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".ogg"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".paf"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".pvf"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".raw"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".rf64"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".sd2"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".sds"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".sf"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".voc"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".w64"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".wav"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".wve"] = MEDIA_TYPE_AUDIO;
	known_file_extensions[".xi"] = MEDIA_TYPE_AUDIO;
	
	known_file_extensions[".pdf"] = MEDIA_TYPE_PDF;
	
	known_file_extensions[".txt"] = MEDIA_TYPE_TEXT;
	known_file_extensions[".xml"] = MEDIA_TYPE_TEXT;
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
		case MEDIA_TYPE_TEXT:
			#if defined (SUPPORT_TEXT)
			return new ACText();
			#endif //defined (SUPPORT_TEXT)
			break;
		default:
			return 0;
			break;
	}
	return 0;
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
		case MEDIA_TYPE_TEXT:
			#if defined (SUPPORT_TEXT)
			return new ACText(*((ACText*) media));
			#endif //defined (SUPPORT_TEXT)
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
	return 0;
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

ACMediaType ACMediaFactory::guessMediaTypeFromString(std::string keyword){
	boost::to_lower(keyword);
	ACMediaType _type;
	if(keyword == "")//hack to use "else if" between #ifdefs
		_type = MEDIA_TYPE_NONE;
	#if defined (SUPPORT_3DMODEL)
	else if(keyword=="3dmodel")
		_type = MEDIA_TYPE_3DMODEL;
	#endif //defined (SUPPORT_3DMODEL)
	#if defined (SUPPORT_AUDIO)
	else if(keyword=="audio")
		_type = MEDIA_TYPE_AUDIO;
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE)
	else if(keyword=="image")
		_type = MEDIA_TYPE_IMAGE;
	#endif //defined (SUPPORT_IMAGE)
	#if defined (SUPPORT_VIDEO)
	else if(keyword=="video")
		_type = MEDIA_TYPE_VIDEO;
	#endif //defined (SUPPORT_VIDEO)
	#if defined (SUPPORT_PDF)
	else if(keyword=="pdf")
		_type = MEDIA_TYPE_PDF;
	#endif //defined (SUPPORT_PDF)
	#if defined (SUPPORT_TEXT)
	else if(keyword=="text")
		_type = MEDIA_TYPE_TEXT;
	#endif //defined (SUPPORT_TEXT)
	else
		_type = MEDIA_TYPE_NONE;
	return _type;
}
std::string ACMediaFactory::getNormalCaseStringFromMediaType(ACMediaType _media_type){
	string smedia = "None";// or ""?
	// use a std::map< ACMediaType, std::string >?
	switch (_media_type) {
		case MEDIA_TYPE_3DMODEL:
			#if defined (SUPPORT_3DMODEL)
			smedia="3DModel";
			#endif //defined (SUPPORT_3DMODEL)
			break;
		case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
			smedia="Audio";
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
			#if defined (SUPPORT_IMAGE)
			smedia="Image";
			#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
			smedia="Video";
			#endif //defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_TEXT:
			#if defined (SUPPORT_TEXT)
			smedia="Text";
			#endif //defined (SUPPORT_TEXT)
			break;	
		case MEDIA_TYPE_MIXED:
#if defined (SUPPORT_MULTIMEDIA)
			smedia="Mixed";
#endif //defined (SUPPORT_MULTIMEDIA)
			break;	
		default:
			break;
	}	
	return smedia;
}	

std::string ACMediaFactory::getLowCaseStringFromMediaType(ACMediaType _media_type){
	string smedia = "none";// or ""?
	smedia = this->getNormalCaseStringFromMediaType(_media_type);
	boost::to_lower(smedia);
	return smedia;
}	

std::vector< std::string > ACMediaFactory::listAvailableMediaTypes(){
	std::vector< std::string > _list;
	// use a std::map< ACMediaType, std::string >?
	#if defined (SUPPORT_3DMODEL)
	_list.push_back("3Dmodel");
	#endif //defined (SUPPORT_3DMODEL)
	#if defined (SUPPORT_AUDIO)
	_list.push_back("audio");
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE)
	_list.push_back("image");
	#endif //defined (SUPPORT_IMAGE)
	#if defined (SUPPORT_TEXT)
	_list.push_back("text");
	#endif //defined (SUPPORT_TEXT)
	#if defined (SUPPORT_PDF)
	_list.push_back("pdf");
	#endif //defined (SUPPORT_PDF)
	#if defined (SUPPORT_VIDEO)
	_list.push_back("video");
	#endif //defined (SUPPORT_VIDEO)
	return _list;	
}	

bool ACMediaFactory::isMediaTypeSegmentable(ACMediaType _media_type){
	if(_media_type == MEDIA_TYPE_AUDIO || _media_type == MEDIA_TYPE_VIDEO)
		return true;
	else
		return false;
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
	std::vector<std::string> audioExt,imageExt,videoExt,model3dExt,textExt,pdfExt,otherExt;
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
	
	#if defined (SUPPORT_PDF)
	if (pdfExt.size()>0){
		std::cout << "-- PDF:";
		std::vector<std::string>::iterator pdfIter = pdfExt.begin();
		for(;pdfIter!=pdfExt.end();++pdfIter)
			std::cout << " " << (*pdfIter);
		std::cout << std::endl;
	}
	#endif //defined (SUPPORT_PDF)	

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
#if defined (SUPPORT_TEXT)
	addAvailableFileExtensionSupport(".txt",MEDIA_TYPE_TEXT);
	addAvailableFileExtensionSupport(".xml",MEDIA_TYPE_TEXT);	
#endif //defined (SUPPORT_TEXT)
#if defined (SUPPORT_PDF)
	//addAvailableFileExtensionSupport(".pdf",MEDIA_TYPE_PDF); // done by addAvailableOsgFileExtensions();
#endif //defined (SUPPORT_PDF)	
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
	#ifdef PARSE_SNDFILE_FORMATS_VERBOSE
	std::cout << "Gathering audio file extensions from sndfile..." << std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE
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
			#ifdef PARSE_SNDFILE_FORMATS_VERBOSE
			std::cout << "ACMediaFactory: adding sndfile extensions .aiff and .aif" << std::endl;
			#endif//def PARSE_SNDFILE_FORMATS_VERBOSE
		}
		else if (ext == ".oga"){
			addAvailableFileExtensionSupport(".oga",MEDIA_TYPE_AUDIO);
			addAvailableFileExtensionSupport(".ogg",MEDIA_TYPE_AUDIO);
			#ifdef PARSE_SNDFILE_FORMATS_VERBOSE
			std::cout << "ACMediaFactory: adding sndfile extensions .oga and .ogg" << std::endl;
			#endif//def PARSE_SNDFILE_FORMATS_VERBOSE
		}
		else{
			checkAvailableFileExtensionSupport(ext,MEDIA_TYPE_AUDIO);
			#ifdef PARSE_SNDFILE_FORMATS_VERBOSE
			std::cout << "ACMediaFactory: adding sndfile extension " << ext << std::endl;
			#endif//def PARSE_SNDFILE_FORMATS_VERBOSE
		}
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

#if defined (SUPPORT_IMAGE) || defined(SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL) || defined(SUPPORT_PDF)
void ACMediaFactory::addAvailableOsgFileExtensions(){

	#ifdef PARSE_OSG_PLUGINS_VERBOSE
	std::cout << "Gathering media file extensions from OSG plugins..." << std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE

	// The osgdb_ffmpeg plugin uses thread locking since 3.0.1
	// To avoid crashes, OSG plugins should be queried through the osgBD registry thread, so that its instance initiates the lock for osgdb_ffmpeg

	// Open all OSG plugins
	osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
	for(osgDB::FileNameList::iterator itr = plugins.begin();itr != plugins.end();++itr)
	{
		bool library_loaded = osgDB::Registry::instance()->loadLibrary(*itr);
		#ifdef PARSE_OSG_PLUGINS_VERBOSE
		if(library_loaded)
			std::cout<<"-- opening plugin "<<*itr<<std::endl;
		#endif//def PARSE_OSG_PLUGINS_VERBOSE;
	}
	
	// Get all registered readers/writers from the loaded OSG plugins and check the extensions they provide
	for(osgDB::Registry::ReaderWriterList::iterator rw_itr = osgDB::Registry::instance()->getReaderWriterList().begin();
		rw_itr != osgDB::Registry::instance()->getReaderWriterList().end();
		++rw_itr)
	{
		osgDB::ReaderWriter::FormatDescriptionMap rwfdm = (*rw_itr)->supportedExtensions();
		osgDB::ReaderWriter::FormatDescriptionMap::iterator fdm_itr;
		for(fdm_itr = rwfdm.begin();fdm_itr != rwfdm.end();++fdm_itr)
		{
			#ifdef PARSE_OSG_PLUGINS_VERBOSE
			std::cout<<"-- adding extension: ."<<fdm_itr->first<<" ("<<fdm_itr->second<<")"<<std::endl;
			#endif//def PARSE_OSG_PLUGINS_VERBOSE
			std::string ext = std::string(".") + fdm_itr->first;
			checkAvailableFileExtensionSupport(ext,MEDIA_TYPE_NONE);
		}
	}
	
	// Close all OSG plugins
	osgDB::Registry::instance()->closeAllLibraries();
	
	// Verify which readers/writers haven't been closed:
	#ifdef PARSE_OSG_PLUGINS_VERBOSE
	for(osgDB::Registry::ReaderWriterList::iterator rw_itr = osgDB::Registry::instance()->getReaderWriterList().begin();
		rw_itr != osgDB::Registry::instance()->getReaderWriterList().end();
		++rw_itr)
		std::cout<<"-- plugin still open: "<<(*rw_itr)->className()<<std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE;
	
	#ifdef PARSE_OSG_PLUGINS_VERBOSE
	std::cout << "Gathering media file extensions from OSG plugins... done" << std::endl;
	#endif//def PARSE_OSG_PLUGINS_VERBOSE
}
#endif //defined (SUPPORT_IMAGE) || defined (SUPPORT_VIDEO) || defined(SUPPORT_3DMODEL) || defined(SUPPORT_PDF)
