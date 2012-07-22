/**
 * @brief ACPosPlugArchipelAtoll.cpp
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

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACPlugin.h"
#include "ACPosPlugArchipelAtoll.h"

//#include <float.h> //FLT_MAX

using namespace arma;
using namespace std;

//#if defined(SUPPORT_ARCHIPEL)
ACPosPlugArchipelAtoll::ACPosPlugArchipelAtoll() : ACClusterPositionsPropellerPlugin()
{
    //vars herited from ACPlugin
	// XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_ALL;
    //this->mPluginType =    this->mPluginType|PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mName = "Archipel Atoll";
    this->mDescription = "Archipel Atoll Visualisation plugin";
    this->mId = "";
	
    //local vars
}

ACPosPlugArchipelAtoll::~ACPosPlugArchipelAtoll()
{
}

void ACPosPlugArchipelAtoll::updateNextPositions(ACMediaBrowser* mediaBrowser){
	float lZoom=mediaBrowser->getCameraZoom();
	
        ACClusterPositionsPropellerPlugin::updateNextPositions(mediaBrowser);

	vector<ACMedia*> loops =  mediaBrowser->getLibrary()->getAllMedia();	
	vector<long> posParents;
	vector<long> posSegments;
	vector<long> posDocuments;

	for (int i=0; i < loops.size(); i++){
		if (loops[i]->getParentId() == -1){
			posParents.push_back(i);
			if(mediaBrowser->getLibrary()->getMedia(i)->getType() == MEDIA_TYPE_MIXED)
				posDocuments.push_back(i);
		}	
		else
			posSegments.push_back(i);
	}
		
	std::vector<ACMedia*> tmpSegments;
	float angle;
	float lDist=0.025;
	for (long i=0; i<posDocuments.size(); i++){
		tmpSegments = loops[posDocuments[i]]->getAllSegments();
		
		ACPoint parent = mediaBrowser->getMediaNode(posDocuments[i]).getNextPosition();
		int nCluster=mediaBrowser->getMediaNode(posDocuments[i]).getClusterId();
		//std::cout << "Media document " << posDocuments[i] << " with position " << parent.x << " " << parent.y << std::endl;

		for (int j=0; j<tmpSegments.size(); j++){
			ACMediaNode& node = mediaBrowser->getMediaNode(tmpSegments[j]->getId());

			angle = (2*arma::math::pi() / (float) tmpSegments.size()) * (float) j + (arma::math::pi()/2);
			
			ACPoint s;
			
			s.x = lDist * cos(angle) + parent.x;
			s.y = lDist * sin(angle) + parent.y;
			s.z = 0;
			
			double t = getTime();
			node.setNextPosition(s, t);
			//node.setClusterId(nCluster);
		
			//std::cout << "Media " << tmpSegments[j]->getId() << " of type " << ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(tmpSegments[j]->getType()) << " with position " << s.x << " " << s.y<< std::endl;				
			//std::cout << "angle = " << angle << std::endl;			
			//std::cout << "posDisp_m.row(tmpSegments[j]->getId())" << posDisp_m.row(tmpSegments[j]->getId()) << std::endl;
		}
	}
}
//#endif //defined(SUPPORT_ARCHIPEL)


