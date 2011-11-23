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

// This library extracts features from video, that can be analyzed by VideoPlugin later
// note : cv::VideoCapture should work for .mov, .avi, ...
// BUT this way depends on the codecs installed on your machine!
// possible problems with ffmpeg (some frames are lost / frame index incorrect)

#include "ACVideoAnalysis.h"
#include "ACColorImageAnalysis.h" 
#include "ACBWImageAnalysis.h" 
#include "ACMediaTimedFeature.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cmath> // for fabs
#include <opencv2/video/tracking.hpp>
#include <iomanip>
using namespace std;

// ----------- uncomment this to get (+/- live) visual display of the time series
//#define VISUAL_CHECK_GNUPLOT
// ----------- uncomment this to get visual display using highgui and verbose output -----
//#define VISUAL_CHECK
//#define VERBOSE


#ifdef VISUAL_CHECK_GNUPLOT
#include "gnuplot_i.hpp"
#endif //  VISUAL_CHECK_GNUPLOT

// ----------- class constants
// this is dancers-specific and defines the y position at which the floor is not visible anymore (try 220)
const int ACVideoAnalysis::ystar = 150;
// -----------

ACVideoAnalysis::ACVideoAnalysis() {
    capture = 0;
    clean();
    // NOT initialized, has to be done from outside after setting file name
}

ACVideoAnalysis::ACVideoAnalysis(const string &filename) {
    clean();
    FROM_FILE = true;
    setFileName(filename);
    capture = new cv::VideoCapture(filename.c_str());
    initialize(); // done here, since we know the file name
}

// this is normally what plugins should call
// since they have access to mediadata

ACVideoAnalysis::ACVideoAnalysis(ACMediaData* media_data, int _frameStart, int _frameStop) {
    clean();
    file_name = media_data->getFileName();
    capture = static_cast<cv::VideoCapture*> (media_data->getData());
    initialize(_frameStart, _frameStop);
}

void ACVideoAnalysis::clean() {
    // no, we should not release the capture, since it comes from outside.
    // we don't make "new" capture, we just set a pointer to an existing one (which will be deleted outside)
    //	if (capture != 0) cvReleaseCapture(&capture);
    FROM_FILE = false;
    HAS_TRAJECTORY = false;
    HAS_BLOBS = false;
    frame_counter = 0;
    all_blobs.clear();
    time_stamps.clear();
    frame_stamps.clear();
    blob_centers.clear();
    blob_speeds.clear();
    contraction_indices.clear();
    bounding_box_ratios.clear();
    bounding_box_heights.clear();
    bounding_box_widths.clear();
    blob_pixel_speeds.clear();
    global_pixel_speeds.clear();
    global_orientations.clear();
    interest_points.clear();
    raw_moments.clear();
    hu_moments.clear();
    fourier_polar_moments.clear();
    fourier_mellin_moments.clear();

    width = height = fps = nframes = 0;
    frameStart = frameStop = 0;
    spf = 0.0;
    threshU = threshL = 0;
    file_name = "";
    //	averageHistogram = 0;
}

// XS double rewind to make sure first frame is zero (or frameStart)
void ACVideoAnalysis::rewind() {
    capture->set(CV_CAP_PROP_POS_FRAMES, this->frameStart);
    cv::Mat tmp_frame;
    *capture >> tmp_frame;
    capture->set(CV_CAP_PROP_POS_FRAMES, this->frameStart);
}

ACVideoAnalysis::~ACVideoAnalysis() {
    // note: the image captured by the device is allocated /released by the capture function.
    // There is no need to release it explicitly, only release capture
    // and release capture only if we created it from file.
    // otherwise we did not generate a "new" capture !!

    if (FROM_FILE && capture) delete capture;
    //	if (averageHistogram) delete averageHistogram;
}

void ACVideoAnalysis::setFileName(const string &filename) {
    // XS TODO: test if file exists
    file_name = filename;
}

int ACVideoAnalysis::initialize(int _frameStart, int _frameStop) {
    // returns 1 if it worked, 0 if not
    int init_ok = 1;
    frame_counter = 0; // reset frame counter, to make sure
    if (!capture->isOpened()) {
        // Either the video does not exist, or it uses a codec OpenCV does not support.
        cerr << "<ACVideoAnalysis::initialize> Could not initialize capturing from file " << file_name << endl;
        return 0;
    }

    // Get capture device properties;
    width = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
    height = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    fps = (int) capture->get(CV_CAP_PROP_FPS);
    if (fps != 0)
        spf = 1.0 / fps;
    else
        // in case fps is badly encoded, time_stamp = frame_stamp
        spf = 1.0;
    nframes = (int) capture->get(CV_CAP_PROP_FRAME_COUNT) - 1; // XS -1 seems necessary in OpenCV 2.3

    //	videocodec = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FOURCC);
    // extra, not stored in capture
    if (nframes == 0) {
        cerr << "<ACVideoAnalysis::initialize> : zero frames for " << file_name << endl;
        return 0;
    }

    if (_frameStart == 0 && _frameStop == 0 ){
        this->frameStart = 0;
        this->frameStop = nframes;
    } else if (_frameStart = _frameStop){
        cerr << "<ACVideoAnalysis::initialize> : zero frames required for " << file_name << endl;
        return 0;
    } else if (_frameStart > _frameStop || _frameStart < 0 || _frameStop > nframes) {
        cerr << "<ACVideoAnalysis::initialize> : wrong frames limits for " << file_name << endl;
        return 0;
    } else {
        this->frameStart = _frameStart;
        this->frameStop = _frameStop;
    }
    
    if (width * height == 0) {
        cerr << "<ACVideoAnalysis::initialize> : zero image size for " << file_name << endl;
        init_ok = 0;
    }
    
#ifdef VERBOSE
    cout << "* Analyzing video file: " << this->file_name << endl;
    cout << "width : " << this->width << endl;
    cout << "height : " << this->height << endl;
    cout << "fps : " << this->fps << endl;
    cout << "spf : " << this->spf << endl;
    cout << "total number of frames in file: " << this->nframes << endl;
    cout << "first frame analyzed: " << this->frameStart << endl;
    cout << "last frame analyzed: " << this->frameStop << endl;

    //	cout << "codec : " << videocodec << endl;
#endif // VERBOSE

    this->setFramePosition(frameStart);
    return init_ok; // to be consistent with MycolorImage::SetImageFile : returns 0 if problem
}

float ACVideoAnalysis::getDuration() {
    // returns duration in seconds or number of frames if fps not available
    //	if( !capture ) this->initialize();
    return (this->frameStop - this->frameStart) *spf;
}

// no check if returning image is empty -> should be done outside : if (!img.data)...

cv::Mat ACVideoAnalysis::getFrame(int i) {
    if (i < frameStart || i > frameStop) {
        cerr << "frame index out of bounds: " << i << endl;
        return cv::Mat();
    }
    cv::Mat img;
    capture->set(CV_CAP_PROP_POS_FRAMES, i);
    *capture >> img;
    return img;
}

void ACVideoAnalysis::setFramePosition(int pos) {
    if ((pos >= frameStart) && (pos < frameStop)) {
        capture->set(CV_CAP_PROP_POS_FRAMES, pos);
    }
}

int ACVideoAnalysis::getFramePosition() {
    return capture->get(CV_CAP_PROP_POS_FRAMES); // XS TODO check borders
}

void ACVideoAnalysis::clearStamps() {
    time_stamps.clear();
    frame_stamps.clear();
}

void ACVideoAnalysis::stamp() {
    int _frame_number = capture->get(CV_CAP_PROP_POS_FRAMES); // XS TODO check borders
    time_stamps.push_back(_frame_number * spf); // in seconds
    frame_stamps.push_back(_frame_number); // in frames
}

//void ACVideoAnalysis::histogramEqualize(const IplImage* bg_img) {
//	if (bg_img == 0){
//		bg_img = this->computeMedianImage();
//		if (bg_img == 0){
//			cerr << "<ACVideoAnalysis::histogramEqualize>: error computing median bg image" << endl;
//		}
//		// reset the capture to the beginning of the video
//		this->rewind();
//	}
//	
//#ifdef VISUAL_CHECK
//	cvNamedWindow( "Source", 1) ;
//	cvNamedWindow( "Back Projection", 1) ;
//#endif // VISUAL_CHECK
//	
//	// Build and fill the histogram
//	int h_bins = 30, s_bins = 32;
//	CvHistogram* hist;
//	int hist_size[] = { h_bins, s_bins };
//	float h_ranges[] = { 0, 180 };
//	float s_ranges[] = { 0, 255 };
//	float* ranges[] = { h_ranges, s_ranges };
//	for(int i = 1; i < nframes-1; i++){
//		cv::Mat frame;
//		capture >> frame;
//		if (!frame.data) {
//			cerr << "unexpected end of video" << endl;
//			break;
//		}
//		cv::Size size = frame.size();
//		cv::Mat back_img (size.width, size.height, CV_8UC1);
//		cv::Mat hsv (size.width, size.height, CV_8UC3);
//		cv::Mat h_plane (size.width, size.height, CV_8UC1);
//		cv::Mat s_plane (size.width, size.height, CV_8UC1);
//		cv::Mat v_plane (size.width, size.height, CV_8UC1);
//
////		IplImage* back_img = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_8U, 1 );
////		IplImage* hsv = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
////		IplImage* h_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
////		IplImage* s_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
////		IplImage* v_plane = cvCreateImage( cvGetSize( frame ), 8, 1 );
////		IplImage* planes[] = { h_plane, s_plane };
//		
//		cvAbsDiff(frame, bg_img, frame);
//		
//		cvCvtColor( frame, hsv, CV_BGR2HSV );
//		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
//		
//		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
//		cvCalcHist( planes, hist, 0, 0 );
//		cvNormalizeHist( hist, 20*255 );
//		
//		cvCalcBackProject( planes, back_img, hist );
//		cvNormalizeHist( hist, 1.0 );
//#ifdef VISUAL_CHECK
//		cvShowImage( "Source", frame );
//#endif // VISUAL_CHECK
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
//		cvReleaseImage( &hsv );
//		cvReleaseImage( &h_plane );
//		cvReleaseImage( &s_plane );
//		cvReleaseImage( &v_plane );
//		//	cvReleaseImage( &hist_img );
//	}
//	return;
//}

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

