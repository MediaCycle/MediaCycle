/**
 * @brief Main application class for the Video Browser Showdown version of VideoCycle
 * @author Christian Frisson
 * @date 7/01/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACVideoBrowserShowdownOsgQt.h"

ACVideoBrowserShowdownOsgQt::ACVideoBrowserShowdownOsgQt() : ACMultiMediaCycleOsgQt() {
}

ACVideoBrowserShowdownOsgQt::~ACVideoBrowserShowdownOsgQt(){
}

 void ACVideoBrowserShowdownOsgQt::postLoadXML(){
    if(!this->media_cycle){
        std::cerr << "ACVideoBrowserShowdownOsgQt: no mediacycle" << std::endl;
         return;
    }

    if(this->media_cycle->getLibrarySize() == 0){
        std::cerr << "ACVideoBrowserShowdownOsgQt: library empty" << std::endl;
         return;
    }

    if(this->media_cycle->getLibrary()->getFirstMedia() == 0){
        std::cerr << "ACVideoBrowserShowdownOsgQt: first media not available" << std::endl;
         return;
    }

    if(this->View->getTimelineRenderer() == 0){
        std::cerr << "ACVideoBrowserShowdownOsgQt: no timeline" << std::endl;
         return;
    }
    ACMediaNode* node = this->media_cycle->getNodeFromMedia( this->media_cycle->getLibrary()->getFirstMedia() );
    if(node)
        node->setActivity(0);
//    if(this->View->getTimelineRenderer()->getNumberOfTracks() == 0)
//        this->View->toggleTimelineVisibility();
    if(this->media_cycle->getLibrarySize()==1){
        this->View->adjustTimelineHeight(1.0f);
    }
    else if(this->media_cycle->getLibrarySize()>1){
        this->View->adjustTimelineHeight(0.25f);
    }
    // Load the first media, supposedly the main media
    this->View->getTimelineRenderer()->addTrack( this->media_cycle->getLibrary()->getFirstMedia() );
    this->View->setFocus();
}


