/*
 *  imageTest.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 30/09/09
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

#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <ctime>

#include "MediaCycle.h"
#include "ACColorImageAnalysis.h"
#include "ACBWImageAnalysis.h"

//#include "ACImagePlugin.h"

#include "gnuplot_i.hpp"

using namespace std;


#include <cv.h>
#include <highgui.h>
#include <math.h>

string image_dir = "/Users/xavier/numediart/Project10.1-Borderlands/Images/";
//string image_dir = "/Users/xavier/Pictures/letters/";
//string image_dir = "/Users/xavier/Pictures/bw_tests/";
//string image_dir = "/Users/xavier/Pictures/101_ObjectCategories/beaver/";
string acl_dir = "/Users/xavier/Desktop/acl_tmp/";

void testshapes(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(f1);
	ACColorImageAnalysis* Im2 = new ACColorImageAnalysis(f2);
	Im1->computeHuMoments();
	Im2->computeHuMoments();
	delete Im1;
	delete Im2;
}

void testHuMoments(string sim1){
//	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(sim1);
	Im1->computeHuMoments();
	Im1->dumpHuMoments(cout);
	delete Im1;
}

void testgabor(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(f1);
	ACColorImageAnalysis* Im2 = new ACColorImageAnalysis(f2);
	Im1->computeGaborMoments();
	Im2->computeGaborMoments();
	delete Im1;
	delete Im2;
}

void testgabor1(string sim1){
	string f1=image_dir+sim1;
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(f1);
	Im1->showInNewWindow("Fabian");
	cvWaitKey(0);
	Im1->computeGaborMoments();
	Im1->closeNewWindow("Fabian");
	delete Im1;
}

void testcolor(string sim1){ //, string sim2){
//	string f1=image_dir+sim1;
//	string f2=image_dir+sim2;
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(sim1);
//	ACColorImageAnalysis* Im2 = new ACColorImageAnalysis(f2);
	Im1->computeColorMoments();
//	Im2->computeColorMoments();
	delete Im1;
//	delete Im2;
}

void testFFT(string sim1){
	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
	Im1->showInNewWindow("image");
	Im1->computeFFT2D_complex();
//	Im1->computeFourierPolarMoments(5, 8);
//	Im1->dumpFourierPolarMoments(cout);

	Im1->showFFTComplexInWindow("fft");
//	Im1->closeNewWindow("image");
	delete Im1;
}

void displayrect(){
	CvScalar c_low = cvScalar(0,0,255);
	CvScalar c_middle = cvScalar(0,255,255);
	CvScalar c_high = cvScalar(0,255,0);

	IplImage* im;
	im = cvCreateImage(cvSize (1200, 800), 8, 3);
	cvRectangle( im, 
				cvPoint(10,10), 
				cvPoint(30,30), 
				c_low,CV_FILLED
	); 
	cvRectangle( im, 
				cvPoint(40,10), 
				cvPoint(60,30), 
				c_middle,CV_FILLED
				); 
	cvRectangle( im, 
				cvPoint(70,10), 
				cvPoint(90,30), 
				c_high,CV_FILLED
				); 
	cvNamedWindow("rect", CV_WINDOW_AUTOSIZE);
    cvShowImage("rect", im);
	cvWaitKey(0);
    cvDestroyWindow("rect");
}

// XS for cluster:
void testoneimage(string sim1){
	string f1=image_dir+sim1;
	int tmp = 0;
    tmp = sim1.find_last_of('.');
	string f2=acl_dir+sim1.substr(0, tmp + 1)+"acl";
	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
	mediacycle->addPluginLibrary("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-xcode/plugins/image/Debug/mc_image.dylib");
	mediacycle->importDirectory(f1,0);
	mediacycle->saveACLLibrary(f2);
	delete mediacycle;
}

int makeNumbered(string sdir, int n){
	CvFont font;
	cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 1.4, 1.4);
	char str[64];
	for (int i=1; i<=n; i++){
		stringstream imfile;
		imfile << sdir << "/number" << i << ".jpg" ;		
		const char *imfile_char = imfile.str().c_str();
		IplImage* img = cvCreateImage(cvSize(128,128),IPL_DEPTH_8U,3);
		snprintf (str, 64, "%04d", i);
		cvPutText (img, str, cvPoint (10, 72), &font, CV_RGB (0, 255, 100));
		if( cvSaveImage(imfile_char, img )) {
			cout << imfile.str() << " has been written successfully." << endl;
		}
		else { 
			cout << "error writing " << imfile << endl;
			return -1;
		}		
		cvReleaseImage(&img);
	}
	return 1;
}

void test_image_class(string sim1){
	string f1=image_dir+sim1;
	IplImage* img = cvLoadImage(f1.c_str(), CV_LOAD_IMAGE_COLOR);
	cvShowImage("test", img);
	BgrImage toto(img);
	cout << "0 0 : " << float(toto[0][0].b)<< endl;
	cout << "10 10 : " <<  float(toto[10][10].b) << endl;
	cvDestroyWindow("test");
	cvReleaseImage(&img);
}

void testLogPolar(string sim1){
	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
	Im1->showLogPolarInWindow("new");
	delete Im1;
}

void testFourierMellin(string sim1){
	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
	Im1->computeFourierMellinMoments();
	delete Im1;
}

void testMakePGM(string sim1){
	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
	Im1->savePGM(f1+".pgm");
	delete Im1;
}

void test_import_directory(string dir1, string f2=""){
	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
	mediacycle->addPluginLibrary("/Users/xavier/development/mediacycle-numediart/mediacycle/Builds/mac-10.5/plugins/image/Debug/mc_image.dylib");
	mediacycle->importDirectory(dir1, 1); // 1 = recursive
	if (f2!="") mediacycle->saveXMLLibrary(f2);
	delete mediacycle;
}

void testOpenCV_2_0(string sim1){
	cout << "starting..." << endl;
	cv::Mat Real = cv::imread(sim1,CV_LOAD_IMAGE_COLOR);
	cout << "done" << endl;
	cv::namedWindow("Real", CV_WINDOW_AUTOSIZE);
	cv::imshow("Real", Real);
	cv::namedWindow("Real", CV_WINDOW_AUTOSIZE);
	cv::waitKey();
}

void testHoughLines(string sim1){
	/* This is a standalone program. Pass an image name as a first parameter
	 of the program.  Switch between standard and probabilistic Hough transform
	 by changing "#if 1" to "#if 0" and back */
	
	cv::Mat src, dst, color_dst;
	src=cv::imread(sim1, 0);	
	cv::Canny( src, dst, 70, 230, 3);
	cv::cvtColor( dst, color_dst, CV_GRAY2BGR );
	
