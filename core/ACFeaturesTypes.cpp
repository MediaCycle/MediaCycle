/*
 *  ACFeaturesTypes.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 27/01/12
 *  @copyright (c) 2012 – UMONS - Numediart
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


#include "ACFeaturesTypes.h"
#include <iostream>
using namespace std;
using namespace boost::numeric::ublas;

/*typedef const double& const_reference;
typedef double& reference;
protected:
 
DistanceType distType;

boost::numeric::ublas::vector<double> v;

boost::numeric::ublas::compressed_vector<double> v2 ;
bool isSparse;*/


typedef const float& const_reference;
typedef float& reference;

FeaturesVector::FeaturesVector(bool isSparse,DistanceType distType){
	this->isSparse=isSparse;
	this->distType=distType;
};

FeaturesVector::FeaturesVector(bool isSparse, unsigned int pSize,DistanceType distType){
	this->isSparse=isSparse;
	if (isSparse)
		vSparse.resize(pSize);
	else
		vDense.resize(pSize);
	this->distType=distType;
};

FeaturesVector::FeaturesVector(std::vector<float> data,DistanceType distType){
	this->isSparse=false;
	vDense.resize(data.size());
	std::vector<float>::iterator iter1=data.begin();
	for (boost::numeric::ublas::vector<float>::iterator iter2=vDense.begin(); iter2!=vDense.end();iter2++){
		(*iter2)=(*iter1);
		iter1++;
	}
	this->distType=distType;	
};


void FeaturesVector::setDistanceType(DistanceType distType){
	this->distType=distType;
};

void FeaturesVector::setIsSparse(bool isSparse){
	if (this->isSparse!=isSparse){
		if (isSparse){
			vSparse=vDense;
			vDense.clear();
		}
		else {
			vDense=vSparse;
			vSparse.clear();
		}
	}
	this->isSparse=isSparse;
};

void FeaturesVector::init(){
	if (isSparse){
		for (unsigned int i=0;i<vSparse.size();i++)
			vSparse.erase_element(i);
	}
	else {
		for (unsigned int i=0;i<vDense.size();i++)
			vDense[i]=0;
	}
}

unsigned int FeaturesVector::size () const {
	if (isSparse)
		return vSparse.size();
	else 
		return vDense.size();
};

FeaturesVector &FeaturesVector::operator = (const FeaturesVector &v){
	this->distType=v.distType;
	this->isSparse=v.isSparse;
	this->vDense.clear();
	this->vSparse.clear();
	this->vDense=v.vDense;
	this->vSparse=v.vSparse;
	return *this;
};

const_reference FeaturesVector::operator [] (unsigned int i) const {
	if (isSparse)
		return vSparse[i];
	else 
		return vDense[i];
};

void FeaturesVector::set(unsigned int index,float val)
{
	if (isSparse){
		if (val!=0.f)
			vSparse[index]=val;
	}
	else 
		vDense[index]=val;
	
}
/*reference FeaturesVector::operator [] (unsigned int i){
	if (isSparse)
		return vSparse.ref(i);
	else 
		return vDense[i];
};*/

FeaturesVector &FeaturesVector::operator += (const FeaturesVector &ae){
	if (isSparse){
		if (ae.isSparse){
			vSparse+=ae.vSparse;
		
		}
		
		else{
			isSparse=false;
			vDense=vSparse+ae.vDense;
			vSparse.clear();
		} 
	}
	else 
		if (ae.isSparse){
			vDense+=ae.vSparse;
		}
		else{
			vDense+=ae.vDense;
		} 
	return (*this);
	
};

FeaturesVector &FeaturesVector::operator -= (const FeaturesVector &ae){
	if (isSparse){
		if (ae.isSparse){
			vSparse-=ae.vSparse;
		}
	
		else{
			isSparse=false;
			vDense=vSparse-ae.vDense;
			vSparse.clear();
		} 
	}
	else 
		if (ae.isSparse){
			vDense-=ae.vSparse;
		}
		else{
			vDense-=ae.vDense;
		} 
	return (*this);	
};

FeaturesVector &FeaturesVector::operator *= (const float &at){
	if (isSparse)
		 vSparse*=at;
	else 
		 vDense*=at;
	return (*this);
};

FeaturesVector &FeaturesVector::operator /= (const float &at){
	if (isSparse)
		 vSparse/=at;
	else 
		 vDense/=at;
	return (*this);
};

void FeaturesVector::resize(unsigned int pSize){
	if (isSparse)
		vSparse.resize(pSize);
	else
		vDense.resize(pSize);
	return;
	
};