void ACVideoAnalysis::computeBlobs(const cv::Mat& bg_img, int bg_thresh, int big_blob, int small_blob) {
    // do background substraction first, using either bg_img (if provided) or
    //  it will compute background image:
    // supposes 3-channels image (in whatever mode) but would work on BW images since OpenCV converts it in 3 channels
    all_blobs.clear();
    this->clearStamps();
    
#ifdef VISUAL_CHECK_GNUPLOT
    Gnuplot g1 = Gnuplot("lines");
    g1.reset_plot();
    vector<double> ci;
#endif //VISUAL_CHECK_GNUPLOT

    cv::Mat bg_img_local;
    if (!bg_img.data) {
        bg_img_local = this->computeMedianImage();
        if (!bg_img_local.data)
            cerr << "<ACVideoAnalysis::computeBlobs>: error computing median image" << endl;
    } else
        bg_img_local = bg_img;

    // reset the capture to the beginning of the video
    this->rewind();

    // initial frame
    cv::Mat frame;
    *capture >> frame;
    cv::Size size = frame.size();

    // 1 channel temporary images
    cv::Mat bitImage(size.height, size.width, CV_8UC1);
    cv::Mat bwImage(size.height, size.width, CV_8UC1);

#ifdef VISUAL_CHECK
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);

    cv::namedWindow("ORIG-BG", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("ORIG-BG", 50, 50);
    cv::namedWindow("BW", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("BW", 700, 400);
    cv::namedWindow("BLOBS", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("BLOBS", 50, 400);
#endif // VISUAL_CHECK

    for (int i = this->frameStart+1; i < this->frameStop; i++) {
        *capture >> frame;
        if (!frame.data) {
            cerr << "<ACVideoAnalysis::computeBlobs> unexpected end of video at frame " << i << endl;
            break;
        }
        cv::absdiff(frame, bg_img_local, frame);
        cv::cvtColor(frame, bwImage, CV_BGR2GRAY);
        cv::threshold(bwImage, bitImage, bg_thresh, 255, CV_THRESH_BINARY_INV);
        CBlobResult blobs;
        IplImage ipl_img = bitImage;
        blobs = CBlobResult(&ipl_img, 0, 255); // find blobs in image, still using the 1.* opencv interface
        blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob);
        // XS TODO for dancers we only stored when there was a blob
        //		if (blobs.GetNumBlobs() > 0){
        all_blobs.push_back(blobs);
        this->stamp();
        //		}
#ifdef VISUAL_CHECK
        string txt;
        std::stringstream stxt;
        stxt << capture->get(CV_CAP_PROP_POS_FRAMES) << " : " << blobs.GetNumBlobs() << "blobs";
        stxt >> txt;
        cv::putText(frame, txt, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8); // CV_RGB (0, 255, 100));
        IplImage ipl_img2 = frame;

        if (blobs.GetNumBlobs() > 0) {
            // for visual purposes only, blobs are not really "merged"
            CvRect rbox = blobs.GetBoundingBox();
#ifdef VISUAL_CHECK_GNUPLOT
            ci.push_back(blobs.Area() / (rbox.width * rbox.height));
            g1.reset_plot();
            g1.plot_x(ci, "ci");
#endif //VISUAL_CHECK_GNUPLOT

            // for visual purposes:
            for (int j = 0; j < blobs.GetNumBlobs(); j++) {
                blobs.GetBlob(j)->FillBlob(&ipl_img2, CV_RGB(255, 0, 0));
            }

            CvPoint ii = cvPoint(rbox.x, rbox.y);
            CvPoint ff = cvPoint(rbox.x + rbox.width, rbox.y + rbox.height);
            cvRectangle(&ipl_img2, ii, ff, CV_RGB(255, 255, 0), 2, 8, 0); // thickness, linetype, shift
        } else {
            cout << "no blobs for frame: " << capture->get(CV_CAP_PROP_POS_FRAMES) << endl;
        }
        //frame and ipl_img2 share the data !
        cv::imshow("ORIG-BG", frame);
        cv::imshow("BW", bwImage);
        cv::imshow("BLOBS", bitImage);
        cv::waitKey(10); // necessary to give highgui the time to show the image
#endif // VISUAL_CHECK
    }
#ifdef VISUAL_CHECK
    cv::destroyWindow("ORIG-BG");
    cv::destroyWindow("BW");
    cv::destroyWindow("BLOBS");
#endif // VISUAL_CHECK

    HAS_BLOBS = true;
}

//void ACVideoAnalysis::computeBlobsInteractively(IplImage* bg_img, bool merge_blobs, int bg_thesh, int big_blob, int small_blob){
//	all_blobs.clear();
//	all_blobs_time_stamps.clear();	// just to make sure...
//	all_blobs_frame_stamps.clear();	// just to make sure...
//
//#ifdef VISUAL_CHECK_GNUPLOT
//	Gnuplot g1 = Gnuplot("lines");
//    g1.reset_plot();
//	vector<double> ci;
//#endif //VISUAL_CHECK_GNUPLOT
//	
//	if (bg_img == 0){
//		bg_img = this->computeMedianImage();
//		if (bg_img == 0){
//			cerr << "<ACVideoAnalysis::computeBlobsInteractively>: error computing average image" << endl;
//		}
//		// reset the capture to the beginning of the video
//		this->rewind();
//	}
//	
//	int slider_big_blob = big_blob;
//	int slider_bg_thresh = bg_thesh;
//	
//	// to display image with blobs superimposed
//	IplImage *saveImage = cvCreateImage(cvSize(width,height),depth,1);
//	// font for frame and blob counter; 
//	CvFont font;
//	char str[64];
//	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
//	cvNamedWindow("ORIG-BG", CV_WINDOW_AUTOSIZE);
//	cvMoveWindow("ORIG-BG", 50, 50);
//	cvNamedWindow( "BW", CV_WINDOW_AUTOSIZE);
//	cvMoveWindow("BW", 700, 400);
//	cvNamedWindow( "BLOBS", CV_WINDOW_AUTOSIZE);
//	cvMoveWindow("BLOBS", 50, 400);	
//	cvCreateTrackbar("Biggest Blob","BLOBS", &slider_big_blob, 1000 ,0);
//	cvCreateTrackbar("Threshold","BLOBS",&slider_bg_thresh,255,0);
//	
//	IplImage* frame;
//	// 1 channel temporary images
//	IplImage* bitImage = cvCreateImage(cvSize(width,height),depth,1);
//	IplImage* bwImage = cvCreateImage(cvSize(width,height),depth,1);
//	
//	int xi,xf,yi,yf;
//	CBlobResult blobs;
//
//	for(int i = 0; i < nframes-1; i++){
//		frame = getNextFrame();
//		saveImage =cvCloneImage(frame);
//		cvAbsDiff(frame, bg_img, frame);
//		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
//		cvThreshold(bwImage, bitImage, slider_bg_thresh,255,CV_THRESH_BINARY_INV);
//		
//		// XS closing filter (to remove dips) 
//		cvDilate(bitImage, bitImage);
//		cvErode(bitImage, bitImage);
//		
//		blobs = CBlobResult( bitImage, 0, 255 );
//		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_INSIDE,  slider_big_blob );
//		all_blobs.push_back(blobs);
//		int _frame_number = capture.get(CV_CAP_PROP_POS_FRAMES);
//		all_blobs_time_stamps.push_back(_frame_number*1.0/fps); // in seconds
//		all_blobs_frame_stamps.push_back(_frame_number); // in frames
//
//		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
//		cvPutText (saveImage, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
//		
//		if (blobs.GetNumBlobs() > 0){
//			if (merge_blobs) {
//				// for visual purposes only, blobs are not really "merged"
//				CvRect rbox = blobs.GetBoundingBox();				
////#ifdef VISUAL_CHECK_GNUPLOT
////				ci.push_back( blobs.Area() / (rbox.width*rbox.height) );
////				g1.reset_plot();
////				g1.plot_x(ci,"ci");
////#endif //VISUAL_CHECK_GNUPLOT
//				
//				// for visual purposes:
//				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
//					blobs.GetBlob(j)->FillBlob(saveImage, CV_RGB(255,0,0));
//				}
//				
//				CvPoint ii = cvPoint(rbox.x,rbox.y);
//				CvPoint ff = cvPoint(rbox.x+rbox.width,rbox.y+rbox.height);
//				cvRectangle( saveImage, ii, ff, CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
//				//				cvEllipseBox( saveImage, mergedBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 
//			}
//			else {
//				for (int j = 0; j < blobs.GetNumBlobs(); j++ ) {
//					CBlob *currentBlob = blobs.GetBlob(j);
//					currentBlob->FillBlob(saveImage, CV_RGB(255,0,0));
//					xf=currentBlob->MaxX();
//					xi=currentBlob->MinX();
//					yf=currentBlob->MaxY();
//					yi=currentBlob->MinY();			
//					cvRectangle( saveImage, cvPoint(xi, yi), cvPoint (xf, yf), CV_RGB(255,255,0), 2, 8, 0); // thickness, linetype, shift
//					//					cvEllipseBox(saveImage, currentBlob->GetEllipse(), CV_RGB(255,0,255), 2,8,0); 					
//				}
//			}
//		}
//		else {
//			cout << "no blobs for frame: " << capture.get(CV_CAP_PROP_POS_FRAMES) <<endl;
//		}
//		
//		cvShowImage("ORIG-BG",frame);
//		cvShowImage("BLOBS", saveImage );					
//		cvShowImage("BW",bitImage);
//		int	c = cvWaitKey(20);
//		if( (char) c == 27 )
//			cvWaitKey(0); //break;			
//	}
//	cvReleaseImage(&bitImage);
//	cvReleaseImage(&bwImage);
//	cvReleaseImage(&saveImage);
//	cvDestroyWindow("ORIG-BG");
//	cvDestroyWindow("BW");
//	cvDestroyWindow("BLOBS");
//	HAS_BLOBS = true;
//
//}

void ACVideoAnalysis::computeBlobsUL(const cv::Mat& bg_img, bool merge_blobs, int big_blob, int small_blob) {
    // different threshold upper/lower part of the image
    all_blobs.clear();
    this->clearStamps();
    
    cv::Mat bg_img_local;
    // 3 reasons why we'd need to recompute the median
    if (!bg_img.data || threshU == 0 || threshL == 0) {
        if (!bg_img.data) {
            cout << "No bg image provided for ACVideoAnalysis::computeBlobsUL. Computing Median" << endl;
        } else {
            cout << "No bg threshold provided for ACVideoAnalysis::computeBlobsUL. Computing it from Median" << endl;
        }
        bg_img_local = this->computeMedianImage();
        if (!bg_img_local.data) {
            cerr << "<ACVideoAnalysis::computeBlobsUL>: error computing median image" << endl;
            return; // XS TODO return false
        }
    } else
        bg_img_local = bg_img;

    // reset the capture to the beginning of the video
    this->rewind();

    // initial frame
    cv::Mat frame;
    *capture >> frame;
    cv::Size size = frame.size();

    // 1 channel temporary images
    cv::Mat bitImage(size.height, size.width, CV_8UC1);
    cv::Mat bwImage(size.height, size.width, CV_8UC1);

    // XS Essai running avg
    //	IplImage* r_avg_img = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
#ifdef VISUAL_CHECK
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);

    cv::namedWindow("ORIG-BG", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("ORIG-BG", 50, 50);
    cv::namedWindow("BW", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("BW", 700, 400);
    cv::namedWindow("BLOBS", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("BLOBS", 50, 400);
#endif // VISUAL_CHECK

    int t_u = threshU * 2;
    int t_l = threshL * 2;
    int joeystar = ystar + 40;

    for (int i = this->frameStart+1; i < this->frameStop; i++) {
        *capture >> frame;
        if (!frame.data) {
            cerr << "<ACVideoAnalysis::computeBlobsInteractively> unexpected end of video at frame " << i << endl;
            break;
        }
        cv::absdiff(frame, bg_img_local, frame);
        cv::cvtColor(frame, bwImage, CV_BGR2GRAY);

        //UP
        cv::Rect roi_up(0, 0, width, joeystar);
        cv::Mat bwImage_up = bwImage(roi_up);
        cv::Mat bitImage_up = bitImage(roi_up);
        cv::threshold(bwImage_up, bitImage_up, t_u, 255, CV_THRESH_BINARY_INV);

        // LOW
        cv::Rect roi_lo(0, joeystar, width, height - joeystar);
        cv::Mat bwImage_lo = bwImage(roi_lo);
        cv::Mat bitImage_lo = bitImage(roi_lo);
        cv::threshold(bwImage_lo, bitImage_lo, t_l, 255, CV_THRESH_BINARY_INV);


        // tried this to smooth the blob, but it removes too much of the blob
        // cvMorphologyEx( bitImage, bitImage, 0, 0, CV_MOP_CLOSE, 2);
        //cvSmooth(bitImage, bitImage, CV_MEDIAN, 3, 5);

        // does not work:
        //		cvRunningAvg (bitImage, r_avg_img, 0.5);
        //		cvConvertImage(r_avg_img, bitImage);

        CBlobResult blobs;
        IplImage ipl_img = bitImage;
        blobs = CBlobResult(&ipl_img, 0, 255); // find blobs in image, still using the 1.* opencv interface
        blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob);

        // XS for dancers we only stored when there was a blob
        //		if (blobs.GetNumBlobs() > 0){
        all_blobs.push_back(blobs);
        this->stamp();
//		}
#ifdef VISUAL_CHECK
        string txt;
        std::stringstream stxt;
        stxt << capture->get(CV_CAP_PROP_POS_FRAMES) << " : " << blobs.GetNumBlobs() << "blobs";
        stxt >> txt;
        cv::putText(frame, txt, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8); // CV_RGB (0, 255, 100));
        IplImage ipl_img2 = frame;
        if (blobs.GetNumBlobs() > 0) {
            if (merge_blobs) {
                // for visual purposes only, blobs are not really "merged"
                CvRect rbox = blobs.GetBoundingBox();
                for (int j = 0; j < blobs.GetNumBlobs(); j++) {
                    blobs.GetBlob(j)->FillBlob(&ipl_img2, CV_RGB(255, 0, 0));
                }

                CvPoint ii = cvPoint(rbox.x, rbox.y);
                CvPoint ff = cvPoint(rbox.x + rbox.width, rbox.y + rbox.height);
                cvRectangle(&ipl_img2, ii, ff, CV_RGB(255, 255, 0), 2, 8, 0); // thickness, linetype, shift
            } else {
                int xi, xf, yi, yf;
                for (int j = 0; j < blobs.GetNumBlobs(); j++) {
                    CBlob *currentBlob = blobs.GetBlob(j);
                    currentBlob->FillBlob(&ipl_img2, CV_RGB(255, 0, 0));
                    xf = currentBlob->MaxX();
                    xi = currentBlob->MinX();
                    yf = currentBlob->MaxY();
                    yi = currentBlob->MinY();
                    cvRectangle(&ipl_img2, cvPoint(xi, yi), cvPoint(xf, yf), CV_RGB(255, 255, 0), 2, 8, 0); // thickness, linetype, shift
                }
            }
        } else {
            cout << "no blobs for frame: " << capture->get(CV_CAP_PROP_POS_FRAMES) << endl;
        }

        //frame and ipl_img2 share the data !
        cv::imshow("ORIG-BG", frame);
        cv::imshow("BW", bwImage);
        cv::imshow("BLOBS", bitImage);
        cv::waitKey(10); // necessary to give highgui the time to show the image
#endif // VISUAL_CHECK

    }
#ifdef VISUAL_CHECK
    cvDestroyWindow("ORIG-BG");
    cvDestroyWindow("BW");
    cvDestroyWindow("BLOBS");
#endif // VISUAL_CHECK
    HAS_BLOBS = true;
}

//void ACVideoAnalysis::computeOpticalFlow(){
//	interest_points.clear();
//
//	// from Sidi Mahmoudi
//	int win_size = 10;
//	const int MAX_COUNT = 500;
//	CvPoint2D32f* points[2] = {0,0}, *swap_points;
//	char* status = 0;
//	int count = 0;
//	int need_to_init = 0;
//	int flags = 0;
//	int add_remove_pt = 0;
//	CvPoint pt;
//#ifdef VISUAL_CHECK
//	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE );
//	cvResizeWindow( "Camera", 800, 600 );
//#endif // VISUAL_CHECK
//	IplImage* frame = 0;
//	int currentframe = 0;
//	
//	IplImage *image = cvCreateImage( cvSize(width,height),depth, 3 );
//	//	image->origin = frame->origin;
//	IplImage *grey = cvCreateImage( cvSize(width,height),depth, 1 );
//	IplImage *prev_grey = cvCreateImage( cvSize(width,height),depth, 1 );
//	IplImage *pyramid = cvCreateImage( cvSize(width,height),depth, 1 );
//	IplImage *prev_pyramid = cvCreateImage( cvSize(width,height),depth, 1 );
//	IplImage *swap_temp;
//	points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
//	points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
//	status = (char*)cvAlloc(MAX_COUNT);
//	
//	for(int ifram=0; ifram<nframes-1; ifram++) {
//		int i, k;
//		frame = getNextFrame(); // XS this could be at the end of loop
//		currentframe++;
//		if (currentframe%5==0) need_to_init = 1;
//		cvCopy( frame, image, 0 ); // XS cvclone
//		cvCvtColor( image, grey, CV_BGR2GRAY );
//		if( need_to_init ) {
//			IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
//			IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
//			double quality = 0.01;
//			double min_distance = 4;
//			count = MAX_COUNT;
//			cvGoodFeaturesToTrack( grey, eig, temp, points[1], &count,
//								  quality, min_distance, 0, 5, 0, 0.04 );
//			cvReleaseImage( &eig );
//			cvReleaseImage( &temp );
//		}
//
//		else if( count > 0 ){
//			cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
//								   points[0], points[1], count, cvSize(win_size,win_size), 3, status, 0,
//								   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
//			flags |= CV_LKFLOW_PYR_A_READY;
//			
//			CvPoint p,q;
//			
//			for( i = k = 0; i < count; i++ ) {
//				if( add_remove_pt ) {
//					double dx = pt.x - points[1][i].x;
//					double dy = pt.y - points[1][i].y;
//					if( dx*dx + dy*dy <= 25 ) {
//						add_remove_pt = 0;
//						continue;
//					}
//				}
//				
//				if( !status[i] )
//					continue;
//				
//				points[1][k++] = points[1][i];
//				p = cvPointFrom32f(points[0][i]);
//				q = cvPointFrom32f(points[1][i]);
//#ifdef VERBOSE		
//				double a = atan2( (double) p.y - q.y, (double) p.x - q.x );
//				double m = sqrt( (p.y - q.y)*(p.y - q.y) + (p.x - q.x)*(p.x - q.x) );
//#endif //VERBOSE
//				cvLine( image, p , q , CV_RGB(255,0,0), 1, 8,0);
//#ifdef VERBOSE		
////				printf("%d;%d;%d;%g;%g\n", currentframe, p.x, p.y, a, m);
//#endif //VERBOSE
//			}
//			count = k;
//			printf("%d;%d\n", currentframe, count);
//		}
//
//		need_to_init = 0;
//		if( add_remove_pt && count < MAX_COUNT ) {
//			points[1][count++] = cvPointTo32f(pt);
//			cvFindCornerSubPix( grey, points[1] + count - 1, 1,
//							   cvSize(win_size,win_size), cvSize(-1,-1),
//							   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
//			add_remove_pt = 0;
//		}
//		CV_SWAP( prev_grey, grey, swap_temp );
//		CV_SWAP( prev_pyramid, pyramid, swap_temp );
//		CV_SWAP( points[0], points[1], swap_points );
//#ifdef VISUAL_CHECK
//		cvShowImage( "Camera", image );
//		int c = cvWaitKey(10);
//		if( (char)c == 27 )
//			break;
//#endif // VISUAL_CHECK
//
//	}	
//#ifdef VISUAL_CHECK
//	cvDestroyWindow("Camera");
//#endif // VISUAL_CHECK
//
//	cvReleaseImage(&image);
//	cvReleaseImage(&grey);
//	cvReleaseImage(&prev_grey);
//	cvReleaseImage(&pyramid);
//	cvReleaseImage(&prev_pyramid);
//	// double free:	cvReleaseImage(&swap_temp);
//}

void ACVideoAnalysis::computeMergedBlobsTrajectory(float blob_dist) {
    blob_centers.clear();
    this->clearStamps();
    CBlobResult currentBlob;
    if (!HAS_BLOBS) this->computeBlobsUL();

#ifdef VISUAL_CHECK_GNUPLOT
    Gnuplot gp = Gnuplot("lines");
    gp.reset_plot();
    vector<double> x, y;
#endif //VISUAL_CHECK_GNUPLOT

    for (unsigned int i = 0; i < all_blobs.size(); i++) {
        vector<float> tmp;
        currentBlob = all_blobs[i];
        if (currentBlob.GetNumBlobs() > 0) {
            CvPoint center = currentBlob.GetCenter();
            tmp.push_back(center.x);
            tmp.push_back(center.y);
#ifdef VISUAL_CHECK_GNUPLOT
            x.push_back(center.x);
            y.push_back(center.y);
            gp.reset_plot();
            gp.plot_xy(x, y, "blob center");
#endif //VISUAL_CHECK_GNUPLOT
        } else {
            tmp.push_back(0);
            tmp.push_back(0);
        }

        blob_centers.push_back(tmp);
        this->stamp();
    }
    HAS_TRAJECTORY = true;

}

vector<blob_center> ACVideoAnalysis::getNormalizedMergedBlobsTrajectory() {
    // this assumes blob_center is 2D (x,y), which is fine in general
    vector<blob_center> normalized_blob_centers;
    for (unsigned int i = 0; i < blob_centers.size(); i++) {
        blob_center tmp;
        tmp.push_back(blob_centers[i][0] / width);
        tmp.push_back(blob_centers[i][1] / height);
        normalized_blob_centers.push_back(tmp);
    }
    return normalized_blob_centers;
}

vector<blob_center> ACVideoAnalysis::getNormalizedMergedBlobsSpeeds() {
    // this assumes blob_center is 2D (x,y), which is fine in general
    vector<blob_center> normalized_blob_speeds;
    for (unsigned int i = 0; i < blob_speeds.size(); i++) {
        blob_center tmp;
        tmp.push_back(blob_speeds[i][0] / width);
        tmp.push_back(blob_speeds[i][1] / height);
        normalized_blob_speeds.push_back(tmp);
    }
    return normalized_blob_speeds;
}

void ACVideoAnalysis::computeMergedBlobsSpeeds(float blob_dist) {
    // XS better: do this in ACMediaTimedFeatures
    if (!HAS_TRAJECTORY) this->computeMergedBlobsTrajectory(blob_dist);
    // stamps have been computed before
#ifdef VISUAL_CHECK_GNUPLOT
    Gnuplot gp = Gnuplot("lines");
    gp.reset_plot();
    vector<double> x, y;
#endif //VISUAL_CHECK_GNUPLOT
    blob_speeds.clear();
    blob_center cb_prev;
    blob_center cb = blob_centers[0];
    blob_center v_init;
    v_init.push_back(0.0);
    v_init.push_back(0.0);
    blob_speeds.push_back(v_init); // so that it has the same size for time stamps
    for (unsigned int i = 1; i < all_blobs.size(); i++) { // not i=0
        blob_center speed;
        cb_prev = cb;
        cb = blob_centers[i];
        for (unsigned j = 0; j < cb.size(); j++) { // normally 2D
            speed.push_back(fabs(cb[j] - cb_prev[j])); // fabs ?
        }
#ifdef VISUAL_CHECK_GNUPLOT
        x.push_back(cb[0] - cb_prev[0]);
        y.push_back(cb[1] - cb_prev[1]);
        gp.reset_plot();
        gp.plot_xy(x, y, "blob center");
#endif //VISUAL_CHECK_GNUPLOT

        blob_speeds.push_back(speed);
    }
}

//IplImage* ACVideoAnalysis::computeAverageImage(int nskip, int nread, int njump, string fsave) { 
//	// nskip = number of frames to skip at the beginning
//	// nread = number of frames to read in the video
//	// ncalc = number of frames to consider
//	// so that we go by steps of nread/ncalc frames
//	// saving in a file if fsave != ""
//	if (nskip + nread > nframes) {
//		cerr << " <ACVideoAnalysis::computeAverageImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
//		cerr << "nframes = " << nframes << "; nskip = " << nskip <<  "; nread = " << nread << "; njump = " << njump << endl; 
//		return 0;
//	}
//	if (nread ==0) nread = nframes-nskip;
//	if (njump == -1) njump = 50;		// default values
//	nread = (nread/njump) * (njump); // integer manipulation to avoid getting over bounds (then cvcapture starts again from beginning)
//	
//#ifdef VERBOSE
//	cout << "Average : skipping " << nskip << " frames" << endl;
//	cout << "total frames to read " << nread << endl;
//	cout << "(by jumps of) " << njump << endl;
//#endif // VERBOSE
//	
//	IplImage *frame;
//	IplImage *av_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_32F, 3);
//	int cnt = 0; 
//	
//	// to skip nskip frames:
//	int cursor = nskip;
//	capture.set(CV_CAP_PROP_POS_FRAMES, cursor); 	
//	
//#ifdef VISUAL_CHECK
//	string title = "check average";
//	cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
//	CvFont font;
//	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
//	char str[64];
//#endif // VISUAL_CHECK
//	
//	for (int i = 0; i <  nread/njump; i++) {
//		cnt++;	
//		frame = getNextFrame();
//		cvAcc (frame, av_img); // note : one can accumuate 8U into 32S (but not Add)
//#ifdef VISUAL_CHECK
//		// note : put text *after* using image
//		snprintf (str, 64, "%04d", cursor);
//		cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
//		showFrameInWindow(title.c_str(),frame);
//#endif // VISUAL_CHECK
//		cursor+=njump;	
//		if (cursor >= nframes) break; // safety
//		capture.set(CV_CAP_PROP_POS_FRAMES, cursor);
//	}
//	cvConvertScale (av_img, av_img, 1.0 / cnt);
//	IplImage *result_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
//	cvConvert (av_img, result_img);
//	
//#ifdef VISUAL_CHECK
//	cvWaitKey(0);
//	cvDestroyWindow("test");	
//#endif // VISUAL_CHECK
//	
//	if (fsave != "")
//		cvSaveImage(fsave.c_str(),result_img);// av_img);
//	return av_img;
//}

// nskip = number of frames to skip at the beginning
// nread = number of frames to read in the video
// ncalc = number of frames to consider
// so that we go by steps of nread/ncalc frames
// if fsave != "": saving the resulting median image in the file fsave
// XS TODO adapt to Start/Stop (segments) ?
cv::Mat ACVideoAnalysis::computeMedianImage(int nskip, int nread, int njump, string fsave) {
    if (nskip + nread > nframes) {
        cerr << " <ACVideoAnalysis::computeMedianImage>: not enough frames in video. reduce number of frames to skip and/or to average" << endl;
        cerr << "nframes = " << nframes << "; nskip = " << nskip << "; nread = " << nread << "; njump = " << njump << endl;
        return cv::Mat();
    }

    if (nread == 0) nread = nframes - nskip;
    if (njump == -1) njump = 50; // default values
    nread = (nread / njump) * (njump); // integer manipulation to avoid getting over bounds (then cvcapture starts again from beginning)

#ifdef VERBOSE
    cout << "Median : skipping " << nskip << " frames" << endl;
    cout << "total frames to read " << nread << endl;
    cout << "(by jumps of) " << njump << endl;
#endif // VERBOSE

    int nbins = 256;
    int histsize = nbins * height*width;
    //	int* histograms = new int[histsize] ;
    //XS make histogram template ?
    BgrPixel* histograms = new BgrPixel[histsize];
    for (int i = 0; i < histsize; i++) {
        histograms[i].b = 0;
        histograms[i].g = 0;
        histograms[i].r = 0;
    }

    cv::Mat frame;
    int cnt = 0;

    // to skip nskip frames:
    int cursor = nskip;
    capture->set(CV_CAP_PROP_POS_FRAMES, cursor);

#ifdef VISUAL_CHECK
    string title = "check median";
    cv::namedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 2;
    int thickness = 3;
    cv::Point textOrg(10, 20);
#endif // VISUAL_CHECK
    for (int i = 0; i < nread / njump; i++) {
        cnt++;
        *capture >> frame;
        if (!frame.data) {
            cerr << "<ACVideoAnalysis::computeMedianImage> unexpected end of video" << endl;
            break;
        }

        // at each location, count how many pixels of each level
        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                histograms [(r * width + c) * nbins + frame.at<cv::Vec3b > (r, c)[0]].b++; // xs missing /256 * nbins
                histograms [(r * width + c) * nbins + frame.at<cv::Vec3b > (r, c)[1]].g++;
                histograms [(r * width + c) * nbins + frame.at<cv::Vec3b > (r, c)[2]].r++;
            }
        }
#ifdef VISUAL_CHECK
        string txt;
        std::stringstream stxt;
        stxt << cursor;
        stxt >> txt;
        putText(frame, txt, textOrg, fontFace, fontScale,
                CV_RGB(0, 255, 100), thickness, 8); // CV_RGB (0, 255, 100));
        showFrameInWindow(title.c_str(), frame);
#endif // VISUAL_CHECK

        cursor += njump;
        if (cursor >= nframes) break; // safety
        capture->set(CV_CAP_PROP_POS_FRAMES, cursor);
    }
#ifdef VISUAL_CHECK
    cv::destroyWindow(title.c_str());
#endif // VISUAL_CHECK

    cout << "finished computing for " << cnt << " images. transfering data" << endl;

    float mid = cnt / 2; // *nbins

    // output median image
    cv::Mat result_frame(height, width, CV_8UC3);

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            BgrPixel accum;
            accum.b = 0;
            accum.g = 0;
            accum.r = 0;
            BgrPixel median;
            median.b = 0;
            median.g = 0;
            median.r = 0;
            for (int i = 0; i < nbins; i++) {
                accum.b += histograms [(r * width + c) * nbins + i].b;
                accum.g += histograms [(r * width + c) * nbins + i].g;
                accum.r += histograms [(r * width + c) * nbins + i].r;
                if (accum.b > mid && median.b == 0) median.b = i;
                if (accum.g > mid && median.g == 0) median.g = i;
                if (accum.r > mid && median.r == 0) median.r = i;
            }
            result_frame.at<cv::Vec3b > (r, c)[0] = median.b; // B
            result_frame.at<cv::Vec3b > (r, c)[1] = median.g; // G
            result_frame.at<cv::Vec3b > (r, c)[2] = median.r; // R
        }
    }

