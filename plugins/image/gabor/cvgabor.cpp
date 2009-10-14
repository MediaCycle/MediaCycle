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
#include <iostream>
using namespace std;

CvGabor::CvGabor() {
    bInitialised = false;
    bKernel = false;
	Real = NULL;
	Imag = NULL;
}

CvGabor::~CvGabor() {
	cvReleaseMat( &Real );
	cvReleaseMat( &Imag );
}
 
// Create a gabor with a orientation iMu*PI/8, a scale iNu, a sigma value dSigma, and a spatial frequence dF.  
// iNu scale can be from -5 to infinity
CvGabor::CvGabor(int iMu, int iNu, double dSigma, double dF){
    bInitialised = true;
    bKernel = false;
	double dPhi = PI*iMu/8;
	Init(dPhi, iNu, dSigma, dF);
}

// Create a gabor with a orientation dPhi (in arc), a scale iNu, a sigma value dSigma, and a spatial frequence dF.
CvGabor::CvGabor(double dPhi, int iNu, double dSigma, double dF){
    bInitialised = false;
    bKernel = false;
 	Init(dPhi, iNu, dSigma, dF);
}

void CvGabor::Reset(int iMu, int iNu, double dSigma, double dF){
    double dPhi = PI*iMu/8;
	Init(dPhi, iNu, dSigma, dF);
}

void CvGabor::Reset(double dPhi, int iNu, double dSigma, double dF){
	Init(dPhi, iNu, dSigma, dF);
}

void CvGabor::Init(double dPhi, int iNu, double dSigma, double dF){
    bInitialised = true;
    Phi = dPhi;
	Kmax = PI/2;
    K = Kmax / pow(F, (double)iNu);
	Sigma = dSigma;
    F = dF; 
	
	// XS TODO : if initialized...
	if(Real!=NULL) cvReleaseMat( &Real );
	if(Imag!=NULL) cvReleaseMat( &Imag );
	
	Width = SetMaskWidth();
    Real = cvCreateMat( Width, Width, CV_32FC1);
    Imag = cvCreateMat( Width, Width, CV_32FC1);
    CreateKernel();  
}

// Determine whether the gabor has been initialized - variables F, K, Kmax, Phi, Sigma are filled.
bool CvGabor::IsInit(){
    return bInitialised;
}

// Return the width of mask (should be NxN) by the value of Sigma and iNu.
long CvGabor::SetMaskWidth() {
    if (IsInit() == false)  {
		perror ("Error: The Object has not been initilised in SetMaskWidth()!\n");
		return 0;
    }
    else {
		//determine the width of Mask
		double dModSigma = Sigma/K;
		double dWidth = round(dModSigma*6 + 1);
		//test whether dWidth is an odd.
		if (fmod(dWidth, 2.0)==0.0) dWidth++;
		return (long)dWidth;
    }
}
 
// Create 2 gabor kernels - REAL and IMAG, with an orientation and a scale 
// XS note : this is a wavelet, not just a filter
void CvGabor::CreateKernel() {
    if (IsInit() == false) {perror("Error: The Object has not been initilised in CreateKernel()!\n");}
    else {
		CvMat *mReal, *mImag;
		mReal = cvCreateMat( Width, Width, CV_32FC1);
		mImag = cvCreateMat( Width, Width, CV_32FC1);
		
		int x, y;
		double xt;
		double dReal;
		double dImag;
		double dTemp1, dTemp2, dTemp3;
		double K2 = pow(K,2);
		double S2 = pow(Sigma,2);
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Width; j++) {
				x = i-(Width-1)/2;
				y = j-(Width-1)/2;
				xt = K*cos(Phi)*x + K*sin(Phi)*y;
				dTemp1 = K2/S2*exp(-(pow((double)x,2)+pow((double)y,2))*K2/(2*S2));
				dTemp2 = cos(xt) - exp(-(S2/2)); 
				dTemp3 = sin(xt);
				dReal = dTemp1*dTemp2;
				dImag = dTemp1*dTemp3; 
				cvSetReal2D((CvMat*)mReal, i, j, dReal );
				cvSetReal2D((CvMat*)mImag, i, j, dImag );
			} 
		}
		bKernel = true;
		cvCopy(mReal, Real, NULL);
		cvCopy(mImag, Imag, NULL);
		cvReleaseMat( &mReal );
		cvReleaseMat( &mImag );
	}
}

