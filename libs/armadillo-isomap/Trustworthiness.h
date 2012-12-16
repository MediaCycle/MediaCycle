//
//  Trustworthiness.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__Trustworthiness__
#define __MediaCycle__Trustworthiness__

#include <iostream>
#include <armadillo>

class Trustworthiness{
public:
    Trustworthiness();
    virtual ~Trustworthiness(){};
    virtual bool setDistanceMatrixHighDim(arma::mat D);
    virtual bool setDistanceMatrixLowDim(arma::mat D);
    virtual bool setFeatureMatrixHighDim(arma::mat F);
    virtual bool setFeatureMatrixLowDim(arma::mat F);
    virtual double compute(unsigned int k);
protected:
    arma::mat Dhd,Dld;
    
};

#endif /* defined(__MediaCycle__Trustworthiness__) */
