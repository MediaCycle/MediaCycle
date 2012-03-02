/***************************************************************************
 *   Copyright (C) 2006 by Mian Zhou   *
 *   M.Zhou@reading.ac.uk   *
 *                                                                         *
 *  @author Xavier Siebert
 *  MediaCycle Project
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CVGABOR_H
#define CVGABOR_H

#include <cstdio>
#include <iostream>
#include "ACOpenCVInclude.h"
#include <cmath>

class CvGabor {
public:
    CvGabor();
    ~CvGabor();

    CvGabor(int iMu, int iNu, double dSigma = 2 * M_PI, double dF = sqrt(2.0));
    CvGabor(double dPhi, int iNu, double dSigma = 2 * M_PI, double dF = sqrt(2.0));
    void reset(int iMu, int iNu, double dSigma = 2 * M_PI, double dF = sqrt(2.0));
    void reset(double dPhi, int iNu, double dSigma = 2 * M_PI, double dF = sqrt(2.0));

    void setImage(cv::Mat);
    bool isInit();
    
    bool hasKernel();
    bool hasSourceImage();
    bool hasConvolImage();

    long setMaskWidth();
    long getMaskWidth();
    //    void output_file(const char *filename, int Type);

    void computeConvolutionImage();
    double* getMeanAndStdevs();

    void showKernel();
    void showConvolImage();
    void printKernel();

protected:
    double Sigma;
    double F;
    double Kmax;
    double K;
    double Phi;
    bool bInitialised;
    bool bKernel;
    long Width;

    cv::Mat Imag, Real;
    cv::Mat src_image, convol_image;

private:
    void createKernel();
    void init(double dPhi, int iNu, double dSigma, double dF);
};

#endif // CVGABOR_H
