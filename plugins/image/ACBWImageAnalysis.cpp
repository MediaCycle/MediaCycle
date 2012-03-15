/*
 *  ACBWImageAnalysis.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 8/06/10
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

#include "ACBWImageAnalysis.h"
#include <fstream>
#include <cmath>
#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;

//#define VISUAL_CHECK

// --------------------------------------------------------------------

// helper function : modified log, returns
// . log(n) if n > 0
// . 0 if n == 0
// . -log(abs(n)) if n < 0
// used mostly for Hu Moments, see p. 256 of OpenCV book [O'Reilly 2008] 
static float modifiedLog(float n){
	if (n == 0)
		return 0;
	else if (n < 0)
		return (- log (-n)/log(10)) ;
	return log(n)/log(10);
};

static double modifiedLog(double n){
	if (n == 0)
		return 0;
	else if (n < 0)
		return (- log (-n)/log(10)) ;
	return log(n)/log(10);
};

// --------------------------------------------------------------------


ACBWImageAnalysis::ACBWImageAnalysis() : ACImageAnalysis(){
	// needs setFileName or setImage later...
}

ACBWImageAnalysis::ACBWImageAnalysis(const string &filename){
	reset();
	setFileName(filename);
	// forcing it to be read as grayscale
	cv::Mat imgp_full_mat = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE); 
	scaleImage(imgp_full_mat);
}

// in this case the filename will remain unknown
// so it is not good to generate ACL files (where one entry is the filename)
// but it can be used for calculations
ACBWImageAnalysis::ACBWImageAnalysis(cv::Mat img_full_mat){
	reset();
	
	if (img_full_mat.channels() == 1) {
		scaleImage(img_full_mat, 1.0);
	}
	else if (img_full_mat.channels() == 3){
		// assume incoming color model is "BGR"
		cv::Mat bw_img_mat;
		cv::cvtColor(img_full_mat, bw_img_mat, CV_BGR2GRAY);
		scaleImage(bw_img_mat, 1.0);
		}
	else {
		cout << "<ACBWImageAnalysis> unsupported number of channels : "<< img_full_mat.channels() << endl;
	}
}

void ACBWImageAnalysis::reset(){
	imgp_mat = cv::Mat();
	fft = 0;
	HAS_FFT = false;
	original_width = original_height = 0;
	file_name = "";
}

void ACBWImageAnalysis::clean(){
	if (HAS_FFT) {
		if (fft != 0) fftw_free (fft);
	}
}

ACBWImageAnalysis::~ACBWImageAnalysis(){
	clean();
}

void ACBWImageAnalysis::computeFFT2D (){
	int height = this->getHeight();
	int width = this->getWidth();
	const int cdim = height * (width / 2 + 1);
	try {
		fft = new fftw_complex [cdim];
	}
	catch (std::bad_alloc) {
		cerr << "Memory allocation problem in fft2d for fft" << endl;
	}
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	ddata = (uchar*) getImageMat().data; // XS TODO check this 2.*
	fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft, FFTW_ESTIMATE); 
	for (int j = 0; j < area; j++)
		data_in[j] = (double) ddata[j];
	fftw_execute(p);
	fftw_destroy_plan(p);
	HAS_FFT = true;
}

void ACBWImageAnalysis::computeFFT2D_complex (){
	int height = this->getHeight();
	int width = this->getWidth();
	
	// make image double sized on x and y, and pad with zeros to avoid aliasing
	const int double_area = 4 * width * height;
	fftw_complex *data_in;
	try {
		fft = new fftw_complex [double_area];
		data_in = new fftw_complex [double_area];
	}
	catch (std::bad_alloc) {
		cerr << "Memory allocation problem in fft2d for fft" << endl;
	}
	int k = 0;
	// XS TODO check this 2.*
//	BwImage Im(getImage());
	for(int i = 0; i < height/2 ; i++ ) {
		for( int j = 0 ; j < 2*width ; j++ ) {
			data_in[k][0] = 0.0;
			data_in[k][1] = 0.0;
			k++;
		}
	}
	for(int i = height/2; i < 3*height/2 ; i++ ) {
		for( int j = 0 ; j < width/2 ; j++ ) {
			data_in[k][0] = 0.0;
			data_in[k][1] = 0.0;
			k++;
		}
		for( int j = width/2 ; j < 3*width/2 ; j++ ) {
//			data_in[k][0] = ( double )Im[i-height/2][j-width/2];
			data_in[k][0] = this->getImageMat().at<double>(i-height/2,j-width/2); // XS TODO check 2.* is order fine ? speed ?
			data_in[k][1] = 0.0;
			k++;
		}
		for( int j = 3*width/2 ; j < 2*width ; j++ ) {
			data_in[k][0] = 0.0;
			data_in[k][1] = 0.0;
			k++;
		}
	}
	for(int i = 3*height/2; i < 2*height ; i++ ) {
		for( int j = 0 ; j < 2*width ; j++ ) {
			data_in[k][0] = 0.0;
			data_in[k][1] = 0.0;
			k++;
		}
	}
	
//	// XS test
//	IplImage* test  = cvCreateImage(cvSize(2*width,2*height),IPL_DEPTH_8U,1);
//	BwImage Imtest(test);
//	k=0;
//	for(int i=0; i< 2*height; i++){
//		for (int j=0; j< 2*width; j++){
//			Imtest[i][j] = (uchar) data_in[k][0];
//			k++;
//		}
//	}
//	cvNamedWindow("test", 1);
//	cvShowImage("test",test );
//	cvResizeWindow("test",height,width); 
//	cvWaitKey();
//	cvDestroyWindow("test");
	
	
	fftw_plan p = fftw_plan_dft_2d(2*height, 2*width, data_in, fft,  FFTW_FORWARD, FFTW_ESTIMATE); 
	fftw_execute(p);
	fftw_destroy_plan(p);
	
// XS debug
//	k=0;
//	for(int i=0; i< 20; i++){
//		for (int j=0; j< 20; j++){
//			cout << sqrt((fft[k][0]*fft[k][0]+fft[k][1]*fft[k][1]))/double_area << " ";// *255/sqrt(xmax);
//			k++;
//		}
//		cout << endl;
//	}
	
	// end XS debug
	
	delete data_in;
	HAS_FFT = true;
}

void ACBWImageAnalysis::computeFFT2D_centered (){
	// 0 0 will appear at the center of the FFT image
	int height = this->getHeight();
	int width = this->getWidth();

 	const int cdim = height * (width / 2 + 1);
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	try {
		fft = new fftw_complex [cdim];
		// XS TODO delete first
	}
	catch (std::bad_alloc) {
		cerr << "Memory allocation problem in fft2d for fft" << endl;
	}
	// XS TODO 2.* check this
	ddata = (uchar*) getImageMat().data; 
	int k=0;
	for(int irow=0; irow< height; irow++){
		for (int icolumn=0; icolumn< width; icolumn++){
			data_in[k] = double(pow(float(-1),float(irow+icolumn)) * (double)(ddata[irow*width+icolumn] +128));
			k++;
		}
	}		
	fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft, FFTW_ESTIMATE); 
	fftw_execute(p);
	fftw_destroy_plan(p);
	HAS_FFT = true;
}

 // image moments (with optional threshold, 0=none by default), not contour moments
void ACBWImageAnalysis::computeHuMoments(int thresh){
	hu_moments.clear();
	raw_moments.clear();
	if (!imgp_mat.data){
		cerr << " <ACBWImageAnalysis::computeHuMoments> : missing or empty image !" << endl;
		return;
	}
	cv::Moments local_raw_moments;
	double* local_Hu_moments = new double[7];
	
	if (thresh !=0) {
		// avoid overwriting image when computing threshold
		cv::Mat BWimg_mat (this->getSize(), CV_8UC1); 
		cv::threshold(imgp_mat, BWimg_mat, thresh, 255, CV_THRESH_BINARY_INV ); 
		local_raw_moments = cv::moments(BWimg_mat);
	}
	else {
		local_raw_moments = cv::moments(imgp_mat);
	}
	
	cv::HuMoments(local_raw_moments, local_Hu_moments); 
		
	raw_moments.push_back (modifiedLog(local_raw_moments.m00)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m10)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m01)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m20)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m11)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m02)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m30)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m21)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m12)) ;
	raw_moments.push_back (modifiedLog(local_raw_moments.m03)) ;

	for (int i=0; i<7 ; i++){
		hu_moments.push_back (modifiedLog(local_Hu_moments[i])) ;
	}
    delete[] local_Hu_moments;
}


//void ACBWImageAnalysis::computeContourHuMoments(int thresh){ 
//	contour_hu_moments.clear();
//	
//	if (!imgp.data){
//		cerr << " <ACBWImageAnalysis::computeContourHuMoments() error> missing image !" << endl;
//		return;
//	}
//	// we create again a BW image because threshold would overwrite it
//	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
//	cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
//	CvMemStorage*  storage  = cvCreateMemStorage(0);
//	CvSeq* contours = 0;
//	cvFindContours( BWimg, storage, &contours ); 
//	
//	CvMoments my_contour_moments;
//	CvHuMoments my_contour_Humoments ;
//	cvContourMoments (BWimg, &my_contour_moments); // apparently it's just an alias to cvMoments
//	cvGetHuMoments(&my_contour_moments, &my_contour_Humoments);
//	
//	cvReleaseImage(&BWimg);
//	cvReleaseMemStorage(&storage );	
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu1)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu2)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu3)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu4)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu5)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu6)) ;
//	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu7)) ;
//}

void ACBWImageAnalysis::computeGaborMoments(int mumax, int numax){ // default 4,2 (7,5 previously)
	gabor_moments.clear();
	if (!imgp_mat.data) {
		cerr << " <ACBWImageAnalysis::computeGaborMoments() error> missing image !" << endl;
		return;
	}
	CvGabor *gabor = new CvGabor();
        gabor->setImage(this->getImageMat());
	for (int i = 0; i < mumax; i++){	
		for (int j = 0; j < numax; j++){
			gabor->reset(i, j);
            gabor->computeConvolutionImage();
            double *tmpft = gabor->getMeanAndStdevs(); // will compute all what's necessary
			gabor_moments.push_back (tmpft[0]) ;
			gabor_moments.push_back (tmpft[1]) ;
			delete [] tmpft;
#ifdef VISUAL_CHECK
                        gabor->showKernel();
                        gabor->showConvolImage();
#endif
		}
	}
	delete gabor;
}

void ACBWImageAnalysis::computeGaborMoments_fft(int numPha_, int numFreq_, uint horizonalMargin_, uint verticalMargin_){ // default 7, 5, 0, 0
	gabor_moments.clear();
	if (!imgp_mat.data) {
		cerr << " <ACBWImageAnalysis::computeGaborMoments() error> missing image !" << endl;
		return;
	}
	
	Gabor* gabor = new Gabor(dynamic_cast<ACImageAnalysis*>(this));
	gabor->calculate(numPha_, numFreq_, horizonalMargin_, verticalMargin_);
	CvScalar mean, stdev;
	for(uint i=0;i<numPha_*numFreq_;++i) {
		IplImage* gaborImage=gabor->getImage(i);
		cvNormalize((IplImage*)gaborImage, (IplImage*)gaborImage, 0, 255, CV_MINMAX, 0 );
		//XS test
		cvNamedWindow("test2", CV_WINDOW_AUTOSIZE);
		cvShowImage("test2", gaborImage);
		cvWaitKey(0);
		cvDestroyWindow("test2");
		//
		
		cvAvgSdv( gaborImage, &mean, &stdev); 
		gabor_moments.push_back(mean.val[0]);
		gabor_moments.push_back(stdev.val[0]);
		cvReleaseImage(&gaborImage);
	}
	delete gabor;
}


void ACBWImageAnalysis::computeColorMoments(int n){ 
	// XS TODO
}


//XS visual debug
// -- at least for now ...
//CvPoint   point;

//void mouseHandler(int event, int x, int y, int flags, void* param) {
//    /* user press left button */
//    if (event == CV_EVENT_LBUTTONDOWN)
//    {
//        point = cvPoint(x, y);
//		cout << x << " - " << y << endl;
//    }
//	cout << x << "- " << y << " -" << ((BwImage*) param) [x][y] << endl;
//	
//}

