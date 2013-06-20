/**
 * @brief cvInterp-test.cpp
 * @author Nicolas Riche
 * @date 20/06/2013
 * @copyright (c) 2013 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

// Fonction Matlab 1
double* vectorize( int size, ... )
{
    // Variables Arguments
    va_list arguments;
    double *tab = (double*)malloc(sizeof(double) * size);
	
    //Use argument
    va_start ( arguments, size );
	
    for(int idx = 0 ; idx < size ; ++idx){
        tab[idx] = va_arg ( arguments, double );
    }
	
    va_end ( arguments );
	
    return tab;
}

// Fonction Matlab2: Create a tab like matlab [start;step;end] 
double* createTab(double start, double step, double end){
    // Size
    int size = 1 + fabs((end - start)/step);
    // Alloc
    double * tab = (double*)malloc(sizeof(double) * size);
    // Fill
    for(int idx = 0; start <= end && idx < size; start += step, ++idx){
        tab[idx] = start;
    }
    return tab;
}

// Fonction Matlab3: Create a tab with a value in all cases */
double* instance(int size, double val){
    // Alloc
    double * tab = (double*)malloc(sizeof(double) * size);
    // Fill
    while(size--){
        tab[size] = val;
    }
    return tab;
}

// Fonction Matlab4: To inter-pole data 
double* interp1(double * x, int sizex, double * y , double * xi, int sizexi){
    
	double * yi; // attention: a été ajouté mais à changer !!!!!! 
	// indexes
    int idyi = 0;
    int idx = 0;
    // After computing, to know the current step progress
    double steps = (y[1] - y[0]) / (x[1] - x[0]);
	
    // fill left points
    while(idyi < sizexi - 1 && xi[idyi] < x[0]){
        yi[idyi] = y[0] - (steps * (x[0] - xi[idyi]) ) ;
        ++idyi;
    }
	
    // fill point in the same interval as the original values
    while(idyi < sizexi && idx < sizex){
        while(idx < sizex && xi[idyi] >= x[idx]){
            ++idx;
        }
        if(idx != sizex){
            steps = (y[idx] - y[idx-1]) / (x[idx] - x[idx-1]);
            yi[idyi] = y[idx] - (steps * (x[idx] - xi[idyi]) ) ;
            ++idyi;
        }
    }
	
    // fill the right points
    if(idyi < sizexi){
        steps = (y[sizex - 1] - y[sizex-2]) / (x[sizex-1] - x[sizex-2]);
        while(idyi < sizexi){
            yi[idyi] = y[sizex-1] + (steps * (xi[idyi] - x[sizex-1])) ;
            ++idyi;
        }
    }
	
    return yi;
}

// main fonction
int main( int argc, char** argv )
{
  
  // Declarations
  Mat src,src_resized,ycbcr;
  double min, max, min1, max1, min2, max2;
  vector<Mat> planes,im_y_pyr,im_cb_pyr,im_cr_pyr;
  
  Mat im_y_norm (200,200,CV_32FC1);
  Mat im_cb_norm(200,200,CV_32FC1);
  Mat im_cr_norm(200,200,CV_32FC1);
	
  int ndp = 4;
	
  // Load image  
  src = imread( "../../Original_Img/33.jpg");
  if( !src.data ){ return -1; }

  // Pre-process: resize, color transformation, convert double and normalize
  resize(src, src_resized,Size(200,200));	
  cvtColor(src_resized, ycbcr, CV_BGR2YCrCb);
  split( ycbcr, planes);

  int ymax = ycbcr.rows;
  int xmax = ycbcr.cols;
	
  minMaxLoc(planes[0],&min ,&max ,0,0);
  minMaxLoc(planes[1],&min1,&max1,0,0);
  minMaxLoc(planes[2],&min2,&max2,0,0);
	
	
	for (int y = 0; y < ymax; y++) {
		for (int x = 0; x < xmax; x++) {
			
			float valeur  = (float)planes[0].at<uchar>(y,x);
			float valeur_norm  = valeur / max;
			im_y_norm.at<float>(y,x) =valeur_norm;
			
			float valeur1 = (float)planes[1].at<uchar>(y,x);
			float valeur1_norm = valeur1 / max1;
			im_cb_norm.at<float>(y,x)=valeur1_norm;
			
			float valeur2 = (float)planes[2].at<uchar>(y,x);
			float valeur2_norm = valeur2 / max2;
			im_cr_norm.at<float>(y,x)=valeur2_norm;
			
		}
	}
    
	// Image pyramid
	buildPyramid(im_y_norm,  im_y_pyr , ndp);	
	buildPyramid(im_cb_norm, im_cb_pyr, ndp);
	buildPyramid(im_cr_norm, im_cr_pyr, ndp);
	
	for (int i = 1; i < (ndp+1); i++) {
		
		resize(im_y_pyr [i], im_y_pyr [i],Size(200,200));
		resize(im_cb_pyr[i], im_cb_pyr[i],Size(200,200));
		resize(im_cr_pyr[i], im_cr_pyr[i],Size(200,200));
	
	}
	
	// Compute Histogram
	double minL, maxL;
	int histSize = 100;
	minMaxLoc(im_y_pyr[2],&minL ,&maxL ,0,0);
	
	float range[] = { minL, maxL } ;
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	
	Mat y2_hist;
	calcHist( &im_y_pyr[2], 1, 0, Mat(),  y2_hist, 1, &histSize, &histRange, uniform, accumulate );

	
	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );
	
	Mat histImage( hist_h, hist_w, CV_8UC1);
	
	/// Normalize the result to [ 0, histImage.rows ]
	normalize(y2_hist, y2_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	
	/// Draw for each channel
	for( int i = 1; i < histSize; i++ )
	{
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(y2_hist.at<float>(i-1)) ) ,
			 Point( bin_w*(i), hist_h - cvRound(y2_hist.at<float>(i)) ),
			 Scalar( 255, 255, 255), 2, 8, 0  );
		
	}
	
	// Fonction Matlab3: B = 88 * ones(50,1)
	double* B = instance(50, 88.0);
	
	// Fonction Matlab1: C = [1 1 2 2 3 3]
	double* C = vectorize(6, 1.0 , 1.0 , 2.0 , 2.0 , 3.0 , 3.0);
	
	// Fonction Matlab2: E = [start;step;end]
	double* E = createTab(start, step, end);
	
	// Fonction Matlab4: Titem = interp1(dt1,dt2,dt3,'linear');
	double* Titem = interp1(dt1,sizedt1,dt2,dt3,sizedt3);
	
	// peux tu me montrer un exemple pour remplir interp1 en c ? 
	// je vais essayer de l'adapter avec l'histogramme ;-) 
	// grand merci ! (mélange c / c++ un peu abstrait pour le moment on en parle semaine prochaine !)
	
	
	
	
	// Holds the colormap version of the image:
	//Mat cm_im_y_pyr;
	// Apply the colormap:
	//applyColorMap(im_y_pyr[2], cm_im_y_pyr, COLORMAP_JET);

	
	// Affichage
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
	imshow("calcHist Demo", histImage );
	//cout <<  y2_hist.rows << "  " << y2_hist.cols ;
	//cout << y2_hist;
	//namedWindow("Y", CV_WINDOW_AUTOSIZE );
	//imshow("Y",im_y_pyr[2]);
	
	//namedWindow("cm_img0", CV_WINDOW_AUTOSIZE );
	//imshow("cm_img0",cm_im_y_pyr);
	

	
  waitKey(0);

  return 0;

}
