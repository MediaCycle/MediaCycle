//
//  Batch.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#ifndef __MediaCycle__Batch__
#define __MediaCycle__Batch__

#include <iostream>
#include <armadillo>

class Batch{
public:
    Batch();
    virtual ~Batch(){};
    virtual bool setTagVector(arma::ucolvec L);//sample tag.
    virtual bool setTagVector(arma::umat L);//2 columns matrix. First with sample index, and second with the sample tag.
    virtual arma::umat select(float procent,bool balancedTaggedClass=false);
protected:
    arma::umat L;
    
};

#endif /* defined(__MediaCycle__Batch__) */
