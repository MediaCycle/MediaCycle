/*
 *  ACVideoAnalysis.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 22/09/09
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

// extracts features from video, that can be analyzed by VideoPlugin later
// XS : AnalyzedVideo and Video{Color, Shape, ...}FeaturesFactory grouped together

// note about cvCreateFileCapture(file_name.c_str())
// should work for .mov, .avi, ...
// BUT this way depends on the codecs installed on your machine!
// alternative: force cvCaptureFromAVI(filename.c_str()) = only for .avi


#include "ACVideoAnalysis.h"
#include <string>

// #include "ACImageAnalysis.h" // used in blob detection, when splitting channels

#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

// XS to get (+/- live) visual display of the time series
#include "gnuplot_i.hpp"

// ----------- uncomment this to get visual display using highgui and verbose -----
//#define VISUAL_CHECK
#define VERBOSE
// ----------- class constants
const int ACVideoAnalysis::ystar = 220;
// -----------


ACVideoAnalysis::ACVideoAnalysis(){
	capture = NULL;
	clean();
}

ACVideoAnalysis::ACVideoAnalysis(const std::string &filename){
	capture = NULL;
	clean();
	setFileName(filename);
}

void ACVideoAnalysis::clean(){
	if (capture) cvReleaseCapture(&capture);
	HAS_TRAJECTORY = false;
	frame_counter = 0;
	all_blobs.clear();
	all_blobs_time_stamps.clear();
	blob_centers.clear();
	blob_speeds.clear(); 
	contraction_indices.clear();
	pixel_speeds.clear();
	width = height = depth = fps = nframes = 0;
	threshU = threshL = 0;

	//	averageHistogram = 0;
}

void ACVideoAnalysis::rewind(){
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0); 	
// old way:
//	if (capture) cvReleaseCapture(&capture);
//	capture = cvCreateFileCapture(file_name.c_str()); 
}

ACVideoAnalysis::~ACVideoAnalysis(){
	// note: the image captured by the device is allocated /released by the capture function. 
	// There is no need to release it explicitly. only release capture
	
	if (capture) cvReleaseCapture(&capture);
	//	if (averageHistogram) delete averageHistogram;
}

void ACVideoAnalysis::setFileName(const std::string &filename){
	file_name=filename;
}

int ACVideoAnalysis::initialize(){
	// returns 1 if it worked, 0 if not
	capture = cvCreateFileCapture(file_name.c_str());		
	frame_counter = 0; // reset frame counter, to make sure
	if( !capture ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		std::cerr << "<ACVideoAnalysis::initialize> Could not initialize capturing from file " << file_name << "..." << endl;
		return 0;
	}
	
	// Get capture device properties; 
	width   = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	height  = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	fps     = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	nframes = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
//	videocodec = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FOURCC);
	// extra, not stored in capture
	depth = IPL_DEPTH_8U; // XS TODO: adapt if necessary ? check with first frame?
	
	int init_ok = 1;
	if (width*height == 0){ 
		std::cerr << "<ACVideoAnalysis::initialize> : zero image size for " << file_name << endl;
		init_ok = 0;
	}
	if (nframes == 0){ 
		std::cerr << "<ACVideoAnalysis::initialize> : zero frames for " << file_name << endl;
		init_ok = 0;
	}
	// test fps ? does not really matter if badly encoded...  

#ifdef VERBOSE
	cout << "width : " << width << endl;
	cout << "height : " << height << endl;
	cout << "fps : " << fps << endl;
	cout << "numFrames : " << nframes << endl;
	//	cout << "codec : " << videocodec << endl;
#endif // VERBOSE
	
	return init_ok; // to be consistent with MycolorImage::SetImageFile : returns 0 if works
}
	
IplImage* ACVideoAnalysis::getNextFrame(){
	IplImage* tmp = 0; 
	if(!cvGrabFrame(capture)){              // capture a frame
		std::cerr << "<ACVideoAnalysis::getNextFrame> Could not find frame..." << endl;
		return NULL;
	}
	tmp = cvRetrieveFrame(capture);           // retrieve the captured frame
	frame_counter++;
	return tmp;
}

void ACVideoAnalysis::histogramEqualize(const IplImage* bg_img) {
	if (bg_img == NULL){
		bg_img = this->computeMedianImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::histogramEqualize>: error computing median bg image" << endl;
		}
		// reset the capture to the beginning of the video
		this->rewind();
	}
	
#ifdef VISUAL_CHECK
	cvNamedWindow( "Source", 1) ;
	cvNamedWindow( "Back Projection", 1) ;
#endif // VISUAL_CHECK
	
	// Build and fill the histogram
	int h_bins = 30, s_bins = 32;
	CvHistogram* hist;
	int hist_size[] = { h_bins, s_bins };
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 255 };
	float* ranges[] = { h_ranges, s_ranges };
	for(int i = 1; i < nframes-1; i++){
		IplImage* frame = getNextFrame();
		IplImage* back_img = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_8U, 1 );
		IplImage* hsv = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
		IplImage* h_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* s_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* v_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* planes[] = { h_plane, s_plane };
		
		cvAbsDiff(frame, bg_img, frame);

		cvCvtColor( frame, hsv, CV_BGR2HSV );
		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
		
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
		cvCalcHist( planes, hist, 0, 0 );
		cvNormalizeHist( hist, 20*255 );
		
		cvCalcBackProject( planes, back_img, hist );
		cvNormalizeHist( hist, 1.0 );
#ifdef VISUAL_CHECK
		cvShowImage( "Source", frame );
#endif // VISUAL_CHECK
	
		// Create an image to visualize the histogram
		//	int scale = 10;
		//	IplImage* hist_img = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
		//	cvZero ( hist_img );
		//	
		//	// populate the visualization
		//	float max_value = 0;
		//	cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
		//	
		//	for( int h = 0; h < h_bins; h++ ){
		//		for( int s = 0; s < s_bins; s++ ){
		//			float bin_val = cvQueryHistValue_2D( hist, h, s );
		//			int intensity = cvRound( bin_val * 255 / max_value );
		//			cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
		//						cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
		//						CV_RGB( intensity, intensity, intensity ),
		//						CV_FILLED );
		//		}
		//	}
		
		
		// Show back projection
		cvShowImage( "Back Projection", back_img );
		
		// Show histogram equalized
		//	cvNamedWindow( "H-S Histogram", 1) ;
		//	cvShowImage( "H-S Histogram", hist_img );
		
		cvWaitKey(20);
		
		cvReleaseImage( &back_img );
		cvReleaseImage( &hsv );
		cvReleaseImage( &h_plane );
		cvReleaseImage( &s_plane );
		cvReleaseImage( &v_plane );
		//	cvReleaseImage( &hist_img );
	}
	return;
}

//void ACVideoAnalysis::histogramEqualize(const IplImage* const bg_img) {
//	// bg_img should not be modified
//	// Set up images
//	// Show original
//	cvNamedWindow( "Source U", 1) ;
//	cvNamedWindow( "Source L", 1) ;
//	cvNamedWindow( "Back Projection", 1) ;
//	int ystar=220;
//
//	// Build and fill the histogram
//	int h_bins = 30, s_bins = 32;
//	CvHistogram* hist;
//	int hist_size[] = { h_bins, s_bins };
//	float h_ranges[] = { 0, 180 };
//	float s_ranges[] = { 0, 255 };
//	float* ranges[] = { h_ranges, s_ranges };
//	
//	for(int i = 1; i < nframes-1; i++){
//		IplImage* frame = getNextFrame();
//		IplImage* back_img = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_8U, 1 );
//		IplImage* hsv = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
//		IplImage* h_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
//		IplImage* s_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
//		IplImage* v_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
//		IplImage* planes[] = { h_plane, s_plane };
//		// XS try show half
////		cvSetImageROI(frame, cvRect(0,0,width,ystar));		
////		cvSetImageROI(bg_img, cvRect(0,0,width,ystar));		
////		cvSetImageROI(back_img, cvRect(0,0,width,ystar));		
////		cvSetImageROI(hsv, cvRect(0,0,width,ystar));		
////		cvSetImageROI(h_plane, cvRect(0,0,width,ystar));		
////		cvSetImageROI(s_plane, cvRect(0,0,width,ystar));		
////		cvSetImageROI(v_plane, cvRect(0,0,width,ystar));		
//
//		cvAbsDiff(frame, bg_img, frame);
//		cvCvtColor( frame, hsv, CV_BGR2HSV );
//		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
//		
//		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
//		cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
//		cvNormalizeHist( hist, 20*255 ); // Normalize it
//		
//		cvCalcBackProject( planes, back_img, hist );// Calculate back projection
//		cvNormalizeHist( hist, 1.0 ); // Normalize it
//		// XS try show half
//		cvShowImage( "Source U", frame );
//		
//		
//		
////		cvSetImageROI(frame, cvRect(0,ystar,width,height));
////		cvSetImageROI(bg_img, cvRect(0,ystar,width,height));
////		cvSetImageROI(back_img, cvRect(0,ystar,width,height));		
////		cvSetImageROI(hsv, cvRect(0,ystar,width,height));		
////		cvSetImageROI(h_plane, cvRect(0,ystar,width,height));		
////		cvSetImageROI(s_plane, cvRect(0,ystar,width,height));		
////		cvSetImageROI(v_plane, cvRect(0,ystar,width,height));		
////		
////		
////		cvAbsDiff(frame, bg_img, frame);
////		cvCvtColor( frame, hsv, CV_BGR2HSV );
////		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
////		
////		int hist_size[] = { h_bins, s_bins };
////		float h_ranges[] = { 0, 180 };
////		float s_ranges[] = { 0, 255 };
////		float* ranges[] = { h_ranges, s_ranges };
////		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
////		cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
////		cvNormalizeHist( hist, 20*255 ); // Normalize it
////		
////		cvCalcBackProject( planes, back_img, hist );// Calculate back projection
////		cvNormalizeHist( hist, 1.0 ); // Normalize it
////		// XS try show half
////		cvShowImage( "Source L", frame );
////		
////		cvSetImageROI(frame, cvRect(0,0,width,height));		
////		cvSetImageROI(bg_img, cvRect(0,0,width,height));		
////		cvSetImageROI(back_img, cvRect(0,0,width,height));		
////		cvSetImageROI(hsv, cvRect(0,0,width,height));		
////		cvSetImageROI(h_plane, cvRect(0,0,width,height));		
////		cvSetImageROI(s_plane, cvRect(0,0,width,height));		
////		cvSetImageROI(v_plane, cvRect(0,0,width,height));		
//		
//		// Create an image to visualize the histogram
//		//	int scale = 10;
//		//	IplImage* hist_img = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
//		//	cvZero ( hist_img );
//		//	
//		//	// populate the visualization
//		//	float max_value = 0;
//		//	cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
//		//	
//		//	for( int h = 0; h < h_bins; h++ ){
//		//		for( int s = 0; s < s_bins; s++ ){
//		//			float bin_val = cvQueryHistValue_2D( hist, h, s );
//		//			int intensity = cvRound( bin_val * 255 / max_value );
//		//			cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
//		//						cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
//		//						CV_RGB( intensity, intensity, intensity ),
//		//						CV_FILLED );
//		//		}
//		//	}
//		
//	
//		// Show back projection
//		cvShowImage( "Back Projection", back_img );
//		
//		// Show histogram equalized
//		//	cvNamedWindow( "H-S Histogram", 1) ;
//		//	cvShowImage( "H-S Histogram", hist_img );
//		
//		cvWaitKey(20);
//		
//		cvReleaseImage( &back_img );
//		//	cvReleaseImage( &hist_img );
//	}
//	return;
//}

void ACVideoAnalysis::computeBlobs(IplImage* bg_img, int bg_thresh, int big_blob, int small_blob){
	// do background substraction first, using either bg_img (if provided) or
	//  it will compute background image:
	// supposes 3-channels image (in whatever mode) but would work on BW images since OpenCV converts it in 3 channels
	all_blobs.clear();
	all_blobs_time_stamps.clear();	// just to make sure...

	if (bg_img == NULL){
		bg_img = this->computeMedianImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::computeBlobs>: error computing average image" << endl;
		}
		// reset the capture to the beginning of the video
		this->clean();
		this->initialize();
	}
	
	// initial frame
	IplImage* frame;
	
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
	
	int xi,xf,yi,yf;
	
	for(int i = 0; i < nframes-1; i++){
		frame = getNextFrame();
		cvAbsDiff(frame, bg_img, frame);
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvThreshold(bwImage, bitImage, bg_thresh,255,CV_THRESH_BINARY_INV);
		CBlobResult blobs;
		blobs = CBlobResult( bitImage, NULL, 255 ); // find blobs in image
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob );
		all_blobs.push_back(blobs);
		all_blobs_time_stamps.push_back(cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES));
	}
	cvReleaseImage(&bitImage);
	cvReleaseImage(&bwImage);
}

void ACVideoAnalysis::computeBlobsInteractively(IplImage* bg_img, bool merge_blobs, int bg_thesh, int big_blob, int small_blob){
	all_blobs.clear();
	all_blobs_time_stamps.clear();	// just to make sure...

#ifdef VISUAL_CHECKe
	Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();
	std::vector<double> ci;
#endif //VISUAL_CHECK
	
	if (bg_img == NULL){
		bg_img = this->computeMedianImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::computeBlobsInteractively>: error computing average image" << endl;
		}
		// reset the capture to the beginning of the video
		this->rewind();
	}
	
	int slider_big_blob = big_blob;
	int slider_bg_thresh = bg_thesh;
		
	// to display image with blobs superimposed
	IplImage *saveImage = cvCreateImage(cvSize(width,height),depth,1);
	// font for frame and blob counter; 
	CvFont font;
	char str[64];
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	cvNamedWindow("ORIG-BG", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("ORIG-BG", 50, 50);
	cvNamedWindow( "BW", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BW", 700, 400);
	cvNamedWindow( "BLOBS", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BLOBS", 50, 400);	
	cvCreateTrackbar("Biggest Blob","BLOBS", &slider_big_blob, 1000 ,NULL);
	cvCreateTrackbar("Threshold","BLOBS",&slider_bg_thresh,255,NULL);
	
	IplImage* frame;
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
	
	int xi,xf,yi,yf;
	for(int i = 0; i < nframes-1; i++){
		frame = getNextFrame();
		saveImage =cvCloneImage(frame);
		cvAbsDiff(frame, bg_img, frame);
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvThreshold(bwImage, bitImage, slider_bg_thresh,255,CV_THRESH_BINARY_INV);
		
		// XS closing filter (to remove dips) 
		cvDilate(bitImage, bitImage);
		cvErode(bitImage, bitImage);

		CBlobResult blobs;
		blobs = CBlobResult( bitImage, NULL, 255 );
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS,  slider_big_blob );
		all_blobs.push_back(blobs);
		all_blobs_time_stamps.push_back(cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES));
		
		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (saveImage, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));

		if (blobs.GetNumBlobs() > 0){
			if (merge_blobs) {
				// XS : blobs not really merge but get stats 
				// on the whole list of blobs (in CBlobResult)
				CvRect rbox = blobs.GetBoundingBox();				

#ifdef VISUAL_CHECKe
				ci.push_back( blobs.Area() / (rbox.width*rbox.height) );
				g1.reset_plot();
				g1.plot_x(ci,"ci");
#endif //VISUAL_CHECK

				// for visual purposes:
				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
					blobs.GetBlob(j)->FillBlob(saveImage, CV_RGB(255,0,0));
				}
				
				CvPoint ii = cvPoint(rbox.x,rbox.y);
				CvPoint ff = cvPoint(rbox.x+rbox.width,rbox.y+rbox.height);
				cvRectangle( saveImage, ii, ff, CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
//				cvEllipseBox( saveImage, mergedBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 
			}
			else {
				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
					CBlob *currentBlob = new CBlob();
					currentBlob = blobs.GetBlob(j);
					currentBlob->FillBlob(saveImage, CV_RGB(255,0,0));
					xf=currentBlob->MaxX();
					xi=currentBlob->MinX();
					yf=currentBlob->MaxY();
					yi=currentBlob->MinY();			
					cvRectangle( saveImage, cvPoint(xi, yi), cvPoint (xf, yf), CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
//					cvEllipseBox(saveImage, currentBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 					
					delete currentBlob;
				}
			}
		}
		else {
			cout << "no blobs : " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) <<endl;
		}
		cvShowImage("ORIG-BG",frame);
		cvShowImage("BLOBS", saveImage );					
		cvShowImage("BW",bitImage);
		int	c = cvWaitKey(20);
		if( (char) c == 27 )
			cvWaitKey(0); //break;			
	}
	cvReleaseImage(&bitImage);
	cvReleaseImage(&bwImage);
	cvReleaseImage(&saveImage);
	cvDestroyWindow("ORIG-BG");
	cvDestroyWindow("BW");
	cvDestroyWindow("BLOBS");
}

void ACVideoAnalysis::computeBlobsUL(IplImage* bg_img, bool merge_blobs, int big_blob, int small_blob){
// different threshold upper/lower part of the image
	all_blobs.clear();
	all_blobs_time_stamps.clear();	// just to make sure...
	if (bg_img == NULL || threshU ==0 || threshL ==0){
		cout << threshU << " " << threshL << " " << endl ;
		if (bg_img == NULL) cout << "NULL" << endl;
		bg_img = this->computeMedianImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::computeBlobsInteractively>: error computing average image" << endl;
		}
		// reset the capture to the beginning of the video
		this->rewind();
	}
	// initial frame
	IplImage* frame;
	
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
	
	// XS Essai running avg
//	IplImage* r_avg_img = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
#ifdef VISUAL_CHECK
	CvFont font;
	char str[64];
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	cvNamedWindow("ORIG-BG", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("ORIG-BG", 50, 50);
	cvNamedWindow( "BW", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BW", 700, 400);
	cvNamedWindow( "BLOBS", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BLOBS", 50, 400);	
#endif // VISUAL_CHECK
	
	int xi,xf,yi,yf;
	int t_u=threshU*2;
	int t_l=threshL*2;
	int joeystar = ystar + 40;
	
	for(int i = 0; i < nframes-1; i++){
		frame = getNextFrame();
		cvAbsDiff(frame, bg_img, frame);
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		
		//UP
		cvSetImageROI(bwImage, cvRect(0,0,width,joeystar));		
		cvSetImageROI(bitImage, cvRect(0,0,width,joeystar));		
		cvThreshold(bwImage, bitImage, t_u,255,CV_THRESH_BINARY_INV);

		// LOW
		cvSetImageROI(bwImage, cvRect(0,joeystar,width,height));		
		cvSetImageROI(bitImage, cvRect(0,joeystar,width,height));		
		cvThreshold(bwImage, bitImage, t_l,255,CV_THRESH_BINARY_INV);

		// RESET ROIs
		cvSetImageROI(bwImage, cvRect(0,0,width,height));	
		cvSetImageROI(bitImage, cvRect(0,0,width,height));	

		// tried this to smooth the blob, but it removes too much of the blob
		// cvMorphologyEx( bitImage, bitImage, NULL, NULL, CV_MOP_CLOSE, 2);
		//cvSmooth(bitImage, bitImage, CV_MEDIAN, 3, 5);
		
		// does not work:
//		cvRunningAvg (bitImage, r_avg_img, 0.5);
//		cvConvertImage(r_avg_img, bitImage);
		
		CBlobResult blobs;
		blobs = CBlobResult( bitImage, NULL, 255 ); // find blobs in image
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob );
		all_blobs.push_back(blobs);
		all_blobs_time_stamps.push_back(cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES));

#ifdef VISUAL_CHECK
		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));

		// for visual purposes:
		for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
			blobs.GetBlob(j)->FillBlob(frame, CV_RGB(255,0,0));
		}

		cvShowImage("ORIG-BG",frame);
		cvShowImage("BLOBS", bwImage );					
		cvShowImage("BW",bitImage);
#endif // VISUAL_CHECK
		
	}
	cvReleaseImage(&bitImage);
	cvReleaseImage(&bwImage);
#ifdef VISUAL_CHECK
	cvDestroyWindow("ORIG-BG");
	cvDestroyWindow("BW");
	cvDestroyWindow("BLOBS");	
#endif // VISUAL_CHECK
}

void ACVideoAnalysis::computeOpticalFlow(){
	int win_size = 10;
	const int MAX_COUNT = 500;
	CvPoint2D32f* points[2] = {0,0}, *swap_points;
	char* status = 0;
	int count = 0;
	int need_to_init = 0;
	int flags = 0;
	int add_remove_pt = 0;
	CvPoint pt;
	//	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE );
	//	cvResizeWindow( "Camera", 800, 600 );
	
	IplImage* frame = 0;
	int currentframe = 0;
	//	int firstframemoved = 0;
	
	IplImage *image = cvCreateImage( cvSize(width,height),depth, 3 );
	//	image->origin = frame->origin;
	IplImage *grey = cvCreateImage( cvSize(width,height),depth, 1 );
	IplImage *prev_grey = cvCreateImage( cvSize(width,height),depth, 1 );
	IplImage *pyramid = cvCreateImage( cvSize(width,height),depth, 1 );
	IplImage *prev_pyramid = cvCreateImage( cvSize(width,height),depth, 1 );
	IplImage *swap_temp;
	points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	status = (char*)cvAlloc(MAX_COUNT);
	
	for(int ifram=0; ifram<nframes-1; ifram++) {
		int i, k, c;
		frame = getNextFrame(); // XS this could be at the end of loop
		currentframe++;
		if (currentframe%5==0) need_to_init = 1;
		cvCopy( frame, image, 0 ); // XS cvclone
		cvCvtColor( image, grey, CV_BGR2GRAY );
		if( need_to_init ) {
			IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
			IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
			double quality = 0.01;
			double min_distance = 4;
			count = MAX_COUNT;
			cvGoodFeaturesToTrack( grey, eig, temp, points[1], &count,
								  quality, min_distance, 0, 5, 0, 0.04 );
			cvReleaseImage( &eig );
			cvReleaseImage( &temp );
		}
		else if( count > 0 ){
			cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
								   points[0], points[1], count, cvSize(win_size,win_size), 3, status, 0,
								   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
			flags |= CV_LKFLOW_PYR_A_READY;
			
			CvPoint p,q;
			
			for( i = k = 0; i < count; i++ ) {
				if( add_remove_pt ) {
					double dx = pt.x - points[1][i].x;
					double dy = pt.y - points[1][i].y;
					if( dx*dx + dy*dy <= 25 ) {
						add_remove_pt = 0;
						continue;
					}
				}
				
				if( !status[i] )
					continue;
				
				points[1][k++] = points[1][i];
				p = cvPointFrom32f(points[0][i]);
				q = cvPointFrom32f(points[1][i]);
				//		double a = atan2( (double) p.y - q.y, (double) p.x - q.x );
				double m = sqrt( (p.y - q.y)*(p.y - q.y) + (p.x - q.x)*(p.x - q.x) );
				cvLine( image, p , q , CV_RGB(255,0,0), 1, 8,0);
				//				printf("%d;%d;%d;%g;%g\n", currentframe, p.x, p.y, a, m);
				// XS break
				//				if (m>0) {
				//					firstframemoved=currentframe;
				//					break;
				//				}				
			}
			count = k;
		}
		
		//		if (firstframemoved > 0) break;
		
		need_to_init = 0;
		if( add_remove_pt && count < MAX_COUNT ) {
			points[1][count++] = cvPointTo32f(pt);
			cvFindCornerSubPix( grey, points[1] + count - 1, 1,
							   cvSize(win_size,win_size), cvSize(-1,-1),
							   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
			add_remove_pt = 0;
		}
		CV_SWAP( prev_grey, grey, swap_temp );
		CV_SWAP( prev_pyramid, pyramid, swap_temp );
		CV_SWAP( points[0], points[1], swap_points );
		//		cvShowImage( "Camera", image );
		//		c = cvWaitKey(10);
		//		if( (char)c == 27 )
		//			break;
	}	
	//	cvDestroyWindow("Camera");
	cvReleaseImage(&image);
	cvReleaseImage(&grey);
	cvReleaseImage(&prev_grey);
	cvReleaseImage(&pyramid);
	cvReleaseImage(&prev_pyramid);
	// double free:	cvReleaseImage(&swap_temp);
}


void ACVideoAnalysis::computeMergedBlobsTrajectory(float blob_dist){
  blob_centers.clear();
  CBlobResult currentBlob;
  
  for (unsigned int i=0; i< all_blobs.size(); i++){
    std::vector<float> tmp;
    currentBlob = all_blobs[i];
    CvPoint center = currentBlob.GetCenter();
    tmp.push_back(center.x);
    tmp.push_back(center.y);
    blob_centers.push_back(tmp);
  }
  HAS_TRAJECTORY = true;
  
}

void ACVideoAnalysis::computeMergedBlobsSpeeds(float blob_dist){
	if (!HAS_TRAJECTORY) this->computeMergedBlobsTrajectory(blob_dist);
	blob_speeds.clear();
	blob_center cb_prev;
	blob_center cb = blob_centers[0];

	for (unsigned int i=1; i< all_blobs.size(); i++){ // not i=0
		blob_center speed;
		cb_prev = cb;
		cb = blob_centers[i];
		for (unsigned j=0; j< cb.size(); j++) { // normally 2D
			speed.push_back(cb[j]-cb_prev[j]);	
		}
		blob_speeds.push_back(speed);
		// XS TODO : use time stamp !
		// speed will have one item less than trajectory
	}
}

IplImage* ACVideoAnalysis::computeAverageImage(int nskip, int nread, int njump, std::string fsave) { 
	// nskip = number of frames to skip at the beginning
	// nread = number of frames to read in the video
	// ncalc = number of frames to consider
	// so that we go by steps of nread/ncalc frames
	// saving in a file if fsave != ""
	if (nskip + nread > nframes) {
		std::cerr << " <ACVideoAnalysis::computeAverageImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
		std::cerr << "nframes = " << nframes << "; nskip = " << nskip <<  "; nread = " << nread << "; njump = " << njump << endl; 
		return NULL;
	}
	if (nread ==0) nread = nframes-nskip;
	if (njump == -1) njump = 50;		// default values
	nread = (nread/njump) * (njump); // integer manipulation to avoid getting over bounds (then cvcapture starts again from beginning)

#ifdef VERBOSE
	cout << "Average : skipping " << nskip << " frames" << endl;
	cout << "total frames to read " << nread << endl;
	cout << "(by jumps of) " << njump << endl;
#endif // VERBOSE

	IplImage *frame;
	IplImage *av_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_32F, 3);
	int cnt = 0; 

	// to skip nskip frames:
	int cursor = nskip;
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursor); 	
	
#ifdef VISUAL_CHECK
	string title = "check average";
	cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	char str[64];
#endif // VISUAL_CHECK

	for (int i = 0; i <  nread/njump; i++) {
		cnt++;	
		frame = getNextFrame();
		cvAcc (frame, av_img); // note : one can accumuate 8U into 32S (but not Add)
#ifdef VISUAL_CHECK
		// note : put text *after* using image
		snprintf (str, 64, "%04d", cursor);
		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		showFrameInWindow(title.c_str(),frame);
#endif // VISUAL_CHECK
		cursor+=njump;	
		if (cursor >= nframes) break; // safety
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursor);
	}
	cvConvertScale (av_img, av_img, 1.0 / cnt);
	IplImage *result_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	cvConvert (av_img, result_img);
	
#ifdef VISUAL_CHECK
	cvWaitKey(0);
	cvDestroyWindow("test");	
#endif // VISUAL_CHECK
	
	if (fsave != "")
		cvSaveImage(fsave.c_str(),result_img);// av_img);
	return av_img;
}

IplImage* ACVideoAnalysis::computeMedianImage(int nskip, int nread, int njump, std::string fsave) { 
	// nskip = number of frames to skip at the beginning
	// nread = number of frames to read in the video
	// ncalc = number of frames to consider
	// so that we go by steps of nread/ncalc frames
	// saving in a file if fsave != ""
#ifdef VISUAL_CHECK
	string title = "check median";
	cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	char str[64];
#endif // VISUAL_CHECK
	
	if (nskip + nread > nframes) {
		std::cerr << " <ACVideoAnalysis::computeMedianImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
		std::cerr << "nframes = " << nframes << "; nskip = " << nskip <<  "; nread = " << nread << "; njump = " << njump << endl; 
		return NULL;
	}
	
	if (nread ==0) nread = nframes-nskip;
	if (njump == -1) njump = 50;		// default values
	nread = (nread/njump) * (njump); // integer manipulation to avoid getting over bounds (then cvcapture starts again from beginning)

#ifdef VERBOSE
	cout << "Median : skipping " << nskip << " frames" << endl;
	cout << "total frames to read " << nread << endl;
	cout << "(by jumps of) " << njump << endl;
#endif // VERBOSE

	int nbins = 256;
	int histsize = nbins*height*width;
//	int* histograms = new int[histsize] ;
	//XS make histogram template ?
	BgrPixel* histograms = new BgrPixel[histsize] ;
	for (int i=0; i<histsize; i++) {
		histograms[i].b=0;
		histograms[i].g=0;
		histograms[i].r=0;
	}
	
	IplImage *frame;
	int cnt = 0; 

	// to skip nskip frames:
	int cursor = nskip;
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursor); 	
	
	for (int i = 0; i < nread/njump ; i++) {
		cnt++;
		frame = getNextFrame();
		BgrImage bgr_image(frame); // XS new ?
		for (int r=0; r<height; r++){
			for (int c=0; c<width; c++){
				histograms [(r*width+c)*nbins+ int(bgr_image[r][c].b)].b ++; // xs missing /256 * nbins
				histograms [(r*width+c)*nbins+ int(bgr_image[r][c].g)].g ++; 
				histograms [(r*width+c)*nbins+ int(bgr_image[r][c].r)].r ++; 
			}
		}
#ifdef VISUAL_CHECK
		// note : put text *after* using image
		snprintf (str, 64, "%04d", cursor);
		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		showFrameInWindow(title.c_str(),frame);
#endif // VISUAL_CHECK
		
		cursor+=njump;	
		if (cursor >= nframes) break; // safety
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursor);
	}
#ifdef VISUAL_CHECK
	cvDestroyWindow(title.c_str());
#endif // VISUAL_CHECK
	
	cout << "finished computing for "<< cnt << " images. transfering data" << endl;
	
	char* new_image_data;
	float mid=cnt/2; // *nbins

	// output median image
	IplImage *result_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);

	for (int r=0; r<height; r++){
		for (int c=0; c<width; c++){
			BgrPixel accum;
			accum.b=0;
			accum.g=0;
			accum.r=0;
			BgrPixel median;
			median.b=0;
			median.g=0;
			median.r=0;
			for (int i=0; i<nbins; i++){
				accum.b+=histograms [(r*width+c)*nbins+i].b;
				accum.g+=histograms [(r*width+c)*nbins+i].g;
				accum.r+=histograms [(r*width+c)*nbins+i].r;
				if (accum.b > mid && median.b ==0) median.b = i;
				if (accum.g > mid && median.g ==0) median.g = i;
				if (accum.r > mid && median.r ==0) median.r = i;
			}
			CvScalar s;
			s.val[0] = median.b; // B
			s.val[1] = median.g; // G
			s.val[2] = median.r; // R
			cvSet2D(result_frame,r,c,s);
		}
	}
	
//	// debug
//	cvWaitKey(0);
//	cvShowImage(title.c_str(), result_frame); 
//	cvWaitKey(0);

	// XS to get separate thresholds
	CvScalar sum_median;
	sum_median = cvSum (result_frame);
	for(int j = 0; j < 3; j++){
		cout << "sum over median: " << j << " = " << sum_median.val[j]/(height*width) << endl;
	}
	
	cvSetImageROI(result_frame, cvRect(0,0,width,ystar));		
	sum_median = cvSum (result_frame);
	threshU=0;
	for(int j = 0; j < 3; j++){
		int v = sum_median.val[j]/(ystar*width);
		cout << "sum over median -- up: " << j << " = " << v << endl;
		threshU+=v;
	}
	threshU/=3;
	cout << "threshU = " << threshU << endl;

	cvSetImageROI(result_frame, cvRect(0,ystar,width,height));		
	sum_median = cvSum (result_frame);
	 this->threshL=0;
	for(int j = 0; j < 3; j++){
		int v=sum_median.val[j]/((height-ystar)*width);
		cout << "sum over median -- low: " << j << " = " << v << endl;
		 this->threshL+=v;
	}	
	threshL/=3;
	cout << "threshL = " << threshL << endl;

	cvSetImageROI(result_frame, cvRect(0,0,width,height));	
	// END XS 
	
	if (fsave != ""){
		cvSaveImage(fsave.c_str(), result_frame);
	}
	
	delete histograms;

	return result_frame;
}

IplImage* ACVideoAnalysis::computeMedianNoBlobImage(std::string fsave, IplImage *first_guess){
	// only on frames that have no blobs
	if (all_blobs_time_stamps.size()==0 || all_blobs.size()==0) {
		// XS debug
		threshU = 3;
		threshL = 14;
		// XS end
		computeBlobsUL(first_guess); // could be NULL, in which case aprox median is calculated
	}
	if (all_blobs_time_stamps.size() != all_blobs.size()) {
		std::cerr << "<ACVideoAnalysis::computeMedianNoBlobImage> : time stamp problem" << endl;
		return NULL;
	}
	
	int nbins = 256;
	int histsize = nbins*height*width;
	//	int* histograms = new int[histsize] ;
	//XS make histogram template ?
	BgrPixel* histograms = new BgrPixel[histsize] ;
	for (int i=0; i<histsize; i++) {
		histograms[i].b=0;
		histograms[i].g=0;
		histograms[i].r=0;
	}
	
	IplImage *frame;
	int cnt = 0; 
	
	int cursor;
	
#ifdef VISUAL_CHECK
	cvNamedWindow("NOBLOBS", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("NOBLOBS", 50, 400);	
#endif // VISUAL_CHECK

	for (int i=0;i<all_blobs.size();i++){
		cout << i << endl;
		if (all_blobs[i].GetNumBlobs() == 0){
			cout << i << endl;
			cnt++;
			cursor = all_blobs_time_stamps[i];
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursor); 	
			frame = getNextFrame();
#ifdef VISUAL_CHECK
			cvShowImage("NOBLOBS", frame );		
			cvWaitKey(0);
#endif // VISUAL_CHECK
			BgrImage bgr_image(frame); // XS new ?
			for (int r=0; r<height; r++){
				for (int c=0; c<width; c++){
					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].b)].b ++; // xs missing /256 * nbins
					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].g)].g ++; 
					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].r)].r ++; 
				}
			}
		}
	}
#ifdef VISUAL_CHECK
	cvDestroyWindow("NOBLOBS");
#endif // VISUAL_CHECK

	char* new_image_data;
	float mid=cnt/2; // *nbins
	
	// output median image
	IplImage *result_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	
	for (int r=0; r<height; r++){
		for (int c=0; c<width; c++){
			BgrPixel accum;
			accum.b=0;
			accum.g=0;
			accum.r=0;
			BgrPixel median;
			median.b=0;
			median.g=0;
			median.r=0;
			for (int i=0; i<nbins; i++){
				accum.b+=histograms [(r*width+c)*nbins+i].b;
				accum.g+=histograms [(r*width+c)*nbins+i].g;
				accum.r+=histograms [(r*width+c)*nbins+i].r;
				if (accum.b > mid && median.b ==0) median.b = i;
				if (accum.g > mid && median.g ==0) median.g = i;
				if (accum.r > mid && median.r ==0) median.r = i;
			}
			CvScalar s;
			s.val[0] = median.b; // B
			s.val[1] = median.g; // G
			s.val[2] = median.r; // R
			cvSet2D(result_frame,r,c,s);
		}
	}
	if (fsave != ""){
		cvSaveImage(fsave.c_str(), result_frame);
	}
	
	delete histograms;
	
	return result_frame;
	
}

void ACVideoAnalysis::computePixelSpeed() {
	// substracts each image from the previous one and sums it all
	// XS: could add option to calculate just on a segment of the video
	pixel_speeds.clear();
	IplImage *frame = 0;
	IplImage *tmp_frame = 0;
	IplImage *previous_frame = 0;
	IplImage *diff_frames = 0;
	IplImage *diff_frames_U = 0;
	
	// XS TODO : check if video has been initialized
	// e.g. index current_frame
	// or dimensions...
	// int width = analysed_video->getWidth();
	//	int height = analysed_video->getHeight();
	
	if (nframes < 2){
		std::cerr << "<ACVideoSpeedFeatures::calculate> : not enough frames in video : " << \
		file_name << endl;
		return;
	}

	CvScalar sum_diff_frames ;
	float speed = 0.0;
	
	// initial frame
	//	tmp_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3); // -- not necessary
	tmp_frame = this->getNextFrame();
	
	frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	previous_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames_U = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	cvSetZero (frame);
	
#ifdef VISUAL_CHECK
	cvNamedWindow ("Input", CV_WINDOW_AUTOSIZE);
	cvNamedWindow ("Substraction", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK

	for(int i = 1; i < nframes; i++){ 
		cvConvert (tmp_frame, previous_frame);
		tmp_frame = this->getNextFrame();
		cvConvert (tmp_frame, frame);
		cvAbsDiff (frame, previous_frame, diff_frames);
		sum_diff_frames = cvSum (diff_frames);
		speed = 0.0;
		for(int j = 0; j < 3; j++){
			speed += sum_diff_frames.val[j];
		}
		speed = speed / (4*width*height);
		pixel_speeds.push_back(speed);
		cvConvert (diff_frames, diff_frames_U);
#ifdef VISUAL_CHECK
		cvShowImage ("Input", tmp_frame);
		cvShowImage ("Substraction", diff_frames_U);
		int c = cvWaitKey (10);
#endif //VISUAL_CHECK
		tmp_frame = this->getNextFrame();
	}
	
	cvReleaseImage (&frame);
	cvReleaseImage (&previous_frame);
	cvReleaseImage (&diff_frames);
	cvReleaseImage (&diff_frames_U);
	//	cvReleaseImage (&tmp_frame); 
	// XS !! tmp_frame will be release by cvReleasedCapture in cvAnalyzedVideo
	// releasing it here will result in "double free" error message
		
#ifdef VISUAL_CHECK
	cvDestroyWindow ("Input");
	cvDestroyWindow ("Substraction");
#endif //VISUAL_CHECK
}

void ACVideoAnalysis::computeContractionIndices(){
	contraction_indices.clear();
	CvRect bbox;
	float ci;
	for (unsigned int i=0; i< all_blobs.size(); i++){ 
		bbox = all_blobs[i].GetBoundingBox();
		ci = all_blobs[i].Area() / (bbox.width*bbox.height);
		contraction_indices.push_back(ci);
		//XS debug
		//cout << i << ": " << ci << endl;
	}
}

std::vector<float> ACVideoAnalysis::getDummyTimeStamps(){
	// XS: I made these FLOAT as in ACMediaFeatures but really it's INT
	// these are dummy time stamps (0,1,2, ...)
	std::vector<float> dummy;
	for (unsigned int i=0; i<blob_centers.size(); i++){
		dummy.push_back((float)i);
	}
	return dummy;
}

std::vector<float> ACVideoAnalysis::getTimeStamps(){
	// XS: I made these FLOAT as in ACMediaFeatures but really it's INT
	// these are "real" time stamps from cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES)
	return all_blobs_time_stamps;
}

// ------------------ visual output functions -----------------
void ACVideoAnalysis::showFrameInWindow(std::string title, IplImage* frame, bool has_win){
	// by default has_win = true, we don't make a new window for each frame !
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	if (frame != NULL) {
		cvShowImage(title.c_str(), frame); 
		cvWaitKey(20);           // wait 20 ms, necesary to display properly.
	}
}

void ACVideoAnalysis::showInWindow(std::string title, bool has_win){
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	
	cvResizeWindow(title.c_str(), 800, 600 );
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	char str[64];
	int c;
	
	IplImage* img = 0;
	for(int i = 0; i < nframes-1; i++){
		img = getNextFrame();
		if( !img ) {
			break;
			cout << "end of movie (aka The End)" << endl;
		}
		snprintf (str, 64, "%03d[frame]", i);
		cvPutText (img, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		cvShowImage(title.c_str(), img); 
		
		c = cvWaitKey (20);
		if (c == '\x1b') // press ESC to pause the video, then any key to continue
			cvWaitKey(0);
		// for a frame by frame display:
		// cvWaitKey(0);
	}
	cvWaitKey(0);
	//	cvReleaseImage(&img); -- will be released by cvReleaseCapture, because img=getNextFrame 
	// do not free twice the same buffer !
	cvDestroyWindow(title.c_str());
}

// ----------- for "fancy" browsing

int        g_slider_position = 0; 
CvCapture* g_capture         = NULL;

void onTrackbarSlide(int pos) { 
	cvSetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos); 
} 
// ----------- 

void ACVideoAnalysis::browseInWindow(std::string title, bool has_win){
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	
	cvResizeWindow(title.c_str(), 800, 600 );
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	char str[64];
	int c;
	g_capture = cvCreateFileCapture(file_name.c_str()); 
    int frames = (int) cvGetCaptureProperty( 
											g_capture, 
											CV_CAP_PROP_FRAME_COUNT 
											); 
	cvCreateTrackbar("Position", title.c_str(), &g_slider_position, nframes, onTrackbarSlide ); 
	
	IplImage* img = 0;
	for(int i = 0; i < nframes-1; i++){
		if(!cvGrabFrame(g_capture)){              // capture a frame
			std::cerr << "<ACVideoAnalysis::browseInWindow> Could not find frame..." << endl;
			return;
		}
		img = cvRetrieveFrame(g_capture);           // retrieve the captured frame
		
		if( !img ) {
			break;
			cout << "end of movie (aka The End)" << endl;
		}
//		snprintf (str, 64, "%03d[frame]", i);
//		cvPutText (img, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		cvShowImage(title.c_str(), img); 
		
		c = cvWaitKey (20);
		if (c == '\x1b') // press ESC to pause the video, then any key to continue
			cvWaitKey(0);
		// for a frame by frame display:
		// cvWaitKey(0);
	}
	cvWaitKey(0);
	//	cvReleaseImage(&img); -- will be released by cvReleaseCapture, because img=getNextFrame 
	// do not free twice the same buffer !
	cvDestroyWindow(title.c_str());
}

// ------------------ file output functions -----------------

void ACVideoAnalysis::saveInFile (std::string fileout, int nskip){
	// CV_FOURCC(‘M’,‘J’,‘P’,‘G’)
	CvVideoWriter* video_writer = cvCreateVideoWriter( fileout.c_str(), -1, fps, cvSize(width,height) );  // "-1" pops up a nice GUI 
	IplImage* img = 0;
	for(int i = nskip; i < nframes-1; i++){
		img = getNextFrame();
		cvWriteFrame(video_writer, img);  
		if( !img ) {
			break;
			cout << "end of movie (aka The End)" << endl;
		}
	}
	cvReleaseVideoWriter(&video_writer); 
}





// TODO: detect blob on a selected channel
// (suffit de nettoyer ci-dessous)
//
//void ACVideoAnalysis::detectBlobs(int ichannel, string cmode, IplImage* bg_img, int bg_thesh, int big_blob, int small_blob){
//	// works on ichannel_th channel (if < 0 : use BW image)
//	IplImage* frame = getNextFrame();
//	frame = getNextFrame();
//	ACAnalysedImage* im = new ACAnalysedImage();
//
//		if (ichannel > 2 || ichannel < 0) {
//			cerr << "detectBlobs : wrong channel number : " << ichannel << endl;
//			return;
//		}
//		
//		for(int i = 1; i < numFrames-1; i++){
//			im->clean();
//			frame = getNextFrame();
//			//cvSub(frame,bg_img,frame);
//			
//			cvAbsDiff(frame,bg_img,frame);
//			im->setImage(frame);
//			im->splitChannels(cmode);
//			//		im->showChannels(col,c0,c1,c2);
//			im->showInWindow("ORIG",true);
//			
//			saveImage =cvCloneImage(frame);
//			
//			cvThreshold(im->getChannel(ichannel), bitImage, slider_bg_thresh,255,CV_THRESH_BINARY_INV);
//			// get blobs and filter them using its area
//			CBlobResult blobs;
//			CBlob *currentBlob;
//			
//			// find blobs in image
//			blobs = CBlobResult( bitImage, NULL, 255 );
//			blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS,  slider_big_blob );
//			
//			// XS  test
//			//		cout << blobs.GetNumBlobs() << endl;
//			for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
//				currentBlob = blobs.GetBlob(j);
//				currentBlob->FillBlob( frame, CV_RGB(255,0,0));
//			}
//			int xi,xf,yi,yf;
//			if (blobs.GetNumBlobs() > 0){
//				currentBlob = blobs.GetBlob(0);
//				//	try to merge all blobs
//				for (int j = 1; j < blobs.GetNumBlobs(); j++ ) {
//					currentBlob-> JoinBlob( blobs.GetBlob(j) );
//				}
//				currentBlob->FillBlob( saveImage, CV_RGB(255,0,0));
//				xf=currentBlob->MaxX();
//				xi=currentBlob->MinX();
//				yf=currentBlob->MaxY();
//				yi=currentBlob->MinY();
//				cvRectangle( saveImage, cvPoint(xi, yi), cvPoint (xf, yf), CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
//				cvEllipseBox( saveImage, currentBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 
//			}
//			
//			cvShowImage( "BLOBS", saveImage );		
//			cvShowImage("BW",bitImage);
//			
//			int	c = cvWaitKey(10);
//			if( (char) c == 27 )
//				break;
//		}
//		
//		cvReleaseCapture( &capture );
//		//		cvDestroyWindow("CamSub");
//		//		cvDestroyWindow("CamSub 1");	
//	}
//}




/*===========================================================================*/
/* quick sort...
 icvSort( double *array, int length )
{
    int i, j, index;
    double swapd;
	
    if( !array || length < 1 )
        return CV_BADFACTOR_ERR;
	
    for( i = 0; i < length - 1; i++ )
    {
		
        index = i;
		
        for( j = i + 1; j < length; j++ )
        {
			
            if( array[j] < array[index] )
                index = j;
        }                     
		
        if( index - i )
        {
			
            swapd = array[i];
            array[i] = array[index];
            array[index] = swapd;
        }                 
    }                     
	
    return CV_NO_ERR;
	
} 
 */
