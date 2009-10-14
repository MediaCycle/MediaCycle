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

// XS tmp test
#include "gnuplot_i.hpp"

// ----------- uncomment this to get visual display using highgui and verbose -----
#define VISUAL_CHECK
//#define VERBOSE
// -----------

ACVideoAnalysis::ACVideoAnalysis(){
	clean();
}

ACVideoAnalysis::ACVideoAnalysis(std::string filename){
	clean();
	setFileName(filename);
}

void ACVideoAnalysis::clean(){
	if (capture) cvReleaseCapture(&capture);
	HAS_TRAJECTORY = false;
	frame_counter = 0;
	//	averageHistogram = 0;
}

void ACVideoAnalysis::rewind(){
	if (capture) cvReleaseCapture(&capture);
	capture = cvCreateFileCapture(file_name.c_str()); 
}

ACVideoAnalysis::~ACVideoAnalysis(){
	// note: the image captured by the device is allocated /released by the capture function. 
	// There is no need to release it explicitly. only release capture
	
	if (capture) cvReleaseCapture(&capture);
	//	if (averageHistogram) delete averageHistogram;
}

void ACVideoAnalysis::setFileName(std::string filename){
	file_name=filename;
}

int ACVideoAnalysis::initialize(){
	// returns 1 if it worked, 0 if not
	capture = cvCreateFileCapture(file_name.c_str());		
	frame_counter = 0; // reset frame counter, to make sure
	if( !capture ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		std::cerr << "<ACVideoAnalysis::initialize> Could not initialize capturing from file " << file_name << "..." << std::endl;
		return 0;
	}
	
	// Get capture device properties; 
	width   = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	height  = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	fps     = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	nframes = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	// extra, not stored in capture
	depth = IPL_DEPTH_8U; // XS TODO: adapt if necessary ? check with first frame?
	
#ifdef VERBOSE
	cout << "width : " << width << endl;
	cout << "height : " << height << endl;
	cout << "fps : " << fps << endl;
	cout << "numFrames : " << numFrames << endl;
#endif // VERBOSE
	int init_ok = 1;
	if (width*height == 0){ 
		std::cerr << "<ACVideoAnalysis::initialize> : zero image size for " << file_name << std::endl;
		init_ok = 0;
	}
	if (nframes == 0){ 
		std::cerr << "<ACVideoAnalysis::initialize> : zero frames for " << file_name << std::endl;
		init_ok = 0;
	}
	// test fps ? does not really matter if badly encoded...  
		
	return init_ok; // to be consistent with MycolorImage::SetImageFile : returns 0 if works
}
	
IplImage* ACVideoAnalysis::getNextFrame(){
	IplImage* tmp = 0; 
	if(!cvGrabFrame(capture)){              // capture a frame 
		std::cerr << "<ACVideoAnalysis::getNextFrame> Could not find frame..." << std::endl;
		return NULL;
	}
	tmp = cvRetrieveFrame(capture);           // retrieve the captured frame
	frame_counter++;
	return tmp;
}

void ACVideoAnalysis::computeBlobs(IplImage* bg_img, int bg_thresh, int big_blob, int small_blob){
	// do background substraction first, using either bg_img (if provided) or
	//  it will compute background image:
	// supposes 3-channels image (in whatever mode) but would work on BW images since OpenCV converts it in 3 channels
	all_blobs.clear();
	
	if (bg_img == NULL){
		bg_img = this->computeAverageImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::computeBlobs>: error computing average image" << std::endl;
		}
		// reset the capture to the beginning of the video
		this->clean();
		this->initialize();
	}
	
	// initial frame
	IplImage* frame = getNextFrame();
	frame = getNextFrame();
	
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	IplImage* bwbgImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	cvCvtColor(frame, bwImage,CV_BGR2GRAY);
	cvCvtColor(bg_img, bwbgImage,CV_BGR2GRAY);
	
	int xi,xf,yi,yf;
	for(int i = 1; i < nframes-1; i++){
		frame = getNextFrame();
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvAbsDiff(bwImage, bwbgImage, bwImage);
		cvThreshold(bwImage, bitImage, bg_thresh,255,CV_THRESH_BINARY_INV);
		CBlobResult blobs;
		blobs = CBlobResult( bitImage, NULL, 255 ); // find blobs in image
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, big_blob );
		all_blobs.push_back(blobs);
	}
	cvReleaseImage(&bitImage);
	cvReleaseImage(&bwbgImage);
	cvReleaseImage(&bwImage);
}

