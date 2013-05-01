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
    mat Dtemp=sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F));
    Dtemp.diag()=zeros(Dtemp.n_rows,1);
    Dtemp=sqrt(Dtemp);
    return this->setDistanceMatrixHighDim(Dtemp);
}
bool dimensionReductionQuality::setFeatureMatrixLowDim(arma::mat F){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    Dtemp.diag()=zeros(Dtemp.n_rows,1);
    Dtemp=sqrt(Dtemp);
    return this->setDistanceMatrixLowDim(Dtemp);
}

bool dimensionReductionQuality::compute(unsigned int k,double &trustworthiness,double &continuity){
    double n=Dhd.n_rows;
    if (k>n)
        k=fmin(k,n-1);
    if (k>=n)
        return false;
    if (Dhd.n_elem==0||Dld.n_elem==0||!Dhd.is_square()||!Dld.is_square()||Dhd.n_elem!=Dld.n_elem)
        return false;
    double trust=0.0;
    double cont=0.0;
    double G;
    if (k<n/2)
        G=(double)n*k*(2*n-3*k-1);
    else
        G=(double)n*(n-k)*(n-k-1);
    for (int i=0;i<Dhd.n_rows;i++){
        
        arma::urowvec sortIndexHd=sort_index(Dhd.row(i));
        arma::urowvec sortIndexLd=sort_index(Dld.row(i));
        for (int j=1;j<=k;j++){
            arma::urowvec findVect=find(sortIndexHd==sortIndexLd(j));
            if (findVect.n_cols==1){
                double _rank=(double)(as_scalar(findVect(0)));
            
                if (_rank>k){
                    trust+=_rank-k;
                }
            }
            else{
                i=i;
                cout<<"dimensionReductionQuality: sorted distance index problem with rank:"<<sortIndexLd(j)<<"  & id "<<j<<endl;
            }
            findVect=find(sortIndexLd==sortIndexHd(j));
            if (findVect.n_cols==1){
                double _rank=(double)as_scalar(findVect(0));
                if (_rank>k){
                    cont+=_rank-k;
                }
            }
            else{
                cout<<"dimensionReductionQuality: sorted distance index problem with rank:"<<sortIndexHd(j)<<"  & id "<<j<<endl;
                i=i;
            }
        }
        
    }
    trustworthiness=1.0-(2.0/G)*trust;
    continuity=1.0-(2.0/G)*cont;
    return true;
}