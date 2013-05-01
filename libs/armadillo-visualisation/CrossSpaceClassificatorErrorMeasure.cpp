//
//  CrossSpaceClassificatorErrorMeasure.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "CrossSpaceClassificatorErrorMeasure.h"
#include <time.h>

#include "CrossSpaceKnn.h"
#include "Batch.h"

using namespace arma;

bool CrossSpaceClassificatorErrorMeasure::setTagVector(arma::ucolvec L,unsigned int batchNb,float procent,unsigned int k){
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
double CrossSpaceClassificatorErrorMeasure::errorKnnMeasure(arma::mat F1,arma::mat F2,double &errMean,double &errStd){
    double err=0.0;
    double errWB=0.0;
    colvec errMat(batchCollection.size());
    colvec sum_F1=sum(pow(F1,2),1);
    mat Dtemp1=(sum_F1*ones(1,sum_F1.n_rows)+ones(sum_F1.n_rows,1)*trans(sum_F1)-2*(F1*trans(F1)));
    Dtemp1.diag()=zeros(Dtemp1.n_rows,1);
    Dtemp1=sqrt(Dtemp1);
    colvec sum_F2=sum(pow(F2,2),1);
    mat Dtemp2=(sum_F2*ones(1,sum_F2.n_rows)+ones(sum_F2.n_rows,1)*trans(sum_F2)-2*(F2*trans(F2)));
    Dtemp2.diag()=zeros(Dtemp2.n_rows,1);
    Dtemp2=sqrt(Dtemp2);
    cout<<"batch";
    cout<<" of size "<<batchCollection[0].n_rows<<" samples\t";
    for (int i=0;i<batchCollection.size();i++){
        if (i%10==0)
            cout<<i<<"\t";
        CrossSpaceKnn algo;
        algo.setDistanceMatrix1(Dtemp1, batchCollection[i]);
        algo.setDistanceMatrix2(Dtemp2, batchCollection[i]);
        ucolvec res=algo.compute(k);
        ucolvec testRes=(res!=L);
        double errLocWB=((double)sum(res!=L))/(L.n_rows);
        errMat(i)=errLocWB;
        errWB+=errLocWB;
        for (int j=0;j<batchCollection[i].n_rows;j++){//to substract batch sample from the result
            testRes(batchCollection[i](j,0))=0;
        }
        double locErr=(double)sum(res!=L)/(L.n_rows-batchCollection[i].n_rows);
        umat resFused(res.n_rows,2);
        resFused.col(0)=L;
        resFused.col(1)=res;
        //cout <<"res:"<<resFused<<endl;
        //cout<<"error for batch "<<i<<" :"<<locErr<<endl;
        err+=locErr;
    }
    cout<<endl;
    err/=batchCollection.size();
    errWB/=batchCollection.size();
    errMean=mean(errMat);
    errStd=stddev(errMat);
    //cout<<"mean error :"<<err<<" on "<<batchCollection.size()<<" batchs with size "<<batchCollection[0].n_rows<<endl;
    //cout<<"mean error with batch :"<<errWB<<" on "<<batchCollection.size()<<" batchs with size "<<batchCollection[0].n_rows<<endl;
    return errWB;
}
