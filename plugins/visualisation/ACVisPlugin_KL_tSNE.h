/**
 * @brief ACVisPlugin_KL_tSNE.h
 * @author Thierry Ravet
 * @date 15/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include <armadillo>

#include "MediaCycle.h"
#include "ACMediaBrowser.h"
#include "ACDistribKLVisPlugin.h"

#ifndef _ACVisPlugin_KL_tSNE_
#define _ACVisPlugin_KL_tSNE_

class ACVisPlugin_KL_tSNE : public ACDistribKLVisPlugin {
public:
  ACVisPlugin_KL_tSNE();
  ~ACVisPlugin_KL_tSNE();

//  virtual void updateNextPositions(ACMediaBrowser* );
    virtual void  dimensionReduction(arma::mat &posDisp_m,arma::mat desc_m,arma::urowvec tag);
    
    void perplexityValueChanged(void);
protected:
    //	void extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, std::vector<std::string> &featureNames);
    double perplexity;
};

#endif	/* _ACVisPlugin_KL_tSNE_ */


