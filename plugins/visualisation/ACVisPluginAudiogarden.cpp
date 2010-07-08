/**
 * @brief ACVisPluginAudiogarden.cpp
 * @author Damien Tardieu
 * @date 09/07/2010
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
	// XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mPluginType = PLUGIN_TYPE_CLUSTERS_POSITIONS;
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
	vector<string> featureList;
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

	featureList.push_back("Mean of MFCC");
	//	featureList.push_back("Mean of Spectral Flatness");
	//featureList.push_back("Interpolated Energy");

	desc_m = extractDescMatrix(mediaBrowser->getLibrary(), featureList);
	mat coef_m;
	mat proj_m;
	princomp(coef_m, proj_m, desc_m);
	theta_v = proj_m.col(0);
	theta_v = (theta_v - min(theta_v))/(max(theta_v)-min(theta_v)) * 2 * math::pi();
	//	std::cout << theta_v << std::endl;

	
	mediaBrowser->setNumberOfDisplayedLoops(desc_m.n_rows);
	
	posDisp_m.col(0) = r_v % cos(theta_v);
	posDisp_m.col(1) = r_v % sin(theta_v);
	posDisp_m = zscore(posDisp_m)*.1;
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopIsDisplayed(i, true);
	  // TODO: make sure you meant next
		mediaBrowser->setNodeNextPosition(i, posDisp_m(i,0), posDisp_m(i,1));
  }
	//   for (int i=0; i<libSize; i++){
	//     mediaBrowser->setLoopIsDisplayed(i, true);
	// 	  // TODO: make sure you meant next
	// 		mediaBrowser->setNodeNextPosition(i, r_v(i) * cos(theta_v(i)), r_v(i) * sin(theta_v(i)));
	//   }
	////////////////////////////////////////////////////////////////
}

mat ACVisPluginAudiogarden::extractDescMatrix(ACMediaLibrary* lib, string featureName){
  vector<ACMedia*> loops = lib->getAllMedia();
  int nbMedia = loops.size(); 
	int featureId = 0;
	int featureSize = 0;

	int nbFeature = loops.back()->getNumberOfFeaturesVectors();

	for(int f=0; f< nbFeature; f++){
		if (loops.back()->getFeaturesVector(f)->getName() == featureName){
			featureId = f;
		}	
	}

	featureSize = loops.back()->getFeaturesVector(featureId)->getSize();
	
  mat desc_m(loops.size(),featureSize);
  
	mat pos_m(nbMedia,2);
  
  for(int i=0; i<loops.size(); i++) {    
		for(int d=0; d < featureSize; d++){
			desc_m(i, d) = loops[i]->getFeaturesVector(featureId)->getFeatureElement(d);
		}
  }
	return desc_m;
}


mat ACVisPluginAudiogarden::extractDescMatrix(ACMediaLibrary* lib, vector<string> featureList){
	mat desc_m;
	mat tmpDesc_m;
	
	desc_m = extractDescMatrix(lib, featureList[0]);
	
	for (int i=1; i<featureList.size(); i++){
		tmpDesc_m = extractDescMatrix(lib, featureList[i]);
		desc_m = join_rows(desc_m, tmpDesc_m);
	}
	return desc_m;
}

