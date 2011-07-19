/*
 *  ACSparseEuclideanKMeansPlugin.cpp
 *  MediaCycle
 *
 *  @author Ravet Thierry
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

#include "ACSparseEuclideanKMeansPlugin.h"
#include "Armadillo-utils.h"

#include "ACEuclideanDistance.h"

ACSparseEuclideanKMeansPlugin::ACSparseEuclideanKMeansPlugin() : ACSparseKMeansPlugin() {
    this->mName = "ACSparseEuclideanKMeans";
    this->mDescription = "Clustering with Euclidean Distance";
}

ACSparseEuclideanKMeansPlugin::~ACSparseEuclideanKMeansPlugin() {
}


double ACSparseEuclideanKMeansPlugin::compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
	
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f], obj2[f]);
		double tempDist=E->distance();
		dis += tempDist*tempDist * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	dis = sqrt(dis);
	
	return dis;
}
double ACSparseEuclideanKMeansPlugin::compute_distance(vector<ACMediaFeatures*> &obj1, const vector<vector <float> > &obj2, const vector<float> &weights, bool inverse_features)
{
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		//ACEuclideanDistance* E = new ACEuclideanDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
		//FeaturesVector tmp  = obj1[f]->getFeaturesVector();
		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
		double tempDist=E->distance();
		dis += tempDist * tempDist * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	dis = sqrt(dis);
	
	return dis;
}

void ACSparseEuclideanKMeansPlugin::meanAccumCompute(std::vector<ACMediaFeatures*> & obj1,std::vector<std::vector<float> >&obj2){


}