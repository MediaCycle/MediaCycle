//
//  ClassificatorErrorMeasure.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "ClassificatorErrorMeasure.h"
#include <time.h>

#include "knn.h"
#include "Batch.h"

using namespace arma;

bool ClassificatorErrorMeasure::setTagVector(arma::ucolvec L,unsigned int batchNb,float procent,unsigned int k){
    this->L=L;
    Batch algo;

    batchCollection.clear();
    for (int i=0;i<batchNb;i++){
        algo.setTagVector(L);
        batchCollection.push_back(algo.select(procent,false));
        //cout<<"L:"<<endl<<batchCollection[i]<<endl;

    }
    
    this->k=k;
    
}
double ClassificatorErrorMeasure::errorKnnMeasure(arma::mat F){
    double err=0.0;
    for (int i=0;i<batchCollection.size();i++){
        knn algo;
        algo.setFeatureMatrix(F, batchCollection[i]);
        ucolvec res=algo.compute(k);
        double locErr=(double)sum(res!=L)/L.n_rows;
        umat resFused(res.n_rows,2);
        resFused.col(0)=L;
        resFused.col(1)=res;
        //cout <<"res:"<<resFused<<endl;
        cout<<"error for batch "<<i<<" :"<<locErr<<endl;
        err+=locErr;
    }
    err/=batchCollection.size();
    cout<<"mean error :"<<err<<endl;
    
    return 0.0;
}
