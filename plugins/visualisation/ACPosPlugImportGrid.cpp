/**
 * @brief Plugin for ordering nodes in a grid from their import rank, a non content-based approach
 * @author Christian Frisson
 * @date 14/05/2013
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

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACPosPlugImportGrid.h"

#include <algorithm>

using namespace arma;
using namespace std;

ACPosPlugImportGrid::ACPosPlugImportGrid() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Import Grid";
    this->mDescription = "Visualization plugin ordering media nodes in a grid from their import rank";
    this->mId = "";

    this->addNumberParameter("Landmower mode",0,0,1,1,"Landmower mode",boost::bind(&ACPosPlugImportGrid::updateGrid,this));
}

ACPosPlugImportGrid::~ACPosPlugImportGrid(){

}

void ACPosPlugImportGrid::updateGrid()
{
    if(media_cycle)
        this->updateNextPositions(media_cycle->getBrowser());
}

void ACPosPlugImportGrid::updateNextPositions(ACMediaBrowser* mediaBrowser){
    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = mediaBrowser->getLibrary()->getSize();
    if(media_cycle->isImporting())
        libSize += media_cycle->getLibrary()->getNumberOfFilesToImport() - media_cycle->getLibrary()->getNumberOfFilesProcessed();

    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugImportGrid::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    int gridSize = ceil(sqrt(libSize));
    if(gridSize == 0)
        return;

    std::cout << "gridSize " << gridSize << " for " << libSize << " elements " << std::endl;

    ACPoint p;
    int row = 0;
    float osg = 0.33f;

    bool landmower = this->getNumberParameterValue("Landmower mode");

    for (int i=0; i<ids.size(); i++){
        //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
        if(i%gridSize==0){
            row++;
        }
        if(landmower)
            p.x = (-osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1))*pow(-1,row+1);
        else
            p.x = -osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1);
        p.y = osg -2*osg*(float)(row-1)/(float)(gridSize-1);
        p.z = 0;
        mediaBrowser->setNodeNextPosition(ids[i], p);
        //std::cout << "ACPosPlugImportGrid::updateNextPositions: media " << ids[i] << " i%gridSize " << i%gridSize << " x " << p.x << " y " << p.y << std::endl;
    }
}