float FeaturesVector::distance(const FeaturesVector &ae){
	return this->distance(&ae);
	/*unsigned int test=ae.size();
	
	if (ae.size() != this->size()){
		cerr << "<ACEuclideanDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = ae.size();
	if (s==0) {
		cerr << "<ACEuclideanDistance::distance> : empty features" << endl;
		return 0.0;
	}
	switch (distType) {
		case 0://euclidean distance	
		{
			double d=0.0;
			if (ae.isSparse&&this->isSparse)
			{
				compressed_vector<double>::iterator it1=this->vSparse.begin();
				compressed_vector<double>::iterator it2=ae.vSparse.begin();
				unsigned int i1=it1.index(),i2=it2.index();
				
				while (it1!=this->vSparse.end()&&it2!=ae.vSparse.end()){
					i1=it1.index();
					i2=it2.index();
					if (i1==i2){
						d+=pow((*it1-*it2),2);
						it1++;
						it2++;
						continue;
					}
					if (i1<i2){	
						d+=pow((*it1),2);
						it1++;
						continue;
					}
					if (i2<i1){	
						d+=pow((*it2),2);
						it2++;
						continue;
					}
				}
				return sqrt(d)/s;
			}
			
			if (ae.isSparse&&!(this->isSparse))
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae.vSparse[i]-this->vDense[i]),2);
				}	
				return sqrt(d)/s;
			}
			
			if (!(ae.isSparse)&&this->isSparse)
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae.vDense[i]-this->vSparse[i]),2);
				}	
				return sqrt(d)/s;
			}
			
			if (!(ae.isSparse)&&!(this->isSparse))
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae.vSparse[i]-this->vDense[i]),2);
				}	
				return sqrt(d)/s;
			}
			
		}
			break;
		case 1://cos distance
		{
			double a = 0.0, b= 0.0, ab= 0.0 ;
			
			if (ae.isSparse&&this->isSparse)
			{
				compressed_vector<double>::iterator it1=this->vSparse.begin();
				compressed_vector<double>::iterator it2=ae.vSparse.begin();
				unsigned int i1=it1.index(),i2=it2.index();
				
				while (it1!=this->vSparse.end()&&it2!=ae.vSparse.end()){
					i1=it1.index();
					i2=it2.index();
					if (i1==i2){
						float temp1=(*it1),temp2=(*it2);
						ab+= temp1*temp2;
						a+= temp1*temp1;
						b+= temp2*temp2;
						it1++;
						it2++;
						continue;
					}
					if (i1<i2){	
						a+= (*it1)*(*it1);
						it1++;
						continue;
					}
					if (i2<i1){	
						b+= (*it2)*(*it2);
						it2++;
						continue;
					}
				}
			}
			
			if (ae.isSparse&&!(this->isSparse))
			{
				for (int i=0; i < s; i++){
					float temp1=ae.vSparse[i],temp2=this->vDense[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
					
				}
			}
			
			if (!(ae.isSparse)&&this->isSparse)
			{
				for (int i=0; i < s; i++){
					float temp1=ae.vDense[i],temp2=this->vSparse[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
				}
			}
			if (!(ae.isSparse)&&!(this->isSparse))
			{
				for (int i=0; i < s; i++){
					float temp1=ae.vDense[i],temp2=this->vDense[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
				}
			}			
			if (a==0||b==0)
				return 1;
			return ((1.1/(0.1+ab/sqrt(a*b))-1)/10);
		}
			break;
			
		default:
			break;
	} */
}


float FeaturesVector::distance( const FeaturesVector *ae){
	unsigned int test=ae->size();
	
	if (ae->size() != this->size()){
		cerr << "<ACEuclideanDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = ae->size();
	if (s==0) {
		cerr << "<ACEuclideanDistance::distance> : empty features" << endl;
		return 0.0;
	}
	switch (distType) {
		case Euclidean://euclidean distance	
		{
			double d=0.0;
			if (ae->isSparse&&this->isSparse)
			{
				compressed_vector<float>::const_iterator it1=this->vSparse.begin();
				compressed_vector<float>::const_iterator it2=ae->vSparse.begin();
				unsigned int i1=it1.index(),i2=it2.index();
				
				while (it1!=this->vSparse.end()&&it2!=ae->vSparse.end()){
					i1=it1.index();
					i2=it2.index();
					if (i1==i2){
						d+=pow((*it1-*it2),2);
						it1++;
						it2++;
						continue;
					}
					if (i1<i2){	
						d+=pow((*it1),2);
						it1++;
						continue;
					}
					if (i2<i1){	
						d+=pow((*it2),2);
						it2++;
						continue;
					}
				}
				return sqrt(d)/s;
			}
			
			if (ae->isSparse&&!(this->isSparse))
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae->vSparse[i]-this->vDense[i]),2);
				}	
				return sqrt(d)/s;
			}
			
			if (!(ae->isSparse)&&this->isSparse)
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae->vDense[i]-this->vSparse[i]),2);
				}	
				return sqrt(d)/s;
			}
			
			if (!(ae->isSparse)&&!(this->isSparse))
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					d+= pow((ae->vDense[i]-this->vDense[i]),2);
				}	
				return sqrt(d)/s;
			}
			
		}
			break;
            case Cosinus: // JU: commented: 1://cos distance
		{
			double a = 0.0, b= 0.0, ab= 0.0 ;
			
			if (ae->isSparse&&this->isSparse)
			{
				compressed_vector<float>::const_iterator it1=this->vSparse.begin();
				compressed_vector<float>::const_iterator it2=ae->vSparse.begin();
				unsigned int i1,i2;
				
				while (it1!=this->vSparse.end()&&it2!=ae->vSparse.end()){
					i1=it1.index();
					i2=it2.index();
					if (i1==i2){
						float temp1=(*it1),temp2=(*it2);
						ab+= temp1*temp2;
						a+= temp1*temp1;
						b+= temp2*temp2;
						it1++;
						it2++;
						continue;
					}
					if (i1<i2){	
						a+= (*it1)*(*it1);
						it1++;
						continue;
					}
					if (i2<i1){	
						b+= (*it2)*(*it2);
						it2++;
						continue;
					}
				}
				while (it1!=this->vSparse.end()){
					i1=it1.index();
					a+= (*it1)*(*it1);
					it1++;
				}
				while (it2!=ae->vSparse.end()){
					i2=it2.index();
					b+= (*it2)*(*it2);
					it2++;
				}
			}
			
			if (ae->isSparse&&!(this->isSparse))
			{
				for (int i=0; i < s; i++){
					float temp1=ae->vSparse[i],temp2=this->vDense[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
					
				}
			}
			
			if (!(ae->isSparse)&&this->isSparse)
			{
				for (int i=0; i < s; i++){
					float temp1=ae->vDense[i],temp2=this->vSparse[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
				}
			}
			if (!(ae->isSparse)&&!(this->isSparse))
			{
				for (int i=0; i < s; i++){
					float temp1=ae->vDense[i],temp2=this->vDense[i];
					ab+= temp1*temp2;
					a+= temp1*temp1;
					b+= temp2*temp2;
				}
			}			
			if (a==0||b==0)
				return 1;
			return ((1.1/(0.1+ab/sqrt(a*b))-1)/10);
		}
			break;
			
		default:
			break;
    }
    return 0.0;
}