IplImage* CvGabor::get_image(int Type) {
	// XS TODO : this is not good style
    if(!HasKernel()) { 
		perror("Error: the Gabor kernel has not been created in get_image()!\n");
		return NULL;
    }
    else {  
		IplImage* pImage;
		IplImage *newimage;
		newimage = cvCreateImage(cvSize(Width,Width), IPL_DEPTH_8U, 1 );		
		pImage = cvCreateImage( cvSize(Width,Width), IPL_DEPTH_32F, 1 );
		CvMat* kernel = cvCreateMat(Width, Width, CV_32FC1);
		double ve;
		CvSize size = cvGetSize( kernel );
		int rows = size.height;
		int cols = size.width;
		switch(Type)
		{
			case 1:  //Real
				cvCopy( (CvMat*)Real, (CvMat*)kernel, NULL );
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {
						ve = cvGetReal2D((CvMat*)kernel, i, j);
						cvSetReal2D( (IplImage*)pImage, j, i, ve );
					}
				}
				break;
			case 2:  //Imag
				cvCopy( (CvMat*)Imag, (CvMat*)kernel, NULL );
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {
						ve = cvGetReal2D((CvMat*)kernel, i, j);
						cvSetReal2D( (IplImage*)pImage, j, i, ve );
					}
				}
				break; 
			case 3:  //Magnitude
				///@todo  
				break;
			case 4:  //Phase
				///@todo
				break;
		}
		
		cvNormalize((IplImage*)pImage, (IplImage*)pImage, 0, 255, CV_MINMAX, NULL );
		cvConvertScaleAbs( (IplImage*)pImage, (IplImage*)newimage, 1, 0 );
		cvReleaseMat(&kernel);
		cvReleaseImage(&pImage);
		return newimage;
    }
}

bool CvGabor::HasKernel() {
    return bKernel;
}

long CvGabor::GetMaskWidth() {
	return Width;
}

// returns gabor kernel, as pointer to matrix structure, or NULL on failure.
CvMat* CvGabor::get_matrix(int Type) {
    if (!HasKernel()) {perror("Error: the gabor kernel has not been created!\n"); return NULL;}
    switch (Type)
    {
		case CV_GABOR_REAL:
			return Real;
			break;
		case CV_GABOR_IMAG:
			return Imag;
			break;
		case CV_GABOR_MAG:
			return NULL;
			break;
		case CV_GABOR_PHASE:
			return NULL;
			break;
    }
}

// Writes a gabor kernel as an image file.
void CvGabor::output_file(const char *filename, int Type) {
	IplImage *pImage;
	pImage = get_image(Type);
	if(pImage != NULL) {
		if( cvSaveImage(filename, pImage )) printf("%s has been written successfully!\n", filename);
		else printf("Error: writting %s has failed!\n", filename);
	}
	else perror("Error: the image is empty in output_file()!\n"); 
	cvReleaseImage(&pImage);
}

void CvGabor::show(int Type) {
    if(!IsInit()) {
        perror("Error: the gabor kernel has not been created!\n");
    }
    else {
		//    IplImage *pImage;
		//pImage = get_image(Type);
		//cvNamedWindow("Testing",1);
		//cvShowImage("Testing",pImage);
		//cvWaitKey(0);
		//cvDestroyWindow("Testing");
		//cvReleaseImage(&pImage);
    }
}

void CvGabor::normalize( const CvArr* src, CvArr* dst, double a, double b, int norm_type, const CvArr* mask ) {
    CvMat* tmp = 0;
    double scale, shift;
    
    if( norm_type == CV_MINMAX ) {
        double smin = 0, smax = 0;
        double dmin = MIN( a, b ), dmax = MAX( a, b );
        cvMinMaxLoc( src, &smin, &smax, 0, 0, mask );
        scale = (dmax - dmin)*(smax - smin > DBL_EPSILON ? 1./(smax - smin) : 0);
        shift = dmin - smin*scale;
    }
    else if( norm_type == CV_L2 || norm_type == CV_L1 || norm_type == CV_C ) {
		scale = cvNorm( src, 0, norm_type, mask );
        scale = scale > DBL_EPSILON ? 1./scale : 0.;
        shift = 0;
    }
    else {} // ????
	
    if( !mask )
        cvConvertScale( src, dst, scale, shift );
    else {
        cvConvertScale( src, tmp, scale, shift );
        cvCopy( tmp, dst, mask );
    }
	
	cvReleaseMat( &tmp );
}

