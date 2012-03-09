/***************************************************************************
 *   Copyright (C) 2006 by Mian Zhou                                       *
 *   M.Zhou@reading.ac.uk                                                  *
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
#include "cvgabor.h"

using namespace std;

//#define VERBOSE

CvGabor::CvGabor() {
    bInitialised = false;
    bKernel = false;
    cv::Mat src_image();
}

CvGabor::~CvGabor() {
    // XS automatic cleanup in opencv 2.*, no cvReleaseMat necessary
}
 
// Create a gabor with a orientation iMu*PI/8, a scale iNu, a sigma value dSigma, and a spatial frequence dF.  
// iNu scale can be from -5 to infinity
CvGabor::CvGabor(int iMu, int iNu, double dSigma, double dF) {
    bInitialised = true;
    bKernel = false;
    cv::Mat src_image();
    reset(iMu, iNu, dSigma, dF);
}

// Create a gabor with a orientation dPhi (in arc), a scale iNu, a sigma value dSigma, and a spatial frequence dF.
CvGabor::CvGabor(double dPhi, int iNu, double dSigma, double dF) {
    bInitialised = false;
    bKernel = false;
    cv::Mat src_image();
    reset(dPhi, iNu, dSigma, dF);
}

void CvGabor::reset(int iMu, int iNu, double dSigma, double dF) {
    double dPhi = M_PI * iMu / 8;
    init(dPhi, iNu, dSigma, dF);
}

void CvGabor::reset(double dPhi, int iNu, double dSigma, double dF) {
    init(dPhi, iNu, dSigma, dF);
}

void CvGabor::init(double dPhi, int iNu, double dSigma, double dF) {
    bInitialised = true;
    F = dF;
    Phi = dPhi;
    Kmax = M_PI / 2;
    K = Kmax / pow(F, (double) iNu);
    Sigma = dSigma;

    Width = setMaskWidth();

    Real = cv::Mat(Width, Width, CV_32FC1);
    Imag = cv::Mat(Width, Width, CV_32FC1);
    createKernel();
}

bool CvGabor::hasSourceImage(){
    bool ok = !(src_image.empty());
    return ok;
}

bool CvGabor::hasConvolImage(){
    return (!convol_image.empty());
}

void CvGabor::setImage(cv::Mat src){
    if (src.empty()){
        cerr << "<CvGabor::setImage> : empty image" << endl;
        return;
    }
    src.convertTo(this->src_image, CV_32FC1); // invokes mat.create(src->size());
}

// Determine whether the gabor has been initialized - variables F, K, Kmax, Phi, Sigma are filled.
bool CvGabor::isInit(){
    return bInitialised;
}

// Return the width of mask (should be NxN) by the value of Sigma and iNu.
long CvGabor::setMaskWidth() {
    if (isInit() == false) {
        cerr << "<CvGabor::setMaskWidth> needs initialization of kernel parameters" << endl;
    } else {
        //determine the width of Mask
        double dModSigma = Sigma / K;
        double dWidth = round(dModSigma * 6 + 1);
        //test whether dWidth is an odd.
        if (fmod(dWidth, 2.0) == 0.0) dWidth++;
        return (long) dWidth;
    }
}
 
// Create 2 gabor kernels - REAL and IMAG, with an orientation and a scale 
// XS note : this is a wavelet, not just a filter
void CvGabor::createKernel() {
    if (isInit() == false) {
        cerr << "<CvGabor::createKernel> needs initialization of kernel parameters" << endl;
    } else {
        int x=0, y=0;
        double xt=0.0;
        double dReal=0.0, dImag=0.0;
        double dTemp1=0.0, dTemp2=0.0, dTemp3=0.0;
        double K2 = pow(K, 2);
        double S2 = pow(Sigma, 2);
        for (int i = 0; i < Width; i++) {
            for (int j = 0; j < Width; j++) {
                x = i - (Width - 1) / 2;
                y = j - (Width - 1) / 2;
                xt = K * cos(Phi) * x + K * sin(Phi) * y;
                dTemp1 = K2 / S2 * exp(-(pow((double) x, 2) + pow((double) y, 2)) * K2 / (2 * S2));
                dTemp2 = cos(xt) - exp(-(S2 / 2));
                dTemp3 = sin(xt);
                dReal = dTemp1*dTemp2;
                dImag = dTemp1*dTemp3;
                Real.at<float>(i, j) = dReal; //*Width*Width;
                Imag.at<float>(i, j) = dImag;
            }
        }
        bKernel = true;
     }
}

void CvGabor::printKernel() {
    for (int i = 0; i < Width; i++) {
        for (int j = 0; j < Width; j++) {
            cout << "norm : " << i << " - " << j << " : " << Real.at<float>(i, j) << " ; " << Imag.at<float>(i, j) << endl;
        }
    }
}

void CvGabor::showKernel() {
    if (!hasKernel()) {
        cerr << "<CvGabor::showKernel> no kernel to show !" << endl;
        return;
    }
    cv::Mat realKernelF(Width, Width, CV_32FC1);
    cv::Mat realKernelU(Width, Width, CV_8UC1);
    cv::Mat imagKernelF(Width, Width, CV_8UC1);
    cv::Mat imagKernelU(Width, Width, CV_8UC1);
    cv::normalize(Real,realKernelF, 0, 255, CV_MINMAX);
    realKernelF.convertTo(realKernelU, CV_8UC1);
    cv::normalize(Imag,imagKernelF, 0, 255, CV_MINMAX);
    imagKernelF.convertTo(imagKernelU, CV_8UC1);

//    for (int i = 0; i < Width; i++) {
//        for (int j = 0; j < Width; j++) {
//            cout << "8U norm : " << i << " - " << j << " : " << (int) realKernelF.at<uchar>(i, j) << " ; " << (int) imagKernelU.at<uchar>(i, j) << endl;
//        }
//    }

    cv::namedWindow("Real Kernel", CV_WINDOW_AUTOSIZE);
    cv::imshow("Real Kernel", realKernelU);
    cvMoveWindow("Real Kernel", 50, 50);
    cv::waitKey(0);
    cv::destroyWindow("Real Kernel");
    cv::namedWindow("Imag Kernel", CV_WINDOW_AUTOSIZE);
    cv::imshow("Imag Kernel", imagKernelU);
    cvMoveWindow("Imag Kernel", 150, 50);
    cv::waitKey(0);
    cv::destroyWindow("Imag Kernel");
}

void CvGabor::showConvolImage() {
    if (!hasConvolImage()) {
        cerr << "<CvGabor::showConvolImage> convolution image not created" << endl;
        return;
    }
    cv::Mat convol_imageF(convol_image.size(), CV_32FC1);
    cv::Mat convol_imageU(convol_image.size(), CV_8UC1);
    cv::normalize(convol_image, convol_imageF, 0, 255, CV_MINMAX);
    convol_imageF.convertTo(convol_imageU, CV_8UC1);
    cv::imshow("Convoluted Image (magnitude)", convol_imageU);
    cvMoveWindow("Convoluted Image (magnitude)", 50, 50);
    cv::waitKey(0);
    cv::destroyWindow("Convoluted Image (magnitude)");
}

bool CvGabor::hasKernel() {
    return bKernel;
}

long CvGabor::getMaskWidth() {
	return Width;
}

// always computes a magnitude image
void  CvGabor::computeConvolutionImage() {
    if (!hasSourceImage()) {
        cerr << "<CvGabor::computeConvolutionImage> : no source image" << endl;
        return;
    }
    int nrows = src_image.rows;
    int ncols = src_image.cols;
//    cv::Mat convol_image(nrows, ncols, CV_32FC1);
    cv::Mat rmat(nrows, ncols, CV_32FC1);
    cv::Mat imat(nrows, ncols, CV_32FC1);

    cv::Point p(-1, -1);
    cv::filter2D(src_image, rmat, -1, Real, p, 0, cv::BORDER_CONSTANT);
    cv::filter2D(src_image, imat, -1, Imag, p, 0, cv::BORDER_CONSTANT); // XS TODO (Width-1)/2, (Width-1)/2) for imag ?
    cv::pow(rmat, 2, rmat);
    cv::pow(imat, 2, imat);
    cv::add(imat, rmat, this->convol_image);
    cv::pow(this->convol_image, 0.5, this->convol_image);
}

// returns a pointer to a double[2] array containing mean and stdev.
double* CvGabor::getMeanAndStdevs() {
    if (!hasConvolImage()) {
        cerr << "<CvGabor::getMeanAndStdevs> : no input image" << endl;
        return 0;
    }

    cv::Scalar mean, stdev;
    cv::meanStdDev(convol_image, mean, stdev);

    // XS TODO : this is clumsy !
    //	double stats[2] = {mean.val[0], stdev.val[0] };
    double *rstats = new double[2];
    rstats[0] = mean.val[0];
    rstats[1] = stdev.val[0];

#ifdef VERBOSE	
    cout << "mean = " << mean.val[0] << "; stdev = " << stdev.val[0] << endl;
#endif // VERBOSE

    return rstats;
}

