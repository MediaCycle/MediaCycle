/**
 * @brief ACVisPluginPCA.cpp
 * @author Damien Tardieu
 * @date 22/06/2010
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
#include "ACVisPluginPCA.h"

using namespace arma;
using namespace std;

ACVisPluginPCA::ACVisPluginPCA()
{
    //vars herited from ACPlugin
	// XS TODO: are these general enough ? can we use this only for video ??
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_CLUSTERS_PIPELINE;
    this->mName = "PCAVis";
    this->mDescription = "PCA Visualisation plugin";
    this->mId = "";
	
    //local vars
}

ACVisPluginPCA::~ACVisPluginPCA()
{
}

int ACVisPluginPCA::initialize()
{
    std::cout << "ACVisPluginPCA initialized" << std::endl;
    return 0;
}

vector<ACMediaFeatures*> ACVisPluginPCA::calculate(){
	vector<ACMediaFeatures*> raf;
	raf.resize(0);
	return raf;
}

vector<ACMediaFeatures*> ACVisPluginPCA::calculate(std::string aFileName) {
	vector<ACMediaFeatures*> raf;
	raf.resize(0);
	return raf;
}

void ACVisPluginPCA::updateClusters(ACMediaBrowser* mediaBrowser){
	//	this->updateNextPositions(mediaBrowser);
}

void ACVisPluginPCA::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int itemClicked, labelClicked, action;
	vector<string> featureNames;
	int libSize = mediaBrowser->getLibrary()->getSize();
	itemClicked = mediaBrowser->getClickedNode();
	labelClicked = mediaBrowser->getClickedLabel();
	
	int nbActiveFeatures;
	mat desc_m, descD_m;
	mat posDisp_m;

	nbActiveFeatures = 9;
	extractDescMatrix(mediaBrowser, desc_m, featureNames);
	mat descN_m = zscore(desc_m);
	mat coeff;
	mat score;
	princomp(coeff, posDisp_m, descN_m);

	for (int i=0; i<nbActiveFeatures; i++)
		std::cout << "featureNames : " << featureNames[i] << std::endl;

	
#ifdef USE_DEBUG
	posDisp_m.save("posDispDef.txt", arma_ascii);
#endif
// 	///////////////////////////////////////////////////////////////////////////////
	
	///for mediacycle osg
// 	posDisp_m = posDisp_m/10;
// 	labelPos_m = labelPos_m/10;
	// Set labels in browser ////////////////////////////////////////////////////////

	mediaBrowser->setNumberOfDisplayedLoops(desc_m.n_rows);
	////////////////////////////////////////////////////////////////////////////////////

  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopIsDisplayed(i, true);
	  // TODO: make sure you meant next
		mediaBrowser->setNodeNextPosition(i, posDisp_m(i,0), posDisp_m(i,1));
  }
	////////////////////////////////////////////////////////////////
}


void ACVisPluginPCA::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
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
