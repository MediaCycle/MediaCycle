/*
 *  ACMediaTimedFeature.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 22/06/09
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
 *  - Julien Dubois - 28/07/09 : 
 *     added readFile
 *
 *  - Julien Dubois - 31/07/09 :
 *     added hist, getExtremaOfVector
 *
 *  - Julien Dubois - 03/08/09 :
 *     added std, weightedStdDeviation, stdAsVector
 *
 *  - Julien Dubois - 04/08/09 :
 *     added similarity, dist
 *
 */

#ifndef _ACMEDIATIMEDFEATURE_H
#define _ACMEDIATIMEDFEATURE_H

#include <armadillo>
#include <vector>
#include "ACMediaFeatures.h"

using namespace arma;
using namespace std;

class ACMediaTimedFeatures {
	// Class describing feature that are time stamped
	
protected:
	fcolvec time_v;
	fmat value_m;
	vector<float> seg_v; // vector containing segments in seconds
	string name;
	
public:
	ACMediaTimedFeatures();
	ACMediaTimedFeatures(fcolvec time_v, fmat value_m);
	ACMediaTimedFeatures(fcolvec time_v, fmat value_m, string name, vector<float> seg_v);
	
	// XS use this
	ACMediaTimedFeatures(const vector<float> &time, const vector< vector<float> > &value, string name, const vector<float> *seg_v=0);
	
	// XS do not use this: why pass the whole vector by value ?
	ACMediaTimedFeatures(vector<float> time, vector< vector<float> > value, string name, vector<float> seg_v);
	ACMediaTimedFeatures(vector<float> time, vector<float> value, string name, vector<float> seg_v);
	ACMediaTimedFeatures( float *time, int length, float **value, int dim, string name, vector< float >seg_v );
	~ACMediaTimedFeatures();
	
	void importFromFile(string filename);
	void importSegmentsFromFile(string filename);
	
	fcolvec getTime();
	float getTime(int index);
	void setTime(fcolvec);
	double* getTimeAsDouble();
	fmat getValue();
	float getValue(float index, float dim);
	void setValue(fmat);
	string getName(){return name;};
	void setName(string name){this->name = name;};
	size_t getLength();
	size_t getDim();
	void setSegments(){};
	vector<float> getSegments();
	float getSegments(int index);
	
	int getNearestTimePosition(float time, int mode);
	fmat getValueAtTime(fcolvec time_v);
	//fcolvec getValueAtColumn(size_t index);
	fmat getValueAtColumn(size_t index);
	//double* getValueAtColumnAsDouble(size_t index);
	
	vector< vector<float> > getValueAsVector();
	vector<float> getTimeAsVector();
	
	fmat weightedMean(ACMediaTimedFeatures* weight);
	fmat mean();
	fmat weightedStdDeviation(ACMediaTimedFeatures* weight);
	fmat std();
	vector<float> meanAsVector();
	vector<float> stdAsVector();
	vector<ACMediaTimedFeatures*> segment();
	ACMediaTimedFeatures* weightedMeanSegment(ACMediaTimedFeatures* weight);
	ACMediaTimedFeatures* meanSegment();
	
	ACMediaTimedFeatures* simpleSplineModeling();
	ACMediaTimedFeatures* delta();
	
	int readFile(std::string);
	
	umat hist(int nbrBin, float min = 0, float max = 0);
	vector<float> getExtremaOfVector(fcolvec column);
	
	fmat similarity(int mode = 0);
	float dist(fmat rowvector1, fmat rowvector2, int mode = 0);
};

#endif