#ifdef VISUAL_CHECK
    cv::imshow(title.c_str(), result_frame);
    cvWaitKey(0);
#endif // VISUAL_CHECK

    // XS to get separate thresholds -- this is dancers-specific
    cv::Scalar sum_median;
    sum_median = cv::sum(result_frame);
    for (int j = 0; j < 3; j++) {
        cout << "sum over median: " << j << " = " << sum_median.val[j] / (height * width) << endl;
    }

    cv::Rect roi_up(0, 0, width, ystar);
    //point a cv::Mat header at it (no allocation is done)
    cv::Mat result_frame_roi_up = result_frame(roi_up);
    sum_median = cv::sum(result_frame_roi_up);
    threshU = 0;
    for (int j = 0; j < 3; j++) {
        int v = sum_median.val[j] / (ystar * width);
        cout << "sum over median -- up: " << j << " = " << v << endl;
        threshU += v;
    }
    threshU /= 3;
    cout << "threshU = " << threshU << endl;

    cout << 0 << "- " << ystar << " - " << width << " - " << height << endl;
    cout << result_frame.cols << "-" << result_frame.rows << endl;

    cv::Rect roi_lo(0, ystar, width, height - ystar);
    //point a cv::Mat header at it (no allocation is done)
    cv::Mat result_frame_roi_lo = result_frame(roi_lo);
    sum_median = cv::sum(result_frame_roi_lo);
    this->threshL = 0;
    for (int j = 0; j < 3; j++) {
        int v = sum_median.val[j] / ((height - ystar) * width);
        cout << "sum over median -- low: " << j << " = " << v << endl;
        this->threshL += v;
    }
    threshL /= 3;
    cout << "threshL = " << threshL << endl;

    //	cv::setImageROI(result_frame, cvRect(0,0,width,height));	// not necessary
    // END XS

    if (fsave != "") {
        cout << "writing median image to: " << fsave.c_str() << endl;
        cv::imwrite(fsave.c_str(), result_frame);
    }
