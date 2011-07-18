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
	IplImage *imgp_full;
	imgp_full = cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	scaleImage(imgp_full);
	cvReleaseImage(&imgp_full);
}

// in this case the filename will remain unknown
// so it is not good to generate ACL files (where one entry is the filename)
// but it can be used for calculations
ACBWImageAnalysis::ACBWImageAnalysis(IplImage* img){
	// check that it's a BW image
	if (img->nChannels == 1) {
		reset();
		scaleImage(img, 1.0);
	}
	else if (img->nChannels == 3){
		if(img->colorModel == "BGR") {
			IplImage* bw_img = cvCreateImage (cvSize (img->width, img->height), IPL_DEPTH_8U, 1); 
			cvCvtColor (img, bw_img, CV_BGR2GRAY);
			scaleImage(bw_img, 1.0);
			cvReleaseImage(&bw_img);
		}
//		else if (img->colorModel == "HSV") {
//		}
	}
	else {
	}
}

void ACBWImageAnalysis::reset(){
	imgp = 0;
	fft = 0;
	HAS_FFT = false;
	original_width = original_height = 0;
	file_name = color_model = "";
}

void ACBWImageAnalysis::clean(){
	if (imgp != 0) cvReleaseImage(&imgp);
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
	ddata = (uchar*) getImage()->imageData;
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
	BwImage Im(getImage());
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
			data_in[k][0] = ( double )Im[i-height/2][j-width/2];
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
	ddata = (uchar*) getImage()->imageData;
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
	if (imgp == 0){
		cerr << " <ACBWImageAnalysis::computeHuMoments() error> missing image !" << endl;
		return;
	}
	CvMoments myRawmoments;
	CvHuMoments myHumoments ;
	
	if (thresh !=0) {
		// avoid overwriting image when computing threshold
		IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
		cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
		cvMoments (BWimg, &myRawmoments);
		cvReleaseImage(&BWimg);
	}
	else {
		cvMoments (imgp, &myRawmoments);
	}
	
	// XS tried opencv 2.0 version of Hu Moments to see if it's any better
	// cv::HuMoments(myRawmoments, momo); 
	
	cvGetHuMoments(&myRawmoments, &myHumoments);
	
	raw_moments.push_back (modifiedLog(myRawmoments.m00)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m10)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m01)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m20)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m11)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m02)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m30)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m21)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m12)) ;
	raw_moments.push_back (modifiedLog(myRawmoments.m03)) ;

	hu_moments.push_back (modifiedLog(myHumoments.hu1)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu2)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu3)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu4)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu5)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu6)) ;
	hu_moments.push_back (modifiedLog(myHumoments.hu7)) ;
}


void ACBWImageAnalysis::computeContourHuMoments(int thresh){ 
	contour_hu_moments.clear();
	
	if (imgp == 0){
		cerr << " <ACBWImageAnalysis::computeContourHuMoments() error> missing image !" << endl;
		return;
	}
	// we create again a BW image because threshold would overwrite it
	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
	cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
	CvMemStorage*  storage  = cvCreateMemStorage(0);
	CvSeq* contours = 0;
	cvFindContours( BWimg, storage, &contours ); 
	
	CvMoments my_contour_moments;
	CvHuMoments my_contour_Humoments ;
	cvContourMoments (BWimg, &my_contour_moments); // apparently it's just an alias to cvMoments
	cvGetHuMoments(&my_contour_moments, &my_contour_Humoments);
	
	cvReleaseImage(&BWimg);
	cvReleaseMemStorage(&storage );	
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu1)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu2)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu3)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu4)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu5)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu6)) ;
	contour_hu_moments.push_back (modifiedLog(my_contour_Humoments.hu7)) ;
}