void ACVideoAnalysis::computeBlobsInteractively(IplImage* bg_img, bool merge_blobs, int bg_thesh, int big_blob, int small_blob){
	all_blobs.clear();
	
	//XS gnuplot
//	Gnuplot g1 = Gnuplot("lines");
//    g1.reset_plot();
//	std::vector<double> dareas;
	
	if (bg_img == NULL){
		bg_img = this->computeAverageImage();
		if (bg_img == NULL){
			std::cerr << "<ACVideoAnalysis::detectBlobs>: error computing average image" << std::endl;
		}
		// reset the capture to the beginning of the video
		this->clean();
		this->initialize();
	}
	
	int slider_big_blob = big_blob;
	int slider_bg_thresh = bg_thesh;
	
	// initial frame
	IplImage* frame = getNextFrame();
	frame = getNextFrame();
	
	// to display image with blobs superimposed
	IplImage *saveImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	// font for frame and blob counter; 
	CvFont font;
	char str[64];
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
	cvNamedWindow("ORIG", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("ORIG", 50, 50);
	cvNamedWindow( "BW", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BW", 700, 400);
	cvNamedWindow( "BLOBS", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("BLOBS", 50, 400);	
	cvCreateTrackbar("Biggest Blob","BLOBS", &slider_big_blob, 1000 ,NULL);
	cvCreateTrackbar("Threshold","BLOBS",&slider_bg_thresh,255,NULL);
	cvShowImage("ORIG",frame);
	
	// 1 channel temporary images
	IplImage* bitImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	IplImage* bwbgImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	IplImage* bwImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	cvCvtColor(frame, bwImage,CV_BGR2GRAY);
	cvCvtColor(bg_img, bwbgImage,CV_BGR2GRAY);
	
	int xi,xf,yi,yf;
	
	//XS debug
	// char* fout = "/Users/xavier/Desktop/test_blobs.out";
	
	for(int i = 1; i < nframes-1; i++){
		frame = getNextFrame();
		cvCvtColor(frame, bwImage,CV_BGR2GRAY);
		cvAbsDiff(bwImage,bwbgImage,bwImage);
		
		cvThreshold(bwImage, bitImage, slider_bg_thresh,255,CV_THRESH_BINARY_INV);
		CBlobResult blobs;
		
		// find blobs in image
		blobs = CBlobResult( bitImage, NULL, 255 );
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS,  slider_big_blob );
		all_blobs.push_back(blobs);
		
		saveImage =cvCloneImage(frame);
		snprintf (str, 64, "[%03d] : %03d blobs", i, blobs.GetNumBlobs());
		cvPutText (saveImage, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));

		if (blobs.GetNumBlobs() > 0){
			if (merge_blobs) {
				// XS : blobs not really merge but get stats 
				// on the whole list of blobs (in CBlobResult)
				CvRect rbox = blobs.GetBoundingBox();				

//				g1.reset_plot();
//				g1.plot_x(dareas,"areas");
				
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
		cvShowImage("ORIG",frame);
		cvShowImage("BLOBS", saveImage );					
		cvShowImage("BW",bitImage);
		int	c = cvWaitKey(20);
		if( (char) c == 27 )
			cvWaitKey(0); //break;			
	}
	cvReleaseImage(&bitImage);
	cvReleaseImage(&bwbgImage);
	cvReleaseImage(&bwImage);
	cvReleaseImage(&saveImage);
	cvDestroyWindow("ORIG");
	cvDestroyWindow("BW");
	cvDestroyWindow("BLOBS");
}
	
void ACVideoAnalysis::computeMergedBlobsTrajectory(float blob_dist){
	blob_centers.clear();
	CBlobResult currentBlob;

	for (unsigned int i=0; i< all_blobs.size(); i++){
		vector<float> tmp;
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

IplImage* ACVideoAnalysis::computeAverageImage(int nskip, int nave, std::string fsave) { 
	// averaging nave frames from a video skipping the first nskip ones
	// saving in a file if fsave != ""
	
	if (nskip + nave > nframes) {
		std::cerr << "not enough frames in video. reduce number of frames to skip and/or to average" << std::endl;
		return NULL;
	}
	
	IplImage *frame = 0;
	IplImage *av_img;
	av_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_32F, 3);
	
	frame = getNextFrame();
	for (int i = 0; i < nskip; i++) {
		frame = getNextFrame();
	}
	
#ifdef VISUAL_CHECK
	cvNamedWindow("skip", CV_WINDOW_AUTOSIZE);
	showFrameInWindow("skip",frame);	
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
#endif // VISUAL_CHECK

	for (int i = 0; i < nave; i++) {
		frame = getNextFrame();
#ifdef VISUAL_CHECK
		showFrameInWindow("test",frame);
#endif // VISUAL_CHECK
		cvAcc (frame, av_img); // note : one can accumuate 8U into 32S (but not Add)
	}
	cvConvertScale (av_img, av_img, 1.0 / nave);
	IplImage *result_img = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);
	cvConvert (av_img, result_img);
	
#ifdef VISUAL_CHECK
	cvNamedWindow("Test average image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Test average image", result_img);
	cvWaitKey(0);
	cvDestroyWindow("Test average image");	
	cvDestroyWindow("test");	
	cvDestroyWindow("skip");	
#endif // VISUAL_CHECK
	if (fsave != "")
		cvSaveImage(fsave.c_str(),result_img);// av_img);
	return av_img;
}

void ACVideoAnalysis::computePixelSpeed() {
	pixel_speeds.clear();
	// substracts each image from the previous one and sums it all
	// XS: could add option to calculate just on a segment of the video
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
		file_name << std::endl;
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
		std::cout << i << ": " << ci << std::endl;
	}
}

std::vector<float> ACVideoAnalysis::getDummyTimeStamps(){
	std::vector<float> dummy;
	for (unsigned int i=0; i<blob_centers.size(); i++){
		dummy.push_back((float)i);
	}
	return dummy;
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
			std::cout << "end of movie (aka The End)" << std::endl;
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

void ACVideoAnalysis::saveInFile (std::string fileout, int nskip){
	// CV_FOURCC(‘M’,‘J’,‘P’,‘G’)
	CvVideoWriter* video_writer = cvCreateVideoWriter( fileout.c_str(), -1, fps, cvSize(width,height) );  // "-1" pops up a nice GUI 
	IplImage* img = 0;
	for(int i = nskip; i < nframes-1; i++){
		img = getNextFrame();
		cvWriteFrame(video_writer, img);  
		if( !img ) {
			break;
			std::cout << "end of movie (aka The End)" << std::endl;
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