#ifdef VISUAL_CHECK
    cv::destroyWindow(title.c_str());
#endif // VISUAL_CHECK

    delete histograms;

    return result_frame;
}

//IplImage* ACVideoAnalysis::computeMedianNoBlobImage(string fsave, IplImage *first_guess){
//	// only on frames that have no blobs
//	if (all_blobs_time_stamps.size()==0 || all_blobs.size()==0) {
//		// XS debug
//		threshU = 3;
//		threshL = 14;
//		// XS end
//		// XS blobs instead of blobsUL for the moment (migration 2.*)
//		computeBlobs(first_guess); //UL(first_guess); // could be 0, in which case aprox median is calculated
//	}
//	if (all_blobs_time_stamps.size() != all_blobs.size()) {
//		cerr << "<ACVideoAnalysis::computeMedianNoBlobImage> : time stamp problem" << endl;
//		return 0;
//	}
//	
//	int nbins = 256;
//	int histsize = nbins*height*width;
//	//	int* histograms = new int[histsize] ;
//	//XS make histogram template ?
//	BgrPixel* histograms = new BgrPixel[histsize] ;
//	for (int i=0; i<histsize; i++) {
//		histograms[i].b=0;
//		histograms[i].g=0;
//		histograms[i].r=0;
//	}
//	
//	IplImage *frame;
//	int cnt = 0; 
//	
//	int cursor;
//	
//#ifdef VISUAL_CHECK
//	cvNamedWindow("NOBLOBS", CV_WINDOW_AUTOSIZE);
//	cvMoveWindow("NOBLOBS", 50, 400);	
//#endif // VISUAL_CHECK
//	
//	for (unsigned int i=0;i<all_blobs.size();i++){
//		if (all_blobs[i].GetNumBlobs() == 0){
//			cnt++;
//			cursor = all_blobs_time_stamps[i];
//			capture.set(CV_CAP_PROP_POS_FRAMES, cursor); 	
//			frame = getNextFrame();
//#ifdef VISUAL_CHECK
//			cvShowImage("NOBLOBS", frame );		
//			cvWaitKey(0);
//#endif // VISUAL_CHECK
//			BgrImage bgr_image(frame); // XS new ?
//			for (int r=0; r<height; r++){
//				for (int c=0; c<width; c++){
//					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].b)].b ++; // xs missing /256 * nbins
//					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].g)].g ++; 
//					histograms [(r*width+c)*nbins+ int(bgr_image[r][c].r)].r ++; 
//				}
//			}
//		}
//	}
//#ifdef VISUAL_CHECK
//	cvDestroyWindow("NOBLOBS");
//#endif // VISUAL_CHECK
//	
//	float mid=cnt/2; // *nbins
//	
//	// output median image
//	IplImage *result_frame = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
//	
//	for (int r=0; r<height; r++){
//		for (int c=0; c<width; c++){
//			BgrPixel accum;
//			accum.b=0;
//			accum.g=0;
//			accum.r=0;
//			BgrPixel median;
//			median.b=0;
//			median.g=0;
//			median.r=0;
//			for (int i=0; i<nbins; i++){
//				accum.b+=histograms [(r*width+c)*nbins+i].b;
//				accum.g+=histograms [(r*width+c)*nbins+i].g;
//				accum.r+=histograms [(r*width+c)*nbins+i].r;
//				if (accum.b > mid && median.b ==0) median.b = i;
//				if (accum.g > mid && median.g ==0) median.g = i;
//				if (accum.r > mid && median.r ==0) median.r = i;
//			}
//			CvScalar s;
//			s.val[0] = median.b; // B
//			s.val[1] = median.g; // G
//			s.val[2] = median.r; // R
//			cvSet2D(result_frame,r,c,s);
//		}
//	}
//	if (fsave != ""){
//		cvSaveImage(fsave.c_str(), result_frame);
//	}
//	
//	delete histograms;
//	
//	return result_frame;
//	
//}

