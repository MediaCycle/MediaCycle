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
#include "ACImageAnalysis.h"
//#include "ACImagePlugin.h"

#include "gnuplot_i.hpp"

using namespace std;

string image_dir = "/Users/xavier/Pictures/bw_128/";
//string image_dir = "/Users/xavier/Pictures/101_ObjectCategories/beaver/";
string acl_dir = "/Users/xavier/Desktop/acl_tmp/";

void testshapes(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACImageAnalysis* Im1 = new ACImageAnalysis(f1);
	ACImageAnalysis* Im2 = new ACImageAnalysis(f2);
	Im1->computeHuMoments();
	Im2->computeHuMoments();
	delete Im1;
	delete Im2;
}

void testgabor(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACImageAnalysis* Im1 = new ACImageAnalysis(f1);
	ACImageAnalysis* Im2 = new ACImageAnalysis(f2);
	Im1->computeGaborMoments();
	Im2->computeGaborMoments();
	delete Im1;
	delete Im2;
}

void testgabor1(string sim1){
	string f1=sim1;
	ACImageAnalysis* Im1 = new ACImageAnalysis(f1);
	Im1->showInWindow("Fabian");
	cvWaitKey(0);
	Im1->computeGaborMoments();
	
	delete Im1;
}

void testcolor(string sim1, string sim2){
	string f1=image_dir+sim1;
	string f2=image_dir+sim2;
	ACImageAnalysis* Im1 = new ACImageAnalysis(f1);
	ACImageAnalysis* Im2 = new ACImageAnalysis(f2);
	Im1->computeColorMoments();
	Im2->computeColorMoments();
	delete Im1;
	delete Im2;
}

void testFFT(string sim1){
	string f1=image_dir+sim1;
	ACImageAnalysis* Im1 = new ACImageAnalysis(f1);
	Im1->showInWindow("image");
	Im1->FFT2D();
	Im1->showFFTInWindow("fft");
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
	mediacycle->addPlugin("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-xcode/plugins/image/Debug/libimage.dylib");
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

//		displayrect();

//		testFFT ("image_0001.jpg");
		testFFT ("small_circle.jpg");
	}
	return (EXIT_SUCCESS);
}

