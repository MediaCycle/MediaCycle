/*
 *  ACCosClustPosition.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 21/02/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACCosClustPosition.h"

#include "ACCosDistance.h"
#include "ACMediaLibrary.h"

#include <sys/time.h>

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    
    gettimeofday(&tv, &tz);
    
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
} 

double ACCosClustPosition::compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
	
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		ACCosDistance* E = new ACCosDistance (obj1[f], obj2[f]);
		dis += (E->distance()) * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	//dis = sqrt(dis);
	
	return dis;
}
double ACCosClustPosition::compute_distance(vector<ACMediaFeatures*> &obj1, const vector<vector <float> > &obj2, const vector<float> &weights, bool inverse_features)
{
	
	
	
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;	
	
	for (int f=0; f<feature_count; f++) {
		//ACCosDistance* E = new ACCosDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
		//FeaturesVector tmp  = obj1[f]->getFeaturesVector();
		ACCosDistance* E = new ACCosDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
		dis += (E->distance()) * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	//dis = sqrt(dis);
	
	return dis;
}

ACCosClustPosition::ACCosClustPosition()
{
    this->mName = "ACCosClustPosition";
    this->mDescription = "Clustering position with Cos Distance";
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
  //  this->mPluginType = PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mId = "";
}
ACCosClustPosition::~ACCosClustPosition()
{
}
void ACCosClustPosition::updateNextPositions(ACMediaBrowser* mediaBrowser)
{
	
		
	std::cout << "ACMediaBrowser::updateNextPositions" <<std::endl;
	ACMediaLibrary *lLibrary=mediaBrowser->getLibrary();
	
	if (lLibrary->isEmpty() ) return;
	int lReferenceNode=mediaBrowser->getReferenceNode();
	if (lReferenceNode < 0 || lReferenceNode >= mediaBrowser->getNumberOfMediaNodes()) return ;
		
		
	float r, theta;
	ACPoint p;
	p.x = p.y = p.z = 0.0;
	double t = getTime();
	mediaBrowser->getMediaNode(lReferenceNode).setNextPosition(p, t);
		
	// srand(1234);
		
	// XS loop on MediaNodes.
	// each MediaNode has a MediaId by which we can access the Media
	int lNode=mediaBrowser->getNumberOfMediaNodes();
	const vector<float> lFeatureWeights=mediaBrowser->getFeatureWeights();
	const float lClusterCount=mediaBrowser->getClusterCount();
	for (int ind =0;ind<lNode;ind++)
	{
		ACMediaNode node=mediaBrowser->getMediaNode(ind);
			
			int ci = node.getClusterId();
			const vector< vector<float> > lClusterCenters=mediaBrowser->getClusterCenter(ci);
			
			// SD TODO - test both approaches
			r=1;
			r = compute_distance(lLibrary->getMedia(lReferenceNode)->getAllPreProcFeaturesVectors(), 
								 lLibrary->getMedia(node.getMediaId())->getAllPreProcFeaturesVectors(), 
								 lFeatureWeights, false) ;//* 10.0;
			//r /= 100.0;
			theta = 2*M_PI * ci / (float)lClusterCount;
			
			double dt = 1;
			dt = compute_distance(lLibrary->getMedia(node.getMediaId())->getAllPreProcFeaturesVectors(), lClusterCenters, lFeatureWeights, false) / 2.0 * 10.0;
			dt /= 3.0;
			theta += dt;
			
			p.x = sin(theta)*r;
			p.y = cos(theta)*r;
			p.z = 0.0;
			
			//printf("computed next position: theta:%f,r=%f,  (%f %f %f)\n", theta, r, p.x, p.y, p.z);//CF free the console
			double t = getTime();
			mediaBrowser->getMediaNode(ind).setCurrentPosition(p);
			mediaBrowser->getMediaNode(ind).setNextPosition(p, t);
		}
		
		// printf("PROPELER \n");
		
		mediaBrowser->setNeedsDisplay(true);
	lLibrary=NULL;
}