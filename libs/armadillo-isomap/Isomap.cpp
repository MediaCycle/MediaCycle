//
//  Isomap.cpp
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
#include "Isomap.h"
#include "mds.h"
#include "limits.h"

using namespace arma;


Isomap::Isomap(){
}


bool Isomap::setFeatureMatrix(arma::mat F,char n_fct,double param){
    uword N=F.n_rows;
    
    //colvec ff=sum(pow(F,2),1);
    //mat S=F*F.t();
    //mat D = sqrt(repmat(ff,1,N) + repmat(ff.t(),N,1) - 2*S);
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
    return this->setDistanceMatrix(D,n_fct,param);
}

bool Isomap::setDistanceMatrix(arma::mat D,char n_fct,double param){
    
    this->n_fcn=n_fct;
    if (n_fcn == 'k')
        Kn=param;
    else
        if (n_fcn == 'e')
            epsilon=param;
    
    if (D.is_square()==false)
        return false;
    //cout<<"diag:"<<endl<<D;
    double tr=trace(abs(D));
    if (trace(abs(D))!=0)
        return false;
    if (prod(prod(D!=symmatu(D))))
        return false;
        
    uword N=D.n_cols;
    if (n_fcn == 'k'){
        for (uword i=0;i<N;i++){
            umat index=sort_index(D.col(i));
            
            for (uword j=1+Kn;j<N;j++)
                D.at(index.at(j),i) = datum::inf ;
        }
    }
    else
        if (n_fcn == 'e'){
            for (uword i=0;i<N;i++){
                umat index=find(D.col(i)>epsilon);
                
                for (uword j=0;j<index.n_cols;j++)
                    D.at(j,i) = datum::inf ;
            }
        }
    
    for (uword i=0;i<N;i++){
        
        mat::col_iterator a = D.begin_col(i);
        mat::row_iterator jt = D.begin_row(i);
        mat::col_iterator b = D.end_col(i);
        for(mat::col_iterator it=a; it!=b; ++it)
        {
            
            (*it)=fmin((*it),(*jt));
            ++jt;
        }
    }
    for (uword k=0;k<N;++k){
        for (uword j=0;j<N;j++){
            for(uword i=0;i<N;i++){
                D.at(i,j)=fmin(D.at(i,j),D.at(i,k)+D.at(k,j));
            }
        }
    }
    //cout<<"matD:"<<endl<<D<<endl;
    if (sum(sum(D==datum::inf)))
        return false;
    
    return mds::setDistanceMatrix(D);
}



Isomap::~Isomap(){}