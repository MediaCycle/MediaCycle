/**
 * @brief ACVisualisationPlugin.cpp
 * @author Damien Tardieu
 * @date 27/11/2009
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
	//	this->updateNextPositions(mediaBrowser);
}

void ACVisualisationPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
  int itemClicked, labelClicked, action;
  int nbVideoDisplay = mediaBrowser->getNumberOfDisplayedLoops(); // should divide by nbClusters
  ucolvec toDisplay_v(nbVideoDisplay);
	vector<string> featureNames;
	int libSize = mediaBrowser->getLibrary()->getSize();
  itemClicked = mediaBrowser->getClickedLoop();
  labelClicked = mediaBrowser->getClickedLabel();
	
	int nbActiveFeatures;
	mat desc_m;
  mat posDisp_m;
	mat labelPos_m;
	ucolvec labelIdx_v;
	colvec labelValue_v;
	mat descD_m;

  if (itemClicked >= 0){
		nbActiveFeatures = 4;
		desc_m = extractDescMatrix(mediaBrowser, nbActiveFeatures, featureNames, descD_m);
		posDisp_m = updateNextPositionsItemClicked(desc_m, nbVideoDisplay, itemClicked, toDisplay_v, labelPos_m, labelIdx_v, labelValue_v);
	}
  else 
    if (labelClicked >= 0){
			nbActiveFeatures = 4;
			desc_m = extractDescMatrix(mediaBrowser, nbActiveFeatures, featureNames, descD_m);
			posDisp_m = updateNextPositionsInit(desc_m, nbVideoDisplay, toDisplay_v, labelPos_m, labelIdx_v, labelValue_v);
		}
    else{
			nbActiveFeatures = 4;
			desc_m = extractDescMatrix(mediaBrowser, nbActiveFeatures, featureNames, descD_m);			
      posDisp_m = updateNextPositionsInit(desc_m, nbVideoDisplay, toDisplay_v, labelPos_m, labelIdx_v, labelValue_v);
		}
	for (int i=0; i<nbActiveFeatures; i++)
		std::cout << "featureNames : " << featureNames[i] << std::endl;

#ifdef USE_DEBUG
	descD_m.save("descD.txt", arma_ascii);
#endif

  vector<string> clusterLabelAdj;
	//   clusterLabelAdj.push_back("Low");
	//   clusterLabelAdj.push_back("Medium");
	//   clusterLabelAdj.push_back("High");
  clusterLabelAdj.push_back("0");
  clusterLabelAdj.push_back("1");
  clusterLabelAdj.push_back("2");
  clusterLabelAdj.push_back("3");



  colvec clusterid_m(desc_m.n_rows), clusterid2_m;
  mat center_m, center2_m;
  // desc_m for first clustering 
  	
	/////// for screen display between 0 and 999  //////////////////////////////////////
	mat posDispOk_m(toDisplay_v.n_rows,2);
	for (int k=0; k<toDisplay_v.n_rows; k++)
		posDispOk_m.row(k) = posDisp_m.row(toDisplay_v(k));
	//  rowvec maxPosDisp_v = max(posDispOk_m);
  //rowvec minPosDisp_v = min(posDispOk_m);
  // to keep the (0,0) in the center of the screen
	rowvec maxPosDisp_v = max(abs(posDispOk_m));
  rowvec minPosDisp_v = -max(abs(posDispOk_m));
  posDisp_m = posDisp_m - repmat(minPosDisp_v, posDisp_m.n_rows, 1);
  labelPos_m = labelPos_m - repmat(minPosDisp_v, labelPos_m.n_rows, 1);
  posDisp_m = posDisp_m/repmat(maxPosDisp_v-minPosDisp_v, posDisp_m.n_rows, 1)*999;
  labelPos_m = labelPos_m/repmat(maxPosDisp_v-minPosDisp_v, labelPos_m.n_rows, 1)*999;
	posDisp_m.col(1) = 999-posDisp_m.col(1);
	labelPos_m.col(1) = 999-labelPos_m.col(1);
	///////////////////////////////////////////////////////////////////////////////

	// Set labels in browser ////////////////////////////////////////////////////////
  string labelValue;
  for (int i=0; i< labelIdx_v.n_rows; i++){
    ACPoint p;
    p.x = labelPos_m(i,0);
    p.y = labelPos_m(i,1);
    p.z = .1;
    labelValue = featureNames[labelIdx_v(i)];
    labelValue.append("");
    labelValue.append(clusterLabelAdj[labelValue_v(i)]);
    mediaBrowser->setLabel(i, labelValue, p);
  }
	std::cout << "toDisplay_v.n_elem : "  << toDisplay_v.n_elem << std::endl;
	mediaBrowser->setNumberOfDisplayedLabels(labelIdx_v.n_rows);
	mediaBrowser->setNumberOfDisplayedLoops(toDisplay_v.n_elem);
	////////////////////////////////////////////////////////////////////////////////////

	//  For osg view : Because there is no way to prevent a media from displaying it display it far away
  for (int i=0; i<libSize; i++){
    mediaBrowser->setLoopPosition(i, 40, 40);    
    mediaBrowser->setLoopIsDisplayed(i, false);
  }
	////////////////////////////////////////////////////////////////
  
  for (int i=0; i < toDisplay_v.n_elem; i++)
    mediaBrowser->setLoopIsDisplayed(toDisplay_v(i), true);
	
	for (int i=0; i<nbVideoDisplay; i++){
		mediaBrowser->setLoopPosition(toDisplay_v(i), posDisp_m(toDisplay_v(i),0), posDisp_m(toDisplay_v(i),1));
		std::cout<<"disp : " << toDisplay_v(i) << ", " << mediaBrowser->getLibrary()->getItem(toDisplay_v(i))->getFileName() << posDisp_m(toDisplay_v(i),0) << ", " << posDisp_m(toDisplay_v(i),1) << std::endl;
	}
// 	if (itemClicked >= 0)
// 		mediaBrowser->setLoopPosition(itemClicked, 0, 0);
}


mat ACVisualisationPlugin::updateNextPositionsInit(mat &desc_m, int nbVideoDisplay, ucolvec &toDisplay_v, mat &labelPos_m, ucolvec &labelIdx_v, colvec &labelValue_v){
	int libSize = desc_m.n_rows;
  int nbClusters = 4;
  // position of screen display
  mat posDisp_m(libSize, 2);  
  mat clusterCenterDisp_m(nbClusters, 2);
  ucolvec clusterLabel_v(nbClusters);  // for each cluster, which feature will be displayed
	colvec clusterSpread_v(nbClusters);
  ucolvec clusterCard_v(nbClusters);
	// setting acive features, ie. used for computation
	labelValue_v.set_size(nbClusters);
	labelPos_m.set_size(nbClusters,2);
	labelIdx_v.set_size(nbClusters);

	clusterCard_v.fill((int)(nbVideoDisplay/nbClusters));
	clusterCard_v(0) = clusterCard_v(0)+nbVideoDisplay-sum(clusterCard_v);

	// setting cluster center position
	clusterCenterDisp_m(0,0) = -1;
	clusterCenterDisp_m(0,1) = -.7;
	// Which feature is used as a label : indice in activeFeatures
	clusterLabel_v(0) = 0;
	clusterSpread_v(0) = .5;
	
	clusterCenterDisp_m(1,0) = -1;
	clusterCenterDisp_m(1,1) = .7;
	clusterLabel_v(1) = 1;
	clusterSpread_v(1) = .5;
	
	clusterCenterDisp_m(2,0) = 1;
	clusterCenterDisp_m(2,1) = -.7;
	clusterLabel_v(2) = 2;
	clusterSpread_v(2) = .5;
	
	clusterCenterDisp_m(3,0) = 1;
	clusterCenterDisp_m(3,1) = .7;
	clusterLabel_v(3) = 0;
	clusterSpread_v(3) = .5;

// 	// setting cluster center position
// 	clusterCenterDisp_m(0,0) = 0;
// 	clusterCenterDisp_m(0,1) = 0;
// 	// Which feature is used as a label : indice in activeFeatures
// 	clusterLabel_v(0) = 0;
// 	clusterSpread_v(0) = 1;
	
// 	clusterCenterDisp_m(1,0) = -1.5;
// 	clusterCenterDisp_m(1,1) = 1.2;
// 	clusterLabel_v(1) = 1;
// 	clusterSpread_v(1) = .3;
	
// 	clusterCenterDisp_m(2,0) = 1.5;
// 	clusterCenterDisp_m(2,1) = -1.2;
// 	clusterLabel_v(2) = 2;
// 	clusterSpread_v(2) = .3;
	
// 	clusterCenterDisp_m(3,0) = 1.5;
// 	clusterCenterDisp_m(3,1) = 1.2;
// 	clusterLabel_v(3) = 0;
// 	clusterSpread_v(3) = .3;

	colvec clusterid_m, clusterid2_m;
	mat center_m, center2_m;
	    // clustering 
	kcluster(desc_m, nbClusters, clusterid_m, center_m);
	// desc2_m used for display. For now the two first dims of desc_m
	mat desc2_m = desc_m; // if I change this, take care to the dimension of the centers of the previous clustering
	

	mat tmpDesc_m;
	int index=0;
	ucolvec pos_v, pos2_v;
	for (int i=0; i<nbClusters; i++){
		pos_v = find(clusterid_m==i);
		tmpDesc_m.zeros(pos_v.n_elem, desc2_m.n_cols);
		// Cluster each cluster to choose the video to display
		for (int k=0; k < pos_v.n_elem; k++){
			tmpDesc_m.row(k) = desc2_m.row(pos_v(k));
		}
		// TODO : Need some tests on the number of elements in each clusters
		kcluster(tmpDesc_m, clusterCard_v(i), clusterid2_m, center2_m);
		for (int k=0; k<nbVideoDisplay/nbClusters; k++){
			pos2_v = find(clusterid2_m==k);
			ucolvec randpos = randperm(pos2_v.n_elem);
			toDisplay_v(index) = pos_v(pos2_v(randpos(0)));
			index++;
		}
		mat tmp_m = tmpDesc_m;
		//tmp_m = tmpDesc_m - repmat(center_m.row(i), tmpDesc_m.n_rows, 1);
		//tmp_m = tmp_m/repmat(max(abs(tmp_m)), tmp_m.n_rows, 1) * clusterSpread_v(i);
		tmp_m.col(0) = clusterSpread_v(i) * cos(2*math::pi() * clusterid2_m/nbClusters) + randn<colvec>(tmp_m.n_rows);
		tmp_m.col(1) = clusterSpread_v(i) * sin(2*math::pi() * clusterid2_m/nbClusters) + randn<colvec>(tmp_m.n_rows);
#ifdef USE_DEBUG
		std::cout << "tmp_m : " << tmp_m << std::endl;
#endif
		for (int k=0; k < tmpDesc_m.n_rows; k++)
			// The clusters are displayed usiing the two first dimensions of desc_m
			posDisp_m.row(pos_v(k)) = clusterCenterDisp_m.row(clusterid_m(pos_v(k))) + tmp_m.submat(k,0,k,1);
	}
	labelPos_m = clusterCenterDisp_m;
	// Selecting the displayed labels.
	// The label used for the diagonal is the one with the biggest range. 
	// TODO : perform a complete search among the cluster centers
	urowvec maxDiff_v = sort_index(conv_to<rowvec>::from(abs(center_m.row(3)-center_m.row(0))), 1);
	int maxDiffPos = maxDiff_v(0);
	labelIdx_v(0) = maxDiffPos;
	labelIdx_v(3) = maxDiffPos;
	labelIdx_v(1) = maxDiff_v(1);
	labelIdx_v(2) = maxDiff_v(2);

	// This operation is also performed in the main for the xml export.
	// TODO : join the two functions
	for (int k=0; k < nbClusters; k++){
    if (center_m(k,labelIdx_v(k)) > .5)
      labelValue_v(k)=3;
    else 
      if (center_m(k,labelIdx_v(k)) < -.5)
				labelValue_v(k) = 1;
      else
				labelValue_v(k) = 2;
		std::cout << "center_m(k,labelIdx_v(k)) : " << center_m(k,labelIdx_v(k)) << std::endl;
		std::cout << "center_m(k,labelIdx_v(k))*3 : " << center_m(k,labelIdx_v(k))*3 << std::endl;
		std::cout << "labelValue_v(k) : " << labelValue_v(k) << std::endl;
	}
#ifdef USE_DEBUG
	std::cout << "debug" << std::endl;
	desc_m.save("desc.txt", arma_ascii);
	clusterid_m.save("clusterid.txt", arma_ascii);
	center_m.save("center.txt", arma_ascii);
	toDisplay_v.save("toDisplay.txt", arma_ascii);
	posDisp_m.save("posDisp.txt", arma_ascii);
#endif

	return posDisp_m;
}

mat ACVisualisationPlugin::updateNextPositionsItemClicked(mat &desc_m, int nbVideoDisplay, int itemClicked, ucolvec &toDisplay_v, mat &labelPos_m, ucolvec &labelIdx_v, colvec &labelValue_v){
	int libSize = desc_m.n_rows;
  int nbClusters = 4;
  // position of screen display
  mat posDisp_m(libSize, 2);  
  mat clusterCenterDisp_m(nbClusters, 2);
  ucolvec clusterLabel_v(nbClusters);  // for each cluster, which feature will be displayed
	colvec clusterSpread_v(nbClusters);
  ucolvec clusterCard_v(nbClusters);

	// setting cluster center position
	clusterCenterDisp_m(0,0) = -1;
	clusterCenterDisp_m(0,1) = -.7;
	// Which feature is used as a label : indice in activeFeatures
	clusterCenterDisp_m(1,0) = -1;
	clusterCenterDisp_m(1,1) = .7;

	clusterCenterDisp_m(2,0) = 1;
	clusterCenterDisp_m(2,1) = -.7;

	clusterCenterDisp_m(3,0) = 1;
	clusterCenterDisp_m(3,1) = .7;

	labelValue_v.set_size(nbClusters);
	labelPos_m.set_size(nbClusters,2);
	labelIdx_v.set_size(nbClusters);
	desc_m = square(desc_m - repmat(desc_m.row(itemClicked), desc_m.n_rows, 1));
	colvec dist_v = min(desc_m,1);
	ucolvec spos_v = sort_index(dist_v);
	toDisplay_v = spos_v.rows(0,nbVideoDisplay-1);
	mat descDisp_m(nbVideoDisplay, desc_m.n_rows);
	colvec distDesc_v = dist_v; //min(desc_m,1);
	urowvec tmpSort_v;
	double angle;
	//	mat posDispOk_m(toDisplay_v.n_rows,2);
	for (int k=0; k<desc_m.n_rows; k++){
		angle = conv_to<double>::from(rand<mat>(1,1)*(math::pi())/2);
		tmpSort_v = sort_index(conv_to<rowvec>::from(desc_m.row(k)));
		posDisp_m(k,0) = pow(distDesc_v(k)+.005,2)*cos(angle) * clusterCenterDisp_m(tmpSort_v(0),0);
		posDisp_m(k,1) = pow(distDesc_v(k)+.005,2)*sin(angle) * clusterCenterDisp_m(tmpSort_v(0),1);
	}
	/// Test  ///
	posDisp_m = (log(1/desc_m)) * (clusterCenterDisp_m/(abs(clusterCenterDisp_m)*.5));
	/////////////
	posDisp_m(itemClicked,0)=0;
	posDisp_m(itemClicked,1)=0;
 	
	mat posDispOk_m(toDisplay_v.n_rows,2);
	for (int k=0; k<toDisplay_v.n_rows; k++)
 		posDispOk_m.row(k) = posDisp_m.row(toDisplay_v(k));
	
 	posDisp_m = posDisp_m - repmat(-max(abs(posDispOk_m)), posDisp_m.n_rows, 1);
	posDisp_m = posDisp_m/repmat(2*max(abs(posDispOk_m)), posDisp_m.n_rows, 1);
	posDisp_m = 3*posDisp_m-1.5;
	labelPos_m = clusterCenterDisp_m;
	for (int k=0; k < desc_m.n_cols; k++){
		labelIdx_v(k) = k;
		labelValue_v(k) = 0;//desc_m(itemClicked,k);
	}
	return posDisp_m;
}

mat ACVisualisationPlugin::extractDescMatrix(ACMediaBrowser* mediaBrowser, int nbActiveFeatures, vector<string> &featureNames, mat &descD_m){
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getMedia();  
  int libSize = mediaBrowser->getLibrary()->getSize();
  int nbMedia = loops.size(); 
	int featDim;
	int totalDim = 0;
	
  // Count nb of feature
  int nbFeature = loops.back()->getFeatures().size();
  for(int f=0; f< nbFeature; f++){
    featDim = loops.back()->getFeatures()[f]->size();
    for(int d=0; d < featDim; d++){
      totalDim++;
    }
  }

	colvec  q_v       = rand<colvec>(totalDim);
  ucolvec perm_v = sort_index(q_v);
  ucolvec activeFeatures_v;
	activeFeatures_v = perm_v.rows(0,nbActiveFeatures-1);  
//   activeFeatures_v.set_size(3);
//   activeFeatures_v(0)=2;
//   activeFeatures_v(1)=3;
//   activeFeatures_v(2)=4;
  // filling desc_m with activeFeatures
  mat desc_m(libSize,activeFeatures_v.n_elem);
  descD_m.set_size(libSize,activeFeatures_v.n_elem);
  mat pos_m(libSize,2);
  
  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
    for(int f=0; f< activeFeatures_v.n_elem; f++){
      featDim = loops.back()->getFeatures()[activeFeatures_v(f)]->size();
			featureNames.push_back(loops.back()->getFeatures()[activeFeatures_v(f)]->getName());
      for(int d=0; d < featDim; d++){
				desc_m(i,tmpIdx) = loops[i]->getFeatures()[activeFeatures_v(f)]->getFeature(d);
				descD_m(i,tmpIdx) = loops[i]->getFeatures()[activeFeatures_v(f)]->getDiscretizedFeature();
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
	return desc_m;
}
