/*
 *  videoTest.cpp
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
#include "ACVideoAnalysis.h"
#include "ACVideoDancersPlugin.h"
#include "ACVideoPixelSpeedPlugin.h"
#include "ACMediaTimedFeature.h"

#include "gnuplot_i.hpp"

using namespace std;
using namespace arma;

const int ndancers=141;
const string dancerslist[ndancers] = {
"Bru_101#1", "Bru_101#2", "Bru_102#1", "Bru_102#2", "Bru_103#1", "Bru_103#2",
"Bru_104#1", "Bru_104#2", "Bru_105#1", "Bru_105#2", "Bru_106#1", "Bru_106#2", "Bru_107#1",
"Bru_107#2", "Bru_108#1", "Bru_108#2", "Bru_109#1", "Bru_109#2", "Bru_110#1", "Bru_110#2",
"Bru_111#1", "Bru_112#2", "Bru_113#1", "Bru_113#2", "Bru_114#1", "Bru_114#2", "Bru_116#1",
"Bru_116#2", "Bru_117#1", "Bru_117#2", "Bru_118#1", "Bru_118#2", "Bru_119#1", "Bru_120#1",
"Bru_120#2", "Bru_121#1", "Bru_121#2", "Bru_122#1", "Bru_122#2", "Bru_123#1", "Bru_123#2",
"Bru_124#1", "Bru_124#2", "Bru_125#1", "Bru_125#2", "Bru_126#1", "Bru_126#2", "Bru_127#1",
"Bru_127#2", "Bru_201#1", "Bru_201#2", "Bru_202#1", "Bru_202#2", "Bru_203#1", "Bru_203#2",
"Bru_204#1", "Bru_204#2", "Bru_205#1", "Bru_205#2", "Bru_206#1", "Bru_206#2", "Bru_207#1",
"Bru_207#2", "Bru_208#1", "Bru_208#2", "Bru_209#1", "Bru_209#2", "Bru_210#1", "Bru_210#2",
"Bru_211#1", "Bru_211#2", "Bru_212#1", "Bru_212#2", "Bru_213#1", "Bru_213#2", "Bru_214#1",
"Bru_214#2", "Bru_215#1", "Bru_215#2", "Bru_216#1", "Bru_216#2", "Bru_217#1", "Bru_217#2",
"Bru_218#1", "Bru_218#2", "Bru_219#1", "Bru_219#2", "Bru_220#1", "Bru_220#2", "Bru_221#1",
"Bru_221#2", "Bru_222#1", "Bru_222#2", "Bru_223#1", "Bru_223#2", "Bru_224#1", "Bru_224#2",
"Bru_225#1", "Bru_225#2", "Bru_301#1", "Bru_301#2", "Bru_302#1", "Bru_302#2", "Bru_303#1",
"Bru_303#2", "Bru_304#1", "Bru_304#2", "Bru_305#1", "Bru_305#2", "Bru_306#1", "Bru_306#2",
"Bru_307#1", "Bru_307#2", "Bru_308#1", "Bru_308#2", "Bru_309#1", "Bru_309#2", "Bru_310#1",
"Bru_310#2", "Bru_311#1", "Bru_311#2", "Bru_312#1", "Bru_312#2", "Bru_313#1", "Bru_313#2",
"Bru_314#1", "Bru_314#2", "Bru_315#1", "Bru_315#2", "Bru_316#1", "Bru_316#2", "Bru_317#1",
"Bru_317#2", "Bru_318#1", "Bru_318#2", "Bru_319#1", "Bru_319#2", "Bru_320#1", "Bru_320#2",
"Bru_321#1", "Bru_321#2"};

// ad hoc: the mean of some videos (where the dancer did not move much) have a ghost shadow
// that is later detected as a blob. To avoid this problem, the "median-noblob" of these
// videos is calculated using the median of 101#1

const string ghostlist[ndancers] = {
"Bru_104#1", "Bru_105#1", "Bru_111#1", "Bru_112#2", "Bru_118#1", "Bru_120#1",
"Bru_211#1", "Bru_218#1", "Bru_218#2", "Bru_224#1", "Bru_302#2", "Bru_313#2", "Bru_320#1"
};

const string videodir = "/Users/xavier/numediart/Project7.3-DancersCycle/VideosSmall/TestSmallSize/Front/";
//const string videodir = "/Users/xavier/numediart/Project10.1-Borderlands/2010_4_prox_alpa/";

// const string videodir = "/Users/xavier/numediart/Project7.3-DancersCycle/Recordings_Raffinerie_0709/FrontShots/";

const string video_plugin_path = "/Users/xavier/development/workingDirectory/ticore-app/Applications/Numediart/MediaCycle/src/Builds/mac/plugins/video/Debug/";
const string audio_plugin_path = "/Users/xavier/development/workingDirectory/ticore-app/Applications/Numediart/MediaCycle/src/Builds/mac/plugins/audio/Debug/";

void get_all_images(){
//	for (int i=0;i<ndancers;i++){	
	for (int i=0;i<13;i++){	
//		string dancer = dancerslist[i];
		string dancer = ghostlist[i];
		string movie_file= videodir+"Front/"+dancer+".mov";
		string median_file_noblob= videodir+"Test/"+dancer+"_med_noblob.jpg";
//		string median_file= videodir+"median/"+dancer+"_med.jpg";
//		string average_file= videodir+"average/"+dancer+"_ave.jpg";
		cout << movie_file << endl;
		ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
		clock_t t0=clock();
//		IplImage *median_img =	V->computeMedianImage(200, 0, 50, median_file);
		string first_guess_file= videodir+"median/Bru_101#1_med.jpg";
		IplImage *first_guess_img = cvLoadImage(first_guess_file.c_str(), CV_LOAD_IMAGE_COLOR);

		IplImage *median_img =	V->computeMedianNoBlobImage(median_file_noblob, first_guess_img);
		clock_t t1=clock();
		cout<<"Median execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;
//		V->rewind();
//		IplImage *average_img =	V->computeAverageImage(200, 0, 50, average_file);
//		clock_t t2=clock();
//		cout<<"Average execution time: " << (t2-t1)/CLOCKS_PER_SEC << " s." << endl;	
		delete V;
		cvReleaseImage(&median_img);
		cvReleaseImage(&first_guess_img);
//		cvReleaseImage(&average_img);		
	}
}

void test_med_ave(std::string dancer){
	string movie_file= videodir+"H264/"+dancer+".mov";
	string median_file= videodir+"median/"+dancer+"_med-test.jpg";
	string average_file= videodir+"average/"+dancer+"_ave-test.jpg";
	cout << movie_file << endl;
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
	clock_t t0=clock();
	IplImage *median_img =	V->computeMedianImage(200, 0, 50, median_file);
	clock_t t1=clock();
	cout<<"Median execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;
	V->rewind();
	IplImage *average_img =	V->computeAverageImage(200, 0, 50,  average_file);
	clock_t t2=clock();
	cout<<"Average execution time: " << (t2-t1)/CLOCKS_PER_SEC << " s." << endl;	
	delete V;
	cvReleaseImage(&median_img);
	cvReleaseImage(&average_img);			
}

void test_med_noblob(std::string dancer){
	string movie_file= videodir+dancer+".mov";
	string median_file= videodir+dancer+"_med-test-noblob.jpg";
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
	clock_t t0=clock();
	V->computeMedianNoBlobImage(median_file);
	clock_t t1=clock();
	cout<<"Median no-blob execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;
	delete V;
}

void test_histogram_equalize(std::string dancer){
	string movie_file= videodir+"H264/"+dancer+".mov";
	string median_file= videodir+"median/"+dancer+"_med.jpg";
	cout << movie_file << endl;
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
	clock_t t0=clock();
	IplImage *imgp_bg = cvLoadImage(median_file.c_str(), CV_LOAD_IMAGE_COLOR);
	V->histogramEqualize(imgp_bg);
	clock_t t1=clock();
	cout<<"Hist Equalizer execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;	
	delete V;
	cvReleaseImage(&imgp_bg);	
}

void test_bg_substraction(std::string dancer){
	string movie_file= videodir+"H264/"+dancer+".mov";
	string median_file= videodir+"median/"+dancer+"_med.jpg";
	cout << movie_file << endl;
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
	clock_t t0=clock();
	IplImage *imgp_bg = cvLoadImage(median_file.c_str(), CV_LOAD_IMAGE_COLOR);
//	V->computeBlobsInteractively(imgp_bg, true);
	V->computeBlobsUL();

	clock_t t1=clock();
	cout<<"Bg Sub execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;	
	delete V;
	cvReleaseImage(&imgp_bg);	
}

void test_browse(std::string movie_file){
//	string movie_file= videodir+"H264/"+dancer+".mov";
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);	
	V->browseInWindow();
	delete V;
}

void test_video_dancers_plugin(std::string dancer){
	clock_t t0=clock();
	string movie_file= videodir+"Front/"+dancer+".mov";
	ACVideoDancersPlugin* P = new ACVideoDancersPlugin();
	std::vector<ACMediaFeatures*> F = P->calculate(movie_file);
	cout << "computed " << F.size() << " features" << endl;
	for (unsigned int i=0; i<F.size(); i++){
		cout << "-- Feature " << i << " : " << F[i]->getName() << endl;
		F[i]->dump();
	}
	
	// clean
	std::vector<ACMediaFeatures*>::iterator iter; 
	for (iter = F.begin(); iter != F.end(); iter++) { 
		delete *iter; 
	}
	delete P;	
	clock_t t1=clock();
	cout<<"Test Video Plugin execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;	
}

void test_video_dancers_plugin_acl_save(std::string dancer){
	string movie_file= videodir+"Front/"+dancer+".mov";
	string acl_file = videodir+"Test/"+dancer+"-cpp.acl";

	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_VIDEO);
	mediacycle->addPlugin(video_plugin_path+"mc_video.dylib");
	mediacycle->importDirectory(movie_file, 0);
	mediacycle->saveACLLibrary(acl_file);
	delete mediacycle;	
}

void test_video_pixel_speed_plugin(std::string movie_file){
	clock_t t0=clock();
	ACVideoPixelSpeedPlugin* P = new ACVideoPixelSpeedPlugin();
	bool save_timed_features = true;
	cout << "1) compute features and saves timedFeatures on disk" << endl;
	cout << "---------------------------------------------------" << endl;

	std::vector<ACMediaFeatures*> F = P->calculate(movie_file, save_timed_features);
	cout << "computed " << F.size() << " features :" << endl;
	for (unsigned int i=0; i<F.size(); i++){
		cout << "-- Feature " << i << " : " << F[i]->getName() << endl;
		F[i]->dump();
	}
	
	cout << "2) read timedFeatures from disk" << endl;
	cout << "-------------------------------" << endl;

	ACMediaTimedFeature* FT = P->getTimedFeatures();
	FT->dump();
	
	// clean up
	std::vector<ACMediaFeatures*>::iterator iter; 
	for (iter = F.begin(); iter != F.end(); iter++) { 
		delete *iter; 
	}
	delete P;	
	delete FT;
	
	clock_t t1=clock();
	cout<<"Test Video Plugin execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;	
}


void test_blobs(std::string dancer){
	string movie_file= videodir+dancer+".mov"; //+"Front/"+dancer+".mov";
	cout << movie_file << endl;
	ACVideoAnalysis* V = new ACVideoAnalysis(movie_file);
	clock_t t0=clock();
	
	// XS TODO tmp hack
	IplImage *imgp_bg = cvLoadImage("/Users/xavier/numediart/Project10.1-Borderlands/work/bg_black.png", CV_LOAD_IMAGE_COLOR);
	V->computeBlobs(imgp_bg);
	
	clock_t t1=clock();
	cout<<"Bg Sub execution time: " << (t1-t0)/CLOCKS_PER_SEC << " s." << endl;	
	delete V;
	cvReleaseImage(&imgp_bg);
}


void test_all_videos_top_front(std::string mypath){
	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_VIDEO);
	mediacycle->addPlugin(video_plugin_path+"mc_video.dylib");
	mediacycle->importDirectory(mypath, 0);
	mediacycle->saveACLLibrary(mypath+"ACL"+"dancers-test.acl");
	delete mediacycle;	
}

void test_read_write_video(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	string file_out = "/Users/xavier/tmp/toto.mov";
	V->saveVideoThumnbailInFile (file_out,640,480,0,5);
	delete V;
}

void test_global_pixel_speed(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	V->computeGlobalPixelsSpeed();
	delete V;
}

void test_video_features(std::string full_video_path, string bg_img_file=""){
	// and output to terminal for Borderlands first tests
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	IplImage *bg_img = NULL;
	if (bg_img_file != "") {
		bg_img = cvLoadImage(bg_img_file.c_str(), CV_LOAD_IMAGE_COLOR);
		V->computeBlobs(bg_img);	
	} 
	else V->computeBlobs();
	
	ofstream f("/Users/xavier/numediart/Project10.1-Borderlands/work/BL_blueNoTC-features-FFT.out");

	
	V->computeMergedBlobsTrajectory(0);
	V->computeContractionIndices();
	V->computeBoundingBoxRatios();
//	V->computeRawMoments();
	V->computeHuMoments(bg_img); // compute Raw too !

	// XS TODO computeEverything at once !
	// may be using imageanalysis...
	
	//***
//	V->computeBlobPixelSpeed(); -- weird 
	//***
//	V->computeMergedBlobsSpeeds();
//	V->dumpTrajectory(cout);
//	V->dumpContractionIndices(f);
//	V->dumpBoundingBoxRatios();
//	V->dumpBlobSpeed();
//	ofstream f("/Users/xavier/numediart/Project10.1-Borderlands/work/ALL_PROT2may_1_all_features.out");
//	V->dumpHuMoments(f);
	V->dumpAll(f);
	f.close();	
	delete V;
	cvReleaseImage(&bg_img);

}

void test_video_hu_moments(std::string full_video_path, string bg_img_file){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	IplImage *bg_img = NULL;
	if (bg_img_file != "") {
		bg_img = cvLoadImage(bg_img_file.c_str(), CV_LOAD_IMAGE_COLOR);
	} 
// XS TODO	else ... 
	V->computeHuMoments(bg_img);
//	ofstream f("/Users/xavier/numediart/Project10.1-Borderlands/work/10151_hu_moments_contour.out");

	V->dumpHuMoments(cout);
//	f.close();
	delete V;
}

void test_video_raw_moments(std::string full_video_path, string bg_img_file=""){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	V->computeHuMoments();
	ofstream f("/Users/xavier/numediart/Project10.1-Borderlands/work/10151_raw_moments.out");
	V->dumpRawMoments(f);
	f.close();
	delete V;
}

void test_optical_flow(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	V->computeOpticalFlow();
	delete V;
}

void test_video_similarity_hu(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	IplImage *bg_img = cvLoadImage("/Users/xavier/numediart/Project10.1-Borderlands/bg/bg_blue.png", CV_LOAD_IMAGE_COLOR);

	V->computeHuMoments(bg_img);
	std::vector< float > M1 = V->getHuMoment(1);
	
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(V->getDummyTimeStamps(M1.size()), M1, "test");
	fmat toto = trajectory_mtf->similarity();
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-sim-hu.txt", arma_ascii);
	delete trajectory_mtf;

	delete V;
	cvReleaseImage(&bg_img);
}

void test_video_similarity_hu(std::string full_video_path1, std::string full_video_path2){
	ACVideoAnalysis* V1 = new ACVideoAnalysis(full_video_path1);
	ACVideoAnalysis* V2 = new ACVideoAnalysis(full_video_path2);
	IplImage *bg_img = cvLoadImage("/Users/xavier/numediart/Project10.1-Borderlands/bg/bg_blue.png", CV_LOAD_IMAGE_COLOR);

	V1->computeHuMoments(bg_img,20);
	V2->computeHuMoments(bg_img,20);

	// NB first hu moment = (0), not (1)
	std::vector< vector<float> > M1 = V1->getHuMoments();
	std::vector< vector<float> > M2 = V2->getHuMoments();
	
	ACMediaTimedFeature *trajectory_mtf1 = new ACMediaTimedFeature(V1->getDummyTimeStamps(M1.size()), M1, "test");
	ACMediaTimedFeature *trajectory_mtf2 = new ACMediaTimedFeature(V2->getDummyTimeStamps(M2.size()), M2, "test");
	
	fmat toto = trajectory_mtf1->similarity(trajectory_mtf2);
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-20102-sim-hu1-nocont-all.txt", arma_ascii);
	delete trajectory_mtf1;	
	delete trajectory_mtf2;	
	
	delete V1;
	delete V2;
	cvReleaseImage(&bg_img);
}

void test_video_similarity_fft(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	V->computeFourierPolarMoments();
	std::vector< vector<float> > FFTPM = V->getFourierPolarMoments();
	//V->dumpFourierPolarMoments(cout);
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(V->getDummyTimeStamps(FFTPM.size()), FFTPM, "fft_pm");
	fmat toto = trajectory_mtf->similarity();
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-BW-sim-fft.txt", arma_ascii);
	delete trajectory_mtf;
	delete V;
}

void test_video_similarity_fft(std::string full_video_path1, std::string full_video_path2){
	ACVideoAnalysis* V1 = new ACVideoAnalysis(full_video_path1);
	ACVideoAnalysis* V2 = new ACVideoAnalysis(full_video_path2);

	V1->computeFourierPolarMoments();
	V2->computeFourierPolarMoments();
	std::vector< vector<float> > FFT1 = V1->getFourierPolarMoments();
	std::vector< vector<float> > FFT2 = V2->getFourierPolarMoments();
	
	ACMediaTimedFeature *trajectory_mtf1 = new ACMediaTimedFeature(V1->getDummyTimeStamps(FFT1.size()), FFT1, "test");
	ACMediaTimedFeature *trajectory_mtf2 = new ACMediaTimedFeature(V2->getDummyTimeStamps(FFT2.size()), FFT2, "test");
	
	//cout << "first video" << endl;
	//	this->dumpFourierPolarMoments(cout);
	//	
	//	cout << "second video" << endl;
	//	V2->dumpFourierPolarMoments(cout);
	
	fmat toto = trajectory_mtf1->similarity(trajectory_mtf2);
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-20102-sim-fft-R7-A10.txt", arma_ascii);

	delete trajectory_mtf1;	
	delete trajectory_mtf2;
	delete V1;
	delete V2;
}

void test_video_similarity_fm(std::string full_video_path1, std::string full_video_path2){
	ACVideoAnalysis* V1 = new ACVideoAnalysis(full_video_path1);
	ACVideoAnalysis* V2 = new ACVideoAnalysis(full_video_path2);

	V1->computeFourierMellinMoments();
	V2->computeFourierMellinMoments();
	std::vector< vector<float> > FFT1 = V1->getFourierMellinMoments();
	std::vector< vector<float> > FFT2 = V2->getFourierMellinMoments();
	
	ACMediaTimedFeature *trajectory_mtf1 = new ACMediaTimedFeature(V1->getDummyTimeStamps(FFT1.size()), FFT1, "test");
	ACMediaTimedFeature *trajectory_mtf2 = new ACMediaTimedFeature(V2->getDummyTimeStamps(FFT2.size()), FFT2, "test");
	
	fmat toto = trajectory_mtf1->similarity(trajectory_mtf2);
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-20102-sim-fm.txt", arma_ascii);
	delete trajectory_mtf1;	
	delete trajectory_mtf2;		
	
	delete V1;
	delete V2;
}

void test_video_similarity_histogram(std::string full_video_path1, std::string full_video_path2){
	ACVideoAnalysis* V1 = new ACVideoAnalysis(full_video_path1);
	ACVideoAnalysis* V2 = new ACVideoAnalysis(full_video_path2);

	V1->computeImageHistograms();
	V2->computeImageHistograms();
	std::vector< vector<float> > I1 = V1->getImageHistograms();
	std::vector< vector<float> > I2 = V2->getImageHistograms();
	
	ACMediaTimedFeature *trajectory_mtf1 = new ACMediaTimedFeature(V1->getDummyTimeStamps(I1.size()), I1, "test_hist");
	ACMediaTimedFeature *trajectory_mtf2 = new ACMediaTimedFeature(V2->getDummyTimeStamps(I2.size()), I2, "test_hist");

	fmat toto = trajectory_mtf1->similarity(trajectory_mtf2);
	toto.save("/Users/xavier/numediart/Project10.1-Borderlands/work/10151-20102-sim-histo.txt", arma_ascii);

	delete trajectory_mtf1;	
	delete trajectory_mtf2;		

	delete V1;
	delete V2;
}


void test_show_fft(std::string full_video_path){
	ACVideoAnalysis* V = new ACVideoAnalysis(full_video_path);
	V->showFFTInWindow("toto", false);

	delete V;

}

void test_Thomas(){
	MediaCycle* mediacycle;
	mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO);
//	mediacycle->addPlugin(video_plugin_path+"mc_audio.dylib");
	mediacycle->importACLLibrary("/Users/xavier/Desktop/ThomasIsrael-Sons/short/extraitsxaviercleans/second_try-atm+mus.acl");
	mediacycle->getLibrary()->saveSorted("/Users/xavier/Desktop/ThomasIsrael-Sons/short/extraitsxaviercleans/second_try-atm+mus_sorted.acl");
	delete mediacycle;	
	
}

int main(int argc, char** argv) {
	cout << "Using Opencv " << CV_VERSION << "(" << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION  << "." <<  CV_SUBMINOR_VERSION << ")" << endl;	

	// get_all_images();
	// test_med_noblob("001011");
	// test_med_noblob("Bru_203#1");
	//test_histogram_equalize("Bru_105#2");
	//test_bg_substraction("Bru_105#2");
	// test_bg_substraction("Bru_203#2");
	//test_browse("Bru_105#2");
	//test_read_write_video(videodir+"Front/001011.mov");
//	test_optical_flow(videodir+"Front/001011.mov");
	//test_video_dancers_plugin("001011");
	//test_video_dancers_plugin_acl_save("001011");
	//test_all_videos_top_front(videodir);
//	test_blobs("001011");

	// christian graupner videos
	//test_blobs("10151");
	test_video_pixel_speed_plugin("/Users/xavier/numediart/Project10.1-Borderlands/2010_4_prox_alpa/10151.mov");
	// 10603
//	test_video_hu_moments("/Users/xavier/numediart/Project10.1-Borderlands/2010_4rgb_alpha/10151.mov",
//						"/Users/xavier/numediart/Project10.1-Borderlands/bg/bg_blue.png");

						   
//	test_video_similarity_fft("/Users/xavier/numediart/Project10.1-Borderlands/2010_4_prox_alpa/10151.mov"); // complete_movies/BL_blueNoTC.mov");
	
//	test_show_fft("/Users/xavier/numediart/Project10.1-Borderlands/2010_4rgb_alpha/10151.mov");

//	test_video_similarity_histogram("/Users/xavier/numediart/Project10.1-Borderlands/2010_4rgb_alpha/10151.mov",
//						  "/Users/xavier/numediart/Project10.1-Borderlands/2010_4rgb_alpha/20102.mov");

//	test_Thomas();
	//	vector<float> ci = V->getContractionIndices();
//	vector<double> dci;
//	for (int i=0; i< ci.size(); i++) {
//		dci[i] = (double) ci[i];
//	}
//	
//	Gnuplot g1 = Gnuplot("lines");
//	g1.reset_plot();
//	g1.plot_x(dci,"dci");

	return (EXIT_SUCCESS);
}