void ACBWImageAnalysis::computeFourierMellinMoments(){
	fourier_mellin_moments.clear();
	
#ifdef VISUAL_CHECK
	cvNamedWindow( "orig", 1 );
//	cvShowImage( "orig", this->getImage() );
	cvWaitKey();
	cvDestroyWindow("orig");
#endif // VISUAL_CHECK

	// XS TODO check if FFT -- here: complex -- has been computed
	this->computeFFT2D_complex();
	
	int height = 2*this->getHeight();
	int width = 2*this->getWidth();
	
	IplImage* fftimage_logpolar = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	IplImage* fftimage  = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	BwImageFloat Bwfftimage(fftimage); // to handle more easily pixel access in fftimage
	
	int x=0;
	int y=0;
	int area = width * height; 
	double xtmp = 0;
	
	int k=0;
	// re-center
	for(int i=0; i< height; i++){
		for (int j=0; j< width; j++){
			if (i<height/2 && j<width/2){ x=i+height/2; y=j+width/2; }
			if (i>=height/2 && j<width/2){ x=i-height/2; y=j+width/2; }
			if (i<height/2 && j>=width/2){ x=i+height/2; y=j-width/2; }
			if (i>=height/2 && j>=width/2){ x=i-height/2; y=j-width/2; }
			xtmp = sqrt((fft[k][0]*fft[k][0]+fft[k][1]*fft[k][1]))/area;// *255/sqrt(xmax);
			Bwfftimage[x][y] = xtmp;
			k++;
		}
	}
	//cvSmooth(Bwfftimage.getImage(), Bwfftimage.getImage(), CV_GAUSSIAN, 3, 3);
	
	// XS remove DC component
	//Bwfftimage[0][0] = 0.0;

	cvLogPolar( fftimage, fftimage_logpolar, cvPoint2D32f(width/2, height/2), 40, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS );
	//cvAbs(fftimage_logpolar, fftimage_logpolar);
//	cvNormalize(fftimage_logpolar, fftimage_logpolar, 1024);
//	cvSmooth(fftimage_logpolar, fftimage_logpolar, CV_GAUSSIAN, 3, 3);	
	
	BwImage FFTImLP(fftimage_logpolar);	

#ifdef VISUAL_CHECK
	cvNamedWindow( "log-polar1", 1 );
	cvShowImage( "log-polar1", fftimage_logpolar );
//	cvSetMouseCallback("log-polar1", mouseHandler, (void*) FFTImLP);
	cvWaitKey();
	cvDestroyWindow("log-polar1");
#endif // VISUAL_CHECK

	fftw_complex *fft_lp;
	fftw_complex *data_in;
	try {
		fft_lp = new fftw_complex [area];
		data_in = new fftw_complex [area];
	}
	catch (std::bad_alloc) {
		cerr << "Memory allocation problem in fft2d for fft" << endl;
	}
	
	// make the plan before filling in the data
	// because fftw measures the speed using different pieces of code and has to write to your reserved memory
	
	fftw_plan p = fftw_plan_dft_2d(height, width, data_in, fft_lp,  FFTW_BACKWARD, FFTW_ESTIMATE); 

	k = 0;
	for(int i = 0; i < height ; i++ ) {
		for( int j = 0 ; j < width ; j++ ) {
			data_in[k][0] = ( double ) FFTImLP[i][j];
			data_in[k][1] = 0.0;
			k++;
		}
	}
	fftw_execute(p);
	fftw_destroy_plan(p);
	
	IplImage* fftimageLP  = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	BwImageFloat BwfftimageLP(fftimageLP); // to handle more easily pixel access in fftimage
	k=0;
	double xnorm=0.0;
	for(int i=0; i< height; i++){
		for (int j=0; j< width; j++){
			if (i<height/2 && j<width/2){ x=i+height/2; y=j+width/2; }
			if (i>=height/2 && j<width/2){ x=i-height/2; y=j+width/2; }
			if (i<height/2 && j>=width/2){ x=i+height/2; y=j-width/2; }
			if (i>=height/2 && j>=width/2){ x=i-height/2; y=j-width/2; }
			xtmp = sqrt((fft_lp[k][0]*fft_lp[k][0]+fft_lp[k][1]*fft_lp[k][1]))/area;// *255/sqrt(xmax);
			//cout <<  x << " " << y << " " << xtmp << endl;
			BwfftimageLP[x][y] = xtmp; 
			xnorm += xtmp;
			k++;
		}
	}
			
	for (int i=0; i< 10; i++){
		for (int j=0; j< 10; j++){
//			cout << std::setw(10) << float(BwfftimageLP[i][j]) << " ";
			fourier_mellin_moments.push_back(BwfftimageLP[i][j]);
		}
//	cout << endl;
	}
#ifdef VISUAL_CHECK
	cvNamedWindow( "log-polar", 1 );
	cvShowImage( "log-polar", fftimageLP );
	cvWaitKey();
	cvDestroyWindow("log-polar");
#endif // VISUAL_CHECK

	IplImage* fftimage_logpolar2  = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage* fftimageLP2  = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage* fftimage2  = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);

	cvConvertImage(fftimage_logpolar, fftimage_logpolar2);
	cvConvertImage(fftimageLP, fftimageLP2);
	cvConvertImage(fftimage, fftimage2);