//	vector<cv::Vec2f> lines;
//	cv::HoughLines( dst, lines, 1, CV_PI/180, 100 );	
//	for( size_t i = 0; i < lines.size(); i++ ){
//		float rho = lines[i][0];
//		float theta = lines[i][1];
//		double a = cos(theta), b = sin(theta);
//		double x0 = a*rho, y0 = b*rho;
//		cv::Point pt1(cvRound(x0 + 1000*(-b)),
//					  cvRound(y0 + 1000*(a)));
//		cv::Point pt2(cvRound(x0 - 1000*(-b)),
//					  cvRound(y0 - 1000*(a)));
//		cv::line( color_dst, pt1, pt2, cv::Scalar(0,0,255), 3, 8 );
//	}

	vector<cv::Vec4i> lines;
	cv::HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
		line( color_dst, cv::Point(lines[i][0], lines[i][1]),
			 cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 2, 8 );
	}

	cv::namedWindow( "Source", 1 );
	cv::imshow( "Source", src );
	
	cv::namedWindow( "Detected Lines", 1 );
	cv::imshow( "Detected Lines", color_dst );
	
	cv::waitKey(0);
}


// Search for objects such as faces in the image using the given parameters,
// storing the multiple cv::Rects into 'objects'.
// Can use Haar cascades or LBP cascades for Face Detection, or even eye, mouth, or car detection.
void detectObjectsCustom(IplImage *img, cv::CascadeClassifier& cascade, float scale, vector<cv::Rect> &objects, int flags, CvSize minFeatureSize, float searchScaleFactor, int minNeighbors)
{
	IplImage *gray = 0;		// Assume it's not used, so it's not freed.
	IplImage *smallImg = 0;	// Assume it's not used, so it's not freed.
	IplImage *inputImg = img;	// Use the original image by default.
	double t;
	t = (double)cvGetTickCount();	// record the timing.
	
	// If the input image is color, convert it from color to greyscale.
	if (img->nChannels >= 3)
	{
		gray = cvCreateImage( cvGetSize(img), 8, 1 );
		cvCvtColor( img, gray, CV_BGR2GRAY );
		inputImg = gray;	// use this new image.
	}
	
	// Possibly shrink the image, to run faster.
	if (scale < 0.9999f || scale > 1.0001f)
	{
		int smallWidth = cvRound(img->width/scale);
		int smallHeight = cvRound(img->height/scale);
		smallImg = cvCreateImage(cvSize(smallWidth, smallHeight), 8, 1);
		cvResize( inputImg, smallImg, CV_INTER_LINEAR );
		inputImg = smallImg;	// use this new image.
	}
	
	// Standardize the brightness and contrast, so that dark images look better.
	cvEqualizeHist( inputImg, inputImg );
	
	// Get a new OpenCV 2.0 C++ style image that references the same IplImage.
	cv::Mat img2(inputImg);
	
	// Detect objects in the small greyscale image.
	cascade.detectMultiScale( img2, objects, searchScaleFactor, minNeighbors, flags, minFeatureSize );
	
	// Resize the results if the image was temporarily scaled smaller
	if (smallImg)
	{
		vector<cv::Rect>::iterator r;
		for (r = objects.begin(); r != objects.end(); r++ )
		{
			r->x = cvRound(r->x * scale);
			r->y = cvRound(r->y * scale);
			r->width = cvRound(r->width * scale);
			r->height = cvRound(r->height * scale);
		}
	}
	
	t = (double)cvGetTickCount() - t;
	cout << "[Detected " << objects.size() << " objects in " << cvRound(t/((double)cvGetTickFrequency()*1000.0)) << " milliseconds]" << endl;
	
	// Free the C resources (but not the C++ resources).
	if (gray)
		cvReleaseImage(&gray);
	if (smallImg)
		cvReleaseImage(&smallImg);
}

