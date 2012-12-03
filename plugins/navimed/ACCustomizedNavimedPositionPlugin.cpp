/**
 * @brief ACCustomizedNavimedPositionPlugin.cpp
 * @author Christian Frisson
 * @date 04/12/2012
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


#include "ACPlugin.h"
#include "ACCustomizedNavimedPositionPlugin.h"

#include <float.h> //FLT_MAX

using namespace arma;
using namespace std;

ACCustomizedNavimedPositionPlugin::ACCustomizedNavimedPositionPlugin() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
	// XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_ALL;
    //this->mPluginType =    this->mPluginType|PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mName = "Navimed Position Plugin";
    this->mDescription = "Navimed Position Plugin";
    this->mId = "";
	
    //local vars
}

ACCustomizedNavimedPositionPlugin::~ACCustomizedNavimedPositionPlugin()
{
}

void ACCustomizedNavimedPositionPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
	int clusterCount=mediaBrowser->getClusterCount();
    int navigationLevel = mediaBrowser->getNavigationLevel();
     
    for (ACMediaNodes::const_iterator node = mediaBrowser->getMediaNodes().begin(); node != mediaBrowser->getMediaNodes().end(); ++node) {
        if(node->second->getNavigationLevel() < navigationLevel) continue;
        
        int ci = node->second->getClusterId();
        
        // SD TODO - test both approaches
        float r=0.25f+0.25f*(float)rand()/RAND_MAX;
        float dt=0.5f*(float)rand()/RAND_MAX;
        ACPoint p;
        p.x = p.y = p.z = 0.0;
        
        float theta = (ci + dt) * 2 * M_PI / (float)clusterCount;
        
        //p.x = 4*sin(theta)*r;//CF dirty trick to optimize the space, waiting for better ;)
        p.x = sin(theta)*r;
        //p.y = 4*cos(theta)*r;//CF dirty trick to optimize the space, waiting for better ;)
        p.y = cos(theta)*r;
        p.z = 0.0;
        
        //printf("computed next position: theta:%f,r=%f,  (%f %f %f)\n", theta, r, p.x, p.y, p.z);//CF free the console
        double t = getTime();
        node->second->setNextPosition(p, t); // (*node) is const hence getMediaNode( getNodeId ) allows node modification
        
#ifdef USE_DEBUG
        //cout<<"media n°"<<node->getMediaId()<<" cluster:"<<node->getClusterId()<<" x="<<p.x<<" y="<<p.y<<endl;
#endif
    }

}
