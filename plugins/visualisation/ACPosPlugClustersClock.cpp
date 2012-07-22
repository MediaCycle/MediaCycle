/*
 *  ACPosPlugClustersClock.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 11/06/2012
 *
 *  @copyright (c) 2012 – UMONS - Numediart
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
 */

#include "ACPosPlugClustersClock.h"

#include <armadillo>
#include "Armadillo-utils.h"

using namespace arma;
using namespace std;

ACPosPlugClustersClock::ACPosPlugClustersClock() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "AudioCycle ClustersClock";
    this->mDescription = "\"ClustersClock\" Visualisation plugin";
    this->mId = "";
}

ACPosPlugClustersClock::~ACPosPlugClustersClock()
{
}

void ACPosPlugClustersClock::updateNextPositions(ACMediaBrowser* mediaBrowser){

    float maxNodeRadius = 0.2f;
    float labelsRadius = 0.3f;
    float radius = 0.0f;
    float angle = 0.0f;

    double maxDuration = 0.0f;
    vector<ACMedia*> medias = mediaBrowser->getLibrary()->getAllMedia();
    for(vector<ACMedia*>::iterator media = medias.begin();media!=medias.end();media++){
        if ( maxDuration < (*media)->getDuration())
            maxDuration = (*media)->getDuration();
    }

    for(vector<ACMedia*>::iterator media = medias.begin();media!=medias.end();media++){
        ACMediaNode &node = mediaBrowser->getNodeFromMedia(*media);
        radius = maxNodeRadius * (*media)->getDuration() / maxDuration;
        angle = (2*arma::math::pi() / (float)  mediaBrowser->getClusterCount()) * (float) node.getClusterId() + (arma::math::pi()/2);
        ACPoint p ( radius*cos(angle) , radius*sin(angle) , 0.0f);
        double t = getTime();
        node.setNextPosition(p,t);
    }

    if(mediaBrowser->getLabelSize() == mediaBrowser->getClusterCount()){
        mediaBrowser->displayAllLabels(true);
        for(int l=0;l<mediaBrowser->getClusterCount();l++){
            radius = labelsRadius;
            angle = (2*arma::math::pi() / (float) mediaBrowser->getClusterCount()) * (float) l + (arma::math::pi()/2);
            mediaBrowser->setLabelPosition(l, radius*cos(angle) , radius*sin(angle) , 0.0f);
        }

    }
}
