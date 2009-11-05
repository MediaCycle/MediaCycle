/**
 * @brief ACVisualisationPlugin.cpp
 * @author Damien Tardieu
 * @date 05/11/2009
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
#include "Armadillo-utils.h"
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
  //  updateNextPositions(mediaBrowser);
}

void ACVisualisationPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int libSize = mediaBrowser->getLibrary()->getSize();
  int totalDim =0;
  int featDim;
  int nbActiveFeatures = 4;
  int nbVideoDisplay=20; // should divide by nbClusters
  int nbClusters = 4;

  mat clusterCenterDisp_m(nbClusters, 2);
  ucolvec clusterCenterDispLabel_v(nbClusters);  // for each cluster, which feature will be displayed

  // setting acive features, ie. used for computation
  colvec  q_v       = rand<colvec>(totalDim);
  ucolvec perm_v = sort_index(q_v);
  ucolvec activeFeatures_v;
    //activeFeatures_v = perm_v.rows(0,3);  
  activeFeatures_v.set_size(2);
  activeFeatures_v(0)=0;
  activeFeatures_v(1)=1;

  ucolvec featureDispLabel_v = activeFeatures_v;
  // setting cluster center position
  clusterCenterDisp_m(0,0) = -1;
  clusterCenterDisp_m(0,1) = -.7;
  // Affectation of text to labels
  clusterCenterDispLabel_v(0) = featureDispLabel_v(0);
  clusterCenterDisp_m(1,0) = -1;
  clusterCenterDisp_m(1,1) = .7;
  clusterCenterDispLabel_v(1) = featureDispLabel_v(1);
  clusterCenterDisp_m(2,0) = 1;
  clusterCenterDisp_m(2,1) = -.7;
  clusterCenterDispLabel_v(2) = featureDispLabel_v(1);
  clusterCenterDisp_m(3,0) = 1;
  clusterCenterDisp_m(3,1) = .7;
  clusterCenterDispLabel_v(3) = featureDispLabel_v(0);


  vector<string> clusterLabelAdj;
  clusterLabelAdj.push_back("Low");
  clusterLabelAdj.push_back("Medium");
  clusterLabelAdj.push_back("High");
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getMedia();  
  vector<string> featureNames;
  int nbMedia = loops.size(); 
  if(nbMedia == 0) 
    return;

  // Count nb of feature
  int nbFeature = loops.back()->getFeatures().size();
  for(int f=0; f< nbFeature; f++){
    featDim = loops.back()->getFeatures()[f]->size();
    featureNames.push_back(loops.back()->getFeatures()[f]->getName());
    for(int d=0; d < featDim; d++){
      totalDim++;
    }
  }


  // filling desc_m with activeFeatures
  mat desc_m(libSize,activeFeatures_v.n_elem);
  mat pos_m(libSize,2);

  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
    for(int f=0; f< activeFeatures_v.n_elem; f++){
      std::cout << activeFeatures_v(f) << std::endl;
      featDim = loops.back()->getFeatures()[activeFeatures_v(f)]->size();
      for(int d=0; d < featDim; d++){
	desc_m(i,tmpIdx) = loops[i]->getFeatures()[f]->getFeature(d);
	tmpIdx++;
      }
    }
  }
  
  // normalizing features between 0 and 1
  rowvec maxDesc_v = max(desc_m);
  rowvec minDesc_v = min(desc_m);
  desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
  desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
  std::cout<< "max : " << max(desc_m) << std::endl;
  std::cout<< "min : " << min(desc_m) << std::endl;

  colvec clusterid_m, clusterid2_m;
  mat center_m, center2_m;
  mat coeff;
  mat desc2_m;
  //  mat desc3_m(nbVideoDisplay, activeFeatures_v.n_elem);
  //princomp(desc_m, coeff, desc2_m);
  desc2_m = desc_m;
  
  ucolvec pos_v, pos2_v;
  ucolvec toDisplay_v(nbVideoDisplay);

  // clusterizing 
  kcluster(desc2_m, nbClusters, clusterid_m, center_m);
  mat posDisp_m(libSize, 2);
  clusterid_m.print("clusterid_m");
  

  mat tmpDesc_m;
  int index=0;
  for (int i=0; i<nbClusters; i++){
    pos_v = find(clusterid_m==i);
    tmpDesc_m.zeros(pos_v.n_elem, desc2_m.n_cols);
    // Cluster each cluster to choose the video to display
    for (int k=0; k < pos_v.n_elem; k++){
      tmpDesc_m.row(k) = desc2_m.row(pos_v(k));
    }
    kcluster(tmpDesc_m, nbVideoDisplay/nbClusters, clusterid2_m, center2_m);
    for (int k=0; k<nbVideoDisplay/nbClusters; k++){
      pos2_v = find(clusterid2_m==k);
      toDisplay_v(index) = pos_v(pos2_v(0));
      index++;
    }
    std::cout << "classe " << i << std::endl;
    tmpDesc_m.print("tmpDesc_m");
    mat tmp_m = tmpDesc_m - repmat(center_m.row(i), tmpDesc_m.n_rows, 1);
    tmp_m = tmp_m/repmat(max(abs(tmp_m)), tmp_m.n_rows, 1);
    tmp_m.print("tmp_m");
    for (int k=0; k < tmpDesc_m.n_rows; k++)
      posDisp_m.row(pos_v(k)) = clusterCenterDisp_m.row(clusterid_m(pos_v(k))) + tmp_m.row(k);
  }
  string labelValue;
  for (int i=0; i< nbClusters; i++){
    ACPoint p;
    center_m.print("Cluster center: ");
    p.x = clusterCenterDisp_m(i,0);
    p.y = clusterCenterDisp_m(i,1);
    p.z = .1;
    std::cout << "Label : " << center_m(i,clusterCenterDispLabel_v(i))*3 << " " <<(int)(center_m(i,clusterCenterDispLabel_v(i))*3)<<std::endl;
    labelValue = clusterLabelAdj[(int)(center_m(i,clusterCenterDispLabel_v(i))*3)];
    labelValue.append(" ");
    labelValue.append(featureNames[clusterCenterDispLabel_v(i)]);
    mediaBrowser->setLabel(i, labelValue, p);
  }

  // To decluter
  //posDisp_m.col(0) = conv_to<mat>::from(conv_to<imat>::from(posDisp_m.col(0)*2))/2;
  //posDisp_m.col(1) = conv_to<mat>::from(conv_to<imat>::from(posDisp_m.col(1)*3))/3;
//  posDisp_m.print("posDisp_m");

//  Because there is no way to prevent a media from displaying it display it far away
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopPosition(i, 40, 40);    
  }

//   for (int i=0; i<libSize; i++){
//     mediaBrowser->setLoopPosition(i, posDisp_m(i,0), posDisp_m(i,1));
//     //    std::cout<<"disp : " << clusterid_m(toDisplay_v(i)) << ", " << posDisp_m(toDisplay_v(i),0) << ", " << posDisp_m(toDisplay_v(i),1) << std::endl;
//  }
  for (int i=0; i<nbVideoDisplay; i++){
    mediaBrowser->setLoopPosition(toDisplay_v(i), posDisp_m(toDisplay_v(i),0), posDisp_m(toDisplay_v(i),1));
    std::cout<<"disp : " << clusterid_m(toDisplay_v(i)) << ", " << posDisp_m(toDisplay_v(i),0) << ", " << posDisp_m(toDisplay_v(i),1) << std::endl;
 }
//   for (int i=0; i<libSize; i++){
//     mediaBrowser->setLoopPosition(i, desc_m(i,0), desc_m(i,1));    
//   }

}


// std::string convertToString(int value){
//   if (value < .4){
    
//   }
//   else{
//   }
// }


