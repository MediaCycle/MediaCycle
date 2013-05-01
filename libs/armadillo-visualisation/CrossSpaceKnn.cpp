//
//  CrossSpaceKnn.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "CrossSpaceKnn.h"

using namespace arma;

bool CrossSpaceKnn::setDistanceMatrix1(arma::mat D,arma::umat L){
    if (this->L.n_elem>0){
        if (sum(sum(this->L!=L)))
            return false;
    }
    else
        this->L=L;
    if (!D.is_finite()){
        return false;
        
    }
    if (D.is_square()==false)
        return false;
    if (L.n_cols!=2)
        return false;
    if (D.n_elem>0){
        if (L.n_rows>D.n_rows)
            return false;
        if (max(L.col(0))>=D.n_rows)
            return false;
    }
    if (sum(L.col(0)==sort(L.col(0)))==0)
        return false;
    Dred1.set_size(D.n_rows,L.n_rows);
    for (int i=0;i<L.n_rows;i++){
        Dred1.col(i)=D.col(L(i,0));
    }
    return true;
}

bool CrossSpaceKnn::setFeatureMatrix1(arma::mat F,arma::umat L){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    Dtemp.diag()=zeros(Dtemp.n_rows,1);
    Dtemp=sqrt(Dtemp);
    return this->setDistanceMatrix1(Dtemp,L);
}


bool CrossSpaceKnn::setDistanceMatrix2(arma::mat D,arma::umat L){
    if (this->L.n_elem>0)
    {
        if (sum(sum((this->L)!=L)))
            return false;
    }
    else
    this->L=L;

    if (!D.is_finite()){
        return false;
    }
    if (D.is_square()==false)
        return false;
    if (L.n_cols!=2)
        return false;
    if (D.n_elem>0){
        if (L.n_rows>D.n_rows)
            return false;
        if (max(L.col(0))>=D.n_rows)
            return false;
    }
    if (sum(L.col(0)==sort(L.col(0)))==0)
        return false;
    Dred2.set_size(D.n_rows,L.n_rows);
    for (int i=0;i<L.n_rows;i++){
        Dred2.col(i)=D.col(L(i,0));
    }
    return true;
}

bool CrossSpaceKnn::setFeatureMatrix2(arma::mat F,arma::umat L){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    Dtemp.diag()=zeros(Dtemp.n_rows,1);
    Dtemp=sqrt(Dtemp);
    return this->setDistanceMatrix2(Dtemp,L);
}

arma::ucolvec CrossSpaceKnn::compute(unsigned int k){
    
    k=fmin(k,Dred1.n_cols-1);
    unsigned int N=Dred1.n_rows;
    arma::ucolvec res(N);
    uword nbClass=max(L.col(1))+1;
    uword lCpt=0;
    int test=0;
    for (int i=0;i<N;i++){
        /*if ((lCpt<L.n_rows)&&(L(lCpt,0)==i)){
            res(i)=L(lCpt,1);
            lCpt++;
            continue;
        }*/
        
        arma::ucolvec tagCpt(nbClass);
        tagCpt.zeros();
        arma::urowvec sortIndex1=sort_index(Dred1.row(i));
        arma::urowvec sortIndex2=sort_index(Dred2.row(i));
        /*if (L(sortIndex1(0),0)==i&&L(sortIndex2(0),0)==i){
            for (int j=1;j<=k;j++){
                tagCpt(L(sortIndex1(j),1))++;
            }
            for (int j=1;j<=k;j++){
                tagCpt(L(sortIndex2(j),1))++;
            }
        }
        else*/{
            for (int j=0;j<k;j++){
                tagCpt(L(sortIndex1(j),1))++;
            }
            for (int j=0;j<k;j++){
                tagCpt(L(sortIndex2(j),1))++;
            }
        }
        tagCpt.max(res(i));
        
    }
    //cout<<"("<<test<<")";
    return res;
}