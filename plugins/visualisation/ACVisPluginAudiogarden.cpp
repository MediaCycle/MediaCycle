/**
 * @brief ACVisPluginAudiogarden.cpp
 * @author Damien Tardieu
 * @date 05/05/2010
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
#include "ACVisPluginAudiogarden.h"

using namespace arma;
using namespace std;

ACVisPluginAudiogarden::ACVisPluginAudiogarden()
{
    //vars herited from ACPlugin
	// XS TODO: are these general enough ? can we use this only for video ??
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "VisAudiogarden";
    this->mDescription = "Audiogarden Visualisation plugin";
    this->mId = "";
	
    //local vars
}

ACVisPluginAudiogarden::~ACVisPluginAudiogarden()
{
}

int ACVisPluginAudiogarden::initialize()
{
    std::cout << "ACVisPluginAudiogarden initialized" << std::endl;
    return 0;
}


void ACVisPluginAudiogarden::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int itemClicked, labelClicked, action;
	vector<string> featureNames;
	int libSize = mediaBrowser->getLibrary()->getSize();
	itemClicked = mediaBrowser->getClickedNode();
	labelClicked = mediaBrowser->getClickedLabel();
	
	mat desc_m, descD_m;
	mat posDisp_m(libSize, 2);
	colvec r_v(libSize);
	colvec theta_v = rand<colvec>(libSize);
	theta_v.ones(libSize); //theta_v * 2 * math::pi();
  vector<ACMedia*> loops =  mediaBrowser->getLibrary()->getAllMedia();	
	for (int i=0; i<libSize; i++){
		r_v(i) = 1./log(1+loops[i]->getDuration());
	}
	r_v = r_v/max(r_v) *.1;
	extractDescMatrix(mediaBrowser, desc_m, featureNames);
	theta_v = desc_m.col(0);
	theta_v = (theta_v - min(theta_v))/(max(theta_v)-min(theta_v)) * 2 * math::pi();
	std::cout << theta_v << std::endl;

	
	mediaBrowser->setNumberOfDisplayedLoops(desc_m.n_rows);
	
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopIsDisplayed(i, true);
	  // TODO: make sure you meant next
		mediaBrowser->setNodeNextPosition(i, r_v(i) * cos(theta_v(i)), r_v(i) * sin(theta_v(i)));
  }
	////////////////////////////////////////////////////////////////
}


void ACVisPluginAudiogarden::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getAllMedia();
  int nbMedia = loops.size(); 
	int featDim;
	int totalDim = 0;
	
	// Count nb of feature
	int nbFeature = loops.back()->getNumberOfFeaturesVectors();
	for(int f=0; f< nbFeature; f++){
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
			featureNames.push_back(loops.back()->getFeaturesVector(f)->getName());
			for(int d=0; d < featDim; d++){
				std::cout << loops[i]->getFeaturesVector(f)->getFeatureElement(d) << std::endl;
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