FeaturesVector & FeaturesVector::meanAdd( const FeaturesVector &ae){
	unsigned int test=ae.size();
	
	if (ae.size() != this->size()){
		cerr << "<ACEuclideanDistance::meanAdd> : inaccumulable features" << endl;
		return (*this);
	}
	int s = ae.size();
	if (s==0) {
		cerr << "<ACEuclideanDistance::meanAdd> : empty features" << endl;
		return (*this);
	}
	switch (distType) {
		case Euclidean://euclidean distance	
		{
			double d=0.0;
			if (ae.isSparse&&this->isSparse)
			{
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++){
					unsigned int i2=it2.index();
					this->vSparse[i2]+=(*it2);
					it2++;
				}
			}
			
			if (ae.isSparse&&!(this->isSparse))
			{
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++){
					unsigned int i2=it2.index();
					this->vDense[i2]+=(*it2);
					it2++;
				}
			}
			
			if (!(ae.isSparse)&&this->isSparse)
			{
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					if (ae.vDense[i]!=0)
						this->vSparse[i]+=ae.vDense[i];
				}	
			}
			
			if (!(ae.isSparse)&&!(this->isSparse))
			{
				this->vDense+=ae.vDense;
				
			}
			
		}
			break;
		case 1://cos distance
		{
			if (ae.isSparse&&this->isSparse)
			{	
				float norm=0.f;
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++)
					norm+=(*it2)*(*it2);
				norm=sqrt(norm);
					
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++){
					unsigned int i2=it2.index();
					this->vSparse[i2]=this->vSparse[i2]+(*it2)/norm;
				}
			}
			
			if (ae.isSparse&&!(this->isSparse))
			{
				float norm=0.f;
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++)
					norm+=(*it2)*(*it2);
				norm=sqrt(norm);
				for (compressed_vector<float>::const_iterator it2=ae.vSparse.begin(); it2!=ae.vSparse.end(); it2++){
					unsigned int i2=it2.index();
					this->vDense[i2]+=(*it2)/norm;
					it2++;
				}
			}
			if (!(ae.isSparse)&&this->isSparse)
			{
				float norm=0.f;
				for (int i=0; i < s; i++)
					norm+=ae.vDense[i]*ae.vDense[i];
				norm=sqrt(norm);
				double d = 0.0 ;
				for (int i=0; i < s; i++){
					if (ae.vDense[i]!=0)
						this->vSparse[i]+=ae.vDense[i]/norm;
				}	
			}
			if (!(ae.isSparse)&&!(this->isSparse))
			{
				float norm=0.f;
				for (int i=0; i < s; i++)
					norm+=ae.vDense[i]*ae.vDense[i];
				norm=sqrt(norm);
				this->vDense+=ae.vDense/norm;
				
			}
			break;
		}
		default:
			break;
		}
	return (*this);
};
	

void FeaturesVector::push_back(float elem){
	if (isSparse){
		int imax=vSparse.size();
		vSparse.resize(imax+1);
		if (elem!=0.0){
			vSparse[imax]=elem;
		}
	}
	else{
		
		int imax=vDense.size();
		vDense.resize(imax+1);
		vDense[imax]=elem;
		
	}
	

};

void FeaturesVector::print_features()
{
    for (int i=0; i<vDense.size(); i++)
    {
        cout << vDense[i] << " ";
    }
    cout << endl;
}
