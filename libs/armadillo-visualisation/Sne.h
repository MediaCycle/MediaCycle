//
//  Sne.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-SNE. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-SNE.html

#ifndef __MediaCycle__Sne__
#define __MediaCycle__Sne__

#include <iostream>
#include <armadillo>

class Sne{
public:
    Sne();
    virtual ~Sne(){};
    virtual bool setDistanceMatrix(arma::mat D);
    virtual bool setFeatureMatrix(arma::mat F);
    virtual arma::mat compute(int ndim);
protected:
    bool d2p(arma::mat Dist);//encode conditionnal probabilties P matrix. Dis =square of distance matrix
    void Hbeta(double &H,arma::rowvec &thisP,arma::rowvec Dist,int ind,double beta);
protected:
    arma::mat P;
    double kk;
    double tol;
};

#endif /* defined(__MediaCycle__Sne__) */