// Search for many objects in the image, such as all the faces,
// storing the results into 'objects' using the C++ style cv::Rect.
// Can use Haar cascades or LBP cascades for Face Detection, or even eye, mouth, or car detection.
// For Haar detectors, detectLargestObject() should be faster than detectManyObjects().
void detectManyObjects( IplImage *img, cv::CascadeClassifier& cascade, float scale, vector<cv::Rect> &objects)
{
	// Search for many objects in the one image.
	int flags = CV_HAAR_SCALE_IMAGE;
	// Smallest object size (by default the size of the images it has been trained on)
	CvSize minFeatureSize = cvSize(20, 20);
	// How detailed should the search be. Must be larger than 1.0.
	float searchScaleFactor = 1.2;
	// How much the detections should be filtered out. This should depend on how bad false detections are to your system.
	// minNeighbors=2 means lots of good+bad detections, and minNeighbors=6 means only good detections are given but some are missed.
	int minNeighbors = 4;
	
	// Perform Object or Face Detection, looking for many objects in the one image.
	detectObjectsCustom(img, cascade, scale, objects, flags, minFeatureSize, searchScaleFactor, minNeighbors);
}

// Search for just a single object in the image, such as the largest face.
// storing the result into 'largestObject' using the C-style CvRect.
// Can use Haar cascades or LBP cascades for Face Detection, or even eye, mouth, or car detection.
// For Haar detectors, detectLargestObject() should be faster than detectManyObjects().
void detectLargestObject( IplImage *img, cv::CascadeClassifier& cascade, float scale, CvRect &largestObject)
{
	// Only search for just 1 object (the biggest in the image).
	int flags = CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH;
	// Smallest object size.
	CvSize minFeatureSize = cvSize(20, 20);
	// How detailed should the search be. Must be larger than 1.0.
	float searchScaleFactor = 1.1f;
	// How much the detections should be filtered out. This should depend on how bad false detections are to your system.
	// minNeighbors=2 means lots of good+bad detections, and minNeighbors=6 means only good detections are given but some are missed.
	int minNeighbors = 4;
	
	// Perform Object or Face Detection, looking for just 1 object (the biggest in the image).
	vector<cv::Rect> objects;
	detectObjectsCustom(img, cascade, scale, objects, flags, minFeatureSize, searchScaleFactor, minNeighbors);
	if (objects.size() > 0) {
		// Return the only detected object.
		largestObject = (CvRect)objects.at(0);
	}
	else
	{
		// Return an invalid rect.
		largestObject = cvRect(-1,-1,-1,-1);
	}
}


