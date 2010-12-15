/*
 *  ACBicSegmentationPlugin.cpp
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

#include "ACBicSegmentationPlugin.h"

#include <vector>
#include <string>

// lambda = weighting factor in BIC formulation = balance between model precision and number of parameters of the model
// Wmin = minimum length of the window in which to search for segment change (in number of frames)
// sampling_rate = n if we skip n frames in the analysis (1 = don't skip any)
// bic_thresh = threshold for the BIC under which there is no segment detected
// jump_width = number of frames to skip after a step has been detected
ACBicSegmentationPlugin::ACBicSegmentationPlugin() : lambda(1), sampling_rate(0), Wmin(20), bic_thresh(1.0), jump_width(5){
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mPluginType = PLUGIN_TYPE_SEGMENTATION;
    this->mName = "BicSegmentation";
    this->mDescription = "BicSegmentation plugin";
    this->mId = "";
}

ACBicSegmentationPlugin::~ACBicSegmentationPlugin() {
}

std::vector<ACMedia*> ACBicSegmentationPlugin::segment(ACMediaData* _data, ACMedia* _theMedia) {
// XS TODO
	cout <<"todo" << endl;
}

std::vector<int> ACBicSegmentationPlugin::segment(const vector< vector<float> > & _allfeatures, \
												  float _lambda, \
												  int _samplingrate){
	this->lambda = _lambda;
	this->sampling_rate = _samplingrate;
	
	// transforming vector< vector<> > into fmat
	int c = _allfeatures.size();
	int l = _allfeatures[0].size();
	this->full_features = arma::fmat((int) c, (int) l); 
	for ( int Itime=0; Itime< c; Itime++){
		for ( int Idim=0; Idim<l; Idim++){
			this->full_features(Itime, Idim) = _allfeatures[Itime][Idim];
		}
	}
	
	return (this->_segment());
}

std::vector<int> ACBicSegmentationPlugin::segment(arma::fmat _M, \
												  float _lambda, \
												  int _samplingrate){
	this->lambda = _lambda;
	this->sampling_rate = _samplingrate;
	this->full_features = _M ; 

	return (this->_segment());
}	
	
//supposes we have defined:
// - this->lambda = _lambda;
// - this->sampling_rate = _samplingrate;
// - this->full_features = _M ; 

std::vector<int> ACBicSegmentationPlugin::_segment(){
	// XS test most significant segment (only one)
	// this->findSingleSegment(0,this->full_features.n_cols-1);
	
	// growing window algorithm (NB not efficient if long uniform segments)

	int seg_i = 0;
	int seg_f = Wmin;
	std::vector<int> segments_tmp;

	while (seg_f < int (this->full_features.n_cols)) {
		int s = this->findSingleSegment(seg_i, seg_f);
		if (s > 0) {
			segments_tmp.push_back(s);
			seg_i = s + jump_width;
			seg_f = seg_i + Wmin;
		}
		seg_f += sampling_rate;
	}
	
	return segments_tmp;

}


int ACBicSegmentationPlugin::findSingleSegment(int A, int B){
	if (B-A <=2){
		std::cout << "<ACBicSegmentationPlugin::findSingleSegment> : need at least 3 samples to segment !" << std::endl;
		return -1;
	}
	else if (A>B){
		std::cout << "<ACBicSegmentationPlugin::findSingleSegment> : swap limits !" << std::endl;
		return -1;
	}
	else if (A < 0 || B < 0){ // one of them is negative
		std::cout << "<ACBicSegmentationPlugin::findSingleSegment> : limits  must be positive !" << std::endl;
		return -1;
	}
		
	float max_bic = bic_thresh;
	int imax_bic = 0;
	float deltaBICi = 0.0;

	// standard notations cf. papers on BIC e.g., ..
	// with S = |sigma| = det(covariance matrix)
	int N = B-A;
	int N1 = 0;
	int N2 = 0;
	int d=this->full_features.n_rows;
	
	float P = .5 * (d + .5 * d * (d+1)) * log(N);
	float lP = lambda * P;
	
	float S1 = 0.0;
	float S2 = 0.0;
	float S = detCovariance (A,B);
	float R = S * N;
	
	for (int i=A+1; i<B-1; i++){
		N1=i-A;
		N2=B-i;
		// XS TODO : use running_stat_vec<type>(true) 
		S1 = detCovariance (A,i);
		S2 = detCovariance (i,B);
		deltaBICi = R - N1*S1 - N2*S2 -lP;
		
		if ( deltaBICi > max_bic) {
			// store current maximum = most likely cut
			max_bic = deltaBICi;
			imax_bic = i;
		}
	}
	if (max_bic > bic_thresh) {
		cout << "max bic : " << max_bic << " ; found at time : " << imax_bic << endl;
	}
	return imax_bic;
}

float ACBicSegmentationPlugin::detCovariance(int _cinf, int _csup){
	return arma::det (arma::cov(this->full_features.cols(_cinf, _csup)));
}