// substracts each image with blob from the previous one and sums all pixels of the difference image
// XS: could add option to calculate just on a segment of the video

void ACVideoAnalysis::computeBlobPixelSpeed() {
    blob_pixel_speeds.clear();
    this->clearStamps();
    this->rewind();

    if (nframes < 2) {
        cerr << "<ACVideoAnalysis::computeBlobPixelSpeed> : not enough frames in video : " << file_name << endl;
        return;
    }

    // initial frame
    cv::Mat tmp_frame;
    *capture >> tmp_frame;
    cv::Size size = tmp_frame.size();
    float speed = 0.0;
    blob_pixel_speeds.push_back(speed); // so that the blob_pixel_speeds vector has the same lenght as the blob_time_stamps
    this->stamp();
    
    cv::Mat previous_frame(size.height, size.width, CV_8UC3);
    cv::Mat diff_frames(size.height, size.width, CV_8UC3);

    cv::Scalar sum_diff_frames;

#ifdef VISUAL_CHECK
    cvNamedWindow("Input", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Subtraction", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK

    // same loop as in computeGlobalPixelsSpeed but restricted to frames with blobs
    for (unsigned int i = 1; i < all_blobs.size(); i++) {
        tmp_frame.copyTo(previous_frame);
        *capture >> tmp_frame;
        if (!tmp_frame.data) {
            cerr << " <ACVideoAnalysis::computeGlobalPixelsSpeed> unexpected end of file at frame " << i << endl;
            break;
        } else {
            this->stamp();
            cv::absdiff(tmp_frame, previous_frame, diff_frames);
            sum_diff_frames = cv::sum(diff_frames);
            speed = 0.0;
            for (int j = 0; j < 3; j++) {
                speed += sum_diff_frames.val[j];
            }
            speed = speed / (4 * width * height);
            blob_pixel_speeds.push_back(speed);

#ifdef VERBOSE
            cout << "frame " << i << ": pixel speed = " << speed << endl;
#endif // VERBOSE

#ifdef VISUAL_CHECK
            cv::imshow("Input", tmp_frame);
            cv::imshow("Subtraction", diff_frames);
            cvWaitKey(10);
#endif //VISUAL_CHECK
        }
    }

#ifdef VISUAL_CHECK
    cvDestroyWindow("Input");
    cvDestroyWindow("Substraction");
#endif //VISUAL_CHECK
}

// substracts each image from the previous one and sums all pixels of the difference image
// XS: could add option to calculate just on a segment of the video

void ACVideoAnalysis::computeGlobalPixelsSpeed() {
    global_pixel_speeds.clear();
    this->clearStamps();
    if (nframes < 2) {
        cerr << "<ACVideoAnalysis::computeGlobalPixelsSpeed> : not enough frames in video : " << file_name << endl;
        return;
    }
    this->rewind();
    cv::Mat tmp_frame;
    CvScalar sum_diff_frames;
    float speed = 0.0;

    // XS NOTE : this should work for all architectures
    // if not, use ffmpeg directly (see #else below)

    //#ifdef APPLE_LEOPARD
    // initial frame
    *capture >> tmp_frame;

    //	#else // heavy, opening the file again !
    //	ACFFmpegToOpenCV file_cap;
    //	int i = 0;
    //	file_cap.init(file_name.c_str());
    //	tmp_frame = cvCreateImage(cvSize(file_cap.nCols,file_cap.nRows),IPL_DEPTH_8U,3);
    //	file_cap.getframe(&tmp_frame);
    //	#endif

    global_pixel_speeds.push_back(speed); // so that the global_pixel_speeds vector has the same lenght as the time_stamps (starting with 0)
    this->stamp();
    
    cv::Size size = tmp_frame.size();
    cv::Mat previous_frame(size.height, size.width, CV_8UC3);
    cv::Mat diff_frames(size.height, size.width, CV_8UC3);

#ifdef VISUAL_CHECK
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);
    cv::namedWindow("Input", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Subtraction", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK

    for (int i = this->frameStart+1; i < this->frameStop; i++) {
        tmp_frame.copyTo(previous_frame);

        //		#ifdef APPLE_LEOPARD
        //		#else
        //		file_cap.getframe(&tmp_frame);
        //		#endif
        *capture >> tmp_frame;
        if (!tmp_frame.data) {
            cerr << " <ACVideoAnalysis::computeGlobalPixelsSpeed> unexpected end of file at frame " << i << "out of " << nframes << endl;
            continue; // or break ?
        } else {
            this->stamp();
            cv::absdiff(tmp_frame, previous_frame, diff_frames);
            sum_diff_frames = cv::sum(diff_frames);
            speed = 0.0;
            // XS we could use time stamp difference to calculate speed
            // but if no frames are missed it is always 1
            for (int j = 0; j < 3; j++) {
                speed += sum_diff_frames.val[j];
            }
            speed = speed / (4 * width * height);

            global_pixel_speeds.push_back(speed);
            // XS TEST to recuperate missing frames
  //          cvWaitKey(20);
#ifdef VERBOSE
            cout << "frame " << i << "(" << capture->get(CV_CAP_PROP_POS_FRAMES) << ") : pixel speed = " << speed << endl;
#endif // VERBOSE

#ifdef VISUAL_CHECK
            string txt;
            std::stringstream stxt;
            stxt.precision(2);
            stxt << i << " : " << speed;
            txt = stxt.str();
            cv::putText(diff_frames, txt, textOrg, fontFace, fontScale, CV_RGB(0, 255, 100), thickness, 8); // CV_RGB (0, 255, 100));
            cv::imshow("Input", tmp_frame);
            cv::imshow("Subtraction", diff_frames);
            cv::waitKey(10);
#endif //VISUAL_CHECK
        }
    }

#ifdef VISUAL_CHECK
    cv::destroyWindow("Input");
    cv::destroyWindow("Subtraction");
#endif //VISUAL_CHECK

    //#ifndef APPLE_LEOPARD
    //file_cap.closeit();
    //#endif
}

// still in opencv1.* syntax because of blobslib

void ACVideoAnalysis::computeContractionIndices() {
    contraction_indices.clear();
    // time stamps have been computed before...
    CvRect bbox;
    float ci;
    for (unsigned int i = 0; i < all_blobs.size(); i++) {
        if (all_blobs[i].GetNumBlobs() > 0) {
            bbox = all_blobs[i].GetBoundingBox();
            ci = all_blobs[i].Area() / (bbox.width * bbox.height);
            contraction_indices.push_back(ci);
        } else
            contraction_indices.push_back(0);
    }
}

void ACVideoAnalysis::computeBoundingBoxRatios() {
    if (!HAS_TRAJECTORY) this->computeMergedBlobsTrajectory();
    bounding_box_ratios.clear();
    bounding_box_heights.clear();
    bounding_box_widths.clear();
    // time stamps have been computed before...
    CvRect bbox;
    float bd;
    for (unsigned int i = 0; i < all_blobs.size(); i++) {
        if (all_blobs[i].GetNumBlobs() > 0) {
            bbox = all_blobs[i].GetBoundingBox();
            bd = all_blobs[i].Area() / (bbox.width * bbox.width); // x/y * n/(xy) --- check this
            bounding_box_ratios.push_back(bd);
            bounding_box_heights.push_back(bbox.height);
            bounding_box_widths.push_back(bbox.width);
        } else {
            bounding_box_ratios.push_back(0);
            bounding_box_heights.push_back(0);
            bounding_box_widths.push_back(0);
        }
    }
}

void ACVideoAnalysis::computeColorMoments(int n, string cm) {
    color_moments.clear();
    this->rewind(); // reset the capture to the beginning of the video
    this->clearStamps();
#ifdef VISUAL_CHECK
    cv::namedWindow("Color Image", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK

    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < this->frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::computeColorMoments> unexpected end of file at frame " << ifram << endl;
            break;
        }

        this->stamp();
        ACColorImageAnalysis color_frame(current_frame);
        // XS TODO check if this worked
        color_frame.computeColorMoments();

#ifdef VISUAL_CHECK
        color_frame.showInWindow("Color Image");
        cvWaitKey(10);
#endif //VISUAL_CHECK

        color_moments.push_back(color_frame.getColorMoments());
    }
#ifdef VISUAL_CHECK
    cv::destroyWindow("Color Image");
#endif //VISUAL_CHECK

}

