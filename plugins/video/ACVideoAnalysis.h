/*
 *  ACVideoAnalysis.h
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

#ifndef _ACVIDEOANALYSIS_H
#define	_ACVIDEOANALYSIS_H

#include <string>
#include <vector>

#include "BlobResult.h" // Main blob library include (in plugin/image/blobs)
#include "ACImageAnalysis.h"
#include "ACVideoData.h"

#include <ACOpenCVInclude.h>

#include <fstream>
#include <cmath>

typedef std::vector<float> blob_center; // 2D, but could be 3D

class ACVideoAnalysis {
public:
    ACVideoAnalysis();
    ACVideoAnalysis(const std::string &filename);
	ACVideoAnalysis(ACMediaData* media_data);

    ~ACVideoAnalysis();
	
	// general I/O, settings
	void clean();
	void rewind();
	void setFileName(const std::string &filename);
	int initialize();
	void writeToFile(const std::string &fileout, int w=320, int h=240, int nskip=0, int istep=1);

	//accessors 
	inline int getWidth() {return width;}
	inline int getHeight() {return height;}
	std::string getFileName() {return file_name;}
	std::string getColorModel(){return color_model;}
	inline int getDepth() {return depth;}
	inline int getNumberOfFrames() {return nframes;}
	float getDuration();

	bool isTrajectoryComputed(){return HAS_TRAJECTORY;}
	bool areBlobsComputed(){return HAS_BLOBS;}
	
	// utilities
	cv::Mat getFrame(int i);
	void setFramePosition(int pos);
	int getFramePosition();

//	IplImage* computeAverageImage(int nskip = 0, int nread = 0, int njump = -1, std::string s =""); 
	cv::Mat computeMedianImage(int nskip = 0, int nread = 0, int njump = -1, std::string s =""); 
//	IplImage* computeMedianNoBlobImage(std::string s ="",IplImage *first_guess=0);
	void backgroundSubstraction(IplImage* bg_img, int nskip=0, std::string cmode="BGR");
	// blob detection could be per channel or in color image
	//	void detectBlobs(int ichannel=0, std::string cmode="HSV", IplImage* bg_img=0, int bg_thesh=40, int big_blob=200, int small_blob=0);
	
	// XS not so useful
	// XS eventually translate into 2.* (started...)
	//void histogramEqualize(const IplImage* bg_img);
	
	// raw features computation
	//   - on blobs
	void computeBlobs(const cv::Mat&bg_img=cv::Mat(), int bg_thesh=10, int big_blob=200, int small_blob=0);
// XS 280611 removed the following one for the moment (migration openCV 2.*)	
//	void computeBlobsInteractively(IplImage* bg_img=0, bool merge_blobs=false, int bg_thesh=20, int big_blob=200, int small_blob=0);
	void computeBlobsUL(const cv::Mat& bg_img=cv::Mat(), bool merge_blobs=true, int big_blob=200, int small_blob=0);
	//   - general (not on blobs)
//	void computeOpticalFlow();

	void computeContractionIndices();
	void computeBoundingBoxRatios();
	void computeBlobPixelSpeed();
	void computeGlobalPixelsSpeed();
	void computeColorMoments(int n=4, string cm = "HSV");
	void computeHuMoments(int tresh=0, cv::Mat bg_img=cv::Mat()); // with background subtraction
	void computeFourierPolarMoments(int RadialBins=5, int AngularBins=8); // without background subtraction
	void computeFourierMellinMoments(); // without background subtraction
	void computeImageHistograms(int w=10, int h=10); // image histogram, turned into a 1D vector
	#if CV_MIN_VERSION_REQUIRED(2,3,1)
	void computeGlobalOrientation();
	#endif
	// features manipulation
	void mergeBlobs(float blob_dist = 0);
	void computeMergedBlobsTrajectory(float blob_dist = 0);
	void computeMergedBlobsSpeeds(float blob_dist = 0);
//	void computeCellOccupation(int nx, int ny);


	// features accessors (to be called by ACVideoDancersPlugin)
	std::vector<blob_center> getMergedBlobsTrajectory() {return blob_centers;}
	std::vector<blob_center> getMergedBlobsSpeeds() {return blob_speeds;}
	std::vector<blob_center> getNormalizedMergedBlobsTrajectory();
	std::vector<blob_center> getNormalizedMergedBlobsSpeeds();

	std::vector<float> getContractionIndices() {return contraction_indices;}
	std::vector<float> getBlobPixelsSpeeds() {return blob_pixel_speeds;}
	std::vector<float> getGlobalPixelsSpeeds() {return global_pixel_speeds;}
	std::vector<float> getBoundingBoxRatios() {return bounding_box_ratios;}
	std::vector<float> getGlobalOrientations(){return global_orientations;}

	std::vector<float> getDummyTimeStamps(int nsize);
	std::vector<float> getBlobsTimeStamps();
	std::vector<float> getGlobalTimeStamps();
	std::vector<float> getGlobalFrameStamps();

	// XS or float* (dim 7)
	std::vector<std::vector<float> > getRawMoments() {return raw_moments;}
	std::vector<std::vector<float> > getHuMoments() {return hu_moments;}
	std::vector<float> getHuMoment(int i);
	std::vector<std::vector<float> > getFourierPolarMoments() {return fourier_polar_moments;}
	std::vector<std::vector<float> > getFourierMellinMoments() {return fourier_mellin_moments;}
	std::vector<std::vector<float> > getColorMoments() {return color_moments;}
	std::vector<std::vector<float> > getImageHistograms() {return image_histograms;}

	// saves stuff in file
	void dumpTrajectory(std::ostream &odump);
	void dumpContractionIndices(std::ostream &odump);
	void dumpBoundingBoxRatios(std::ostream &odump);
	void dumpBlobSpeed(std::ostream &odump);
	void dumpRawMoments(std::ostream &odump);
	void dumpHuMoments(std::ostream &odump);
	void dumpFourierPolarMoments(std::ostream &odump);
	void dumpAll(std::ostream &odump);
	
	// for display (ifdef VISUAL_CHECK) using highgui
	void showInWindow(std::string title, bool has_win=false);
	void showFFTInWindow(std::string title, bool has_win=false);
	void showFrameInWindow(std::string title, const cv::Mat& frame, bool has_win=true);

	// fancy browsing with trackbar to set position
	static void onTrackbarSlide(int, void*); // callback needs to be static or global
	void browseWithTrackbarInWindow(std::string title, bool has_win=false);
	// ?	void showBlobsInWindow(std::string="VIDEO", bool has_win=false);
	

	//handy - remove path, keeps only filename
	std::string extractFilename(std::string path);
	
	// XS tmp tests
	void test_match_shapes (ACVideoAnalysis *V2, IplImage* bg_img);
	
private:
	std::string file_name;
	std::string color_model; // "BGR" or "HSV" : these are already in IPLimage, but not used in OpenCV (see manual !)
//	IplImage* thumbnail;
	
	cv::VideoCapture* capture ;
	int frame_counter;
	bool FROM_FILE; // true if capture initialized from file (then it has to be deleted afterwards)
	
	bool HAS_TRAJECTORY;
	bool HAS_BLOBS;

	// threshold for lower and upper parts of the image
	int threshU;
	int threshL;
	static const int ystar; // where to split the image in half (horizontally)

	// NB: blobs (CBlobResult) may contain more than one blob per frame
	std::vector<CBlobResult> all_blobs; // XS make this pointers ?
	std::vector<float> all_blobs_time_stamps;
	std::vector<float> all_blobs_frame_stamps;

	std::vector<blob_center> blob_centers;
	std::vector<blob_center> blob_speeds; // speed of center of blob
	std::vector<float> blob_pixel_speeds; // pixel-per-pixel speed in blob

	std::vector<float> contraction_indices;
	std::vector<float> bounding_box_ratios;
	std::vector<float> bounding_box_heights;
	std::vector<float> bounding_box_widths;
	std::vector<float> global_pixel_speeds;  // pixel-per-pixel speed in whole image
	std::vector<float> interest_points;
	std::vector<float> global_orientations;
	
	std::vector<std::vector<float> > raw_moments;
	std::vector<std::vector<float> > hu_moments;
	std::vector<std::vector<float> > fourier_polar_moments;
	std::vector<std::vector<float> > fourier_mellin_moments;
	std::vector<std::vector<float> > color_moments;

	std::vector<std::vector<float> > image_histograms;

	int width, height, depth, fps, nframes;
	//	int videocodec;
	int g_slider_position;
	//	MyHistogram *averageHistogram;
	
};

#endif	/* _ACVIDEOANALYSIS_H */
