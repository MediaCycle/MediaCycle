/*
 *  ACImageAnalysis.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 8/10/09
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

#ifndef _ACIMAGEANALYSIS_H
#define	_ACIMAGEANALYSIS_H
#include <vector>

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
	return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
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

// --------------------------------------------------------------------

class ACImageAnalysis {
public:
    ACImageAnalysis(){};
  //  ACImageAnalysis(std::string filename){};
    ~ACImageAnalysis(){};
};

#endif	/* _ACIMAGEANALYSIS_H */