void CvGabor::conv_img(IplImage *src, IplImage *dst, int Type) {
	double ve;
	
	CvMat *mat = cvCreateMat(src->width, src->height, CV_32FC1);
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			ve = CV_IMAGE_ELEM(src, uchar, j, i); // fast way to access pixels (faster than GetReal2D)
			CV_MAT_ELEM(*mat, float, i, j) = (float)ve; // fast way to access pixels (faster than SetReal2D)
		}
	}
	
	CvMat *rmat = cvCreateMat(src->width, src->height, CV_32FC1);
	CvMat *imat = cvCreateMat(src->width, src->height, CV_32FC1);
	
	switch (Type)
	{
		case CV_GABOR_REAL:
			cvFilter2D( (CvMat*)mat, (CvMat*)mat, (CvMat*)Real, cvPoint( (Width-1)/2, (Width-1)/2));
			break;
		case CV_GABOR_IMAG:
			cvFilter2D( (CvMat*)mat, (CvMat*)mat, (CvMat*)Imag, cvPoint( (Width-1)/2, (Width-1)/2));
			break;
		case CV_GABOR_MAG:
			cvFilter2D( (CvMat*)mat, (CvMat*)rmat, (CvMat*)Real, cvPoint( (Width-1)/2, (Width-1)/2));
			cvFilter2D( (CvMat*)mat, (CvMat*)imat, (CvMat*)Imag, cvPoint( (Width-1)/2, (Width-1)/2));
			
			cvPow(rmat,rmat,2); 
			cvPow(imat,imat,2);
			cvAdd(imat,rmat,mat); 
			cvPow(mat,mat,0.5); 
			break;
		case CV_GABOR_PHASE:
			//TODO
			break;
	}
	
	if (dst->depth == IPL_DEPTH_8U) {
		cvNormalize((CvMat*)mat, (CvMat*)mat, 0, 255, CV_MINMAX);
		for (int i = 0; i < mat->rows; i++) {
			for (int j = 0; j < mat->cols; j++) {
				ve = CV_MAT_ELEM(*mat, float, i, j);
				CV_IMAGE_ELEM(dst, uchar, j, i) = (uchar)cvRound(ve);
			}
		}
	}
	else if (dst->depth == IPL_DEPTH_32F) {
		for (int i = 0; i < mat->rows; i++) {
			for (int j = 0; j < mat->cols; j++) {
				ve = cvGetReal2D((CvMat*)mat, i, j);
				cvSetReal2D( (IplImage*)dst, j, i, ve );
			}
		}
	}
	cvReleaseMat(&imat);
	cvReleaseMat(&rmat);
	cvReleaseMat(&mat);
}

double* CvGabor::GetFeatures(IplImage *src) {	
	// returns a pointer to a double[2] array containing mean and stdev.
	CvMat *mat = cvCreateMat(src->width, src->height, CV_32FC1);
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			// fast way to access pixels (faster than Set/GetReal2D)
			CV_MAT_ELEM(*mat, float, i, j) = (float) CV_IMAGE_ELEM(src, uchar, j, i);
		}
	}
	
	CvMat *rmat = cvCreateMat(src->width, src->height, CV_32FC1);
	CvMat *imat = cvCreateMat(src->width, src->height, CV_32FC1);
	
// CV_GABOR_MAG -- XS TODO : speed this up ?!
	cvFilter2D( (CvMat*)mat, (CvMat*)rmat, (CvMat*)Real, cvPoint( (Width-1)/2, (Width-1)/2));
	cvFilter2D( (CvMat*)mat, (CvMat*)imat, (CvMat*)Imag, cvPoint( (Width-1)/2, (Width-1)/2));
	cvPow(rmat,rmat,2); 
	cvPow(imat,imat,2);
	cvAdd(imat,rmat,mat); 
	cvPow(mat,mat,0.5); // not necessary ?!
	
	CvScalar mean, stdev;
	cvAvgSdv( mat, &mean, &stdev); 
	
	// XS TODO : this is clumsy !
//	double stats[2] = {mean.val[0], stdev.val[0] };
	double *rstats = new double[2] ;
	rstats[0] = mean.val[0];
	rstats[1] = stdev.val[0];

	
	cout << "mean = " << mean.val[0] << "; stdev = " << stdev.val[0] <<endl;
	cvReleaseMat(&imat);
	cvReleaseMat(&rmat);
	cvReleaseMat(&mat);
	return rstats;
}