//	cvSaveImage("/Users/xavier/Desktop/fftimage_logpolar-M90.png", fftimage_logpolar2);
//	cvSaveImage("/Users/xavier/Desktop/fftimageLP-M90.png", fftimageLP2);
//	cvSaveImage("/Users/xavier/Desktop/fftimage-M90.png", fftimage2);
	
	cvReleaseImage(&fftimage_logpolar2);
	cvReleaseImage(&fftimage2);
	cvReleaseImage(&fftimageLP2);
	
	cvReleaseImage(&fftimage_logpolar);
	cvReleaseImage(&fftimage);
	cvReleaseImage(&fftimageLP);
	delete fft_lp;
	delete data_in;

}


void ACBWImageAnalysis::computeFourierPolarMoments(int RadialBins, int AngularBins){
	if (RadialBins <= 0 || AngularBins <= 0) {
		cerr << "<ACBWImageAnalysis::computeFourierPolarMoments> : number of bins must be positive" << endl;
		exit(1);
	}
	fourier_polar_moments.clear();
	
	// XS TODO check if FFT has been computed
	//this->computeFFT2D(); 
	this->computeFFT2D_complex(); 

	double height = this->getHeight();
	double width = this->getWidth();

	double h2 = height/2;
	double w2 = width/2;
	double Rmax;
	h2 > w2 ? Rmax = w2 : Rmax = h2;
	double R = Rmax / RadialBins ;
	double T = M_PI/AngularBins;		
	
	double **RTbins = new double * [ RadialBins ];       
	int **RTcount = new int * [ RadialBins ];       
	
	for ( int ir = 0; ir < RadialBins; ir++ ){
		// Allocate the column array for this row:
		RTbins[ir] = new double[AngularBins];
		RTcount[ir] = new int[AngularBins];
		for ( int ia = 0; ia < AngularBins; ia++ ){
			RTbins[ir][ia] = 0.0;
			RTcount[ir][ia] = 0;
		}
	}
	
//	const double rmin = Rmax/10;
//	const double rmax = Rmax;
	
	// XS debug
	// cout << "resolution limits : " << rmin << " - " << rmax << endl;

	double r, t;
	int br, bt;
	double F00 = sqrt(fft[0][0] * fft[0][0] + fft[0][1] * fft[0][1]);
	if (F00 == 0){
		cerr << "<ACBWImageAnalysis::computeFourierPolar> : blank image" << endl;
		return;
	}
//	for (int i = 0; i <  h2 ; i++){
//		for (int j = 0; j <  w2 + 1; j++)  {
//			r = sqrt(i*i + j*j); 
//			j == 0 ? t=0 : t = atan2(i,j);
//			// here t is always > 0
//			if (t<0) cout <<  "weird : t <O" << endl;
//			if ( (r > rmin) && (r < rmax)) {
//				br = r/R;
//				bt = t/T;
//				if (br < RadialBins && bt < AngularBins){
//					int k2 = i + j * width;
//					RTbins [br][bt] +=  sqrt(fft[k2][0] * fft[k2][0] + fft[k2][1] * fft[k2][1])/F00;
//					RTcount [br][bt] ++;
//				}
//			}
//		}
//	}
//	
//	// other half of FFT
//	for (int i = h2; i <  this->getHeight() ; i++){ 
//		int ii = i-this->getHeight();
//		for (int j = 0; j <  w2 + 1; j++)  {
//			r = sqrt(ii*ii + j*j);
//			j == 0 ? t=0 : t = atan2(ii,j);
//			if (t<0) t+= M_PI;
//			br = r/R;
//			bt = t/T;
//			if (br < RadialBins && bt < AngularBins){
//				int k2 = i + j * this->getWidth();
//				RTbins [br][bt] +=  sqrt(fft[k2][0] * fft[k2][0] + fft[k2][1] * fft[k2][1])/F00;
//				RTcount [br][bt] ++;
//			}
//		}
//		
//	}
		
	for (int i = 0; i <  height ; i++){ 
		int ii = i-height/2; // shift
 		for (int j = 0; j < width; j++)  {
			int jj = j-width/2; // shift
			r = sqrt(ii*ii + jj*jj);
			jj == 0 ? t=0 : t = atan2(ii,jj);
			if (t<0) t+= M_PI;
			br = r/R;
			bt = t/T;
			if (br < RadialBins && bt < AngularBins){
				int k2 = i + j * width;
				RTbins [br][bt] +=  sqrt(fft[k2][0] * fft[k2][0] + fft[k2][1] * fft[k2][1])/F00;
				RTcount [br][bt] ++;
			}
		}
	}
	
	
	for ( int ir = 0; ir < RadialBins; ir++ ){
		for ( int ia = 0; ia < AngularBins; ia++ ){
			if (RTcount[ir][ia] != 0) RTbins[ir][ia] /= RTcount[ir][ia];
			fourier_polar_moments.push_back(RTbins[ir][ia]);
//			cout << RTbins[ir][ia] << " " ;
		}
//		cout << endl;
	}
	
	// To clean up we must first delete the column arrays for
	// each row...
	for ( int ir = 0; ir < RadialBins; ir++ ) {
		delete [] RTbins[ir];
		delete [] RTcount[ir];
	}
	
	// ... and then delete the row pointer array itself:
	delete [] RTbins;
	delete [] RTcount;
	
	return;
}