// computes Raw moments for the same price

void ACVideoAnalysis::computeHuMoments(int thresh, cv::Mat bg_img) {
    hu_moments.clear();
    raw_moments.clear();
    this->rewind(); // reset the capture to the beginning of the video
    this->clearStamps();
#ifdef VISUAL_CHECK
    cv::namedWindow("Thresh", CV_WINDOW_AUTOSIZE);
#endif //VISUAL_CHECK

    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < this->frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::computeHuMoments> unexpected end of file at frame " << ifram << endl;
            break;
        }
        this->stamp();
        if (bg_img.data) cv::absdiff(current_frame, bg_img, current_frame);
        ACColorImageAnalysis color_frame(current_frame);
        color_frame.computeHuMoments(thresh);

#ifdef VISUAL_CHECK
        color_frame.showInWindow("Thresh");
        cvWaitKey(10);
#endif //VISUAL_CHECK

        raw_moments.push_back(color_frame.getRawMoments());
        hu_moments.push_back(color_frame.getHuMoments());
    }
#ifdef VISUAL_CHECK
    cv::destroyWindow("Thresh");
#endif //VISUAL_CHECK
}

void ACVideoAnalysis::computeFourierPolarMoments(int RadialBins, int AngularBins) {
    fourier_polar_moments.clear();
    this->rewind(); // reset the capture to the beginning of the video
    this->clearStamps();
    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::computeFourierPolarMoments> unexpected end of file at frame " << ifram << endl;
            break;
        }
        this->stamp();
        ACColorImageAnalysis color_frame(current_frame);
        color_frame.computeFourierPolarMoments(RadialBins, AngularBins);
        fourier_polar_moments.push_back(color_frame.getFourierPolarMoments());
    }
}

