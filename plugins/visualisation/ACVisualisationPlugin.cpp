/**
 * @brief ACVisualisationPlugin.cpp
 * @author Damien Tardieu
 * @date 03/11/2009
 * @copyright (c) 2009 – UMONS - Numediart
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
#include "ACPlugin.h"
#include "ACVisualisationPlugin.h"

using namespace arma;
using namespace std;

ACVisualisationPlugin::ACVisualisationPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "Visualisation";
    this->mDescription = "Visualisation plugin";
    this->mId = "";

    //local vars
}

ACVisualisationPlugin::~ACVisualisationPlugin()
{
}

int ACVisualisationPlugin::initialize()
{
    std::cout << "ACVisualisationPlugin initialized" << std::endl;
    return 0;
}

vector<ACMediaFeatures*> ACVisualisationPlugin::calculate(){
  vector<ACMediaFeatures*> raf;
  raf.resize(0);
  return raf;
}

vector<ACMediaFeatures*> ACVisualisationPlugin::calculate(std::string aFileName) {
  vector<ACMediaFeatures*> raf;
  raf.resize(0);
  return raf;
}

void ACVisualisationPlugin::updateClusters(ACMediaBrowser* mediaBrowser){
}

void ACVisualisationPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int libSize = mediaBrowser->getLibrary()->getSize();
  int totalDim =0;
  int featDim;
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getMedia();  
  int nbMedia = loops.size(); 
  if(nbMedia == 0) 
    return;
  int nbFeature = loops.back()->getFeatures().size();
  for(int f=0; f< nbFeature; f++){
    featDim = loops.back()->getFeatures()[f]->size();
    for(int d=0; d < featDim; d++){
      totalDim++;
    }
  }

//   ucolvec perm_v = randperm(totalDim);
//   // nbFeatures = 5
//   ucolvec activeFeatures = perm_v.rows(0,5);

  mat desc_m(libSize,totalDim);
  mat pos_m(libSize,2);

  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
    for(int f=0; f< nbFeature; f++){
      featDim = loops.back()->getFeatures()[f]->size();
      for(int d=0; d < featDim; d++){
	desc_m(i,tmpIdx) = loops[i]->getFeatures()[f]->getFeature(d);
	tmpIdx++;
      }
    }
  }
  
  
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopPosition(i, 0, 0);
  }

}



