/**
 * @brief ACNeighborhoodsPluginRandom.cpp
 * @author Christian Frisson
 * @date 03/08/2012
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
    //int _clickednode = mediaBrowser->getClickedNode();
    //std::cout << "ACNeighborhoodsPluginRandom::updateNeighborhoods" << std::endl;

    if(mediaBrowser->getLibrary()->getSize() < 10 ){
        std::cerr << "ACNeighborhoodsPluginRandom::updateNeighborhoods: requires a library with at least 10 elements." << std::endl;
        return;
    }

    //CF: NB 19 audio samples on the mercurialized dataset
    // add his neighbors
    long refNode = mediaBrowser->getReferenceNode();
    mediaBrowser->addNeighborNode(refNode, 1, 0); // 1
    mediaBrowser->addNeighborNode(refNode, 2, 0); // 2
    mediaBrowser->addNeighborNode(refNode, 3, 0); // 3
    mediaBrowser->addNeighborNode(refNode, 4, 0); // 4
    mediaBrowser->addNeighborNode(refNode, 5, 0);  // 5

    // CF test for positions
    // /*
    mediaBrowser->addNeighborNode(refNode, 6, 0);
    mediaBrowser->addNeighborNode(refNode, 7, 0);
    mediaBrowser->addNeighborNode(refNode, 8, 0);
    mediaBrowser->addNeighborNode(refNode, 9, 0);
    mediaBrowser->setClickedNode(4);
    mediaBrowser->addNeighborNode(4, 1, 0);
    mediaBrowser->addNeighborNode(4, 2, 0);
    mediaBrowser->addNeighborNode(4, 3, 0);
    mediaBrowser->addNeighborNode(4, 4, 0);
    mediaBrowser->addNeighborNode(4, 5, 0);
    mediaBrowser->addNeighborNode(4, 6, 0);
    mediaBrowser->addNeighborNode(4, 7, 0);
    mediaBrowser->addNeighborNode(4, 8, 0);
    mediaBrowser->addNeighborNode(4, 9, 0);

    mediaBrowser->setClickedNode(6);
    mediaBrowser->addNeighborNode(6, 1, 0);
    mediaBrowser->addNeighborNode(6, 2, 0);
    mediaBrowser->addNeighborNode(6, 3, 0);
    mediaBrowser->addNeighborNode(6, 4, 0);
    mediaBrowser->addNeighborNode(6, 5, 0);
    mediaBrowser->addNeighborNode(6, 6, 0);
    mediaBrowser->addNeighborNode(6, 7, 0);
    mediaBrowser->addNeighborNode(6, 8, 0);
    mediaBrowser->addNeighborNode(6, 9, 0);
    // */
    /*
        // clicked on node 3 at time 1
        mediaBrowser->setClickedNode(3);

        // add his neighbors
        mediaBrowser->addNeighborNode(3, 0, 0);  // 6 == 0
        mediaBrowser->addNeighborNode(3, 9, 0); // 9
        mediaBrowser->addNeighborNode(3, 10, 0) ; // 10

        mediaBrowser->addNeighborNode(1, 11, 0); // 11
        mediaBrowser->addNeighborNode(1, 12, 0);  // 12

        mediaBrowser->addNeighborNode(2, 13, 0); // 13
        mediaBrowser->addNeighborNode(2, 14, 0); // 14
        mediaBrowser->addNeighborNode(2, 15, 0) ; // 15


        mediaBrowser->addNeighborNode(13, 16, 0); // 16
        mediaBrowser->addNeighborNode(13, 17, 0); // 17
        mediaBrowser->addNeighborNode(13, 18, 0) ; // 18
        */

    //mediaBrowser->dumpNeighborNodes();
}
