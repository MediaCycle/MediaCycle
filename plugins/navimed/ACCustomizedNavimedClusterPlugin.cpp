/*
 *  ACCustomizedNavimedClusterPlugin.cpp
 *  MediaCycle
 *
 *  @author Ravet Thierry
 *  @date 11/10/2010
 *  @copyright (c) 2010 – UMONS - Numediart
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

#include <float.h>

#include "ACCustomizedNavimedClusterPlugin.h"

ACCustomizedNavimedClusterPlugin::ACCustomizedNavimedClusterPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "Navimed Cluster Plugin";
    this->mDescription = "Navimed Cluster Plugin";
    this->mId = "";
    this->addNumberParameter("clusters",5,1,10,1,"number of desired clusters",boost::bind(&ACCustomizedNavimedClusterPlugin::clusterNumberChanged,this));
}

ACCustomizedNavimedClusterPlugin::~ACCustomizedNavimedClusterPlugin() {
}


void ACCustomizedNavimedClusterPlugin::updateClusters(ACMediaBrowser* mediaBrowser,bool needsCluster){
	int clusterCount=mediaBrowser->getClusterCount();
    int object_count=mediaBrowser->getNumberOfMediaNodes();
    
	ACMediaLibrary *library=mediaBrowser->getLibrary();
	
	for(int i=0; i<object_count; i++) {
		// check if we should include this object
		if(mediaBrowser->getMediaNode(i)->getNavigationLevel() < mediaBrowser->getNavigationLevel()) continue;
		
		// compute distance between this object and every cluster
		mediaBrowser->getMediaNode(i)->setClusterId(library->getMedia(i)->getFeaturesVector(0)->getFeatureElement(clusterCount-1));
	}
}

void ACCustomizedNavimedClusterPlugin::clusterNumberChanged(){
    if(!this->media_cycle) return;
    
    media_cycle->setClusterNumber( this->getNumberParameterValue("clusters") );
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}
