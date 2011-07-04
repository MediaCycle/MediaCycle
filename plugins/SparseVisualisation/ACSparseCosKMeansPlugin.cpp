/*
 *  ACSparseCosKMeansPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 20/10/10
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


#include "ACSparseCosKMeansPlugin.h"
#include "Armadillo-utils.h"

#include "ACSparseCosDistance.h"
#include "time.h"

using namespace std;

ACSparseCosKMeansPlugin::ACSparseCosKMeansPlugin() : ACSparseKMeansPlugin() {
    this->mName = "ACSparseCosKMeans";
    this->mDescription = "Clustering with Cos Distance";
}

ACSparseCosKMeansPlugin::~ACSparseCosKMeansPlugin() {
}


double ACSparseCosKMeansPlugin::compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
	
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size()*3);
	assert(obj1.size()%3==0 && obj1.size()%3==0);
	int feature_count = obj1.size()/3;
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		vector<ACMediaFeatures*>  temp1,temp2;
		temp1.push_back(obj1[3*f+0] );
		temp1.push_back(obj1[3*f+1] );
		temp1.push_back(obj1[3*f+2] );
		temp2.push_back(obj2[3*f+0] );
		temp2.push_back(obj2[3*f+1] );
		temp2.push_back(obj2[3*f+2] );
		
		ACSparseCosDistance* E = new ACSparseCosDistance (temp1, temp2);
		dis += (E->distance()) * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	//dis = sqrt(dis);
	
	return dis;
}
double ACSparseCosKMeansPlugin::compute_distance(vector<ACMediaFeatures*> &obj1, const vector<vector <float> > &obj2, const vector<float> &weights, bool inverse_features)
{
	
	
	
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size()*3);
	assert(obj1.size() %3==0);

	int feature_count = obj1.size()/3;
	
	double dis = 0.0;	
	
	for (int f=0; f<feature_count; f++) {
		ACSparseVector temp1,temp2;
		temp1.push_back(*(obj1[3*f+0]->getFeaturesVector() ));
		temp1.push_back(*(obj1[3*f+1]->getFeaturesVector() ));
		temp1.push_back(*(obj1[3*f+2]->getFeaturesVector() ));
	//	for (int i=0;i<temp1[2].size();i++)
	//		cout << temp1[2][i]<<"\t";
	//	cout << "\n";
		temp2.push_back(obj2[3*f+0] );
		temp2.push_back(obj2[3*f+1] );
		temp2.push_back(obj2[3*f+2] );
		//ACCosDistance* E = new ACCosDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
		//FeaturesVector tmp  = obj1[f]->getFeaturesVector();
		ACSparseCosDistance* E = new ACSparseCosDistance (temp1,temp2);
		dis += (E->distance()) * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	//dis = sqrt(dis);
	
	return dis;
}

void ACSparseCosKMeansPlugin::meanAccumCompute(std::vector<ACMediaFeatures*> & obj1,std::vector<std::vector<float> >&obj2){

	assert(obj1.size()==obj2.size());
	int feature_count=obj1.size()/3;
	for (int i=0;i<feature_count;i++)
	{
		vector<float> *a0=obj1[3*i+0]->getFeaturesVector();
		vector<float> *a1=obj1[3*i+1]->getFeaturesVector();
		vector<float> *a2=obj1[3*i+2]->getFeaturesVector();
		float norm=0.f;
		for (int i=0;i<a2->size();i++)
			norm+=(*a2)[i]*(*a2)[i];
		norm=sqrt(norm);
		int numTerm=obj1[3*i+0]->getFeatureElement(0);
		obj2[3*i+0].clear();
		vector<float> b1=obj2[3*i+1];
		vector<float> b2=obj2[3*i+2];
		obj2[3*i+1].clear();
		obj2[3*i+2].clear();
		
		
		unsigned int cpt1=0,cpt2=0;
		vector<float> *nbTerm=&obj2[3*i+0],*index=&obj2[3*i+1],*values=&obj2[3*i+2];
		nbTerm->push_back(numTerm);
		while ((cpt1<a1->size())&&(cpt2<b1.size())){
			if ((*a1)[cpt1]<b1[cpt2]){
				index->push_back((*a1)[cpt1]);
				values->push_back((*a2)[cpt1]/norm);
				cpt1++;
				continue;
			}
			if ((*a1)[cpt1]>b1[cpt2]){
				index->push_back(b1[cpt2]);
				values->push_back(b2[cpt2]);
				cpt2++;
				continue;
			}
			if ((*a1)[cpt1]==b1[cpt2]){
				index->push_back((*a1)[cpt1]);
				values->push_back((*a2)[cpt1]/norm+b2[cpt2]);
				cpt1++;
				cpt2++;
				continue;
			}
		}
		if (cpt1<a1->size()){
			while (cpt1<a1->size()) {
				index->push_back((*a1)[cpt1]);
				values->push_back((*a2)[cpt1]/norm);
				cpt1++;			
			}
		}
		else {
			while (cpt2<b1.size()) {
				index->push_back(b1[cpt2]);
				values->push_back(b2[cpt2]);
				cpt2++;			
			}
		}
		norm=0.f;
		for (int i=0;i<values->size();i++)
			norm+=(*values)[i]*(*values)[i];
		norm=sqrt(norm);
		
	}
}