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
}

CvGabor::~CvGabor() {
	// XS automatic cleanup in opencv 2.*, no cvReleaseMat necessary
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
		
	Width = SetMaskWidth();
	
	// XS TODO do old Real and Imag get cleaned up properly ?
	Real = cv::Mat( Width, Width, CV_32FC1);
	Imag = cv::Mat( Width, Width, CV_32FC1);
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
				Real.at<float>(i,j)=dReal; //*Width*Width;
				Imag.at<float>(i,j)=dImag;
			} 
		}
		bKernel = true;
//		for (int i = 0; i < Width; i++) {
//			for (int j = 0; j < Width; j++) {
//				cout << "norm : " << i << " - " << j << " : " << Real.at<float>(i,j) << " ; " << Imag.at<float>(i,j) << endl;
//			} 
//		}
	}
}

//IplImage* CvGabor::get_image(int Type) {
cv::Mat CvGabor::get_image(int Type) {
	// XS TODO : this is not good style
    if(!HasKernel()) { 
		perror("Error: the Gabor kernel has not been created in get_image()!\n");
		return cv::Mat(); // empty mat
    }
    else {  
		cv::Mat pImage(Width,Width, CV_8UC1);
		cv::Mat newimage(Width,Width, CV_32FC1);
		cv::Mat kernel(Width, Width, CV_32FC1);
//		double ve;
//		CvSize size = cv::getSize( kernel );
//		int rows = size.height;
//		int cols = size.width;
		switch(Type)
		{
			case 1:  //Real
//				cvCopy( (CvMat*)Real, (CvMat*)kernel, 0 );
				Real.copyTo(kernel);

//				for (int i = 0; i < rows; i++) {
//					for (int j = 0; j < cols; j++) {
//						ve = cvGetReal2D((CvMat*)kernel, i, j);
//						cvSetReal2D( (IplImage*)pImage, j, i, ve );
//					}
//				}
				break;
			case 2:  //Imag
//				cvCopy( (CvMat*)Imag, (CvMat*)kernel, 0 );
				Imag.copyTo(kernel);

//				for (int i = 0; i < rows; i++) {
//					for (int j = 0; j < cols; j++) {
//						ve = cvGetReal2D((CvMat*)kernel, i, j);
//						cvSetReal2D( (IplImage*)pImage, j, i, ve );
//					}
//				}
				break; 
			case 3:  //Magnitude
				///@todo  
				break;
			case 4:  //Phase
				///@todo
				break;
		}
		
	// XS TODO ??
//		cvNormalize((IplImage*)pImage, (IplImage*)pImage, 0, 255, CV_MINMAX, 0 );
//		cvConvertScaleAbs( (IplImage*)pImage, (IplImage*)newimage, 1, 0 );
		cv::normalize(pImage,pImage, 0, 255);
		cv::convertScaleAbs(pImage,newimage, 1, 0 );
		return newimage;
    }
}

bool CvGabor::HasKernel() {
    return bKernel;
}

long CvGabor::GetMaskWidth() {
	return Width;
}

// returns gabor kernel, as pointer to matrix structure, or 0 on failure.
//CvMat* CvGabor::get_matrix(int Type) {
//    if (!HasKernel()) {perror("Error: the gabor kernel has not been created!\n"); return 0;}
//    switch (Type)
//    {
//		case CV_GABOR_REAL:
//			return Real;
//			break;
//		case CV_GABOR_IMAG:
//			return Imag;
//			break;
//		case CV_GABOR_MAG:
//			return 0;
//			break;
//		case CV_GABOR_PHASE:
//			return 0;
//			break;
//    }
//}

// Writes a gabor kernel as an image file.
//void CvGabor::output_file(const char *filename, int Type) {
//	IplImage *pImage;
//	pImage = get_image(Type);
//	if(pImage != 0) {
//		if( cvSaveImage(filename, pImage )) printf("%s has been written successfully!\n", filename);
//		else printf("Error: writting %s has failed!\n", filename);
//	}
//	else perror("Error: the image is empty in output_file()!\n"); 
//	cvReleaseImage(&pImage);
//}

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

