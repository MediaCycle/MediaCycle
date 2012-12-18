//
//  dimensionReductionQuality.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__dimensionReductionQuality__
#define __MediaCycle__dimensionReductionQuality__

#include <iostream>
#include <armadillo>

class dimensionReductionQuality{
public:
    dimensionReductionQuality();
    virtual ~dimensionReductionQuality(){};
    virtual bool setDistanceMatrixHighDim(arma::mat D);
    virtual bool setDistanceMatrixLowDim(arma::mat D);
    virtual bool setFeatureMatrixHighDim(arma::mat F);
    virtual bool setFeatureMatrixLowDim(arma::mat F);
    virtual bool compute(unsigned int k,double &trustworthiness,double &continuity);
protected:
    arma::mat Dhd,Dld;
    
};

#endif /* defined(__MediaCycle__dimensionReductionQuality__) */
