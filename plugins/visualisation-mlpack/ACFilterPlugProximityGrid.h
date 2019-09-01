/**
 * @brief Plugin for gridding nodes in a proximity grid
 * @author Christian Frisson
 * @date 23/02/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#include "MediaCycle.h"
#include "ACPlugin.h"
#include "ACMediaBrowser.h"

#ifndef _ACFilterPlugProximityGrid_
#define _ACFilterPlugProximityGrid_

class ACFilterPlugProximityGrid : virtual public ACFilteringPlugin {
public:
    ACFilterPlugProximityGrid();
    ~ACFilterPlugProximityGrid();
    virtual void filter();
    virtual void librarySizeChanged();
    void evalNeighborhoodness();
private:
    void setProximityGrid();
    void spiralSearch(int id, ACPoint p, std::string method);

protected:
    void extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, std::vector<std::string> &featureNames);
private:
    std::map<long,ACPoint> preFilterPositions;
    arma::imat cell_ids;
    int gridSize;
    std::vector<std::string> methods,distances,sortings;
    float min_x,min_y,max_x,max_y;
    std::map<long,bool> id_celled;
};

//Wrapper to prevent mlpack::emst duplicate symbols when compiling
extern arma::mat emst(arma::mat desc_m, bool naive=false, const size_t leafSize=1);

#endif // _ACFilterPlugProximityGrid_
