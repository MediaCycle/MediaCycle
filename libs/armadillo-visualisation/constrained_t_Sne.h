//
//  constrained_t_Sne.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-constrained_t_Sne. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-Sne.html

#ifndef __MediaCycle__constrained_t_Sne__
#define __MediaCycle__constrained_t_Sne__

#include <iostream>
#include <armadillo>

class constrained_t_Sne{
public:
    constrained_t_Sne();
    virtual ~constrained_t_Sne(){};
    virtual bool setDistanceMatrix(arma::mat D);
    virtual bool setFeatureMatrix(arma::mat F);
    virtual bool setXInit(arma::colvec X);
    arma::colvec getX(){return this->X;};
    virtual arma::mat compute(int ndim,arma::mat yInit=arma::mat(0,0));
    void setPerplexity(double perplexity){kk=perplexity;};
    void setIterMax(unsigned int im){max_iter=im;};
protected:
    virtual bool d2p(arma::mat Dist);//encode conditionnal probabilties P matrix. Dis =square of distance matrix
    void Hbeta(double &H,arma::rowvec &thisP,arma::rowvec Dist,int ind,double beta);
protected:
    arma::mat P;
    double kk;
    double tol;                          // iteration at which lying about P-values is stopped
    unsigned int max_iter;
    arma::colvec X;
};

#endif /* defined(__MediaCycle__constrained_t_Sne__) */
