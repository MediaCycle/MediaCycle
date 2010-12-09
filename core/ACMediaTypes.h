/*
 *  ACMediaTypes.h
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

#ifndef _ACMEDIA_TYPE_H
#define _ACMEDIA_TYPE_H

#include <string>
#include <map>

enum ACMediaType {
	MEDIA_TYPE_NONE,   // when type is not defined yet
	MEDIA_TYPE_AUDIO,
	MEDIA_TYPE_IMAGE,
	MEDIA_TYPE_VIDEO,
	MEDIA_TYPE_3DMODEL,
	MEDIA_TYPE_TEXT,
	MEDIA_TYPE_MIXED,  // for (future...) interface mixing media
	MEDIA_TYPE_ALL     // for plugins valid for all media
};

// conversion between MediaTypes and text string (e.g., to be used in the labels for the gui)
typedef std::map<std::string, ACMediaType> stringToMediaTypeConverter;

// initialize static class variable
const stringToMediaTypeConverter::value_type _initm[] = {
stringToMediaTypeConverter::value_type("", MEDIA_TYPE_NONE), \
stringToMediaTypeConverter::value_type("Audio", MEDIA_TYPE_AUDIO), \
stringToMediaTypeConverter::value_type("Image", MEDIA_TYPE_IMAGE), \
stringToMediaTypeConverter::value_type("Video", MEDIA_TYPE_VIDEO), \
stringToMediaTypeConverter::value_type("3DModel",MEDIA_TYPE_3DMODEL), \
stringToMediaTypeConverter::value_type("Text",MEDIA_TYPE_TEXT), \
stringToMediaTypeConverter::value_type("Mixed", MEDIA_TYPE_MIXED), \
stringToMediaTypeConverter::value_type("All", MEDIA_TYPE_ALL)
};

const stringToMediaTypeConverter stringToMediaType(_initm, _initm + sizeof _initm / sizeof *_initm);

#endif // _ACMEDIA_TYPE_H
