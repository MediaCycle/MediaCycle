//
//  CrossSpaceKnn.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__CrossSpaceKnn__
#define __MediaCycle__CrossSpaceKnn__

#include <iostream>
#include <armadillo>

class CrossSpaceKnn{
public:
    virtual ~CrossSpaceKnn(){};
    virtual bool setDistanceMatrix1(arma::mat D,arma::umat L);
    virtual bool setFeatureMatrix1(arma::mat F,arma::umat L);
    virtual bool setDistanceMatrix2(arma::mat D,arma::umat L);
    virtual bool setFeatureMatrix2(arma::mat F,arma::umat L);
    virtual arma::ucolvec compute(unsigned int k);
protected:
    arma::mat Dred1,Dred2;
    arma::umat L;
    
};

#endif /* defined(__MediaCycle__CrossSpaceKnn__) */
