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

#include <string>

using namespace std;

// file_extensions is static and thus has to be initialized outside class
// TODO: complete this...
const filext::value_type _ini[] = { filext::value_type(".wav", AUDIO), \
									filext::value_type(".aif", AUDIO), \
									filext::value_type(".mp3", AUDIO), \

									filext::value_type(".jpg", IMAGE), \
									filext::value_type(".tiff", IMAGE), \
									filext::value_type(".tif", IMAGE), \
									filext::value_type(".png", IMAGE), \

									filext::value_type(".mov", VIDEO), \
									filext::value_type(".avi", VIDEO)
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
		case AUDIO:
			return new ACAudio;
			break;
		case IMAGE:
			return new ACImage;
			break;
		case VIDEO:
			return new ACVideo;
			break;
		default:
			return NULL;
			break;
	}
}
