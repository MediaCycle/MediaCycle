//
//  mds.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__mds__
#define __MediaCycle__mds__

#include <iostream>
#include <armadillo>

class mds{
public:
    virtual ~mds(){};
    virtual bool setDistanceMatrix(arma::mat D);
    virtual bool setFeatureMatrix(arma::mat F);
    virtual arma::mat compute(int ndim);
protected:
    arma::mat S;
};

#endif /* defined(__MediaCycle__mds__) */
