/**
 * @brief cvInterp-test.cpp
 * @author Christian Frisson
 * @date 10/07/2013
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
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <stdio.h>




using namespace std;
using namespace cv;

//https://github.com/Itseez/opencv/blob/master/modules/contrib/src/colormap.cpp

static Mat linspace(float x0, float x1, int n)
{
    Mat pts(n, 1, CV_32FC1);
    float step = (x1-x0)/(n-1);
    for(int i = 0; i < n; i++)
        pts.at<float>(i,0) = x0+i*step;
    return pts;
}

static void sortMatrixRowsByIndices(InputArray _src, InputArray _indices, OutputArray _dst)
{
    //    if(_indices.getMat().type() != CV_32SC1)
    //        CV_Error(cv::Error::StsUnsupportedFormat, "cv::sortRowsByIndices only works on integer indices!");
    Mat src = _src.getMat();
    std::vector<int> indices = _indices.getMat();
    _dst.create(src.rows, src.cols, src.type());
    Mat dst = _dst.getMat();
    for(size_t idx = 0; idx < indices.size(); idx++) {
        Mat originalRow = src.row(indices[idx]);
        Mat sortedRow = dst.row((int)idx);
        originalRow.copyTo(sortedRow);
    }
}

static Mat sortMatrixRowsByIndices(InputArray src, InputArray indices)
{
    Mat dst;
    sortMatrixRowsByIndices(src, indices, dst);
    return dst;
}


static Mat argsort(InputArray _src, bool ascending=true)
{
    Mat src = _src.getMat();
    //    if (src.rows != 1 && src.cols != 1)
    //        CV_Error(Error::StsBadArg, "cv::argsort only sorts 1D matrices.");
    int flags = SORT_EVERY_ROW | (ascending ? SORT_ASCENDING : SORT_DESCENDING);
    Mat sorted_indices;
    sortIdx(src.reshape(1,1),sorted_indices,flags);
    return sorted_indices;
}

template <typename _Tp> static
Mat interp1_(const Mat& X_, const Mat& Y_, const Mat& XI)
{
    int n = XI.rows;
    // sort input table
    std::vector<int> sort_indices = argsort(X_);

    Mat X = sortMatrixRowsByIndices(X_,sort_indices);
    Mat Y = sortMatrixRowsByIndices(Y_,sort_indices);
    // interpolated values
    Mat yi = Mat::zeros(XI.size(), XI.type());
    for(int i = 0; i < n; i++) {
        int c = 0;
        int low = 0;
        int high = X.rows - 1;
        // set bounds
        if(XI.at<_Tp>(i,0) < X.at<_Tp>(low, 0))
            high = 1;
        if(XI.at<_Tp>(i,0) > X.at<_Tp>(high, 0))
            low = high - 1;
        // binary search
        while((high-low)>1) {
            c = low + ((high - low) >> 1);
            if(XI.at<_Tp>(i,0) > X.at<_Tp>(c,0)) {
                low = c;
            } else {
                high = c;
            }
        }
        // linear interpolation
        yi.at<_Tp>(i,0) += Y.at<_Tp>(low,0)
                + (XI.at<_Tp>(i,0) - X.at<_Tp>(low,0))
                * (Y.at<_Tp>(high,0) - Y.at<_Tp>(low,0))
                / (X.at<_Tp>(high,0) - X.at<_Tp>(low,0));
    }
    return yi;
}

static Mat interp1(InputArray _x, InputArray _Y, InputArray _xi)
//static Mat interp1(Mat x, Mat Y, Mat xi)
{
    // get matrices
    Mat x = _x.getMat();
    Mat Y = _Y.getMat();
    Mat xi = _xi.getMat();
    // check types & alignment
    CV_Assert((x.type() == Y.type()) && (Y.type() == xi.type()));
    CV_Assert((x.cols == 1) && (x.rows == Y.rows) && (x.cols == Y.cols));
    // call templated interp1
    switch(x.type()) {
    case CV_8SC1: return interp1_<char>(x,Y,xi); break;
    case CV_8UC1: return interp1_<unsigned char>(x,Y,xi); break;
    case CV_16SC1: return interp1_<short>(x,Y,xi); break;
    case CV_16UC1: return interp1_<unsigned short>(x,Y,xi); break;
    case CV_32SC1: return interp1_<int>(x,Y,xi); break;
    case CV_32FC1: return interp1_<float>(x,Y,xi); break;
    case CV_64FC1: return interp1_<double>(x,Y,xi); break;
        //       default: CV_Error(Error::StsUnsupportedFormat, ""); break;
    }
    return Mat();
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
    //src = imread( "/Volumes/data/Datasets/mc-test-image/tcts-faces/filtered/riche.jpg");
    //src = imread( "/Volumes/data/Datasets/mc-test-image/coil-100/obj1__00.png");
    src = imread( "/Users/nicolasriche/mediacycle-datasets-temp/toronto-eyetracking/111.jpg");
    if( !src.data ){ return -1; }

    // Pre-process: resize, color transformation, convert double and normalize
    resize(src, src_resized,Size(200,200));
    cvtColor(src_resized, ycbcr, cv::COLOR_BGR2YCrCb);
    split( ycbcr, planes);

    int ymax = ycbcr.rows;
    int xmax = ycbcr.cols;

    minMaxLoc(planes[0],&min ,&max ,0,0);
    minMaxLoc(planes[1],&min1,&max1,0,0);
    minMaxLoc(planes[2],&min2,&max2,0,0);


    for (int y = 0; y < ymax; y++) {
        for (int x = 0; x < xmax; x++) {

            float valeur  = (float)(planes[0].at<uchar>(y,x));
            float valeur_norm  = valeur / max;
            im_y_norm.at<float>(y,x) =valeur_norm;

            float valeur1 = (float)(planes[1].at<uchar>(y,x));
            float valeur1_norm = valeur1 / max1;
            im_cb_norm.at<float>(y,x)=valeur1_norm;

            float valeur2 = (float)(planes[2].at<uchar>(y,x));
            float valeur2_norm = valeur2 / max2;
            im_cr_norm.at<float>(y,x)=valeur2_norm;

        }
    }

    /*planes[0].convertTo(im_y_norm, CV_32F);
    planes[1].convertTo(im_cb_norm, CV_32F);
    planes[2].convertTo(im_cr_norm, CV_32F);*/

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

    Mat histImage ( hist_h, hist_w, CV_8UC1);
    Mat histImage1( hist_h, hist_w, CV_8UC1);
    /// Normalize the result to [ 0, histImage.rows ]
    normalize(y2_hist, y2_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(y2_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(y2_hist.at<float>(i)) ),
              Scalar( 255, 255, 255), 2, 8, 0  );

    }

    // 1. resize feature map
    Mat channel;
    resize(im_y_pyr[2], channel,Size(1,200*200));

    // 2. build x
    double mint,maxt;
    minMaxLoc(channel,&mint ,&maxt ,0,0);

    int nn = 100;
    Mat x = linspace(mint, maxt,nn);

    // 3. build h
    Mat h = y2_hist;
    h = h+1;

    Scalar d = sum(h);
    double sommeh = d.val[0];
    h = h/sommeh;
    log(h,h);
    h = -h;

    // 4. interp1
    Mat S =  interp1(x, h, channel);
    resize(S, S,Size(200,200));

    // 5. Apply the colormap:
    Mat TEST;
    im_y_pyr[2].convertTo(TEST, /*int rtype*/CV_8UC1, 255, 0);//, double alpha=1, double beta=0 ) const;
    applyColorMap(TEST, TEST, COLORMAP_SUMMER);

    // Affichage
/*    namedWindow("calcHist Demo", cv::WINDOW_AUTOSIZE );
    imshow("calcHist Demo", histImage );


    namedWindow("calcHist Demo1", cv::WINDOW_AUTOSIZE );
    imshow("calcHist Demo1", im_y_pyr[2] );*/

    namedWindow("calcHist Demo2", cv::WINDOW_AUTOSIZE );
    imshow("calcHist Demo2", TEST );

    waitKey(0);

    return 0;

}