void ACVideoAnalysis::computeFourierMellinMoments() {
    fourier_mellin_moments.clear();
    this->rewind(); // reset the capture to the beginning of the video
    this->clearStamps();
    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < this->frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "unexpected end of file at frame " << ifram << endl;
            break;
        }
        this->stamp();
        ACColorImageAnalysis color_frame(current_frame);
        color_frame.computeFourierMellinMoments();
        fourier_mellin_moments.push_back(color_frame.getFourierMellinMoments());
    }
}

// XS TODO port 2.*
//void ACVideoAnalysis::computeImageHistograms(int w, int h){
//	image_histograms.clear();
//	this->rewind(); // reset the capture to the beginning of the video
//	
//	cv::Mat current_frame;
//	for(int ifram=0; ifram<nframes; ifram++) {
//		*capture >> current_frame;
//		if(!current_frame.data){
//			cerr << "<ACVideoAnalysis::computeImageHistograms> unexpected end of file at frame " << ifram << endl;
//			break;
//		}
//		ACColorImageAnalysis color_frame(current_frame);
//		color_frame.computeImageHistogram(w,h);
//		image_histograms.push_back(color_frame.getImageHistogram());
//	}
//}

#if CV_MIN_VERSION_REQUIRED(2,3,0)

void ACVideoAnalysis::computeGlobalOrientation() {
    global_orientations.clear();
    this->clearStamps();
    // some of these constants could be parameters...
    const int diff_threshold = 10;
    const double MHI_DURATION = 1; //(in seconds)
    const double MAX_TIME_DELTA = 0.5;
    const double MIN_TIME_DELTA = 0.05;
    const int small_motion_threshold = 100; // width + height < 100
    //	const float small_motion_percent_threshold = 0.02; // width * height * 0.05
    const int N = 4; // number of cyclic frame buffer used for motion detection
    int last = 0;

    this->rewind(); // reset the capture to the beginning of the video

    // initial frame
    cv::Mat current_frame_0;
    *capture >> current_frame_0;
    cv::Size size = current_frame_0.size();
    global_orientations.push_back(0); // first frame
    this->stamp();
    
    cv::Mat silh_mat;
    vector <cv::Mat> buf_mat;
    for (int i = 0; i < N; i++) {
        buf_mat.push_back(cv::Mat(size, CV_8UC1, cv::Scalar::all(0)));
    }

    cv::Mat mhi_mat(size, CV_32FC1, cv::Scalar::all(0)); // clear MHI at the beginning
    cv::Mat orient_mat(size, CV_32FC1);
    cv::Mat mask_mat(size, CV_8UC1);
    cv::Mat segmask_mat(size, CV_32FC1);
    vector<cv::Rect> storage_rects;

#ifdef VISUAL_CHECK
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);
    cv::namedWindow("Motion");
    cv::Mat motion_mat(size, CV_8UC3, cv::Scalar::all(0));
#endif //VISUAL_CHECK

    for (int ifram = this->frameStart+1; ifram < this->frameStop; ifram++) {
        cv::Mat current_frame;
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::computeGlobalOrientation> unexpected end of file at frame " << ifram << endl;
            break;
        }
        this->stamp();
        double timestamp = (double) clock() / CLOCKS_PER_SEC; // get current time in seconds
        int idx1 = last;
        int idx2;
        cv::Rect comp_rect_roi;
        double count;
        double angle;
        cv::Point center;
        cv::Scalar color = CV_RGB(255, 0, 0);

        cv::cvtColor(current_frame, buf_mat[last], CV_BGR2GRAY); // convert frame to grayscale

        idx2 = (last + 1) % N; // index of (last - (N-1))th frame
        last = idx2;

        silh_mat = buf_mat[idx2];
        cv::absdiff(buf_mat[idx1], buf_mat[idx2], silh_mat); // get difference between frames

        cv::threshold(silh_mat, silh_mat, diff_threshold, 1, CV_THRESH_BINARY); // and threshold it
        cv::updateMotionHistory(silh_mat, mhi_mat, timestamp, MHI_DURATION); // update MHI
        mhi_mat.convertTo(mask_mat, CV_8UC1, 255. / MHI_DURATION,
                (MHI_DURATION - timestamp)*255. / MHI_DURATION);

#ifdef VISUAL_CHECK
        motion_mat = cv::Mat::zeros(size, CV_8UC3);
        // might be a better way to do this
        // mask_mat is put as the blue channel of motion_mat
        vector<cv::Mat> mv;
        mv.push_back(mask_mat);
        mv.push_back(cv::Mat(size, CV_8UC1, cv::Scalar::all(0)));
        mv.push_back(cv::Mat(size, CV_8UC1, cv::Scalar::all(0)));
        cv::merge(mv, motion_mat);
#endif //VISUAL_CHECK	

        // calculate motion gradient orientation and valid orientation mask
        cv::calcMotionGradient(mhi_mat, mask_mat, orient_mat, MAX_TIME_DELTA, MIN_TIME_DELTA, 3);

        // segment motion: get sequence of motion components
        // segmask is marked motion components map. It is not used further
        cv::segmentMotion(mhi_mat, segmask_mat, storage_rects, timestamp, MAX_TIME_DELTA);
        // iterate through the motion components,
        float global_orientation = 0;

        double xx = 0, yy = 0;
        double cosa, sina;
        int count_angles = 0;
        for (unsigned int i = 0; i < storage_rects.size(); i++) {
            comp_rect_roi = storage_rects[i];
            if (comp_rect_roi.width + comp_rect_roi.height < small_motion_threshold) // reject very small components
                continue;

            count_angles++;

            cv::Mat silh_roi = silh_mat(comp_rect_roi);
            cv::Mat mhi_roi = mhi_mat(comp_rect_roi);
            cv::Mat orient_roi = orient_mat(comp_rect_roi);
            cv::Mat mask_roi = mask_mat(comp_rect_roi);

            // calculate orientation
            angle = cv::calcGlobalOrientation(orient_roi, mask_roi, mhi_roi, timestamp, MHI_DURATION);
            angle = (360.0 - angle) * CV_PI / 180; // adjust for images with top-left origin
            cosa = cos(angle);
            sina = sin(angle);
            // to average angles, add vectors (otherwize (0+360)/2=180 instead of 0)
            xx += cosa;
            yy += sina;
            count = cv::norm(silh_roi, CV_L1); // calculate number of points within silhouette ROI

            // check for the case of little motion
            //			if( count < comp_rect_roi.width*comp_rect_roi.height * small_motion_percent_threshold )
            //				continue;

#ifdef VISUAL_CHECK
            double magnitude = 30;
            // draw a clock with arrow indicating the direction
            center = cv::Point((comp_rect_roi.x + comp_rect_roi.width / 2),
                    (comp_rect_roi.y + comp_rect_roi.height / 2));

            cv::circle(motion_mat, center, round(magnitude * 1.2), color, 3, CV_AA, 0);
            cv::line(motion_mat, center, cv::Point(round(center.x + magnitude * cosa),
                    round(center.y - magnitude * sina)), color, 3, CV_AA, 0);
#endif //VISUAL_CHECK	
        }
        if (count_angles > 0 && !(xx == 0 && yy == 0)) // atan2(0,0) undefined, global_orientation set to 0 in this case
            global_orientation = atan2(yy, xx)*180 / CV_PI; // in degrees
        global_orientations.push_back(global_orientation);
#ifdef VISUAL_CHECK
        string txt;
        std::stringstream stxt;
        stxt.precision(3);
        stxt << ifram << " : " << global_orientation;
        txt = stxt.str();
        cv::putText(motion_mat, txt, textOrg, fontFace, fontScale, CV_RGB(0, 255, 100), thickness, 8); // CV_RGB (0, 255, 100));
        cv::imshow("Motion", motion_mat);
        if (cv::waitKey(10) >= 0)
            break;
#endif //VISUAL_CHECK	
    }

#ifdef VISUAL_CHECK
    cv::destroyWindow("Motion");
#endif //VISUAL_CHECK	
}
#endif //CV_MIN_VERSION_REQUIRED(2,3,0)

// XS TODO watch out index !!
// first moment = [0]
// use armadillo instead ?

vector<float> ACVideoAnalysis::getHuMoment(int momi) {
    if (momi < 0 || momi >= 7) {
        cerr << "no Hu moment of this order, " << momi << " not in [0,6]" << endl;
        exit(1);
    }
    vector<float> dummy;
    for (unsigned int i = 0; i < hu_moments.size(); i++) {
        dummy.push_back(hu_moments[i][momi]);
    }
    return dummy;
}

