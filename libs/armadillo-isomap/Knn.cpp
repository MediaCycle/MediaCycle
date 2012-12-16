//
//  knn.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "knn.h"

using namespace arma;

bool knn::setDistanceMatrix(arma::mat D,arma::umat L){
    this->L=L;
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
    Dred.set_size(D.n_rows,L.n_rows);
    for (int i=0;i<L.n_rows;i++){
        Dred.col(i)=D.col(L(i,0));
    }
    return true;
}

bool knn::setFeatureMatrix(arma::mat F,arma::umat L){
    colvec sum_F=sum(pow(F,2),1);
    mat Dtemp=sqrt(sum_F*ones(1,sum_F.n_rows)+ones(sum_F.n_rows,1)*trans(sum_F)-2*(F*trans(F)));
    return this->setDistanceMatrix(Dtemp,L);
}

arma::ucolvec knn::compute(unsigned int k){
    k=fmin(k,L.n_rows);
    arma::ucolvec res(Dred.n_rows);
    uword nbClass=max(L.col(1))+1;
    uword lCpt=0;
    for (int i=0;i<Dred.n_rows;i++){
        if ((lCpt<L.n_rows)&&(L(lCpt,0)==i)){
            res(i)=L(lCpt,1);
            lCpt++;
            continue;
        }
        
        arma::ucolvec tagCpt(nbClass);
        tagCpt.zeros();
        arma::urowvec sortIndex=sort_index(Dred.row(i));
        for (int j=0;j<k;j++)
            tagCpt(L(sortIndex(j),1))++;
        tagCpt.max(res(i));
        
    }
    return res;
}