void CvGabor::conv_img(cv::Mat src, cv::Mat dst, int Type) {
//	double ve;
	
//	CvMat *mat = cvCreateMat(src->width, src->height, CV_32FC1);
	cv::Size size = src.size();
	cv::Mat mat (size.width, size.height, CV_32FC1);
	src.copyTo(mat);
//	for (int i = 0; i < src->width; i++) {
//		for (int j = 0; j < src->height; j++) {
//			ve = CV_IMAGE_ELEM(src, uchar, j, i); // fast way to access pixels (faster than GetReal2D)
//			CV_MAT_ELEM(*mat, float, i, j) = (float)ve; // fast way to access pixels (faster than SetReal2D)
//		}
//	}
	
//	CvMat *rmat = cvCreateMat(src->width, src->height, CV_32FC1);
//	CvMat *imat = cvCreateMat(src->width, src->height, CV_32FC1);

	cv::Mat rmat (size.width, size.height, CV_32FC1);
	cv::Mat imat (size.width, size.height, CV_32FC1);

	switch (Type)
	{
		case CV_GABOR_REAL:
//			cvFilter2D( (CvMat*)mat, (CvMat*)mat, (CvMat*)Real, cvPoint( (Width-1)/2, (Width-1)/2));
			cv::filter2D(mat, mat, 1, Real, cv::Point(-1,-1));
			break;
		case CV_GABOR_IMAG:
//			cvFilter2D( (CvMat*)mat, (CvMat*)mat, (CvMat*)Imag, cvPoint( (Width-1)/2, (Width-1)/2));
			cv::filter2D(mat,mat, 1, Imag, cvPoint( (Width-1)/2, (Width-1)/2));
			break;
		case CV_GABOR_MAG:
//			cvFilter2D( (CvMat*)mat, (CvMat*)rmat, (CvMat*)Real, cvPoint( (Width-1)/2, (Width-1)/2));
//			cvFilter2D( (CvMat*)mat, (CvMat*)imat, (CvMat*)Imag, cvPoint( (Width-1)/2, (Width-1)/2));
			
			cv::filter2D(mat, rmat, 1, Real, cv::Point(-1,-1));
			cv::filter2D(mat, imat, 1, Imag, cv::Point(-1,-1));

//			cvPow(rmat,rmat,2); 
//			cvPow(imat,imat,2);
//			cvAdd(imat,rmat,mat); 
//			cvPow(mat,mat,0.5); 
			cv::pow(rmat,2,rmat); 
			cv::pow(imat,2,imat);
			cv::add(imat,rmat,mat); 
			cv::pow(mat,0.5,mat); 
			break;
		case CV_GABOR_PHASE:
			//TODO
			break;
	}
	
	if (dst.depth() == IPL_DEPTH_8U) {
		//cvNormalize((CvMat*)mat, (CvMat*)mat, 0, 255, CV_MINMAX);
		cv::normalize(mat,mat, 0, 255);
		mat.copyTo(dst);
		//		for (int i = 0; i < mat->rows; i++) {
//			for (int j = 0; j < mat->cols; j++) {
//				ve = CV_MAT_ELEM(*mat, float, i, j);
//				CV_IMAGE_ELEM(dst, uchar, j, i) = (uchar)cvRound(ve);
//			}
//		}
	}
	else if (dst.depth() == IPL_DEPTH_32F) {
		mat.copyTo(dst);
//		for (int i = 0; i < mat->rows; i++) {
//			for (int j = 0; j < mat->cols; j++) {
//				ve = cvGetReal2D((CvMat*)mat, i, j);
//				cvSetReal2D( (IplImage*)dst, j, i, ve );
//			}
//		}
	}
//	cvReleaseMat(&imat);
//	cvReleaseMat(&rmat);
//	cvReleaseMat(&mat);
}

double* CvGabor::getMeanAndStdevs(cv::Mat src) {	
	// returns a pointer to a double[2] array containing mean and stdev.
	//CvMat *mat = cvCreateMat(src->width, src->height, CV_32FC1);
	
//	cv::namedWindow("mat", CV_WINDOW_AUTOSIZE);
//	cv::imshow("mat", mat);
//	cv::waitKey(0);
	
//	for (int i = 0; i < src->width; i++) {
//		for (int j = 0; j < src->height; j++) {
//			// fast way to access pixels (faster than Set/GetReal2D)
//			CV_MAT_ELEM(mat, float, i, j) = (float) CV_IMAGE_ELEM(src, uchar, j, i);
//		}
//	}
	
	//CvMat *rmat = cvCreateMat(src->width, src->height, CV_32FC1);
	//CvMat *imat = cvCreateMat(src->width, src->height, CV_32FC1);
	//XS NEW c++ way
		
	cv::Mat mat;
	src.copyTo(mat);
	int nrows=src.rows;
	int ncols=src.cols;
	
  	cv::Mat rmat(nrows, ncols, CV_32FC1);
	cv::Mat imat(nrows, ncols, CV_32FC1);
	
// CV_GABOR_MAG -- XS TODO : speed this up ?!
	//int wtf = (Width-1)/2;
	//CvPoint p = cvPoint(wtf, wtf);
	//cvFilter2D( (CvMat*)mat, (CvMat*)rmat, (CvMat*)Real, p);
	//cvFilter2D( (CvMat*)mat, (CvMat*)imat, (CvMat*)Imag, p);

	//XS NEW c++ way
	
//	for (int i = 0; i < nrows; i++){	
//		for (int j = 0; j < ncols; j++){
//			cout << i << " - " << j << " : " << mat.at<float>(i,j) << endl;
//		}
//	}
	cv::Point p(-1,-1);
	cv::filter2D(mat, rmat, -1, Real, p, 0, cv::BORDER_CONSTANT);
	cv::filter2D(mat, imat, -1, Imag, p, 0, cv::BORDER_CONSTANT);
	
	cv::pow(rmat,2,rmat); 
	cv::pow(imat,2,imat);
	cv::add(imat,rmat,mat); 
	
//	cv::Mat omat = mat.clone();
//
	cv::pow(mat,0.5,mat); 	
	cv::Scalar mean, stdev;
	cv::meanStdDev(mat, mean, stdev); 
	
	// XS TODO : this is clumsy !
//	double stats[2] = {mean.val[0], stdev.val[0] };
	double *rstats = new double[2] ;
	rstats[0] = mean.val[0];
	rstats[1] = stdev.val[0];

#ifdef VERBOSE	
	cout << "mean = " << mean.val[0] << "; stdev = " << stdev.val[0] <<endl;
#endif // VERBOSE

	return rstats;
}

