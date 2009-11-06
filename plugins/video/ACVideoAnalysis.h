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

typedef std::vector<float> blob_center; // 2D, but could be 3D

class ACVideoAnalysis {
public:
    ACVideoAnalysis();
    ACVideoAnalysis(const std::string &filename);
    ~ACVideoAnalysis();
	
	// general I/O, settings
	void clean();
	void rewind();
	void setFileName(const std::string &filename);
	int initialize();
	void saveInFile(std::string fileout, int nskip = 0);

	inline int getWidth() {return width;}
	inline int getHeight() {return height;}
	std::string getFileName() {return file_name;}
	std::string getColorModel(){return color_model;}
	inline int getDepth() {return depth;}
	inline int getNumberOfFrames() {return nframes;}
		
	bool isTrajectoryComputed(){return HAS_TRAJECTORY;}
	bool areBlobsComputed(){return HAS_BLOBS;}
	// utilities
	IplImage* getNextFrame();
	IplImage* computeAverageImage(int nskip = 0, int nread = 0, int njump = -1, std::string s =""); 
	IplImage* computeMedianImage(int nskip = 0, int nread = 0, int njump = -1, std::string s =""); 
	IplImage* computeMedianNoBlobImage(std::string s ="",IplImage *first_guess=NULL);
	void backgroundSubstraction(IplImage* bg_img, int nskip=0, std::string cmode="BGR");
	// blob detection could be per channel or in color image
	//	void detectBlobs(int ichannel=0, std::string cmode="HSV", IplImage* bg_img=NULL, int bg_thesh=40, int big_blob=200, int small_blob=0);
	void trimBlank(IplImage* bg_img);
	int getFirstFrameMove();
	
	// XS test
	void histogramEqualize(const IplImage* bg_img);
	
	// raw features computation
	void computeBlobs(IplImage* bg_img=NULL, int bg_thesh=20, int big_blob=200, int small_blob=0);
	void computeBlobsInteractively(IplImage* bg_img=NULL, bool merge_blobs=true, int bg_thesh=20, int big_blob=200, int small_blob=0);
	void computeBlobsUL(IplImage* bg_img=NULL, bool merge_blobs=true, int big_blob=200, int small_blob=0);
	void computeOpticalFlow();
	
	// features manipulation
	void mergeBlobs(float blob_dist = 0);
	void computeMergedBlobsTrajectory(float blob_dist = 0);
	void computeMergedBlobsSpeeds(float blob_dist = 0);
//	void computeCellOccupation(int nx, int ny);
	void computeContractionIndices();
	void computePixelSpeed();

	// features accessors (to be called by ACVideoPlugin)
	std::vector<blob_center> getMergedBlobsTrajectory() {return blob_centers;}
	std::vector<blob_center> getMergedBlobsSpeeds() {return blob_speeds;}
	std::vector<float> getContractionIndices() {return contraction_indices;}
	std::vector<float> getPixelSpeeds() {return pixel_speeds;}
	
	std::vector<float> getDummyTimeStamps();
	std::vector<float> getTimeStamps();

	// saves stuff in file
//XXX TODO	void 
	
	// for display (ifdef VISUAL_CHECK) using highgui
	void showInWindow(std::string="VIDEO", bool has_win=false);
	void showFrameInWindow(std::string="VIDEO", IplImage* frame=NULL, bool has_win=true);
//	void onTrackbarSlide(int pos); 
	void browseInWindow(std::string="VIDEO", bool has_win=false);
	// ?	void showBlobsInWindow(std::string="VIDEO", bool has_win=false);
	
private:
	std::string file_name;
	std::string color_model; // "BGR" or "HSV" : these are already in IPLimage, but not used in OpenCV (see manual !)
	
	CvCapture* capture ;
	int frame_counter;
	
	bool HAS_TRAJECTORY;
	bool HAS_BLOBS;

	// threshold for lower and upper parts of the image
	int threshU;
	int threshL;
	static const int ystar; // where to split the image in half (horizontally)

	// NB: blobs (CBlobResult) may contain more than one blob per frame
	std::vector<CBlobResult> all_blobs; // XS make this pointers ?
	std::vector<float> all_blobs_time_stamps;
	
	std::vector<blob_center> blob_centers;
	std::vector<blob_center> blob_speeds; 
	std::vector<float> contraction_indices;

	std::vector<float> pixel_speeds;
	int width, height, depth, fps, nframes;
	//	int videocodec;
	
	//	MyHistogram *averageHistogram;
	
};

#endif	/* _ACVIDEOANALYSIS_H */
