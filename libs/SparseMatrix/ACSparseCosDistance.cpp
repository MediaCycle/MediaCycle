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

ACSparseCosDistance::ACSparseCosDistance(ACMediaFeatures* F1Num,ACMediaFeatures* F1Index,ACMediaFeatures* F1Value,ACMediaFeatures* F2Num,ACMediaFeatures* F2Index,ACMediaFeatures* F2Value)
:ACSparseDistance( F1Num, F1Index, F1Value, F2Num, F2Index, F2Value){

	
	num1=F1Num->getFeaturesVector();
	num2=F2Num->getFeaturesVector();
	index1=F1Index->getFeaturesVector();
	index2=F2Index->getFeaturesVector();
	value1=F1Value->getFeaturesVector();
	value2=F2Value->getFeaturesVector();
}

ACSparseCosDistance::ACSparseCosDistance(const FeaturesVector* F1Num,const FeaturesVector* F1Index,const FeaturesVector* F1Value,const FeaturesVector* F2Num,const FeaturesVector* F2Index,const FeaturesVector* F2Value)
:ACSparseDistance( F1Num, F1Index, F1Value, F2Num, F2Index, F2Value){	

	num1=F1Num;
	num2=F2Num;
	index1=F1Index;
	index2=F2Index;
	value1=F1Value;
	value2=F2Value;
	
}

ACSparseCosDistance::~ACSparseCosDistance(){
}

double ACSparseCosDistance::distance(){
	

	if (((num1)->size()!=1 )|| ((num2)->size()!=1)){
		cerr << "<ACSparseCosDistance::distance> : incompatible sparse vector type" << endl;
		return 0.0;
	}
	if ((index1->size()!=value1->size() )|| (index2->size()!=value2->size())){
		cerr << "<ACSparseCosDistance::distance> : incompatible sparse vector type" << endl;
		return 0.0;
	}
	if (num1->at(0)!=num2->at(0)){
		cerr << "<ACSparseCosDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = num1->at(0);
	if (s==0) {
		cerr << "<ACSparseCosDistance::distance> : empty features" << endl;
		return 0.0;
	}
	
	unsigned int cpt1=0,cpt2=0;
	
	float a = 0.f, b= 0.f, ab= 0.f ;
	
	for (unsigned int i=0;i<value1->size();i++){
		a+=value1->at(i)*value1->at(i);
	}
	a=sqrt(a);
	
	for (unsigned int i=0;i<value2->size();i++){
		b+=value2->at(i)*value2->at(i);
	}
	b=sqrt(b);
	
//	cout<<V1[2]->size()<<"\t"<<(*V1[0])[0]<<"\t"<<(*V2[0])[0]<<"\t"<<V2[2]->size()<<endl;
	
	while ((cpt1<value1->size())&& (cpt2<value2->size())){

		
		if ((int)index1->at(cpt1)==(int)index2->at(cpt2)) {
		
			ab+=(value1->at(cpt1)*value2->at(cpt2));
	//		cout << cpt1 << "\t"<<(*V1[1])[cpt1] << "\t"<<(*V1[2])[cpt1]<< "\t"<<cpt2 << "\t"<<(*V2[1])[cpt2] << "\t"<<(*V2[2])[cpt2]<<"\n";
			cpt1++;
			cpt2++;
			continue;
		}
		if ((int)index1->at(cpt1)<(int)index2->at(cpt2)) {

		//		cout << cpt1 << "\t"<<(*V1[1])[cpt1] << "\t"<<(*V1[2])[cpt1]<< "\t"<<cpt2 << "\t"<<(*V2[1])[cpt2] << "\t"<<"0"<<"\n";
			cpt1++;
			continue;
		}
		if ((int)index1->at(cpt1)>(int)index2->at(cpt2)) {
			
			
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