//void ACBWImageAnalysis::computeImageHistogram(int w, int h){
//	// compute a resized version of the image and dumps it as a vector
//	
//	// XS TODO test if w, h are valid ?
//	
//	image_histogram.clear();
//
//	IplImage* histo;
//	histo = cvCreateImage(cvSize (w, h), this->getDepth(), this->getNumberOfChannels());
//	cvResize(this->getImage(), histo, CV_INTER_CUBIC);
//	BwImage Im(histo);
//	for(int i=0; i<h; i++){
//		for (int j=0; j< w; j++){
//		image_histogram.push_back(float(Im[i][j]));
//		}
//	}
//	cvReleaseImage(&histo);
//}



// ------------------ visual output functions -----------------

void ACBWImageAnalysis::showThreshold(int thresh){
	// we create again a BW image because threshold would overwrite it
	cv::Mat BWimg_mat (this->getSize(), CV_8UC1);
	cv::threshold(imgp_mat, BWimg_mat, thresh, 255, CV_THRESH_BINARY_INV ); 
	cv::namedWindow( "Orig", CV_WINDOW_AUTOSIZE ); 
	cv::imshow( "Orig", imgp_mat ); 
	cv::namedWindow( "Threshold", CV_WINDOW_AUTOSIZE ); 
	cv::imshow( "Threshold", imgp_mat ); 
	cvWaitKey(0); 
}

