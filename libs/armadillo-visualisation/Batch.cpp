//
//  Batch.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "Batch.h"
#include <time.h>

using namespace arma;
Batch::Batch(){
    srand ( time(NULL) );
}
bool Batch::setTagVector(arma::ucolvec L){
    this->L.set_size(L.n_rows,2);
    this->L.col(0)=linspace<ucolvec>(0, L.n_rows-1,L.n_rows);
    this->L.col(1)=L;
    
    return true;
    
}


bool Batch::setTagVector(arma::umat L){
    if (L.n_cols!=2)
        return false;
    this->L=L;
    return true;
}

arma::umat Batch::select(float procent,bool balancedTaggedClass){
        arma::umat res;
    if (balancedTaggedClass){
        uword sampleNb=((float)L.n_rows*procent);
        arma::umat tempRes(sampleNb,2);
        uword cpt=0;
        for (int i=0;i<=max(L.col(1));i++){
            ucolvec indexVec=find(L.col(1)==i);
            uword locSplNb=((float)indexVec.n_rows*procent);
            indexVec=shuffle(indexVec);
            for (int j=0;j<locSplNb;j++)
                tempRes.row(cpt+j)=L.row(indexVec(j));
            cpt+=locSplNb;
        }
        ucolvec indexTotVec=sort_index(tempRes.col(0).subvec(0,cpt-1));
        res.set_size(cpt,2);
        for (int i=0;i<cpt;i++)
            res.row(i)=tempRes.row(indexTotVec(i));
        
    }
    else {
        uword sampleNb=((float)L.n_rows*procent);
        res.set_size(sampleNb,2);
        umat shuf=shuffle(L,0);
        shuf=shuf.submat(0,0,sampleNb-1,1);
        ucolvec indexVec=sort_index(shuf.col(0));
        for (int i=0;i<sampleNb;i++)
            res.row(i)=shuf.row(indexVec(i));
    }
    return res;
}

