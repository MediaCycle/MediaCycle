//
//  t_Sne.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//  This class is an adaptation of the matlab code written by Laurens van der Maaten, 2008 ( Maastricht University). The algorithm is described in:
//L.J.P. van der Maaten and G.E. Hinton. Visualizing High-Dimensional Data Using t-t_Sne. Journal of Machine Learning Research 9(Nov):2579-2605, 2008
//http://homepage.tudelft.nl/19j49/t-Sne.html

#ifndef __MediaCycle__t_Sne__
#define __MediaCycle__t_Sne__

#include <iostream>
#include <armadillo>

class t_Sne{
public:
    t_Sne();
    virtual ~t_Sne(){};
    virtual bool setDistanceMatrix(arma::mat D);
    virtual bool setFeatureMatrix(arma::mat F);
    virtual arma::mat compute(int ndim,arma::mat yInit=arma::mat(0,0));
    void setPerplexity(double perplexity){kk=perplexity;};
    void setIterMax(unsigned int im){max_iter=im;};
    void setConvergenceThreshold(double ct){convergence_threshold=ct;};
protected:
    virtual bool d2p(arma::mat Dist);//encode conditionnal probabilties P matrix. Dis =square of distance matrix
    void Hbeta(double &H,arma::rowvec &thisP,arma::rowvec Dist,int ind,double beta);
protected:
    arma::mat P;
    double kk;
    double tol;                          // iteration at which lying about P-values is stopped
    unsigned int max_iter;
    double convergence_threshold;
};

#endif /* defined(__MediaCycle__t_Sne__) */
