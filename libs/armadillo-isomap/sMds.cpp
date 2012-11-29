//
//  sMds.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 25/10/12
//  @copyright (c) 2012 – UMONS - Numediart
//  
//  MediaCycle of University of Mons – Numediart institute is 
//  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
//  licence (the “License”); you may not use this file except in compliance 
//  with the License.
//  
//  This program is free software: you can redistribute it and/or 
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  Each use of this software must be attributed to University of Mons – 
//  Numediart Institute
//  
//  Any other additional authorizations may be asked to avre@umons.ac.be 
//  <mailto:avre@umons.ac.be>
//

#include <iostream>
#include "smds.h"
#include "mds.h"
#include "limits.h"

using namespace arma;

sMds::sMds(double a){
    this->alpha=a;
}


bool sMds::setFeatureMatrix(arma::mat F,arma::urowvec label){
    uword N=F.n_rows;
    if (N!=label.n_cols)
        return false;
    mat D(F.n_rows,F.n_rows) ;
    for (uword i=0;i<F.n_rows;i++){
        for (uword j=0;j<i;j++){
            D(j,i)=sqrt(sum(pow((F.row(i)-F.row(j)),2)));
        }
        D(i,i)=0.0;
    }
    D=symmatu(D);
    //cout<<D<<endl;
  //  cout<<"computedD:"<<endl<<D<<endl;
    return this->setDistanceMatrix(D,label);
}

bool sMds::setDistanceMatrix(arma::mat D,arma::urowvec label){
    uword N=D.n_rows;
    double beta=sum(sum(D))/(N*N);
    for (uword i=0;i<N;i++){
        for (uword j=0;j<i;j++){
            if(label[i]==label[j])
                D(j,i)=sqrt(1-exp(-pow(D(j,i),2)/beta));
            else
                D(j,i)=sqrt(exp(pow(D(j,i),2)/beta))-alpha;
            
            
            
        }
        D(i,i)=0.0;
        
    }
    D=symmatu(D);
    return mds::setDistanceMatrix(D);
}



sMds::~sMds(){}