//void ACBWImageAnalysis::showContours(int thresh){
//	// we create again a BW image because threshold would overwrite it
//	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
//	cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
//	CvMemStorage*  storage  = cvCreateMemStorage(0);
//	CvSeq* contours = 0;
//	cvFindContours( BWimg, storage, &contours ); 
//	IplImage* cont_img = cvCreateImage( cvGetSize(BWimg), 8, 1 );
//	cvZero( cont_img); 	
//	if( contours ) cvDrawContours( cont_img, contours, cvScalarAll(255), cvScalarAll(255), thresh ); 
//	cvNamedWindow( "Orig", CV_WINDOW_AUTOSIZE ); 
//	cvShowImage( "Orig", imgp ); 
//	cvNamedWindow( "Contour", CV_WINDOW_AUTOSIZE ); 
//	cvShowImage( "Contour", cont_img ); 
//	cvWaitKey(0); 
//	cvDestroyWindow("Orig");
//	cvDestroyWindow("Contour");
//	cvReleaseImage(&BWimg);
//	cvReleaseImage(&cont_img);
//}

void ACBWImageAnalysis::showFFTInWindow(const std::string title){
	int height = this->getHeight();
	int width = this->getWidth();

	IplImage* fftimage  = cvCreateImage(cvSize(width/2 +1,height),8,1);
	BwImage Bwfftimage(fftimage);
	int k = 0;
	for(int i=0; i< height; i++){
		for (int j=0; j< width/2 + 1; j++){
			Bwfftimage[i][j] = (uchar)(sqrt(fft[k][0]*fft[k][0]+fft[k][1]*fft[k][1]));
			k++;
		}
	}
	cvNamedWindow(title.c_str() ,CV_WINDOW_AUTOSIZE);
	cvShowImage(title.c_str(),fftimage);
	cvWaitKey(0);
	cvReleaseImage(&fftimage);
	cvDestroyWindow(title.c_str());
}

