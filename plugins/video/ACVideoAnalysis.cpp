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
#include "ACColorImageAnalysis.h" 
#include "ACBWImageAnalysis.h" 
#include "ACMediaTimedFeature.h" // essai similarity matrix

#include <string>
#include <iostream>
#include <sstream>
#include <cmath> // for fabs

#ifndef APPLE_LEOPARD
#include "ACFFmpegToOpenCV.h"
#endif

#include <iomanip>
using namespace std;

// ----------- uncomment this to get (+/- live) visual display of the time series
//#define VISUAL_CHECK_GNUPLOT

#ifdef VISUAL_CHECK_GNUPLOT
#include "gnuplot_i.hpp"
#endif //  VISUAL_CHECK_GNUPLOT

// ----------- uncomment this to get visual display using highgui and verbose -----
//#define VISUAL_CHECK
//#define VERBOSE
// ----------- class constants
const int ACVideoAnalysis::ystar = 150; // 220
// -----------

 
ACVideoAnalysis::ACVideoAnalysis(){
	capture = 0;
	clean();
	// NOT initialized, has to be done from outside after setting file name
}

ACVideoAnalysis::ACVideoAnalysis(const string &filename){
	clean();
	FROM_FILE = true;
	setFileName(filename);
	capture = cvCreateFileCapture(file_name.c_str());		
	initialize(); // done here, since we know the file name
}

// this is normally what plugins should call
// since they have access to mediadata
ACVideoAnalysis::ACVideoAnalysis(ACMediaData* media_data){
	clean();
	file_name = media_data->getFileName();
	capture = media_data->getVideoData();
	initialize();
}


void ACVideoAnalysis::clean(){
	// no, we should not release the capture, since it comes from outside.
	// we don't make "new" capture, we just set a pointer to an existing one (which will be deleted outside)
//	if (capture != 0) cvReleaseCapture(&capture);
	FROM_FILE = false;
	HAS_TRAJECTORY = false;
	HAS_BLOBS = false;
	frame_counter = 0;
	all_blobs.clear();
	all_blobs_time_stamps.clear();
	all_blobs_frame_stamps.clear();
	blob_centers.clear();
	blob_speeds.clear(); 
	contraction_indices.clear();
	bounding_box_ratios.clear();
	bounding_box_heights.clear();
	bounding_box_widths.clear();	
	blob_pixel_speeds.clear();
	global_pixel_speeds.clear();
	interest_points.clear();
	raw_moments.clear();
	hu_moments.clear();
	fourier_polar_moments.clear();
	fourier_mellin_moments.clear();

	width = height = depth = fps = nframes = 0;
	threshU = threshL = 0;
	file_name = "";
	//	averageHistogram = 0;
}

void ACVideoAnalysis::rewind(){
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0); 	
}

ACVideoAnalysis::~ACVideoAnalysis(){
	// note: the image captured by the device is allocated /released by the capture function. 
	// There is no need to release it explicitly. only release capture
	
	// release capture only if we created it from file.
	// otherwise we did not generate a "new" capture !!
	
	if (FROM_FILE && capture) cvReleaseCapture(&capture);
	//	if (averageHistogram) delete averageHistogram;
}

void ACVideoAnalysis::setFileName(const string &filename){
	// XS TODO: test if file exists

	file_name=filename;
}

int ACVideoAnalysis::initialize(){
	// returns 1 if it worked, 0 if not
	frame_counter = 0; // reset frame counter, to make sure
	if( !capture ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACVideoAnalysis::initialize> Could not initialize capturing from file " << file_name << endl;
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
		cerr << "<ACVideoAnalysis::initialize> : zero image size for " << file_name << endl;
		init_ok = 0;
	}
	if (nframes == 0){ 
		cerr << "<ACVideoAnalysis::initialize> : zero frames for " << file_name << endl;
		init_ok = 0;
	}
	// test fps ? does not really matter if badly encoded...  
	
#ifdef VERBOSE
	cout << "* Analyzing video file: " << file_name << endl;
	cout << "width : " << width << endl;
	cout << "height : " << height << endl;
	cout << "fps : " << fps << endl;
	cout << "numFrames : " << nframes << endl;
	//	cout << "codec : " << videocodec << endl;
#endif // VERBOSE
	
	return init_ok; // to be consistent with MycolorImage::SetImageFile : returns 0 if problem
}

float ACVideoAnalysis::getDuration(){
	// returns duration in seconds or number of frames if fps not available
//	if( !capture ) this->initialize();
	float tmp = 0.0;
	if (fps != 0) tmp = nframes * 1.0/fps;
	else tmp = nframes;
	return tmp;
}

IplImage* ACVideoAnalysis::getNextFrame(){
	// returns a pointer to the next frame of the video (called capture in OpenCV)
	// this is done in 2 steps in the OpenCV jargon: cvGrabFrame + cvRetrieveFrame
	// keeps an "independent" (non-OpenCV) record of the frame number (frame_counter)
	#ifdef USE_DEBUG
	std::cout << "ACVideoAnalysis::getNextFrame " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) << " / " << cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT) << "(fps " << cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) << ")" << std::endl;
	#endif

	if(!cvGrabFrame(capture)){      // attemps to capture a frame
		cerr << "<ACVideoAnalysis::getNextFrame> Could not find frame..." << endl;
		return 0;
	}
	IplImage* tmp;
	tmp = cvRetrieveFrame(capture);  // retrieve the captured frame
	frame_counter++;
	return tmp;
}

