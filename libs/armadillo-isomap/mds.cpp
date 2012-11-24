//
//  mds.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 21/11/12
//
//

#include "mds.h"

using namespace arma;

bool mds::setDistanceMatrix(arma::mat D){
    if (D.is_square()==false)
        return false;
    arma::mat D2=arma::pow(D,2);
    int N=D.n_rows;
    arma::mat H=diagmat(ones(N,1))-ones(N,1)*ones(1,N)/N;
    this->S=-0.5*H*D2*H;
    if (S.is_square()==false)
        return false;
    
    return true;
}

bool mds::setFeatureMatrix(arma::mat F){
    
    this->S=(F-ones(F.n_rows,1)*mean(F));
    this->S*=this->S.t();
    if (S.is_square()==false)
        return false;
    return true;
}
arma::mat mds::compute(int ndim){
    int N=this->S.n_rows;
    if (ndim>N)
        return arma::mat(0,0);
    mat eigvec;
    colvec eigval;
    eig_sym(eigval, eigvec, this->S);
    umat index=sort_index(abs(eigval),1);
    mat ret(N,ndim);
    colvec val(ndim);
    for (int i=0;i<ndim;i++){
        ret.col(i)=eigvec.col(index(i));
        val(i)=eigval(index(i));
    }
    ret=ret*diagmat(sqrt(val));
    return ret;
}