//
//  CrossSpaceClassificatorErrorMeasure.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__CrossSpaceClassificatorErrorMeasure__
#define __MediaCycle__CrossSpaceClassificatorErrorMeasure__

#include <iostream>
#include <armadillo>

class CrossSpaceClassificatorErrorMeasure{
public:
    virtual ~CrossSpaceClassificatorErrorMeasure(){};
    virtual bool setTagVector(arma::ucolvec L,unsigned int batchNb,float procent,unsigned int k);
    double errorKnnMeasure(arma::mat F1,arma::mat F2,double &errMean,double &errStd);
protected:
    std::vector<arma::umat> batchCollection;
    arma::ucolvec L;
    unsigned int k;
    
};

#endif /* defined(__MediaCycle__CrossSpaceClassificatorErrorMeasure__) */