void ACBWImageAnalysis::showFFTComplexInWindow(const std::string title){
	int height = this->getHeight();
	int width = this->getWidth();

	IplImage* fftimage  = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
	BwImageFloat Bwfftimage(fftimage);
	int x=0;
	int y=0;
	float xtmp = 0;
//	float xmax = 0;
	int area = height * width; 
//	for(int kk=1; kk < height * width; kk++){
//		xtmp = (fft[kk][0]*fft[kk][0]+fft[kk][1]*fft[kk][1]);
//		//cout << xtmp << endl;
//		if (xtmp > xmax) xmax = xtmp;
//	}
	int k=0;
	for(int i=0; i< height; i++){
		for (int j=0; j< width; j++){
			if (i<height/2 && j<width/2){ x=i+height/2; y=j+width/2; }
			if (i>=height/2 && j<width/2){ x=i-height/2; y=j+width/2; }
			if (i<height/2 && j>=width/2){ x=i+height/2; y=j-width/2; }
			if (i>=height/2 && j>=width/2){ x=i-height/2; y=j-width/2; }
			xtmp = sqrt((fft[k][0]*fft[k][0]+fft[k][1]*fft[k][1])/area); //*255/sqrt(xmax);
			Bwfftimage[x][y] = xtmp;
			k++;
		}
	}
//	cvNamedWindow(title.c_str() ,CV_WINDOW_AUTOSIZE);
	cvShowImage(title.c_str(),fftimage);
	cvWaitKey(0);
	cvReleaseImage(&fftimage);
//	cvDestroyWindow(title.c_str());
}

