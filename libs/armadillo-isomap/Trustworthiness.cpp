//
//  Trustworthiness.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "Trustworthiness.h"

using namespace arma;
Trustworthiness::Trustworthiness(){
    Dld=mat(0,0);
    Dhd=mat(0,0);
    
}
bool Trustworthiness::setDistanceMatrixHighDim(arma::mat D){
    if (Dld.n_elem>0)
        if (D.n_rows!=Dld.n_rows)
    if (D.is_square()==false)
        return false;
    this->Dhd=D;
    return true;
}
bool Trustworthiness::setDistanceMatrixLowDim(arma::mat D){
    if (Dhd.n_elem>0)
        if (D.n_rows!=Dhd.n_rows)
            return false;
    if (D.is_square()==false)
        return false;
    this->Dld=D;
    return true;
}

bool Trustworthiness::setFeatureMatrixHighDim(arma::mat F){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    return this->setDistanceMatrixHighDim(Dtemp);
}
bool Trustworthiness::setFeatureMatrixLowDim(arma::mat F){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    return this->setDistanceMatrixLowDim(Dtemp);
}

double Trustworthiness::compute(unsigned int k){
    double n=Dhd.n_rows;
    if (k>=n)
        return 1.0;
    double trust=0.0;
    for (int i=0;i<Dhd.n_rows;i++){
        
        arma::urowvec sortIndexHd=sort_index(Dhd.row(i));
        sortIndexHd=sortIndexHd.subvec(k,sortIndexHd.n_cols-1);
        arma::urowvec sortIndexLd=sort_index(Dld.row(i));
        sortIndexLd=sortIndexLd.subvec(k,sortIndexLd.n_cols-1);
        for (int j=0;j<sortIndexLd.n_cols;j++){
            int test=sum(sortIndexHd==sortIndexLd(j));
            if (test==0)
                trust+=j;//(R(i,j)-k) is j here
        }
        
    }
    trust=1.0-(2.0/((double)n*k*(2*n-3*k-1)))*trust;
    return trust;
}