// ------------------ visual output functions -----------------

void ACVideoAnalysis::showFrameInWindow(string title, const cv::Mat& frame, bool has_win) {
    // by default has_win = true, we don't make a new window for each frame !
    if (not has_win)
        cv::namedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
    if (frame.data) {
        cv::imshow(title.c_str(), frame);
        cv::waitKey(20); // wait 20 ms, necesary to display properly.
    }
}

void ACVideoAnalysis::showInWindow(string title, bool has_win) {
    if (not has_win)
        cv::namedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);

    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);

    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < this->frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::showInWindow> unexpected end of file at frame " << ifram << endl;
            break;
        }
        string txt;
        std::stringstream stxt;
        stxt << capture->get(CV_CAP_PROP_POS_FRAMES);
        stxt >> txt;
        cv::putText(current_frame, txt, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8); // CV_RGB (0, 255, 100));
        cv::imshow(title.c_str(), current_frame);

        if (cv::waitKey(20) == '\x1b') // press ESC to pause the video, then any key to continue
            cv::waitKey(0);
    }
    cv::waitKey(0);
    cv::destroyWindow(title.c_str());
}

// FFT computed on BW image

void ACVideoAnalysis::showFFTInWindow(string title, bool has_win) {
    if (not has_win)
        cv::namedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);

    cv::Mat current_frame;
    for (int ifram = this->frameStart; ifram < this->frameStop; ifram++) {
        *capture >> current_frame;
        if (!current_frame.data) {
            cerr << "<ACVideoAnalysis::showFFTInWindow> unexpected end of file at frame " << ifram << endl;
            break;
        }
        ACColorImageAnalysis color_frame(current_frame);
        color_frame.makeBWImage();
        ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(color_frame.getBWImageMat());
        bw_helper->computeFFT2D_complex();
        bw_helper->showFFTComplexInWindow(title.c_str());
        delete bw_helper;
    }
    cv::waitKey(0);
    cv::destroyWindow(title.c_str());
}

// ----------- for "fancy" browsing

void ACVideoAnalysis::onTrackbarSlide(int pos, void* video_instance) {
    ACVideoAnalysis * cap = (ACVideoAnalysis*) video_instance;
    cap->setFramePosition(pos);
}

// browse a video with trackbar and frame numbers

void ACVideoAnalysis::browseWithTrackbarInWindow(string title, bool has_win) {
    g_slider_position = 0;
    if (not has_win)
        cv::namedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
    int fontFace = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 0.7;
    cv::Point textOrg(10, 20);
    cv::createTrackbar("Video Frame", title.c_str(), &g_slider_position, nframes, &ACVideoAnalysis::onTrackbarSlide, this);
    cv::setTrackbarPos("Video Frame", title.c_str(), 0);
    cv::Mat img;
    capture->set(CV_CAP_PROP_POS_FRAMES, 0);
    for (int i = this->frameStart; i < this->frameStop; i++) {
        *capture >> img;
        cv::setTrackbarPos("Video Frame", title.c_str(), capture->get(CV_CAP_PROP_POS_FRAMES));
        if (cv::waitKey(20) == '\x1b') // press ESC to pause the video, then any key to continue
            cv::waitKey(0);
        string txt;
        std::stringstream stxt;
        if ((!img.data) || capture->get(CV_CAP_PROP_POS_FRAMES) > nframes) break;
        stxt << capture->get(CV_CAP_PROP_POS_FRAMES);
        stxt >> txt;
        cv::putText(img, txt, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8); // CV_RGB (0, 255, 100));
        cv::imshow(title.c_str(), img);
    }
    cv::destroyWindow(title.c_str());
}

// ------------------ file output functions -----------------

//XS TODO fixme
//bool ACVideoAnalysis::saveInFile (string fileout, int nskip){
//	// CV_FOURCC('M','J','P','G')
//	bool ok = true;
//	if (this->getNumberOfFrames() <=0) ok = false;
//	CvVideoWriter* video_writer = cvCreateVideoWriter( fileout.c_str(), -1, fps, cvSize(width,height) );  // "-1" pops up a nice GUI (Windows only)
//	cv::Mat img = 0;
//	rewind();
//	for(int i = nskip; i < nframes-1; i++){
//		*capture >> current_frame;
//		if(!current_frame.data){
//			cerr << "<ACVideoAnalysis::saveInFile> unexpected end of file at frame " << ifram << endl;
//			break;
//		}
//		cvWriteFrame(video_writer, img);  
//		if( !img ) {
//			break;
//			cout << "end of movie (aka The End)" << endl;
//		}
//	}
//	cvReleaseVideoWriter(&video_writer); 
//	return ok;
//}

// saves in fileout a video (i.e., not an thumbnail image) thumnail consisting of frames from the video 
// format : CV_FOURCC('M','J','P','G')
// optionally:
//   - reduced to size wxh
//   - skips the first nskip frames
//   - by steps of istep frames
// XS TODO start & stop
void ACVideoAnalysis::writeToFile(const string& fileout, int _w, int _h, int _nskip, int _istep) {
    cv::VideoWriter video_writer(fileout, CV_FOURCC('X', 'V', 'I', 'D'), fps, cv::Size(_w, _h));
    rewind();
    for (int i = _nskip; i < nframes - 1; i += _istep) {
        cv::Mat frame = getFrame(i);
        if (!frame.data) {
            cerr << "<ACVideoAnalysis::writeToFile> unexpected end of video" << endl;
            break;
        }
        cv::Mat img_sm(cv::Size(_w, _h), CV_8UC3);
        cv::resize(frame, img_sm, cv::Size(_w, _h)); // LINEAR interpolation by default
        video_writer << img_sm;
    }
}

void ACVideoAnalysis::dumpTrajectory(ostream &odump) {
    // XS TODO check if computed
    odump << setiosflags(ios::fixed) << setprecision(3); // 0.123
    odump << showpoint;
    for (int i = 0; i < int(blob_centers.size()); i++) {
        odump << setw(10);
        odump << frame_stamps[i] << " ";
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
    for (int i = 0; i < int(contraction_indices.size()); i++) {
        odump << setw(10);
        odump << frame_stamps[i] << " ";
        odump << setw(10);
        odump << contraction_indices[i] << endl;
    };
}

void ACVideoAnalysis::dumpBoundingBoxRatios(ostream &odump) {
    // XS TODO check if computed
    odump.precision(3); // 0.123
    odump << showpoint;
    for (int i = 0; i < int(bounding_box_ratios.size()); i++) {
        odump << setw(10);
        odump << frame_stamps[i] << " ";
        odump << setw(10);
        odump << bounding_box_ratios[i] << endl;
    };
}

void ACVideoAnalysis::dumpBlobSpeed(ostream &odump) {
    // XS TODO check if computed
    odump << setiosflags(ios::fixed) << setprecision(3); // 0.123
    odump << showpoint;
    for (int i = 0; i < int(blob_speeds.size()); i++) {
        odump << setw(10);
        odump << frame_stamps[i] << " ";
        odump << setw(10);
        odump << blob_speeds[i][0] << " ";
        odump << setw(10);
        odump << blob_speeds[i][1] << endl;
    };
}

void ACVideoAnalysis::dumpRawMoments(ostream &odump) {
    // XS TODO check if computed
    // m00, m10, m01, m20, m11, m02, m30, m21, m12, m03;
    for (unsigned int i = 0; i < raw_moments.size(); i++) {
        odump << setw(10);
        odump << i << " ";
        for (unsigned int momi = 0; momi < 10; momi++) {
            odump << setw(10);
            odump << raw_moments[i][momi];
        }
        odump << endl;
    }
}

void ACVideoAnalysis::dumpHuMoments(ostream &odump) {
    // XS TODO check if computed
    for (unsigned int i = 0; i < hu_moments.size(); i++) {
        odump << setw(10);
        odump << i << " ";
        for (unsigned int momi = 0; momi < 7; momi++) {
            odump << setw(10);
            odump << hu_moments[i][momi];
        }
        odump << endl;
    }
}

void ACVideoAnalysis::dumpFourierPolarMoments(ostream &odump) {
    // XS TODO check if computed
    for (unsigned int i = 0; i < fourier_polar_moments.size(); i++) {
        odump << setw(10);
        odump << i << " ";
        for (unsigned int momi = 0; momi < fourier_polar_moments[i].size(); momi++) {
            odump << setw(10);
            odump << fourier_polar_moments[i][momi] << " ";
        }
        odump << endl;
    }
}

void ACVideoAnalysis::dumpAll(ostream &odump) {
    string fn = extractFilename(this->getFileName());
    string todor = fn.substr(0, fn.size() - 4) + "_";
    odump << setiosflags(ios::fixed) << setprecision(3); // 0.123
    odump << showpoint;
    for (int i = 0; i < int(blob_centers.size()); i++) {
        odump << todor;
        odump << frame_stamps[i] << ", ";
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
        for (unsigned int momi = 0; momi < 7; momi++) {
            odump << setw(10);
            odump << hu_moments[i][momi];
        }
        for (unsigned int momi = 0; momi < 10; momi++) {
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

string ACVideoAnalysis::extractFilename(string path) {
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\');
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(index + 1);
}

void ACVideoAnalysis::test_match_shapes(ACVideoAnalysis *V2, IplImage* bg_img) {
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
