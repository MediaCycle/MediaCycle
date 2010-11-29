/*
 *  ACMediaFactory.h
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

#ifndef _ACMEDIAFACTORY_H
#define _ACMEDIAFACTORY_H

#include "ACMedia.h"
#include "ACMediaTypes.h"

#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;

typedef  std::map<std::string, ACMediaType> filext;
typedef  std::map<std::string, ACMediaType> mediaplugin;

class ACMediaFactory {
//private, not protected, since there is no ACImageFactory
private:
	// for log(n) search through extensions:
	static filext available_file_extensions,possible_file_extensions;
public:
	ACMediaFactory();
	~ACMediaFactory();
	
	// 2 ways to specify which new media to create:
	// 1) give file extension
	static ACMedia* create(std::string file_ext);//CF To improve, if extension has been "forgotten" as often in OSX
	// 2) directly specify which media (e.g. for openLibrary)
	static ACMedia* create(ACMediaType media_type);

	// 3) copy a media 
	static ACMedia* create(ACMedia* media);//CF we don't want this, data duplication!
	
	static ACMediaType getMediaTypeFromExtension(std::string file_ext);
	static void listMediaExtensions();
	static std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type);// no check in the possible formats list
	
private:
	static bool addFileExtensionSupport(std::string file_ext, ACMediaType media_type);	
#if defined (USE_SNDFILE)
	static void addSndFileExtensions();
#endif
	static void addOsgFileExtensions();
	
};

#endif // _ACMEDIAFACTORY_H
