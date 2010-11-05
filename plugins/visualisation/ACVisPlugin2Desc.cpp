/**
 * @brief ACVisPlugin2Desc.cpp
 * @author Stéphane Dupont
 * @date 05/11/2010
 * @copyright (c) 2010 – UMONS - Numediart
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
#include "ACVisPlugin2Desc.h"

using namespace arma;
using namespace std;

ACVisPlugin2Desc::ACVisPlugin2Desc()
{
    //vars herited from ACPlugin
	// XS TODO: are these general enough ? can we use this only for video ??
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_CLUSTERS_POSITIONS;//CF or PLUGIN_TYPE_ANYMODE_POSITIONS?
    this->mName = "Vis2Desc";
    this->mDescription = "2 features Visualisation plugin";
    this->mId = "";
	
    //local vars
}

ACVisPlugin2Desc::~ACVisPlugin2Desc()
{
}

int ACVisPlugin2Desc::initialize()
{
    std::cout << "ACVisPlugin2Desc initialized" << std::endl;
    return 0;
}


void ACVisPlugin2Desc::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int itemClicked, labelClicked, action;
	vector<string> featureNames;
	int libSize = mediaBrowser->getLibrary()->getSize();
	itemClicked = mediaBrowser->getClickedNode();
	labelClicked = mediaBrowser->getClickedLabel();
	
	mat desc_m, descD_m;
	mat posDisp_m;	

	extractDescMatrix(mediaBrowser, desc_m, featureNames);
	if (desc_m.n_cols < 2){
		std::cout << "Not enough features for this display" << std::endl;
		return;
	}
		
	for (int i=0; i< featureNames.size(); i++)
		std::cout << "featureNames : " << featureNames[i] << std::endl;

	mediaBrowser->setNumberOfDisplayedLoops(desc_m.n_rows);

	ACPoint p;
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopIsDisplayed(i, true);
	  // TODO: make sure you meant next
	  p.x = desc_m(i,0)*.1;
	  p.y = desc_m(i,1)*.1;
	  p.z = 0;
		mediaBrowser->setNodeNextPosition(i, p);
  }
	////////////////////////////////////////////////////////////////
}


void ACVisPlugin2Desc::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getAllMedia();
  int nbMedia = loops.size(); 
	int featDim;
	int totalDim = 0;
	
	// Count nb of feature
	int nbFeature = loops.back()->getNumberOfFeaturesVectors();
	for(int f=0; f< nbFeature; f++){
		featureNames.push_back(loops.back()->getFeaturesVector(f)->getName());
		featDim = loops.back()->getFeaturesVector(f)->getSize();
		for(int d=0; d < featDim; d++){
			totalDim++;
		}
	}
	
  desc_m.set_size(nbMedia,totalDim);
  mat pos_m(nbMedia,2);
  
  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
    for(int f=0; f< nbFeature; f++){
			std::cout << f << std::endl;
			featDim = loops.back()->getFeaturesVector(f)->getSize();
			for(int d=0; d < featDim; d++){
				desc_m(i,tmpIdx) = loops[i]->getFeaturesVector(f)->getFeatureElement(d);
				tmpIdx++;
      }
    }
  }
  // normalizing features between 0 and 1 ///////////////////////////////////////
//   rowvec maxDesc_v = max(desc_m);
//   rowvec minDesc_v = min(desc_m);
//   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
//   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
	/////////////////////////////////////////////////////////////////////////////////
}
