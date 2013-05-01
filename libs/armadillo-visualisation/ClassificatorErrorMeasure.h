//
//  ClassificatorErrorMeasure.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__ClassificatorErrorMeasure__
#define __MediaCycle__ClassificatorErrorMeasure__

#include <iostream>
#include <armadillo>

class ClassificatorErrorMeasure{
public:
    virtual ~ClassificatorErrorMeasure(){};
    virtual bool setTagVector(arma::ucolvec L,unsigned int batchNb,float procent,unsigned int k);
    double errorKnnMeasure(arma::mat F,double &errMean,double &errStd);
protected:
    std::vector<arma::umat> batchCollection;
    arma::ucolvec L;
    unsigned int k;
    
};

#endif /* defined(__MediaCycle__ClassificatorErrorMeasure__) */
