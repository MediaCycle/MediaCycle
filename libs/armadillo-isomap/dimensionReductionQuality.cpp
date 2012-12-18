//
//  dimensionReductionQuality.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "dimensionReductionQuality.h"

using namespace arma;
dimensionReductionQuality::dimensionReductionQuality(){
    Dld=mat(0,0);
    Dhd=mat(0,0);
    
}
bool dimensionReductionQuality::setDistanceMatrixHighDim(arma::mat D){
    if (Dld.n_elem>0)
        if (D.n_rows!=Dld.n_rows)
    if (D.is_square()==false)
        return false;
    this->Dhd=D;
    return true;
}
bool dimensionReductionQuality::setDistanceMatrixLowDim(arma::mat D){
    if (Dhd.n_elem>0)
        if (D.n_rows!=Dhd.n_rows)
            return false;
    if (D.is_square()==false)
        return false;
    this->Dld=D;
    return true;
}

bool dimensionReductionQuality::setFeatureMatrixHighDim(arma::mat F){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    return this->setDistanceMatrixHighDim(Dtemp);
}
bool dimensionReductionQuality::setFeatureMatrixLowDim(arma::mat F){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    return this->setDistanceMatrixLowDim(Dtemp);
}

bool dimensionReductionQuality::compute(unsigned int k,double &trustworthiness,double &continuity){
    double n=Dhd.n_rows;
    if (k>=n)
        return false;
    if (Dhd.n_elem==0||Dld.n_elem==0||!Dhd.is_square()||!Dld.is_square()||Dhd.n_elem!=Dld.n_elem)
        return false;
    double trust=0.0;
    double cont=0.0;
    for (int i=0;i<Dhd.n_rows;i++){
        
        arma::urowvec sortIndexHd=sort_index(Dhd.row(i));
        arma::urowvec sortIndexLd=sort_index(Dld.row(i));
        for (int j=1;j<=k;j++){
            double rank=(double)(as_scalar(find(sortIndexHd==sortIndexLd(j))));
            if (rank>k){
                trust+=rank-k;
            }
            rank=(double)as_scalar(find(sortIndexLd==sortIndexHd(j)));
            if (rank>k){
                cont+=rank-k;
            }
        }
        
    }
    trustworthiness=1.0-(2.0/((double)n*k*(2*n-3*k-1)))*trust;
    continuity=1.0-(2.0/((double)n*k*(2*n-3*k-1)))*cont;
    return true;
}