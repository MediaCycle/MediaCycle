//
//  knn.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__knn__
#define __MediaCycle__knn__

#include <iostream>
#include <armadillo>

class knn{
public:
    virtual ~knn(){};
    virtual bool setDistanceMatrix(arma::mat D,arma::umat L);
    virtual bool setFeatureMatrix(arma::mat F,arma::umat L);
    virtual arma::ucolvec compute(unsigned int k);
protected:
    arma::mat Dred;
    arma::umat L;
    
};

#endif /* defined(__MediaCycle__knn__) */
