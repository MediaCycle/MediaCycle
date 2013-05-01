//
//  sIsomap.h
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
// Reference:
//Supervised nonlinear dimensionality reduction for visualization and classification.
//by: Xin Geng, De-Chuan Zhan, and Zhi-Hua Zhou
//In: IEEE Transactions on Systems, Man, and Cybernetics, Part B, Vol. 35, Nr. 6 (2005) , p. 1098-1107.
//

#ifndef MediaCycle_sIsomap_h
#define MediaCycle_sIsomap_h

#include <armadillo>
#include "mds.h"
#include "Isomap.h"

class sIsomap: public Isomap {
public:
    sIsomap(double a=0.5);
    ~sIsomap();
    bool setDistanceMatrix(arma::mat D,arma::urowvec label, char n_fct,double param);
    bool setFeatureMatrix(arma::mat F,arma::urowvec label,char n_fct,double param);
protected:
    double alpha;
};


#endif