void ACBWImageAnalysis::computeGaborMoments(int mumax, int numax){ // default 7, 5
	gabor_moments.clear();
	if (imgp == 0) {
		cerr << " <ACBWImageAnalysis::computeGaborMoments() error> missing image !" << endl;
		return;
	}
	CvGabor *gabor = new CvGabor();
	IplImage* im_src = this->getImage();
	cv::Size size(im_src->width, im_src->height);
	cv::Mat src(im_src);	
	cv::Mat mat(size.height, size.width, CV_32FC1);
	src.convertTo(mat, CV_32FC1); // invokes mat.create(src->size());
	
//	for (int i = 0; i < size.height; i++){	
//		for (int j = 0; j < size.width; j++){
//			cout << i << " - " << j << " : " << (int)src.at<char>(i,j) << " ; " << mat.at<float>(i,j) << endl;
//		}
//	}
	
//	cv::namedWindow("Src0", CV_WINDOW_AUTOSIZE);
//	cv::imshow("Src0", src);
//	cv::namedWindow("Mat0", CV_WINDOW_AUTOSIZE);
//	cv::imshow("Mat0", mat);
//	cv::waitKey(0);
//	
	for (int i = 0; i < mumax; i++){	
		for (int j = 0; j < numax; j++){
			gabor->Reset(i,j);
			double *tmpft = new double[2];
			tmpft = gabor->getMeanAndStdevs(mat); // will compute all what's necessary
			gabor_moments.push_back (tmpft[0]) ;
			gabor_moments.push_back (tmpft[1]) ;
			delete [] tmpft;
#ifdef VISUAL_CHECK
			IplImage *kernel = cvCreateImage( cvSize(gabor->GetMaskWidth(), gabor->GetMaskWidth()), IPL_DEPTH_8U, 1);
 			kernel = gabor->get_image(CV_GABOR_REAL);
			cvNamedWindow("Gabor Kernel", CV_WINDOW_AUTOSIZE);
			cvShowImage("Gabor Kernel", kernel);
			cvWaitKey(0);
			cvDestroyWindow("Gabor Kernel");			
			
			IplImage *magimg = cvCreateImage(cvSize(BWimg->width,BWimg->height), IPL_DEPTH_8U, 1);
			gabor->conv_img(BWimg, magimg, CV_GABOR_MAG);
			cvNamedWindow("Magnitude Response",CV_WINDOW_AUTOSIZE);
			cvShowImage("Magnitude Response",magimg);
			cvWaitKey(0);
			cvDestroyWindow("Magnitude Response");		
			cvReleaseImage( &kernel );	
			cvReleaseImage( &magimg );	
#endif
		}
	}
	delete gabor;
}

