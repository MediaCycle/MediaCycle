//
//  st_Sne.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-st_Sne. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-Sne.html

#ifndef __MediaCycle__st_Sne__
#define __MediaCycle__st_Sne__

#include <iostream>
#include <armadillo>
#include "t_Sne.h"

class st_Sne: public t_Sne{
public:
    st_Sne();
    virtual ~st_Sne(){};
    virtual bool setDistanceMatrix(arma::mat D,arma::urowvec label);
    virtual bool setFeatureMatrix(arma::mat F,arma::urowvec label);
   // virtual arma::mat compute(int ndim);
    //void setPerplexity(double perplexity){kk=perplexity;};
    void setSpervizedBoost(double slope){this->slope=slope;};
protected:
    bool d2p(arma::mat Dist,arma::urowvec label);//encode conditionnal probabilties P matrix. Dis =square of distance matrix
    //void Hbeta(double &H,arma::rowvec &thisP,arma::rowvec Dist,int ind,double beta);
protected:
    //arma::mat P;
    //double kk;
    //double tol;
    double slope;
};

#endif /* defined(__MediaCycle__st_Sne__) */
