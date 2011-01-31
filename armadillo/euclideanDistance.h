// euclideanDistance - computes Euclidean distance matrix
//
// E = euclideanDistance(A,B)
//
//    A - (MxD) matrix 
//    B - (NxD) matrix
//
// Returns:
//    E - (MxN) Euclidean distances between vectors in A and B
//
//
// Description : 
//    fully vectorized (VERY FAST!), computes the 
//    Euclidean distance between two vectors by:
//
//                 ||A-B|| = sqrt ( ||A||^2 + ||B||^2 - 2*A.B )
//
// Author   : Roland Bunschoten
//            University of Amsterdam
//            Intelligent Autonomous Systems (IAS) group
//            Kruislaan 403  1098 SJ Amsterdam
//            tel.(+31)20-5257524
//            bunschot@wins.uva.nl
// Last Rev : Oct 29 16:35:48 MET DST 1999
// Tested   : PC Matlab v5.2 and Solaris Matlab v5.3
// Thanx    : Nikos Vlassis

// Copyright notice: You are free to modify, extend and distribute 
//    this code granted that the author of the original code is 
//    mentioned as the original author of the code.


#ifndef EUCLIDEANDISTANCE_H
#define EUCLIDEANDISTANCE_H

#include "Armadillo-utils.h"

template<typename eT>
const arma::Mat<eT> euclideanDistance(const arma::Mat<eT>& A, const arma::Mat<eT>& B){
	if (A.n_cols != B.n_cols){
		std::cerr << "A and B should be of the same dimensionality" << std::endl;
		exit(1);
	}
	arma::Mat<eT> AA = arma::sum(A % A, 1); 
	arma::Mat<eT> BB = arma::sum(B % B, 1); 
	arma::Mat<eT> AB = A*arma::trans(B); 
	arma::Mat<eT> D = sqrt(abs(arma::repmat(AA, 1, BB.n_rows) + repmat(arma::trans(BB), AA.n_rows, 1) - 2*AB));
	return D;
}

#endif