void ACBWImageAnalysis::computeGaborMoments_fft(int numPha_, int numFreq_, uint horizonalMargin_, uint verticalMargin_){ // default 7, 5, 0, 0
	gabor_moments.clear();
	if (imgp == 0) {
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
	cvShowImage( "orig", this->getImage() );
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

void ACBWImageAnalysis::computeImageHistogram(int w, int h){
	// compute a resized version of the image and dumps it as a vector
	
	// XS TODO test if w, h are valid ?
	
	image_histogram.clear();

	IplImage* histo;
	histo = cvCreateImage(cvSize (w, h), this->getDepth(), this->getNumberOfChannels());
	cvResize(this->getImage(), histo, CV_INTER_CUBIC);
	BwImage Im(histo);
	for(int i=0; i<h; i++){
		for (int j=0; j< w; j++){
		image_histogram.push_back(float(Im[i][j]));
		}
	}
	cvReleaseImage(&histo);
}



// ------------------ visual output functions -----------------

void ACBWImageAnalysis::showThreshold(int thresh){
	// we create again a BW image because threshold would overwrite it
	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
	cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
	cvNamedWindow( "Orig", CV_WINDOW_AUTOSIZE ); 
	cvShowImage( "Orig", imgp ); 
	cvNamedWindow( "Threshold", CV_WINDOW_AUTOSIZE ); 
	cvShowImage( "Threshold", BWimg ); 
	cvWaitKey(0); 
	cvDestroyWindow("Orig");
	cvDestroyWindow("Threshold");
}

void ACBWImageAnalysis::showContours(int thresh){
	// we create again a BW image because threshold would overwrite it
	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
	cvThreshold(imgp, BWimg, thresh, 255, CV_THRESH_BINARY_INV ); 
	CvMemStorage*  storage  = cvCreateMemStorage(0);
	CvSeq* contours = 0;
	cvFindContours( BWimg, storage, &contours ); 
	IplImage* cont_img = cvCreateImage( cvGetSize(BWimg), 8, 1 );
	cvZero( cont_img); 	
	if( contours ) cvDrawContours( cont_img, contours, cvScalarAll(255), cvScalarAll(255), thresh ); 
	cvNamedWindow( "Orig", CV_WINDOW_AUTOSIZE ); 
	cvShowImage( "Orig", imgp ); 
	cvNamedWindow( "Contour", CV_WINDOW_AUTOSIZE ); 
	cvShowImage( "Contour", cont_img ); 
	cvWaitKey(0); 
	cvDestroyWindow("Orig");
	cvDestroyWindow("Contour");
	cvReleaseImage(&BWimg);
	cvReleaseImage(&cont_img);
}

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

void ACBWImageAnalysis::showLogPolarInWindow(const std::string title){
	IplImage* dst = cvCreateImage( cvSize(this->getWidth(),this->getHeight()), IPL_DEPTH_32F, 1 );
	IplImage* src2 = cvCreateImage( cvGetSize(this->getImage()), IPL_DEPTH_32F, 1 );
	
	this->computeFFT2D_complex();
	int height = this->getHeight();
	int width = this->getWidth();
	IplImage* fftimage  = cvCreateImage(cvSize(this->getWidth(),this->getHeight()),IPL_DEPTH_32F,1);
	BwImageFloat Bwfftimage(fftimage);
	int x=0;
	int y=0;
	float xtmp = 0;
//	float xmax = 0;
	int area = this->getWidth() * this->getHeight(); 
//	for(int kk=1; kk < area; kk++){
//		xtmp = (fft[kk][0]*fft[kk][0]+fft[kk][1]*fft[kk][1]);
		//cout << xtmp << endl;
		//if (xtmp > xmax) xmax = xtmp;
//	}
	int k=0;
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
	
	cvLogPolar( fftimage, dst, cvPoint2D32f(this->getWidth()/2, this->getHeight()/2), 20	, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS );

	cvAbs(dst,dst);
	cvLogPolar( dst, src2, cvPoint2D32f(this->getWidth()/2, this->getHeight()/2), 20, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS+CV_WARP_INVERSE_MAP );
	cvNamedWindow( "log-polar", 1 );
	cvShowImage( "log-polar", dst );
	cvNamedWindow( "inverse log-polar", 1 );
	cvShowImage( "inverse log-polar", src2 );
	cvWaitKey();
	cvDestroyWindow("log-polar");
	cvDestroyWindow("inverse log-polar");
	cvReleaseImage(&fftimage);
}


bool ACBWImageAnalysis::savePGM (string file_name) {
	ofstream filePGM (file_name.c_str());
	int height = this->getHeight();
	int width = this->getWidth();
	
	filePGM << "P2" << endl;
	filePGM << width << " " << height << endl;
	filePGM << "255 " << endl; // max value
	
	BwImage Im(this->getImage());
	
	// Normalize image between 0 and 255
	float fmax=0.0;
	float fmin=0.0;
	for(int i=0; i< height; i++){
		for (int j=0; j< width; j++){
			if (Im[i][j] > fmax) fmax = Im[i][j];
			else if (Im[i][j] < fmin) fmin = Im[i][j];
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
				filePGM << (int) ((255*(Im[i][j]-fmin))/fdif) << endl;
	}
	filePGM.close();
	
	return true;
}

