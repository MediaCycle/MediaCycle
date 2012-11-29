/*
 *  ACTagKMeansPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
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

#include "ACTagKMeansPlugin.h"

ACTagKMeansPlugin::ACTagKMeansPlugin()
: ACKMeansPlugin() {
    this->mName = "MediaCycle KMeans & Tag";
    this->mDescription = "Plugin for KMeans Clustering  with tag media information exploitation";
}

ACTagKMeansPlugin::~ACTagKMeansPlugin() {
}


void ACTagKMeansPlugin::updateClusters(ACMediaBrowser* mediaBrowser,bool needsCluster){
    ACKMeansPlugin::updateClusters(mediaBrowser,needsCluster);

    ACMediaLibrary *library=mediaBrowser->getLibrary();
    vector<int> currId;
    //    for (int i=0;i<currTempId.size();i++){
    for (ACMediaNodes::const_iterator it=mediaBrowser->getMediaNodes().begin(); it !=mediaBrowser->getMediaNodes().end();it++){
        if(it->second){
            if ((library->getMedia(it->first)->getMediaType() == library->getMediaType()) &&(it->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()))
                currId.push_back(it->first);
        }
    }
    int object_count = currId.size();

	for(int i=0; i<object_count; i++) {
        // check if we should include this object
        if(mediaBrowser->getMediaNode(currId[i])->getNavigationLevel() < mediaBrowser->getNavigationLevel()) continue;
        int classId=library->getMediaTaggedClassId(currId[i]);
        if (classId!=-1){
            mediaBrowser->getMediaNode(currId[i])->setClusterId(classId);
        }
        
    }

    
}
