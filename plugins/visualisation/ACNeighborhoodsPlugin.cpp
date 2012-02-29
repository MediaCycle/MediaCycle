/**
 * @brief ACNeighborhoodsPlugin.cpp
 * @author Christian Frisson
 * @date 29/02/2012
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

#include "ACNeighborhoodsPlugin.h"

//using namespace arma;
using namespace std;

ACNeighborhoodsPlugin::ACNeighborhoodsPlugin() {
    //this->mMediaType = MEDIA_TYPE_MIXED; // ALL
   // this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "Neighborhoods";
    this->mDescription = "Plugin for the computation of  neighborhoods";
    this->mId = "";
	
    //local vars
}

ACNeighborhoodsPlugin::~ACNeighborhoodsPlugin() {
}

void ACNeighborhoodsPlugin::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	int _clickednode = mediaBrowser->getClickedNode();
	// TODO: pareto
}
