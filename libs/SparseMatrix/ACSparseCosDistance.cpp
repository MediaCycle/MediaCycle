/*
 *  ACSparseCosDistance.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 21/06/11
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

#include "ACSparseCosDistance.h"

#include <math.h>
#include <iostream>
using namespace std;

ACSparseCosDistance::ACSparseCosDistance(const vector<ACMediaFeatures*> &F1,const  vector<ACMediaFeatures*> &F2) : ACSparseDistance( F1, F2) {
	if (F1.size()==3){
		V1.push_back((F1[0]->getFeaturesVector()));
		V1.push_back((F1[1]->getFeaturesVector()));
		V1.push_back((F1[2]->getFeaturesVector()));
	}

	if (F2.size()==3){
		
		V2.push_back((F2[0]->getFeaturesVector()));
		V2.push_back((F2[1]->getFeaturesVector()));
		V2.push_back((F2[2]->getFeaturesVector()));
	}
	
}

ACSparseCosDistance::ACSparseCosDistance( ACSparseVector &F1,  ACSparseVector &F2) : ACSparseDistance( F1, F2) {

	for (unsigned int i=0;i<F1.size();i++)//;iter!=F1->end();iter++)
		V1.push_back(&(F1.at(i)));
	
	for (unsigned int i=0;i<F2.size();i++)//;iter!=F1->end();iter++)
		V2.push_back(&(F2.at(i)));	
	copyVec=true;
}

ACSparseCosDistance::~ACSparseCosDistance(){
}

double ACSparseCosDistance::distance(){
	
	if ((V1.size()!=3 )|| (V2.size()!=3)){
		cerr << "<ACSparseCosDistance::distance> : incompatible sparse vector type" << endl;
		return 0.0;
	}
	if (((V1[0])->size()!=1 )|| ((V2[0])->size()!=1)){
		cerr << "<ACSparseCosDistance::distance> : incompatible sparse vector type" << endl;
		return 0.0;
	}
	if ((V1[1]->size()!=V1[2]->size() )|| (V2[1]->size()!=V2[2]->size())){
		cerr << "<ACSparseCosDistance::distance> : incompatible sparse vector type" << endl;
		return 0.0;
	}
	if ((*V1[0])[0]!=(*V2[0])[0]){
		cerr << "<ACSparseCosDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = (*V1[0])[0];
	if (s==0) {
		cerr << "<ACSparseCosDistance::distance> : empty features" << endl;
		return 0.0;
	}
	
	unsigned int cpt1=0,cpt2=0;
	
	float a = 0.f, b= 0.f, ab= 0.f ;
	
	for (unsigned int i=0;i<V1[2]->size();i++){
		a+=(*V1[2])[i]*(*V1[2])[i];
	}
	a=sqrt(a);
	for (unsigned int i=0;i<V2[2]->size();i++){
		b+=(*V2[2])[i]*(*V2[2])[i];
	}
	b=sqrt(b);
//	cout<<V1[2]->size()<<"\t"<<(*V1[0])[0]<<"\t"<<(*V2[0])[0]<<"\t"<<V2[2]->size()<<endl;
	while ((cpt1<V1[2]->size())&& (cpt2<V2[2]->size())){

		if ((int)(*V1[1])[cpt1]==(int)(*V2[1])[cpt2]) {
			ab+=((*V1[2])[cpt1])*((*V2[2])[cpt2]);
	//		cout << cpt1 << "\t"<<(*V1[1])[cpt1] << "\t"<<(*V1[2])[cpt1]<< "\t"<<cpt2 << "\t"<<(*V2[1])[cpt2] << "\t"<<(*V2[2])[cpt2]<<"\n";
			cpt1++;
			cpt2++;
			continue;
		}
		if ((*V1[1])[cpt1]<(*V2[1])[cpt2]) {
	//		cout << cpt1 << "\t"<<(*V1[1])[cpt1] << "\t"<<(*V1[2])[cpt1]<< "\t"<<cpt2 << "\t"<<(*V2[1])[cpt2] << "\t"<<"0"<<"\n";
			cpt1++;
			continue;
		}
		if ((*V1[1])[cpt1]>(*V2[1])[cpt2]) {
			
	//		cout << cpt1 << "\t"<<(*V1[1])[cpt1] << "\t"<<"0"<< "\t"<<cpt2 << "\t"<<(*V2[1])[cpt2] << "\t"<<(*V2[2])[cpt2]<<"\n";
			cpt2++;
			continue;
		}
	}
	
	if (a==0||b==0)
		return 1;
	//	return 1-(ab/sqrt(a*b));
	double desc=ab/(a*b);
	desc=desc;
	return ((1.1/(0.1+ab/(a*b))-1)/10);
	
}