//void ACBWImageAnalysis::showLogPolarInWindow(const std::string title){
//	IplImage* dst = cvCreateImage( cvSize(this->getWidth(),this->getHeight()), IPL_DEPTH_32F, 1 );
//	IplImage* src2 = cvCreateImage( cvGetSize(this->getImage()), IPL_DEPTH_32F, 1 );
//	
//	this->computeFFT2D_complex();
//	int height = this->getHeight();
//	int width = this->getWidth();
//	IplImage* fftimage  = cvCreateImage(cvSize(this->getWidth(),this->getHeight()),IPL_DEPTH_32F,1);
//	BwImageFloat Bwfftimage(fftimage);
//	int x=0;
//	int y=0;
//	float xtmp = 0;
////	float xmax = 0;
//	int area = this->getWidth() * this->getHeight(); 
////	for(int kk=1; kk < area; kk++){
////		xtmp = (fft[kk][0]*fft[kk][0]+fft[kk][1]*fft[kk][1]);
//		//cout << xtmp << endl;
//		//if (xtmp > xmax) xmax = xtmp;
////	}
//	int k=0;
//	for(int i=0; i< height; i++){
//		for (int j=0; j< width; j++){
//			if (i<height/2 && j<width/2){ x=i+height/2; y=j+width/2; }
//			if (i>=height/2 && j<width/2){ x=i-height/2; y=j+width/2; }
//			if (i<height/2 && j>=width/2){ x=i+height/2; y=j-width/2; }
//			if (i>=height/2 && j>=width/2){ x=i-height/2; y=j-width/2; }
//			xtmp = sqrt((fft[k][0]*fft[k][0]+fft[k][1]*fft[k][1]))/area;// *255/sqrt(xmax);
//			Bwfftimage[x][y] = xtmp;
//			k++;
//		}
//	}
//	
//	cvLogPolar( fftimage, dst, cvPoint2D32f(this->getWidth()/2, this->getHeight()/2), 20	, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS );
//
//	cvAbs(dst,dst);
//	cvLogPolar( dst, src2, cvPoint2D32f(this->getWidth()/2, this->getHeight()/2), 20, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS+CV_WARP_INVERSE_MAP );
//	cvNamedWindow( "log-polar", 1 );
//	cvShowImage( "log-polar", dst );
//	cvNamedWindow( "inverse log-polar", 1 );
//	cvShowImage( "inverse log-polar", src2 );
//	cvWaitKey();
//	cvDestroyWindow("log-polar");
//	cvDestroyWindow("inverse log-polar");
//	cvReleaseImage(&fftimage);
//}


bool ACBWImageAnalysis::savePGM (string file_name) {
	ofstream filePGM (file_name.c_str());
	int height = this->getHeight();
	int width = this->getWidth();
	
	filePGM << "P2" << endl;
	filePGM << width << " " << height << endl;
	filePGM << "255 " << endl; // max value
	
//  XS TODO check this 2.*
	//	BwImage Im(this->getImage());
	
	// Normalize image between 0 and 255
	float fmax=0.0;
	float fmin=0.0;
	for(int i=0; i< height; i++){
		for (int j=0; j< width; j++){
			float fij = this->getImageMat().at<float>(i,j) ;
			if ( fij > fmax) fmax = fij;
			else if (fij < fmin) fmin = fij;
		}
	}
	
	if (fmin==fmax) {
		// blank image
		for(int i=0; i< height; i++)
			for (int j=0; j< width; j++)
				filePGM << "0" << endl;
	}
	
	else {
		float fdif = fmax-fmin;
		// Write image data
		for(int i=0; i< height; i++)
			for (int j=0; j< width; j++)
				filePGM << (int) ((255*( this->getImageMat().at<float>(i,j)-fmin))/fdif) << endl;
	}
	filePGM.close();
	
	return true;
}

