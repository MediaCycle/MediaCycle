/**
 * @brief ACNeighborhoodsPluginRandom.cpp
 * @author Christian Frisson
 * @date 22/07/2012
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

#include "ACNeighborhoodsPluginRandom.h"

//using namespace arma;
using namespace std;

ACNeighborhoodsPluginRandom::ACNeighborhoodsPluginRandom() : ACNeighborMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Random";
    this->mDescription = "Plugin for the computation of random neighborhoods";
    this->mId = "";
	
    //local vars
}

ACNeighborhoodsPluginRandom::~ACNeighborhoodsPluginRandom() {
}

void ACNeighborhoodsPluginRandom::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	//int _clickedloop = mediaBrowser->getClickedNode();
    //std::cout << "ACNeighborhoodsPluginRandom::updateNeighborhoods" << std::endl;
    //std::cout << "Last clicked Node = " << mediaBrowser->getUserLog()->getLastClickedNodeId() << std::endl;
	if (mediaBrowser->getUserLog()->getLastClickedNodeId() == -1) { 
        //std::cout << "ACNeighborhoodsPluginRandom: filling tree..." << std::endl;
		// initialize -  node #0                      // node ID
		mediaBrowser->getUserLog()->addRootNode(0, 0); // 0
		// clicked on node 0 at time 0
		mediaBrowser->getUserLog()->clickNode(0,0);

        //CF: NB 19 audio samples on the mercurialized dataset
        // add his neighbors
        mediaBrowser->addNode(0, 1, 0); // 1
        mediaBrowser->addNode(0, 2, 0); // 2
        mediaBrowser->addNode(0, 3, 0); // 3
        mediaBrowser->addNode(0, 4, 0); // 4
        mediaBrowser->addNode(0, 5, 0);  // 5

        // CF test for positions
        // /*
        mediaBrowser->addNode(0, 6, 0);
        mediaBrowser->addNode(0, 7, 0);
        mediaBrowser->addNode(0, 8, 0);
        mediaBrowser->addNode(0, 9, 0);
        mediaBrowser->getUserLog()->clickNode(4,1);
        mediaBrowser->addNode(4, 1, 0);
        mediaBrowser->addNode(4, 2, 0);
        mediaBrowser->addNode(4, 3, 0);
        mediaBrowser->addNode(4, 4, 0);
        mediaBrowser->addNode(4, 5, 0);
        mediaBrowser->addNode(4, 6, 0);
        mediaBrowser->addNode(4, 7, 0);
        mediaBrowser->addNode(4, 8, 0);
        mediaBrowser->addNode(4, 9, 0);

        mediaBrowser->getUserLog()->clickNode(6,2);
        mediaBrowser->addNode(6, 1, 0);
        mediaBrowser->addNode(6, 2, 0);
        mediaBrowser->addNode(6, 3, 0);
        mediaBrowser->addNode(6, 4, 0);
        mediaBrowser->addNode(6, 5, 0);
        mediaBrowser->addNode(6, 6, 0);
        mediaBrowser->addNode(6, 7, 0);
        mediaBrowser->addNode(6, 8, 0);
        mediaBrowser->addNode(6, 9, 0);
        // */
        /*
        // clicked on node 3 at time 1
        mediaBrowser->getUserLog()->clickNode(3,1);

        // add his neighbors
        mediaBrowser->addNode(3, 0, 0);  // 6 == 0
        mediaBrowser->addNode(3, 9, 0); // 9
        mediaBrowser->addNode(3, 10, 0) ; // 10

        mediaBrowser->addNode(1, 11, 0); // 11
        mediaBrowser->addNode(1, 12, 0);  // 12

        mediaBrowser->addNode(2, 13, 0); // 13
        mediaBrowser->addNode(2, 14, 0); // 14
        mediaBrowser->addNode(2, 15, 0) ; // 15


        mediaBrowser->addNode(13, 16, 0); // 16
        mediaBrowser->addNode(13, 17, 0); // 17
        mediaBrowser->addNode(13, 18, 0) ; // 18
        */

        //mediaBrowser->getUserLog()->dump();
	}	
}