IplImage* ACVideoAnalysis::getFrame(int i){
	if (i < 0 || i > nframes) {
		cerr << "frame index out of bounds: " << i << endl;
		return 0;
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, i); 	
	IplImage* img = getNextFrame();
	return img;
}

void ACVideoAnalysis::histogramEqualize(const IplImage* bg_img) {
	if (bg_img == 0){
		bg_img = this->computeMedianImage();
		if (bg_img == 0){
			cerr << "<ACVideoAnalysis::histogramEqualize>: error computing median bg image" << endl;
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

//void ACVideoAnalysis::histogramEqualizeUL(const IplImage* const bg_img) {
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
	all_blobs_frame_stamps.clear();	// just to make sure...

	if (bg_img == 0){
		bg_img = this->computeMedianImage();
		if (bg_img == 0){
			cerr << "<ACVideoAnalysis::computeBlobs>: error computing average image" << endl;
		}
		// reset the capture to the beginning of the video
		this->rewind();
	}
	
	// initial frame
	IplImage* frame;
	
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
		
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
	
	for(int i = 0; i < nframes; i++){
		frame = getNextFrame();
		cvAbsDiff(frame, bg_img, frame);
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvThreshold(bwImage, bitImage, bg_thresh,255,CV_THRESH_BINARY_INV);
		CBlobResult blobs;
		blobs = CBlobResult( bitImage, 0, 255 ); // find blobs in image
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob );
		// XS for dancers we only stored when there was a blob
		
		//		if (blobs.GetNumBlobs() > 0){
			all_blobs.push_back(blobs);
			int _frame_number = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
			all_blobs_time_stamps.push_back(_frame_number*1.0/fps); // in seconds
			all_blobs_frame_stamps.push_back(_frame_number); // in frames
//		}
#ifdef VISUAL_CHECK
		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		if (blobs.GetNumBlobs() > 0){
				// for visual purposes only, blobs are not really "merged"
				CvRect rbox = blobs.GetBoundingBox();				
				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
					blobs.GetBlob(j)->FillBlob(frame, CV_RGB(255,0,0));
				}
				
				CvPoint ii = cvPoint(rbox.x,rbox.y);
				CvPoint ff = cvPoint(rbox.x+rbox.width,rbox.y+rbox.height);
				cvRectangle( frame, ii, ff, CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
		}
		else {
			cout << "no blobs for frame: " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) <<endl;
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
	
	HAS_BLOBS = true;
}

void ACVideoAnalysis::computeBlobsInteractively(IplImage* bg_img, bool merge_blobs, int bg_thesh, int big_blob, int small_blob){
	all_blobs.clear();
	all_blobs_time_stamps.clear();	// just to make sure...
	all_blobs_frame_stamps.clear();	// just to make sure...

#ifdef VISUAL_CHECK_GNUPLOT
	Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();
	vector<double> ci;
#endif //VISUAL_CHECK_GNUPLOT
	
	if (bg_img == 0){
		bg_img = this->computeMedianImage();
		if (bg_img == 0){
			cerr << "<ACVideoAnalysis::computeBlobsInteractively>: error computing average image" << endl;
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
	cvCreateTrackbar("Biggest Blob","BLOBS", &slider_big_blob, 1000 ,0);
	cvCreateTrackbar("Threshold","BLOBS",&slider_bg_thresh,255,0);
	
	IplImage* frame;
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
	
	int xi,xf,yi,yf;
	CBlobResult blobs;

	for(int i = 0; i < nframes-1; i++){
		frame = getNextFrame();
		saveImage =cvCloneImage(frame);
		cvAbsDiff(frame, bg_img, frame);
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvThreshold(bwImage, bitImage, slider_bg_thresh,255,CV_THRESH_BINARY_INV);
		
		// XS closing filter (to remove dips) 
		cvDilate(bitImage, bitImage);
		cvErode(bitImage, bitImage);
		
		blobs = CBlobResult( bitImage, 0, 255 );
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_INSIDE,  slider_big_blob );
		all_blobs.push_back(blobs);
		int _frame_number = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
		all_blobs_time_stamps.push_back(_frame_number*1.0/fps); // in seconds
		all_blobs_frame_stamps.push_back(_frame_number); // in frames

		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (saveImage, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		
		if (blobs.GetNumBlobs() > 0){
			if (merge_blobs) {
				// for visual purposes only, blobs are not really "merged"
				CvRect rbox = blobs.GetBoundingBox();				
//#ifdef VISUAL_CHECK_GNUPLOT
//				ci.push_back( blobs.Area() / (rbox.width*rbox.height) );
//				g1.reset_plot();
//				g1.plot_x(ci,"ci");
//#endif //VISUAL_CHECK_GNUPLOT
				
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
					CBlob *currentBlob = blobs.GetBlob(j);
					currentBlob->FillBlob(saveImage, CV_RGB(255,0,0));
					xf=currentBlob->MaxX();
					xi=currentBlob->MinX();
					yf=currentBlob->MaxY();
					yi=currentBlob->MinY();			
					cvRectangle( saveImage, cvPoint(xi, yi), cvPoint (xf, yf), CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
					//					cvEllipseBox(saveImage, currentBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 					
				}
			}
		}
		else {
			cout << "no blobs for frame: " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) <<endl;
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
	HAS_BLOBS = true;

}

void ACVideoAnalysis::computeBlobsUL(IplImage* bg_img, bool merge_blobs, int big_blob, int small_blob){
	// different threshold upper/lower part of the image
	all_blobs.clear();
	all_blobs_time_stamps.clear();	// just to make sure...
	all_blobs_frame_stamps.clear();	// just to make sure...
	if (bg_img == 0 || threshU ==0 || threshL ==0){
		if (bg_img == 0){
			cout << "No bg image provided for ACVideoAnalysis::computeBlobsUL. Computing Median" << endl;
		}
		else{
			cout << "No bg threshold provided for ACVideoAnalysis::computeBlobsUL. Computing it from Median" << endl;
		}
		bg_img = this->computeMedianImage();
		if (bg_img == 0){
			cerr << "<ACVideoAnalysis::computeBlobsInteractively>: error computing average image" << endl;
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
	int xi,xf,yi,yf;
#endif // VISUAL_CHECK
	
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
		// cvMorphologyEx( bitImage, bitImage, 0, 0, CV_MOP_CLOSE, 2);
		//cvSmooth(bitImage, bitImage, CV_MEDIAN, 3, 5);
		
		// does not work:
		//		cvRunningAvg (bitImage, r_avg_img, 0.5);
		//		cvConvertImage(r_avg_img, bitImage);
		
		CBlobResult blobs;
		blobs = CBlobResult( bitImage, 0, 255 ); // find blobs in image
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob );

// XS for dancers we only stored when there was a blob
//		if (blobs.GetNumBlobs() > 0){
			all_blobs.push_back(blobs);
			int _frame_number = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
			all_blobs_time_stamps.push_back(_frame_number*1.0/fps); // in seconds
			all_blobs_frame_stamps.push_back(_frame_number); // in frames
//		}
#ifdef VISUAL_CHECK
		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
		if (blobs.GetNumBlobs() > 0){
			if (merge_blobs) {
				// for visual purposes only, blobs are not really "merged"
				CvRect rbox = blobs.GetBoundingBox();				
				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
					blobs.GetBlob(j)->FillBlob(frame, CV_RGB(255,0,0));
				}
				
				CvPoint ii = cvPoint(rbox.x,rbox.y);
				CvPoint ff = cvPoint(rbox.x+rbox.width,rbox.y+rbox.height);
				cvRectangle( frame, ii, ff, CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
			}
			else {
				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
					CBlob *currentBlob = blobs.GetBlob(j);
					currentBlob->FillBlob(frame, CV_RGB(255,0,0));
					xf=currentBlob->MaxX();
					xi=currentBlob->MinX();
					yf=currentBlob->MaxY();
					yi=currentBlob->MinY();			
					cvRectangle( frame, cvPoint(xi, yi), cvPoint (xf, yf), CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
				}
			}
		}
		else {
			cout << "no blobs for frame: " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) <<endl;
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
	HAS_BLOBS = true;
}

void ACVideoAnalysis::computeOpticalFlow(){
	interest_points.clear();

	// from Sidi Mahmoudi
	int win_size = 10;
	const int MAX_COUNT = 500;
	CvPoint2D32f* points[2] = {0,0}, *swap_points;
	char* status = 0;
	int count = 0;
	int need_to_init = 0;
	int flags = 0;
	int add_remove_pt = 0;
	CvPoint pt;
#ifdef VISUAL_CHECK
	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE );
	cvResizeWindow( "Camera", 800, 600 );
#endif // VISUAL_CHECK
	IplImage* frame = 0;
	int currentframe = 0;
	
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
		int i, k;
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
#ifdef VERBOSE		
				double a = atan2( (double) p.y - q.y, (double) p.x - q.x );
				double m = sqrt( (p.y - q.y)*(p.y - q.y) + (p.x - q.x)*(p.x - q.x) );
#endif //VERBOSE
				cvLine( image, p , q , CV_RGB(255,0,0), 1, 8,0);
#ifdef VERBOSE		
//				printf("%d;%d;%d;%g;%g\n", currentframe, p.x, p.y, a, m);
#endif //VERBOSE
			}
			count = k;
			printf("%d;%d\n", currentframe, count);
		}

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
#ifdef VISUAL_CHECK
		cvShowImage( "Camera", image );
		int c = cvWaitKey(10);
		if( (char)c == 27 )
			break;
#endif // VISUAL_CHECK

	}	
#ifdef VISUAL_CHECK
	cvDestroyWindow("Camera");
#endif // VISUAL_CHECK

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
	if (!HAS_BLOBS) this->computeBlobsUL();
	
	for (unsigned int i=0; i< all_blobs.size(); i++){
		vector<float> tmp;
		currentBlob = all_blobs[i];
		if (currentBlob.GetNumBlobs() > 0){
			CvPoint center = currentBlob.GetCenter();
			tmp.push_back(center.x);
			tmp.push_back(center.y);
		}
		else{
			tmp.push_back(0);
			tmp.push_back(0);
		}
			
		blob_centers.push_back(tmp);
	}
	HAS_TRAJECTORY = true;
	
}

vector<blob_center> ACVideoAnalysis::getNormalizedMergedBlobsTrajectory() {
	// this assumes blob_center is 2D (x,y), which is fine in general
	vector<blob_center> normalized_blob_centers;
	for (unsigned int i=0; i<blob_centers.size();i++){
		blob_center tmp;
		tmp.push_back (blob_centers[i][0]/width);
		tmp.push_back (blob_centers[i][1]/height);
		normalized_blob_centers.push_back(tmp);
	}
	return normalized_blob_centers;
}

vector<blob_center> ACVideoAnalysis::getNormalizedMergedBlobsSpeeds() {
	// this assumes blob_center is 2D (x,y), which is fine in general
	vector<blob_center> normalized_blob_speeds;
	for (unsigned int i=0; i<blob_speeds.size();i++){
		blob_center tmp;
		tmp.push_back (blob_speeds[i][0]/width);
		tmp.push_back (blob_speeds[i][1]/height);
		normalized_blob_speeds.push_back(tmp);
	}
	return normalized_blob_speeds;
}

void ACVideoAnalysis::computeMergedBlobsSpeeds(float blob_dist){
	// XS better: do this in ACMediaTimedFeatures
	if (!HAS_TRAJECTORY) this->computeMergedBlobsTrajectory(blob_dist);
	blob_speeds.clear();
	blob_center cb_prev;
	blob_center cb = blob_centers[0];
	blob_center v_init;
	v_init.push_back(0.0);
	v_init.push_back(0.0);
	blob_speeds.push_back(v_init); // so that it has the same size for time stamps
	for (unsigned int i=1; i< all_blobs.size(); i++){ // not i=0
		blob_center speed;
		cb_prev = cb;
		cb = blob_centers[i];
		for (unsigned j=0; j< cb.size(); j++) { // normally 2D
			speed.push_back(cb[j]-cb_prev[j]); // fabs ?	
		}
		blob_speeds.push_back(speed);
	}
}

IplImage* ACVideoAnalysis::computeAverageImage(int nskip, int nread, int njump, string fsave) { 
	// nskip = number of frames to skip at the beginning
	// nread = number of frames to read in the video
	// ncalc = number of frames to consider
	// so that we go by steps of nread/ncalc frames
	// saving in a file if fsave != ""
	if (nskip + nread > nframes) {
		cerr << " <ACVideoAnalysis::computeAverageImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
		cerr << "nframes = " << nframes << "; nskip = " << nskip <<  "; nread = " << nread << "; njump = " << njump << endl; 
		return 0;
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

IplImage* ACVideoAnalysis::computeMedianImage(int nskip, int nread, int njump, string fsave) { 
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
		cerr << " <ACVideoAnalysis::computeMedianImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
		cerr << "nframes = " << nframes << "; nskip = " << nskip <<  "; nread = " << nread << "; njump = " << njump << endl; 
		return 0;
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

IplImage* ACVideoAnalysis::computeMedianNoBlobImage(string fsave, IplImage *first_guess){
	// only on frames that have no blobs
	if (all_blobs_time_stamps.size()==0 || all_blobs.size()==0) {
		// XS debug
		threshU = 3;
		threshL = 14;
		// XS end
		computeBlobsUL(first_guess); // could be 0, in which case aprox median is calculated
	}
	if (all_blobs_time_stamps.size() != all_blobs.size()) {
		cerr << "<ACVideoAnalysis::computeMedianNoBlobImage> : time stamp problem" << endl;
		return 0;
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
	
	for (unsigned int i=0;i<all_blobs.size();i++){
		if (all_blobs[i].GetNumBlobs() == 0){
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

// substracts each image with blob from the previous one and sums all pixels of the difference image
// XS: could add option to calculate just on a segment of the video
void ACVideoAnalysis::computeBlobPixelSpeed() {
	blob_pixel_speeds.clear();
	this->rewind();
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
		cerr << "<ACVideoAnalysis::computeBlobPixelSpeed> : not enough frames in video : " << \
		file_name << endl;
		return;
	}
	
	CvScalar sum_diff_frames ;
	float speed = 0.0;
	
	// initial frame
	tmp_frame = this->getNextFrame();
	blob_pixel_speeds.push_back(speed); // so that the blob_pixel_speeds vector has the same lenght as the blob_time_stamps
	
	frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	previous_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames_U = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	cvSetZero (frame);
	
#ifdef VISUAL_CHECK
	cvNamedWindow ("Input", CV_WINDOW_AUTOSIZE);
	cvNamedWindow ("Subtraction", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK
	
	for(unsigned int i = 1; i < all_blobs.size(); i++){ 
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
#ifdef VERBOSE
		cout << "frame " << i << ": pixel speed = " << speed << endl;
#endif // VERBOSE

		blob_pixel_speeds.push_back(speed);
		cvConvert (diff_frames, diff_frames_U);
#ifdef VISUAL_CHECK
		cvShowImage ("Input", tmp_frame);
		cvShowImage ("Substraction", diff_frames_U);
		cvWaitKey (10);
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

// substracts each image from the previous one and sums all pixels of the difference image
// XS: could add option to calculate just on a segment of the video
void ACVideoAnalysis::computeGlobalPixelSpeed() {
	global_pixel_speeds.clear();
	this->rewind();
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
		cerr << "<ACVideoAnalysis::computeGlobalPixelSpeed> : not enough frames in video : " << \
		file_name << endl;
		return;
	}
	
	CvScalar sum_diff_frames ;
	float speed = 0.0;
	
	#ifdef APPLE_LEOPARD // assuming this is the only OpenCV QuickTime working installation
	// initial frame
	tmp_frame = this->getNextFrame();
	#else
	ACFFmpegToOpenCV file_cap;
	int i = 0;
	file_cap.init(file_name.c_str());
	tmp_frame = cvCreateImage(cvSize(file_cap.nCols,file_cap.nRows),IPL_DEPTH_8U,3);
	file_cap.getframe(&tmp_frame);
	#endif
	
	#if defined(USE_DEBUG) && defined(APPLE_LEOPARD)
	std::cout << "ACVideoAnalysis::computeGlobalPixelSpeed " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) << " / " << cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT) << "(fps " << cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) << ")" << std::endl;
	#endif
	global_pixel_speeds.push_back(speed); // so that the global_pixel_speeds vector has the same lenght as the time_stamps
	
	frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	previous_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames = cvCreateImage (cvSize (width, height), IPL_DEPTH_32S, 3);
	diff_frames_U = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	cvSetZero (frame);
	
#ifdef VISUAL_CHECK
	cvNamedWindow ("Input", CV_WINDOW_AUTOSIZE);
	cvNamedWindow ("Subtraction", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK
	
	for(unsigned int i = 0; i < nframes-1; i++){ 
		cvConvert (tmp_frame, previous_frame);
		#ifdef APPLE_LEOPARD
		tmp_frame = this->getNextFrame();
		#else
		file_cap.getframe(&tmp_frame);
		#endif

		#if defined(USE_DEBUG) && defined(APPLE_LEOPARD)
		std::cout << "ACVideoAnalysis::computeGlobalPixelSpeed " << cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) << " / " << cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT) << "(fps " << cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) << ")" << std::endl;
		#endif
		cvConvert (tmp_frame, frame);
		cvAbsDiff (frame, previous_frame, diff_frames);
		sum_diff_frames = cvSum (diff_frames);
		speed = 0.0;
		for(int j = 0; j < 3; j++){
			speed += sum_diff_frames.val[j];
		}
		speed = speed / (4*width*height);
#ifdef VERBOSE
		cout << "frame " << i << ": pixel speed = " << speed << endl;
#endif // VERBOSE
		
		global_pixel_speeds.push_back(speed);
		cvConvert (diff_frames, diff_frames_U);
#ifdef VISUAL_CHECK
		cvShowImage ("Input", tmp_frame);
		cvShowImage ("Substraction", diff_frames_U);
		cvWaitKey (10);
#endif //VISUAL_CHECK
		//tmp_frame = this->getNextFrame();
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
	#ifndef APPLE_LEOPARD
	file_cap.closeit();
	#endif
}


void ACVideoAnalysis::computeContractionIndices(){
	contraction_indices.clear();
	CvRect bbox;
	float ci;
	for (unsigned int i=0; i< all_blobs.size(); i++){ 
		if (all_blobs[i].GetNumBlobs() > 0){
			bbox = all_blobs[i].GetBoundingBox();
			ci = all_blobs[i].Area() / (bbox.width*bbox.height);
			contraction_indices.push_back(ci);
		}
		else 
			contraction_indices.push_back(0);
	}
}

void ACVideoAnalysis::computeBoundingBoxRatios(){
	if (!HAS_TRAJECTORY) this->computeMergedBlobsTrajectory();
	bounding_box_ratios.clear();
	bounding_box_heights.clear();
	bounding_box_widths.clear();
	CvRect bbox;
	float bd;
	for (unsigned int i=0; i< all_blobs.size(); i++){ 
		if (all_blobs[i].GetNumBlobs() > 0){
			bbox = all_blobs[i].GetBoundingBox();
			bd = all_blobs[i].Area() / (bbox.width*bbox.width); // x/y * n/(xy) --- check this
			bounding_box_ratios.push_back(bd);
			bounding_box_heights.push_back(bbox.height);
			bounding_box_widths.push_back(bbox.width);
		}
		else {
			bounding_box_ratios.push_back(0);
			bounding_box_heights.push_back(0);
			bounding_box_widths.push_back(0);
		}
	}
}

void ACVideoAnalysis::computeGlobalPixelsSpeed(){
	// reset the capture to the beginning of the video
	this->rewind();
	IplImage* current_frame  = getNextFrame();
	IplImage* previous_frame = cvCreateImage( cvGetSize(current_frame), IPL_DEPTH_8U, 3 );;
	IplImage* diff_frames = cvCreateImage( cvGetSize(current_frame), IPL_DEPTH_8U, 3 );
	CvScalar sum_diff_frames ;
	float frame_diff = 0.0;
	
	cvNamedWindow("check", CV_WINDOW_AUTOSIZE);

	// XS todo check bounds
	for(int ifram=1; ifram<nframes-1; ifram++) {
		cvCopy (current_frame, previous_frame) ;
		current_frame = getNextFrame();
		showFrameInWindow("check", current_frame);
		cvAbsDiff (current_frame, previous_frame, diff_frames);
		sum_diff_frames = cvSum (diff_frames);
		frame_diff = 0.0;
		for(int j = 0; j < 3; j++){
			frame_diff += sum_diff_frames.val[j];
		}
		frame_diff = frame_diff / (4*width*height);
#ifdef VERBOSE
		cout << "frame " << ifram << " : diff = " << frame_diff << endl;
#endif //VERBOSE

	}
	//cvReleaseImage (&current_frame); // non, sinon double free !
	cvReleaseImage (&previous_frame);
	cvReleaseImage (&diff_frames);	
}

// computes Raw moments for the same price
void ACVideoAnalysis::computeHuMoments(int tresh){
}

void ACVideoAnalysis::computeHuMoments(IplImage* bg_img, int thresh){
	hu_moments.clear();
	raw_moments.clear();
	this->rewind(); // reset the capture to the beginning of the video

#ifdef VISUAL_CHECK
	cvNamedWindow ("Thresh", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK
	
	IplImage* current_frame;
	for(int ifram=0; ifram<nframes; ifram++) {
		current_frame  = getNextFrame();
		cvAbsDiff(current_frame, bg_img, current_frame);
		ACColorImageAnalysis color_frame(current_frame);
		color_frame.computeHuMoments(thresh);
		
#ifdef VISUAL_CHECK
		color_frame.showInWindow ("Thresh");
		cvWaitKey (10);
#endif //VISUAL_CHECK
		
		raw_moments.push_back(color_frame.getRawMoments());
		hu_moments.push_back(color_frame.getHuMoments());
	}
#ifdef VISUAL_CHECK
	cvDestroyWindow ("Thresh");
#endif //VISUAL_CHECK
}

void ACVideoAnalysis::computeFourierPolarMoments(int RadialBins, int AngularBins){
	fourier_polar_moments.clear();
	this->rewind(); // reset the capture to the beginning of the video
		
	IplImage* current_frame;
	for(int ifram=0; ifram<nframes; ifram++) {
		current_frame  = getNextFrame();
		ACColorImageAnalysis color_frame(current_frame);
		color_frame.computeFourierPolarMoments(RadialBins,AngularBins);
		fourier_polar_moments.push_back(color_frame.getFourierPolarMoments());
	}
}

void ACVideoAnalysis::computeFourierMellinMoments(){
	fourier_mellin_moments.clear();
	this->rewind(); // reset the capture to the beginning of the video
	
	IplImage* current_frame;
	for(int ifram=0; ifram<nframes; ifram++) {
		current_frame  = getNextFrame();
		ACColorImageAnalysis color_frame(current_frame);
		color_frame.computeFourierMellinMoments();
		fourier_mellin_moments.push_back(color_frame.getFourierMellinMoments());
	}
}

void ACVideoAnalysis::computeImageHistograms(int w, int h){
	image_histograms.clear();
	this->rewind(); // reset the capture to the beginning of the video
	
	IplImage* current_frame;
	for(int ifram=0; ifram<nframes; ifram++) {
		current_frame  = getNextFrame();
		ACColorImageAnalysis color_frame(current_frame);
		color_frame.computeImageHistogram(w,h);
		image_histograms.push_back(color_frame.getImageHistogram());
	}
}

// XS TODO watch out index !!
// first moment = [0]
// use armadillo instead ?

vector<float> ACVideoAnalysis::getHuMoment(int momi){
	if (momi < 0 || momi >= 7) {
		cerr << "no Hu moment of this order, " << momi << " not in [0,6]" << endl;
		exit(1);
	}
	vector<float> dummy;
	for (unsigned int i=0; i<hu_moments.size(); i++){
		dummy.push_back (hu_moments[i][momi] ) ;
	}
	return dummy;
}

// to get dummy time stamps (i.e., the indices 0,1,2,...)
// XS: I made these FLOAT as in ACMediaFeatures but really it's INT
vector<float> ACVideoAnalysis::getDummyTimeStamps(int nsize){
	vector<float> dummy;
	for (int i=0; i<nsize; i++){ 
		dummy.push_back((float)i);
	}
	return dummy;
}

vector<float> ACVideoAnalysis::getBlobsTimeStamps(){
	// XS: I made these FLOAT as in ACMediaFeatures but really it's INT
	// these are "real" time stamps from cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES)
	return all_blobs_time_stamps;
}

vector<float> ACVideoAnalysis::getGlobalTimeStamps(){
	vector<float> global_ts;
	for(int ifram=0; ifram<nframes; ifram++) {
		global_ts.push_back(ifram*1.0/fps); // in seconds
	}
	return global_ts;
}

vector<float> ACVideoAnalysis::getGlobalFrameStamps(){
	vector<float> global_fs;
	for(int ifram=0; ifram<nframes; ifram++) {
		global_fs.push_back(ifram); // in frame numbers
	}
	return global_fs;
}


// ------------------ visual output functions -----------------
void ACVideoAnalysis::showFrameInWindow(string title, IplImage* frame, bool has_win){
	// by default has_win = true, we don't make a new window for each frame !
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	if (frame != 0) {
		cvShowImage(title.c_str(), frame); 
		cvWaitKey(20);           // wait 20 ms, necesary to display properly.
	}
}

void ACVideoAnalysis::showInWindow(string title, bool has_win){
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

void ACVideoAnalysis::showFFTInWindow(string title, bool has_win){
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	
	cvResizeWindow(title.c_str(), 800, 600 );

	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	
	IplImage* img = 0;
	for(int i = 0; i < nframes; i++){
		img = getNextFrame();
		ACColorImageAnalysis color_frame(img);
		color_frame.makeBWImage();
		ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(color_frame.getBWImage());
		bw_helper->computeFFT2D_complex();
		bw_helper->showFFTComplexInWindow (title.c_str());
		delete bw_helper;
	}
	cvWaitKey(0);
	//	cvReleaseImage(&img); -- will be released by cvReleaseCapture, because img=getNextFrame 
	// do not free twice the same buffer !
	cvDestroyWindow(title.c_str());

}

// ----------- for "fancy" browsing

int        g_slider_position = 0; 
CvCapture* g_capture         = 0;

void onTrackbarSlide(int pos) { 
	cvSetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos); 
} 
// ----------- 

void ACVideoAnalysis::browseInWindow(string title, bool has_win){
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
	
	cvResizeWindow(title.c_str(), 800, 600 );
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	int c;
	g_capture = cvCreateFileCapture(file_name.c_str()); 
	cvCreateTrackbar("Starting Position", title.c_str(), &g_slider_position, nframes, onTrackbarSlide ); 
	
	IplImage* img = 0;
	for(int i = 0; i < nframes-1; i++){
		if(!cvGrabFrame(g_capture)){              // capture a frame
			cerr << "<ACVideoAnalysis::browseInWindow> Could not find frame..." << endl;
			return;
		}
		img = cvRetrieveFrame(g_capture);           // retrieve the captured frame

		if( !img ) {
			break;
			cout << "end of movie (aka The End)" << endl;
		}
		cvShowImage(title.c_str(), img); 
		
		c = cvWaitKey (20);
		if (c == '\x1b') // press ESC to pause the video, then any key to continue
			cvWaitKey(0);
	}
	cvWaitKey(0);
	//	cvReleaseImage(&img); -- will be released by cvReleaseCapture, because img=getNextFrame 
	// do not free twice the same buffer !
	cvDestroyWindow(title.c_str());
}

// ------------------ file output functions -----------------

bool ACVideoAnalysis::saveInFile (string fileout, int nskip){
	// CV_FOURCC('M','J','P','G')
	bool ok = true;
	if (this->getNumberOfFrames() <=0) ok = false;
	CvVideoWriter* video_writer = cvCreateVideoWriter( fileout.c_str(), -1, fps, cvSize(width,height) );  // "-1" pops up a nice GUI (Windows only)
	IplImage* img = 0;
	rewind();
	for(int i = nskip; i < nframes-1; i++){
		img = getNextFrame();
		cvWriteFrame(video_writer, img);  
		if( !img ) {
			break;
			cout << "end of movie (aka The End)" << endl;
		}
	}
	cvReleaseVideoWriter(&video_writer); 
	return ok;
}

void ACVideoAnalysis::saveVideoThumnbailInFile (string fileout, int _w, int _h, int _nskip, int _istep){
	// saves in fileout a video (i.e., not an image) thumnail consisting of frames from the video 
	// format : CV_FOURCC('M','J','P','G')
	// optionally:
	//   - reduced to size wxh
	//   - skips the first nskip frames
	//   - by steps of istep frames
	
	CvVideoWriter* video_writer = cvCreateVideoWriter( fileout.c_str(),  CV_FOURCC('M','J','P','G'), fps, cvSize(_w,_h) );
	IplImage* img = 0;
	rewind();
	IplImage* img_sm = cvCreateImage(cvSize (_w, _h), depth, 3); // XS TODO : 3 = nb channels -- generealize
	for(int i = _nskip; i < nframes-1; i+=_istep){
		img = getFrame(i);
		if( !img ) {
			cout << "end of movie (aka The End)" << endl;
			break;
		}
		// SD TODO - This is stange cvCreateImage creates image wit as a widthStep of 152
		cvResize(img, img_sm, CV_INTER_CUBIC); // OR LINEAR
		cvWriteFrame(video_writer, img_sm);  
	}
	cvReleaseImage(&img_sm);
	cvReleaseVideoWriter(&video_writer); 
}

void ACVideoAnalysis::dumpTrajectory(ostream &odump) {
	// XS TODO check if computed
	odump << setiosflags(ios::fixed) <<setprecision(3); // 0.123
	odump << showpoint;
	for (int i = 0; i < int(blob_centers.size()); i++){
		odump << setw(10);
		odump << all_blobs_frame_stamps[i]  << " ";
		odump << setw(10);
		odump << blob_centers[i][0] << " ";
		odump << setw(10);
		odump << blob_centers[i][1] << endl; 
	}; 
}	

void ACVideoAnalysis::dumpContractionIndices(ostream &odump) {
	// XS TODO check if computed
	odump.precision(3); // 0.123
	odump << showpoint;
	for (int i = 0; i < int(contraction_indices.size()); i++){
		odump << setw(10);
		odump << all_blobs_frame_stamps[i]  << " ";
		odump << setw(10);
		odump << contraction_indices[i] << endl; 
	}; 
}	

void ACVideoAnalysis::dumpBoundingBoxRatios(ostream &odump) {
	// XS TODO check if computed
	odump.precision(3); // 0.123
	odump << showpoint;
	for (int i = 0; i < int(bounding_box_ratios.size()); i++){
		odump << setw(10);
		odump << all_blobs_frame_stamps[i]  << " ";
		odump << setw(10);
		odump << bounding_box_ratios[i] << endl; 
	}; 
}	


void ACVideoAnalysis::dumpBlobSpeed(ostream &odump) {
	// XS TODO check if computed
	odump << setiosflags(ios::fixed) << setprecision(3); // 0.123
	odump << showpoint;
	for (int i = 0; i < int(blob_speeds.size()); i++){
		odump << setw(10);
		odump << all_blobs_frame_stamps[i]  << " ";
		odump << setw(10);
		odump << blob_speeds[i][0] << " ";
		odump << setw(10);
		odump << blob_speeds[i][1] << endl; 
	}; 	
}

void ACVideoAnalysis::dumpRawMoments(ostream &odump) {
	// XS TODO check if computed
	// m00, m10, m01, m20, m11, m02, m30, m21, m12, m03; 
	for (unsigned int i = 0; i < raw_moments.size(); i++){
		odump << setw(10);
		odump << i << " ";
		for (unsigned int momi = 0; momi < 10; momi++){
			odump << setw(10); 
			odump << raw_moments[i][momi];
		}
		odump << endl;
	}
}

void ACVideoAnalysis::dumpHuMoments(ostream &odump) {
	// XS TODO check if computed
	for (unsigned int i = 0; i < hu_moments.size(); i++){
		odump << setw(10);
		odump << i << " ";
		for (unsigned int momi = 0; momi < 7; momi++){
			odump << setw(10); 
			odump << hu_moments[i][momi];
		}
		odump << endl;
	}	
}

void ACVideoAnalysis::dumpFourierPolarMoments(ostream &odump) {
	// XS TODO check if computed
	for (unsigned int i = 0; i < fourier_polar_moments.size(); i++){
		odump << setw(10);
		odump << i << " ";
		for (unsigned int momi = 0; momi < fourier_polar_moments[i].size(); momi++){
			odump << setw(10); 
			odump << fourier_polar_moments[i][momi] << " ";
		}
		odump << endl;
	}	
}

void ACVideoAnalysis::dumpAll(ostream &odump) {
	string fn = extractFilename(this->getFileName());
	string todor = fn.substr(0, fn.size()-4)+"_";
	odump << setiosflags(ios::fixed) <<setprecision(3); // 0.123
	odump << showpoint;
	for (int i = 0; i < int(blob_centers.size()); i++){
		odump << todor ;
		odump << all_blobs_frame_stamps[i]  << ", ";
		odump << setw(10);
		odump << blob_centers[i][0] << " ";
		odump << setw(10);
		odump << blob_centers[i][1] << " ";
		odump << setw(10);
		odump << contraction_indices[i] << " "; 
		odump << setw(10);
		odump << bounding_box_ratios[i] << " "; 
		odump << setw(10);
		odump << bounding_box_heights[i] << " ";
		odump << setw(10);
		odump << bounding_box_widths[i] << " ";
		for (unsigned int momi = 0; momi < 7; momi++){
			odump << setw(10); 
			odump << hu_moments[i][momi];
		}
		for (unsigned int momi = 0; momi < 10; momi++){
			odump << setw(10); 
			odump << raw_moments[i][momi];
		}
		
//		odump << setw(10);
//		odump << blob_speeds[i][0] << " ";
//		odump << setw(10);
//		odump << blob_speeds[i][1] << " "; 		
		odump << " ; " << endl; 
	}
}


string ACVideoAnalysis::extractFilename(string path)
{
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\' );
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(index + 1);
}


void ACVideoAnalysis::test_match_shapes(ACVideoAnalysis *V2, IplImage* bg_img){
// XS TODO CVMATCHSHAPES
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
//			blobs = CBlobResult( bitImage, 0, 255 );
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
