/*
 *  ACPosPlugArchipelAtollTimeline.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/09/11
 *  @copyright (c) 2011 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACPlugin.h"
#include "ACPosPlugArchipelAtollTimeline.h"

//#include <float.h> //FLT_MAX

using namespace arma;
using namespace std;

ACPosPlugArchipelAtollTimeline::ACPosPlugArchipelAtollTimeline() : ACVisPlugin2Desc()
{
    //vars herited from ACPlugin
    // XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_MIXED;
    //this->mPluginType = this->mPluginType|PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mName = "Archipel Atoll Timeline";
    this->mDescription = "Archipel Atoll Timeline Visualisation plugin: Atoll vs Year";
    this->mId = "";
    std::vector<std::string> year;
    year.push_back("Year");
    std::vector<std::string> atoll;
    atoll.push_back("First Atoll");
    this->addStringParameter("x",year[0],year,"Year assigned to x axis");//,boost::bind(&ACPosPlugArchipelAtollTimeline::assignedFeaturesChanged,this));
    this->addStringParameter("y",atoll[0],atoll,"Principal atoll assigned to y axis");//,boost::bind(&ACPosPlugArchipelAtollTimeline::assignedFeaturesChanged,this));
}

ACPosPlugArchipelAtollTimeline::~ACPosPlugArchipelAtollTimeline()
{
}

void ACPosPlugArchipelAtollTimeline::mediaCycleSet()
{
    // CF this will work only since/if we load feature plugins before this plugin...
    //this->updateAvailableFeatures();
}

/*bool ACPosPlugArchipelAtollTimeline::updateAvailableFeatures(){
    if(this->media_cycle){
        if(this->media_cycle->getLibrarySize()>0 ){
            if(this->hasStringParameterNamed("x"))
                this->updateStringParameter("x",feature_names.front(),feature_names);
            else
                this->addStringParameter("x",feature_names.front(),feature_names,"feature assigned to x axis",boost::bind(&ACVisPlugin2Desc::assignedFeaturesChanged,this));
            return true;
        }
    }
    return false;
}*/

void ACPosPlugArchipelAtollTimeline::updateNextPositions(ACMediaBrowser* mediaBrowser){
    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    #if defined (SUPPORT_MULTIMEDIA)
    if(!media_cycle->getLibrary()->containsMediaDocumentsOfIdentifier("archipel")){
        std::cerr << "ACPosPlugArchipelAtollTimeline::updateNextPositions: the library doesn't contain archipel documents."<< std::endl;
        return;
    }
    #else
    return;
    #endif
    ACVisPlugin2Desc::updateNextPositions(mediaBrowser);
}
