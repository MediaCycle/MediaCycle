/* 
 * File:   ACOpenCVInclude.h
 * Author: Alexis Moinet
 *
 * @date 15 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef ACOPENCVINCLUDE_H
#define	ACOPENCVINCLUDE_H

//#ifdef	__cplusplus
//extern "C" {
//#endif

//#define MAC_FRAMEWORKS

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
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#endif
#endif
#else
//#include <opencv/cv.h>
//#include <opencv/cxcore.h>
//#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#endif

// Since we need OpenCV 2.x.x
#include <opencv2/core/version.hpp>
#define CV_MIN_VERSION_REQUIRED(MAJOR, MINOR, PATCH) ((CV_MAJOR_VERSION>MAJOR) || (CV_MAJOR_VERSION==MAJOR && (CV_MINOR_VERSION>MINOR || (CV_MINOR_VERSION==MINOR && CV_SUBMINOR_VERSION>=PATCH))))
#define CV_VERSION_LESS_THAN(MAJOR, MINOR, PATCH) ((CV_MAJOR_VERSION<MAJOR) || (CV_MAJOR_VERSION==MAJOR && (CV_MINOR_VERSION<MINOR || (CV_MINOR_VERSION==MINOR && CV_SUBMINOR_VERSION<PATCH))))
#define CV_VERSION_LESS_OR_EQUAL(MAJOR, MINOR, PATCH) ((CV_MAJOR_VERSION<MAJOR) || (CV_MAJOR_VERSION==MAJOR && (CV_MINOR_VERSION<MINOR || (CV_MINOR_VERSION==MINOR && CV_SUBMINOR_VERSION<=PATCH))))
#define CV_VERSION_GREATER_THAN(MAJOR, MINOR, PATCH) ((CV_MAJOR_VERSION>MAJOR) || (CV_MAJOR_VERSION==MAJOR && (CV_MINOR_VERSION>MINOR || (CV_MINOR_VERSION==MINOR && CV_SUBMINOR_VERSION>PATCH))))
#define CV_VERSION_GREATER_OR_EQUAL(MAJOR, MINOR, PATCH) ((CV_MAJOR_VERSION>MAJOR) || (CV_MAJOR_VERSION==MAJOR && (CV_MINOR_VERSION>MINOR || (CV_MINOR_VERSION==MINOR && CV_SUBMINOR_VERSION>=PATCH))))

//#ifdef	__cplusplus
//};
//#endif

// --------------------------------------------------------------------
// XS TODO 2.* plus necessaire !!
//http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00053000000000000000
// C++ wrapper around IplImage that allows convenient (and hopefully fast) pixel access

template<class T> class Image {
private:
    IplImage* imgp;
public:
    Image(IplImage* img=0) {imgp=img;}
    ~Image(){imgp=0;}
    void operator=(IplImage* img) {imgp=img;}
    inline T* operator[](const int rowIndx) {
        return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
    }
    IplImage* getImage(){return imgp;}
};

typedef struct{
    unsigned char b,g,r;
} BgrPixel;

typedef struct{
    float b,g,r;
} BgrPixelFloat;

typedef Image<BgrPixel>       BgrImage;
typedef Image<BgrPixelFloat>  BgrImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

#endif	/* ACOPENCVINCLUDE_H */
