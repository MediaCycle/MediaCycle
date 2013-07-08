/**
 * @brief cvGabor-test.cpp
 * @author Nicolas Riche
 * @date 08/07/2013
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

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;


// Fonction qui permet de créer le kernel de Gabor
Mat GaborKernel(int ks, double sig, double th, double lm, double ps)
{
    int hks = (ks-1)/2;
    double theta = th*CV_PI/180;
    double psi = ps*CV_PI/180;
    double del = 2.0/(ks-1);
    double lmbd = lm;
    double sigma = sig/ks;
    double x_theta;
    double y_theta;
    Mat kernel(ks,ks, CV_32F);
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x*del*cos(theta)+y*del*sin(theta);
            y_theta = -x*del*sin(theta)+y*del*cos(theta);
            kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos(2*CV_PI*x_theta/lmbd + psi);
        }
    }
    return kernel;
}

// Fonction qui crée un filtre de Gabor  partir de filter2D et d'un kernel de Gabor (GaborKernel)
Mat GaborFilter(Mat src_f,int pos_lm,int pos_th,int kernel_size)
{
	int pos_sigma= 5;
	int pos_psi = 90;
    double sig = pos_sigma;
    double lm = 0.5+pos_lm/100.0;
    double th = pos_th;
    double ps = pos_psi;
	Mat dest;
    Mat kernel = GaborKernel(kernel_size, sig, th, lm, ps);
    filter2D(src_f, dest, CV_32F, kernel);
	
	return dest;
}


// Fonction qui crée un banc de filtres de Gabor à différentes échelles
Mat GaborBankScale(Mat src_f,double th,int kernel_size)
{
	double min,max;
	Mat scale1, scale2, scale3, scale4,scale5,Ori;// Horrible ! Il faut créer un vector<Mat> pour scale 1-5 (mais problème avec l'initialisation de vector)
	scale1 = GaborFilter(src_f,0  ,th,kernel_size); // faire une boucle pour appeler une seule fois la fonction 
	scale2 = GaborFilter(src_f,25 ,th,kernel_size); 
	scale3 = GaborFilter(src_f,50 ,th,kernel_size); 
	scale4 = GaborFilter(src_f,75 ,th,kernel_size); 
	scale5 = GaborFilter(src_f,100,th,kernel_size);
	Ori = scale1+scale2+scale3+scale4+scale5; // j'ai l'impression que le filtre sélectionne énormement d'info: à vérifier ! 
	
	// PROBLEME: la normalisation 
	// methode 1
	//minMaxLoc(Ori,&min ,&max ,0,0);
	//Ori = (Ori - min) / (max-min);
	// methode 2
	//normalize(Ori,Ori,1);
	// ou sans normalisation.... j'ai du mal a visualiser les résultats et voir ce qui est correct ou non ! 
	return Ori;
}


// il faudra créer une fonction qui crée un banc de filtres mais avec différentes orientations (idem que GaborBankScale)


// main fonction
int main(int argc, char** argv)
{  
   
	//Mat image = imread( "/Volumes/data/Datasets/mc-test-image/tcts-faces/filtered/riche.jpg");
	Mat image = imread( "/Users/nicolasriche/mediacycle-datasets-temp/toronto-eyetracking/chien.jpg");
    Mat src;
	Mat src_f;
	int kernel_size = 21;
	double th = 90; 
	
    cvtColor(image, src, CV_BGR2GRAY);
    src.convertTo(src_f, CV_32F, 1.0/255, 0);
	
    if (!kernel_size%2)
    {
        kernel_size+=1;
    }

    Mat Ori =  GaborBankScale(src_f,th,kernel_size);
	
	namedWindow("Ori", CV_WINDOW_AUTOSIZE );
	imshow("Ori",Ori);
	
    waitKey(0);	
    return 0;
}





















