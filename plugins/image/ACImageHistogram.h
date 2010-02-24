/*
 *  ACImageHistogram.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 13/03/09
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


#ifndef _ACIMAGEHISTOGRAM_H
#define	_ACIMAGEHISTOGRAM_H

#include "ACOpenCVInclude.h"

#include <string>
#include <vector>

class ACImageHistogram{
public:
	// XS TODO: same size on all channels ? yes if BRG, but what if HSV ?
	ACImageHistogram(IplImage** image, std::string cmode= "BGR", int size = 128, int norm = 1); // if image already split in 3
	ACImageHistogram(IplImage* image, std::string cmode= "BGR", int size = 128, int norm = 1); // image still to be splitted
	~ACImageHistogram();

	void initialize(IplImage** image, std::string cmode= "BGR", int size = 128, int norm = 1); 

	ACImageHistogram& operator+= (const ACImageHistogram& H); 
	// XS  NB: You can't overload a relational operator for a pointer type.
	ACImageHistogram& operator+ (const ACImageHistogram& H);

	void normalize(const double&);
	void show();
	void show(std::string);
	void dump();
	void dump(std::string);
	void reset();
	double compare(ACImageHistogram*);
	
	CvHistogram *getChannel(int);
	inline int getSize() {return size;}
	inline int getNorm() {return norm;}
	float getValue(int,int);
	void getStats();
	void showStats();
	void computeMoments(int highest_order);
	std::vector<double*> getMoments(){return moments;}
	double* getMoment(int i);
	double* getMean(){return mean;}
	double* getStdev(){return stdev;}

private:
	CvHistogram *hist[3];
	int norm, size;
	float range [3] [2]; // beginning and end values for each channel
	double mean[3], stdev[3];

	std::vector<double*> moments;
	std::string cmode;
};

#endif	/* _ACIMAGEHISTOGRAM_H */
