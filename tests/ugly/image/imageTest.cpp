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
#include "ACColorImageHistogram.h"
#include "ACBWImageAnalysis.h"

//#include "ACImagePlugin.h"

#include "gnuplot_i.hpp"

using namespace std;

#include<ACOpenCVInclude.h>

string image_dir = "/Users/xavier/numediart/Project10.1-Borderlands/Images/";
string path_to_mc_build = "/home/xavier/development/mediacycle/Builds/ubuntu_10.10/";
string path_to_mc_data = "/usr/local/share/mediacycle/data/";
//string image_file = "/Users/xavier/Pictures/artBDD/Image_BDD_Application/Cubism/426px-Mblanchard2.jpg";
string image_file = path_to_mc_data + "images/coil-100/images/obj1__00.png";
//string image_file = "/media/Macintosh HD/Users/xavier/Pictures/color_tests/lena.tif";

void testHuMoments(string sim1){
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(sim1);
	Im1->computeHuMoments();
	Im1->dumpHuMoments(cout);
	delete Im1;
}

void testGaborMoments(string sim1){
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(sim1);
	Im1->computeGaborMoments(4,5);
	Im1->dumpGaborMoments(cout);
	delete Im1;
}

void testColorMoments(string sim1){
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(sim1);
	Im1->computeColorMoments();
	Im1->dumpColorMoments(cout);
	delete Im1;
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
//void testoneimage(string sim1){
//	string f1=image_dir+sim1;
//	int tmp = 0;
//    tmp = sim1.find_last_of('.');
//	string f2=acl_dir+sim1.substr(0, tmp + 1)+"acl";
//	MediaCycle* mediacycle;
//	mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
//	mediacycle->loadPluginLibraryFromBasename("image");
//	mediacycle->importDirectory(f1,0);
//	mediacycle->saveACLLibrary(f2);
//	delete mediacycle;
//}

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

//void testLogPolar(string sim1){
//	string f1=image_dir+sim1;
//	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
//	Im1->showLogPolarInWindow("new");
//	delete Im1;
//}

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
    mediacycle->loadPluginLibraryFromBasename("image");
	mediacycle->importDirectory(dir1, 1); // 1 = recursive
	if (f2!="") mediacycle->saveXMLLibrary(f2);
	delete mediacycle;
}

// XS custom file names
void test_multiple_import_normalize(){
	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
    mediacycle->loadPluginLibraryFromBasename("image");
	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0100.png",1);
	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0105.png",1);
	mediacycle->saveXMLLibrary("/Users/xavier/tmp/f2.xml");
	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0110.png",1);
	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0115.png",1);
	mediacycle->saveXMLLibrary("/Users/xavier/tmp/f4.xml");
//	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0120.png",1);
//	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0125.png",1);
//	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0130.png",1);
//	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0135.png",1);
//	mediacycle->importDirectory("/Users/xavier/Pictures/coil-100/images/obj100__0140.png",1);	
//	mediacycle->saveXMLLibrary("/Users/xavier/tmp/f2.xml");
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
	ACColorImageAnalysis Im(sim1);
	Im.computeHoughLines();
}

void testHoughLinesP(string sim1){
	ACColorImageAnalysis Im(sim1);
	Im.computeHoughLinesP();
}

int testDetectFaces(string imageFile){
	ACColorImageAnalysis im(imageFile);
	string cascadeFileHaar = "/Users/xavier/numediart/Project14.5-DiskHover/tests/haarcascade_frontalface_alt.xml";
//	string cascadeFileLBP = "/Users/xavier/numediart/Project14.5-DiskHover/tests/lbpcascade_frontalface.xml";
	im.computeNumberOfFaces(cascadeFileHaar);
}

int test_histogram_0(string imageFile){
	cv::Mat src, hsv; 
	if( !(src=cv::imread(imageFile, 1)).data ) 
		return -1; 
	cv::cvtColor(src, hsv, CV_BGR2HSV);			
	// Quantize the hue to 30 levels 
	// and the saturation to 32 levels 
	int hbins = 30, sbins = 32; 
	int histSize[] = {hbins, sbins}; 
	// hue varies from 0 to 179, see cvtColor 
	float hranges[] = { 0, 180 }; 
	// saturation varies from 0 (black-gray-white) to 
	// 255 (pure spectrum color) 
	float sranges[] = { 0, 256 }; 
	const float* ranges[] = { hranges, sranges }; 
	cv::MatND hist; 
	// we compute the histogram from the 0-th and 1-st channels 
	int channels[] = {0, 1}; 
	cv::calcHist( &hsv, 1, channels, cv::Mat(), // do not use mask 
				 hist, 2, histSize, ranges, 
				 true, // the histogram is uniform 
				 false ); 
	double maxVal=0; 
	cv::minMaxLoc(hist, 0, &maxVal, 0, 0); 
	int scale = 10; 
	cv::Mat histImg = cv::Mat::zeros(sbins*scale, hbins*10, CV_8UC3); 
	for( int h = 0; h < hbins; h++ ) 
		for( int s = 0; s < sbins; s++ ) 
		{ 
			float binVal = hist.at<float>(h, s); 
			int intensity = cvRound(binVal*255/maxVal); 
			cv::rectangle( histImg, cv::Point(h*scale, s*scale), 
						  cv::Point( (h+1)*scale - 1, (s+1)*scale - 1), 
						  cv::Scalar::all(intensity), 
						  CV_FILLED ); 
		} 
	cv::namedWindow( "Source", 1 ); 
	cv::imshow( "Source", src ); 
	cv::namedWindow( "H-S Histogram", 1 ); 
	cv::imshow( "H-S Histogram", histImg ); 
	cv::waitKey(); 
}

void test_histogram(string imageFile){
	ACColorImageAnalysis* im = new ACColorImageAnalysis(imageFile);
	//im->showInWindow("input");
	ACColorImageHistogram* hist = new ACColorImageHistogram(im);
	hist->dump();
	hist->computeMoments(4);
	hist->showStats();
	hist->show();
	delete hist;
	delete im;
}

void test_sorted(string xml_file) {
    MediaCycle* mediacycle;
    mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
    mediacycle->readXMLConfigFile(xml_file);
//      mediacycle->getKSortedOnFeature(10, 0, 0);
    delete mediacycle;
}

void test_plugins() {
    MediaCycle* mediacycle;
    mediacycle = new MediaCycle(MEDIA_TYPE_IMAGE);
    mediacycle->loadPluginLibraryFromBasename("image");
    delete mediacycle;
}


int main(int argc, char** argv) {
	cout << "Using Opencv " << CV_VERSION << "(" << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION  << "." <<  CV_SUBMINOR_VERSION << ")" << endl;	
//        test_plugins();
//        test_sorted();
//	test_histogram(image_file);
//	test_multiple_import_normalize();
//	testColorMoments(image_file);
//	testHuMoments(image_file);
	testGaborMoments(image_file);
//	testHoughLines(image_file);
//	testHoughLinesP(image_file);
//	testDetectFaces(image_file);

//	for (int i=0; i<argc; i++){
//		cout << argv[i] << endl;
//	}
//	if (argc==2) {
//		testHoughLines(argv[1]);
//		//testDetectFaces(argv[1]);
//	}
//	else if (argc==3){
//		cout << "opening directory" << argv[1] << endl;
//		test_import_directory(argv[1], argv[2]);
//		cout << "saved file" << argv[2] << endl;
//	}
	
	return (EXIT_SUCCESS);
}

