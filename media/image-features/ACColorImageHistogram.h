/*
 *  ACColorImageHistogram.h
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


#ifndef _ACCOLORIMAGEHISTOGRAM_H
#define	_ACCOLORIMAGEHISTOGRAM_H

#include "ACOpenCVInclude.h"
#include "ACColorImageAnalysis.h"

#include <string>
#include <vector>
#include <fstream> 

// forward declaration
class ACColorImageAnalysis;

class ACColorImageHistogram{
public:
	ACColorImageHistogram(ACColorImageAnalysis* color_img, std::string _cmode="BGR", int _norm = 1);
	ACColorImageHistogram(cv::Mat image, std::string _cmode= "BGR", int _norm = 1); // image still to be splitted
	~ACColorImageHistogram();

//	ACColorImageHistogram& operator+= (const ACColorImageHistogram& H); 
//	// XS  NB: You can't overload a relational operator for a pointer type.
//	ACColorImageHistogram& operator+ (const ACColorImageHistogram& H);

	void normalize(const double&);
	void show();
	void dump(std::ostream &odump = std::cout);
	void reset();
//	double compare(ACColorImageHistogram*);
	
	cv::Mat getChannel(int);
//	inline int getSize() {return size;}
	inline int getNorm() {return norm;}
	float getValue(int,int);
	void showStats();
	void computeMoments(int highest_order);
	std::vector<double*> getMoments(){return moments;}
	double* getMoment(int i);
	double* getMean(){return mean;}
	double* getStdev(){return stdev;}
private:
	//method
	bool buildHistogram(); 
	void computeStats();

	// variables
	std::vector<cv::Mat> histos;
	//cv::MatND hist_mat; // this will make a Ndim histogram, but we want 3 1-dim
	cv::Mat im_src_mat;
	int norm;
	float range [3] [2]; // beginning and end values for each channel
	double mean[3], stdev[3];

	std::vector<double*> moments;
	std::string cmode;
};

#endif	// _ACCOLORIMAGEHISTOGRAM_H
