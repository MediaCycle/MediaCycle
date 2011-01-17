/*
 *  ACBicSegmentationPlugin.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 6/12/10
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

#ifndef _ACBICSEGMENTATIONPLUGIN_H
#define	_ACBICSEGMENTATIONPLUGIN_H

#include "Armadillo-utils.h" 
#include "MediaCycle.h"
#include "ACMediaTimedFeature.h"

class ACBicSegmentationPlugin : public ACPlugin {
public:
	ACBicSegmentationPlugin();
	~ACBicSegmentationPlugin();
	
	virtual int initialize(){return 1;};
	virtual int start(){return 1;};
	virtual int stop(){return 1;};
	
	virtual std::vector<ACMedia*> segment(ACMediaData* _data, ACMedia* _theMedia);
	std::vector<int> segment(const vector< vector<float> > & _allfeatures, float _lambda=1, int _samplingrate=1);
	// XS todo: synchronize default values between constructor and segment
	std::vector<int> segment(arma::fmat _M, float _lambda=1, int _samplingrate=1, int _Wmin=20, float _bic_thresh = 1, int _jump_width=5);
        std::vector<int> segment(std::vector <ACMediaTimedFeature*> _ACMTF, float _lambda=1, int _samplingrate=1, int _Wmin=20, float _bic_thresh = 1, int _jump_width=5);

        arma::fmat get_features() {return full_features;}; //JU: added to ease the visualization during the tests

private:
	int findSingleSegment(int _A, int _B);
	float detCovariance (int _cinf, int _csup);
	std::vector<int> _segment();

	float lambda;
	int sampling_rate;
	int Wmin; 
	float bic_thresh;
	int jump_width;

	arma::fmat full_features;
};


#endif	// _ACBICSEGMENTATIONPLUGIN_H
