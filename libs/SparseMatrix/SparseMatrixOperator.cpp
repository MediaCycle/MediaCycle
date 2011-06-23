/*
 *  SparseMatrixOperator.cpp
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

#include "SparseMatrixOperator.h"
#include <math.h>
using namespace std;
ACSparseVector &SparseMatrixOperator::addVectors(std::vector<float> a0,std::vector<float> a1
													 ,std::vector<float> a2,std::vector<float> b0,
													 std::vector<float> b1,std::vector<float> b2){
	
	
	ACSparseVector desc;//=new ACSparseVector;
	if ((a0.size()!=1)||(b0.size()!=1))
		return desc;
	if (a0[0]!=b0[0])
		return desc;
	if ((a1.size()!=a2.size())||(b1.size()!=b2.size()))
		return desc;
	if ((a1[a1.size()-1]>a0[0])||(b1[b1.size()-1]>b0[0]))
		return desc;
	unsigned int cpt1=0,cpt2=0;
	vector<float> nbTerm,index,values;
	nbTerm.push_back(a0[0]);
	while ((cpt1<a1.size())&&(cpt2<b1.size())){
		if (a1[cpt1]<b1[cpt2]){
			index.push_back(a1[cpt1]);
			values.push_back(a2[cpt1]);
			cpt1++;
			continue;
		}
		if (a1[cpt1]>b1[cpt2]){
			index.push_back(b1[cpt2]);
			values.push_back(b2[cpt2]);
			cpt2++;
			continue;
		}
		if (a1[cpt1]==b1[cpt2]){
			index.push_back(a1[cpt1]);
			values.push_back(a1[cpt1]+b2[cpt2]);
			cpt1++;
			cpt2++;
			continue;
		}
	}
	if (cpt1<a1.size()){
		while (cpt1<a1.size()) {
			index.push_back(a1[cpt1]);
			values.push_back(a2[cpt1]);
			cpt1++;			
		}
	}
	else {
		while (cpt2<b1.size()) {
			index.push_back(b1[cpt2]);
			values.push_back(b2[cpt2]);
			cpt2++;			
		}
	}
	desc.push_back(nbTerm);
	desc.push_back(index);
	desc.push_back(values);
	return desc;	
}
void SparseMatrixOperator::multipleVectorByScalar(std::vector<float> &out0,std::vector<float> &out1,std::vector<float> &out2,std::vector<float> a0,std::vector<float> a1,std::vector<float> a2,float mult){
	
	out0.clear();
	out1.clear();
	out2.clear();
	if ((a0.size()!=1))
		return;
	out0.push_back(a0[0]);
	if (a1.size()==0||a2.size()==0)
		return;
	if ((a1[a1.size()-1]>a0[0]))
		return;
	if (a1.size()!=a2.size())
		return;
	//vector<float> nbTerm,index,values;	
	if (mult!=0.f){
		vector<float>::iterator iter2=a2.begin();
		for (vector<float>::iterator iter1=a1.begin();iter1!=a1.end();iter1++,iter2++){
			out1.push_back(*iter1);
			out2.push_back((*iter2)*mult);
		}
	}
}


float SparseMatrixOperator::norm(const  std::vector<float> &a0,const std::vector<float> &a1,const std::vector<float> &a2){
	float desc=0;
	if ((a0.size()!=1))
		return 0;
	if ((a1[a1.size()-1]>a0[0]))
		return 0;
	if (a1.size()!=a2.size())
		return desc;
	for (unsigned int i=0;i<a2.size();i++){
		desc+=a2[i];
	}
	desc=sqrt(desc);
	
	return desc;
}

ACSparseVector& fullToSparseVector(std::vector<float> in){
	ACSparseVector desc;
	vector<float> nbTerm,index,values;
	nbTerm.push_back(in.size());
	unsigned int cpt=0;
	for (vector<float>::iterator iter=in.begin();iter!=in.end();iter++){
		if ((*iter)!=0.f){
			
			index.push_back(cpt);
			values.push_back(*iter);
			cpt++;
		}
	}
	desc.push_back(nbTerm);
	desc.push_back(index);
	desc.push_back(values);
	return desc;
}

std::vector<float>& SparseToFullVector(ACSparseVector in){
	vector<float> desc;
	if (in.size()!=3)
		return desc;
	vector<float> a0=in[0];
	vector<float> a1=in[1];
	vector<float> a2=in[2];
	
	if ((a0.size()!=1))
		return desc;
	if ((a1[a1.size()-1]>a0[0]))
		return desc;
	if (a1.size()!=a2.size())
		return desc;
	unsigned int cpt=0;
	vector<float>::iterator iter2=a2.begin();
	for (vector<float>::iterator iter1=a1.begin();iter1!=a1.end();iter1++,iter2++){
		for (;cpt<(*iter1);cpt++)
			desc.push_back(0.f);
		desc.push_back(*iter2);
		cpt++;
	}
	for (;cpt<a0[0];cpt++)
		desc.push_back(0);
	return desc;
}

