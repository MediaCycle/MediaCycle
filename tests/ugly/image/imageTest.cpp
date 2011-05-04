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
	string f1=image_dir+sim1;
	ACBWImageAnalysis* Im1 = new ACBWImageAnalysis(f1);
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

void testcolor(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACColorImageAnalysis* Im1 = new ACColorImageAnalysis(f1);
	ACColorImageAnalysis* Im2 = new ACColorImageAnalysis(f2);
	Im1->computeColorMoments();
	Im2->computeColorMoments();
	delete Im1;
	delete Im2;
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

int main(int argc, char** argv) {
	cout << "Using Opencv " << CV_VERSION << "(" << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION  << "." <<  CV_SUBMINOR_VERSION << ")" << endl;	
	// testshapes("beaver/image_0001.jpg", "beaver/image_0012.jpg");
	// testgabor ("beaver/image_0001.jpg", "beaver/image_0012.jpg");
	// testcolor ("beaver/image_0001.jpg", "beaver/image_0012.jpg");

	// displayrect();
	// testgabor1("/Users/xavier/Desktop/Images-Fabian/C00428-sm.tif");
		
	if (argc==2) {
		// command-line, e.g., for cluster
		testoneimage(argv[1]);//"image_0001.jpg");
	}
	else{
//		makeNumbered("/Users/xavier/Pictures/numbered",2000);
		test_import_directory("/Volumes/Other Stuff/256_ObjectCategories/part_B", "/Users/xavier/tmp/256_B.xml");

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
	}
	return (EXIT_SUCCESS);
}