// Search for frontal faces in the image, drawing a rect or circle on the image
// and storing the rects into 'faces'.
// Can use Haar cascades or LBP cascades for Face Detection.
void detectAndDrawFaces( IplImage *imgIn, IplImage *imgOut, cv::CascadeClassifier& cascade, float scale, vector<cv::Rect>& faces, CvScalar color, bool showRect)
{
	int thickness = 2;
	
	// Just search for a single face.
	//CvRect rect;
	//detectLargestObject( img, cascade, scale, rect );
	//CvRect *r = &rect;
	
	// Do face detection
	detectManyObjects( imgIn, cascade, scale, faces );
	
	// Loop through each of the detected faces, to display them.
	for( vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ )
	{
		if (showRect)
		{
			// Render a rectangle
			CvPoint topleft, bottomright;
			topleft.x = r->x;
			topleft.y = r->y;
			bottomright.x = r->x + r->width - 1;
			bottomright.y = r->y + r->height - 1;
			cvRectangle( imgOut, topleft, bottomright, color, thickness, CV_AA);	// Draw anti-aliased lines
		}
		else
		{
			// Render a circle
			CvPoint center;
			int radius;
			center.x = r->x + cvRound(r->width/2.0f);
			center.y = r->y + cvRound(r->height/2.0f);
			radius = cvRound(r->width/2.0f);
			cvCircle( imgOut, center, radius, color, thickness, CV_AA);	// Draw anti-aliased circles
		}
	}  
}

int testDetectFaces(string imageFile){
	string cascadeFileHaar = "/Users/xavier/numediart/Project14.5-DiskHover/tests/haarcascade_frontalface_alt.xml";
	string cascadeFileLBP = "/Users/xavier/numediart/Project14.5-DiskHover/tests/lbpcascade_frontalface.xml";

	// How much to shrink the image before face detection.
	// A value upto 1.3f will make it run faster and detect faces almost as reliably.
	float scale = 1.0;
	
	cv::CascadeClassifier cascadeHaar;
	cv::CascadeClassifier cascadeLBP;
	vector<cv::Rect> faces;
	cout << "Loading Haar classifier cascade: " << cascadeFileHaar << endl;
	if( !cascadeHaar.load( cascadeFileHaar ) )
	{
		cerr << "ERROR: Could not load Haar classifier cascade: " << cascadeFileHaar << endl;
		return -1;
	}
	// Load the Haar Face Detection Cascade Classifier.
	cout << "Loading LBP classifier cascade: " << cascadeFileLBP << endl;
	if( !cascadeLBP.load( cascadeFileLBP ) )
	{
		cerr << "ERROR: Could not load LBP classifier cascade: " << cascadeFileLBP << endl;
		return -1;
	}

	// Create a GUI Window
	char *title = "FaceDetector";
	cvNamedWindow( title, 1 );
	
	// Load the image file.
	cout << "Loading image file: " << imageFile << endl;
	IplImage *imageOriginal = cvLoadImage( imageFile.c_str(), 1 );
	if( !imageOriginal ) {
		cerr << "ERROR: couldnt load image file: " << imageFile << endl;
		return -1;
	}
	IplImage *imageOut = cvCloneImage(imageOriginal);
	
	// Show the Haar detected faces as blue circles.
	cout << endl;
	cout << "Detecting faces in the image using the Haar-detector ..." << endl;
	detectAndDrawFaces( imageOriginal, imageOut, cascadeHaar, scale, faces, CV_RGB(0,0,255), false );

	cout << "Haar-detector found " << faces.size() << " faces in the image (blue circles)." << endl;
	
	// Show the LBP detected faces as green squares.
	cout << endl;
	cout << "Detecting faces in the image using the LBP-detector ..." << endl;
	detectAndDrawFaces( imageOriginal, imageOut, cascadeLBP, scale, faces, CV_RGB(0,255,0), true );
	cout << "LBP-detector found " << faces.size() << " faces in the image (green squares)." << endl;
	
	// Display the data
	cvShowImage( title, imageOut );
	cout << "Press any key in the window to continue ..." << endl;
	cvWaitKey(0);
	
	// Close the GUI window.
	cvDestroyWindow(title);
	cvReleaseImage(&imageOriginal);
	cvReleaseImage(&imageOut);
	
}

