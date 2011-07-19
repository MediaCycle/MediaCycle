/*
 *  ArchipelXmlFile.h
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 16/11/10
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef ArchipelXmlFile_H
#define ArchipelXmlFile_H

#include <string>

//#include <ACOpenCVInclude.h>
#ifdef __APPLE__
#ifdef MAC_FRAMEWORKS
#include <OpenCV/cv.h>
#include <OpenCV/cxcore.h>
#include <OpenCV/highgui.h> // for cvloadimage
#else
#ifdef APPLE_IOS //CF in reality, it is because of OpenCV 2.1... test to improve!
#include <cv.h>
#include <cxcore.h>
//#include <highgui.h> // for cvloadimage
#else
#include "OpenCV/cv.h"
#include "OpenCV/cxcore.h"
#include "OpenCV/highgui.h" // for cvloadimage
#endif
#endif
#else
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#endif

#include <osgDB/ReadFile>

std::string* ArchipelXmlFileRead(std::string filePath);
std::string ArchipelLabelFileRead(std::string filePath);
IplImage*  ArchipelThumbFileRead(std::string filePath);
std::string ArchipelGetThumbFileName(std::string filePath);
#endif