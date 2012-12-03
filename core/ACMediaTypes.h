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

typedef		unsigned int ACMediaType;
const ACMediaType	MEDIA_TYPE_NONE			=	0x0000;
const ACMediaType	MEDIA_TYPE_AUDIO		=	0x0001;
const ACMediaType	MEDIA_TYPE_IMAGE		=	0x0002;
const ACMediaType	MEDIA_TYPE_VIDEO		=	0x0004;
const ACMediaType	MEDIA_TYPE_3DMODEL		=	0x0008;	
const ACMediaType	MEDIA_TYPE_TEXT			=	0x0010;
const ACMediaType	MEDIA_TYPE_PDF			=	0x0020;
const ACMediaType	MEDIA_TYPE_SENSOR		=	0x0040;
const ACMediaType	MEDIA_TYPE_MIXED		=	0x0080;
const ACMediaType	MEDIA_TYPE_ALL			=	0xFFFF;

// conversion between MediaTypes and text string (e.g., to be used in the labels for the gui)
typedef std::map<std::string, ACMediaType> ACMediaTypeNames;

// initialize static class variable
static const ACMediaTypeNames::value_type _initm[] = {
ACMediaTypeNames::value_type("", MEDIA_TYPE_NONE), \
ACMediaTypeNames::value_type("Audio", MEDIA_TYPE_AUDIO), \
ACMediaTypeNames::value_type("Image", MEDIA_TYPE_IMAGE), \
ACMediaTypeNames::value_type("Video", MEDIA_TYPE_VIDEO), \
ACMediaTypeNames::value_type("3DModel",MEDIA_TYPE_3DMODEL), \
ACMediaTypeNames::value_type("Text",MEDIA_TYPE_TEXT), \
ACMediaTypeNames::value_type("PDF", MEDIA_TYPE_PDF), \
ACMediaTypeNames::value_type("Sensor", MEDIA_TYPE_SENSOR), \
ACMediaTypeNames::value_type("Mixed", MEDIA_TYPE_MIXED), \
ACMediaTypeNames::value_type("All", MEDIA_TYPE_ALL)
};

static const ACMediaTypeNames media_type_names(_initm, _initm + sizeof _initm / sizeof *_initm);

#endif // _ACMEDIA_TYPE_H