int main(int argc, char** argv) {
	
	
    MediaCycle* mediacycle;
    mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
    mediacycle->addPluginLibrary("/Users/ravet/Desktop/ticore/mediacycle/Builds/darwin-xcode/plugins/image/Debug//mc_image.dylib");
    mediacycle->importDirectory("/Users/ravet/Desktop/MediaCycleData/testImage1/",1);
    mediacycle->importDirectory("/Users/ravet/Desktop/MediaCycleData/testImage2/",1);
    mediacycle->saveXMLLibrary("mon_premier.xml");
    mediacycle->importDirectory("/Users/ravet/Desktop/MediaCycleData/testImage3/",1);
    mediacycle->importDirectory("/Users/ravet/Desktop/MediaCycleData/testImage4/",1);
    mediacycle->saveXMLLibrary("mon_deuxième.xml"); 
	
	return 0;
	cout << "Using Opencv " << CV_VERSION << "(" << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION  << "." <<  CV_SUBMINOR_VERSION << ")" << endl;	
	// testshapes("beaver/image_0001.jpg", "beaver/image_0012.jpg");
	// testgabor ("beaver/image_0001.jpg", "beaver/image_0012.jpg");
	// testcolor ("beaver/image_0001.jpg", "beaver/image_0012.jpg");

	// displayrect();
	// testgabor1("/Users/xavier/Desktop/Images-Fabian/C00428-sm.tif");
	for (int i=0; i<argc; i++){
		cout << argv[i] << endl;
	}
	if (argc==2) {
		//testHoughLines(argv[1]);
		testDetectFaces(argv[1]);
	}
	else if (argc==3){
		cout << "opening directory" << argv[1] << endl;
		test_import_directory(argv[1], argv[2]);
		cout << "saved file" << argv[2] << endl;
	}
	
	
	
//	displayrect();

//		cout << "-------- M ------------" << endl;
// 		testFFT ("M.png");
//		cout << "-------- Mt ------------" << endl;
//		testFFT  ("Mt.png");

//		cout << "-------- W ------------" << endl;
// 		testFourierMellin ("W.png");
//		cout << "-------- M90 ------------" << endl;
// 		testFourierMellin ("M90.png");
//		cout << "-------- W90 ------------" << endl;
// 		testFourierMellin ("W90.png");
//		cout << "-------- I ------------" << endl;
//		testFourierMellin ("I.png");
//		cout << "-------- A ------------" << endl;
//		testFourierMellin ("A.png");
//		cout << "-------- O ------------" << endl;
//		testFourierMellin ("O.png");
//		cout << "-------- Ot ------------" << endl;
// 		testFourierMellin ("Ot.png");
//		cout << "-------- Or ------------" << endl;
// 		testFourierMellin ("Or.png");
//		cout << "-------- Os ------------" << endl;
// 		testFourierMellin ("Os.png");
		
//		testFFT ("10151-100-flipV.jpg");

//		cout << "-------- 10151-100 ------------" << endl;
// 		testFourierMellin ("10151-100.png");
//		cout << "-------- 10151-250 ------------" << endl;
// 		testFourierMellin ("10151-250.png");
//		cout << "-------- 10151-450 ------------" << endl;
// 		testFourierMellin ("10151-450.png");
//		cout << "-------- 10151-500 ------------" << endl;
// 		testFourierMellin ("10151-500.png");
//		cout << "-------- 10151-600 ------------" << endl;
// 		testFourierMellin ("10151-600.png");
//		cout << "-------- 10151-750 ------------" << endl;
// 		testFourierMellin ("10151-750.png");
//		cout << "-------- 10151-950 ------------" << endl;
// 		testFourierMellin ("10151-950.png");
//		cout << "-------- 10151-1100 ------------" << endl;
// 		testFourierMellin ("10151-1100.png");
		
//		testMakePGM ("M90.png");
//		testMakePGM ("W90.png");
		
//		cout << "-------- Ot ------------" << endl;
//		testFFT ("Ot.png");
//		cout << "-------- Olittle ------------" << endl;
//		testFFT ("Olittle.png");
//		cout << "-------- Os ------------" << endl;
//		testFFT ("Os.png");
//		cout << "-------- I ------------" << endl;
//		testFFT("I.png");

//		testFFT("lena_bw.png");
//		testFFT("10151_F288.png");
//		testHuMoments("A.png");
//		testHuMoments("I.png");
//		testHuMoments("O.png");
//		testHuMoments("O2.png");
//		testHuMoments("Ot.png");
//		testHuMoments("Os.png");
//		testHuMoments("M.png");
//	}
	return (EXIT_SUCCESS);
}

