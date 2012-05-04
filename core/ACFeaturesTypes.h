/*
 *  ACFeaturesTypes.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

// Definitions used along with features


#ifndef _ACFEATURESTYPES_H
#define _ACFEATURESTYPES_H

#ifndef Q_MOC_RUN

#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#endif

class FeaturesVector{
    typedef const float& const_reference;
    typedef float& reference;
public:
	enum DistanceType {
		Euclidean,
		Cosinus
	};
protected:
	DistanceType distType;
	
#ifndef Q_MOC_RUN
    boost::numeric::ublas::vector<float> vDense;
	boost::numeric::ublas::compressed_vector<float> vSparse ;
#endif
	bool isSparse;
	
public:
	FeaturesVector(bool isSparse=false,DistanceType distType=Euclidean);
	FeaturesVector(bool isSparse, unsigned int pSize,DistanceType distType=Euclidean );
	FeaturesVector(std::vector<float> data,DistanceType distType=Euclidean);
	bool getIsSparse(void){return isSparse;};
	void setIsSparse(bool isSparse);
	void setDistanceType(DistanceType distType);
	DistanceType getDistanceType(void){return distType;};
	
	unsigned int size () const;
	void init();
	FeaturesVector &operator = (const FeaturesVector &v);
	const_reference operator [] (unsigned int i) const ;
	void set(unsigned int index,float val);
	//reference operator [] (unsigned int i);
	FeaturesVector &operator += (const FeaturesVector &ae);
	FeaturesVector &operator -= (const FeaturesVector &ae);
	FeaturesVector &operator *= (const float &at);
	FeaturesVector &operator /= (const float &at);
	void resize(unsigned int pSize);
	float distance(const FeaturesVector &ae);
	float distance( const FeaturesVector *ae);
	FeaturesVector & meanAdd( const FeaturesVector &ae);
	void push_back(float elem);
};
//typedef std::vector<double> FeaturesVectorAsDouble;
// other option : make FeaturesVector a class or a template

#endif  // ACFEATURESTYPES_H
