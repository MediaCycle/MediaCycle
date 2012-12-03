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

class ACMediaTimedFeature {
	// Class describing feature that are time stamped
	
protected:
	arma::fcolvec time_v;
	arma::fmat value_m;
	std::vector<float> seg_v; // vector containing segments in seconds
	std::string name;
    std::vector<std::string> names;
	
public:
	ACMediaTimedFeature();
	ACMediaTimedFeature(long iSize, int iDim, std::string name);
	ACMediaTimedFeature(arma::fcolvec time_v, arma::fmat value_m);
	ACMediaTimedFeature(arma::fcolvec time_v, arma::fmat value_m, std::string name, std::vector<float> *seg_v=0);
	// XS use const & to avoid passing the whole vector by value
	ACMediaTimedFeature(const std::vector<float> &time, const std::vector< std::vector<float> > &value, std::string name, const std::vector<float> *seg_v=0);
	ACMediaTimedFeature(const std::vector<float> &time, const std::vector<float> &value, std::string name, const std::vector<float> *seg_v=0);
	ACMediaTimedFeature( float *time, int length, float **value, int dim, std::string name, std::vector< float > *seg_v=0 );
	~ACMediaTimedFeature();
	
	arma::fcolvec getTime();
	float getTime(int index);
	void setTime(arma::fcolvec);
	double* getTimeAsDouble();
	arma::fmat getValue();
	arma::fmat getValue(std::string featuresName);
	float getValue(float index, float dim);
	void setValue(arma::fmat);
    void setTimeAndValueForIndex(long iIndex, double iTime, arma::frowvec iVal_v);
	void setTimeAndValueForIndex(long iIndex, double iTime, std::vector<float> iVal);
	std::string getName(){return name;};
    const std::vector<std::string> &getNames(){return names;};
    std::vector<std::string> getDistinctNames();
	void setName(std::string name);
	size_t getLength();
	size_t getDim();
        bool isConsistent();
	void setSegments(std::vector<float> iSeg_v){seg_v=iSeg_v;};
	std::vector<float> getSegments();
	float getSegments(int index);
	
	int getNearestTimePosition(float time, int mode);
	arma::fmat getValueAtTime(arma::fcolvec time_v);
	//arma::fcolvec getValueAtColumn(size_t index);
	arma::fmat getValueAtColumn(size_t index);
	//double* getValueAtColumnAsDouble(size_t index);
	
	std::vector< std::vector<float> > getValueAsVector();
	std::vector<float> getTimeAsVector();
	
	ACMediaFeatures* weightedMean(ACMediaTimedFeature* weight);
	ACMediaFeatures* mean();
	ACMediaFeatures* weightedStdDeviation(ACMediaTimedFeature* weight);
	ACMediaFeatures* std();
	ACMediaFeatures* hist();
	ACMediaFeatures* max();
	// Using boost library and moments calculation
	ACMediaFeatures* centroid();
	ACMediaFeatures* spread();
	ACMediaFeatures* skew();
	ACMediaFeatures* kurto(); 

	
	ACMediaFeatures* toMediaFeatures(); // only for one dim features

	//	std::vector<float> meanAsVector();
	//      std::vector<float> stdAsVector();
	std::vector<ACMediaTimedFeature*> segment();
	ACMediaTimedFeature* weightedMeanSegment(ACMediaTimedFeature* weight);
	ACMediaTimedFeature* meanSegment();
	
	ACMediaTimedFeature* simpleSplineModeling();
	ACMediaFeatures* temporalModel(double, double);
	ACMediaTimedFeature* delta();

	ACMediaTimedFeature* interpN(int n);
	
	//	umat hist(int nbrBin, float min = 0, float max = 0);
	
	arma::fmat similarity(int mode = 0);
	arma::fmat similarity(ACMediaTimedFeature *B);
	float dist(arma::fmat rowvector1, arma::fmat rowvector2, int mode = 0);
	
	// concatenate with another timed Feature
	// XS TODO: must have the same time stamps
	bool appendTimedFeature(ACMediaTimedFeature* B);
        bool appendTimedFeatureAlongTime(ACMediaTimedFeature* B);

	// I/O
	// old
	int readFile(std::string);
	int saveAsTxt(std::string fname);
	void importFromFile(std::string filename);
	void importSegmentsFromFile(std::string filename);

	// new (180111)
	bool saveInFile(std::string _fname, bool _binary = true);
	bool loadFromFile(std::string _fname, bool _binary = true);
	void dump(); // in terminal

	// SDIF is a library developed at IRCAM
	#ifdef USE_SDIF
		int saveAsSdif(const char* name);
	#endif

};

arma::fmat vectorACMTF2fmat(std::vector <ACMediaTimedFeature*> _ACMTF);

#endif //_ACMEDIATIMEDFEATURE_H
