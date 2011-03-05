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
ACBicSegmentationPlugin::ACBicSegmentationPlugin() : lambda(1), sampling_rate(1), Wmin(20), bic_thresh(0.5), jump_width(5), discard_borders(5){
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mPluginType = PLUGIN_TYPE_SEGMENTATION;
    this->mName = "BicSegmentation";
    this->mDescription = "BicSegmentation plugin";
    this->mId = "";
// equivalently	setParameters(1, 1, 20, 0.5, 5, 5)
}

ACBicSegmentationPlugin::~ACBicSegmentationPlugin() {
}

void ACBicSegmentationPlugin::setParameters(float _lambda, int _samplingrate, int _Wmin, float _bic_thresh, int _jump_width, int _discard_borders){
	this->lambda = _lambda;
	this->sampling_rate = _samplingrate;
	this->Wmin = _Wmin;
	this->bic_thresh = _bic_thresh;
	this->jump_width = _jump_width;
	this->discard_borders=_discard_borders;
	
}

// remember :
// - ACMediaData* _data = the full information about a media (e.g. whole image) that usually gets deleted by MC after analysis
// - ACMedia* _theMedia = what remains from the media after import (e.g. features + thumbnail...)
// here we mostly need _theMedia but this structure is compatible with ACAudioSegmentationPlugin

//std::vector<ACMedia*> ACBicSegmentationPlugin::segment(ACMediaData* _data, ACMedia* _theMedia) {
//// XS TODO : how to set _lambda, _sampling_rate from MediaCycle's GUI ?
//// could add a .ui file with the segmentation plugin to set the options ?
//	
//	std::vector<ACMediaFeatures*> _allfeatures = _theMedia->getAllFeaturesVectors();
//	int c = _allfeatures.size();
//	int l = _allfeatures[0]->getSize();
//	this->full_features = arma::fmat((int) c, (int) l); 
//	for ( int Itime=0; Itime< c; Itime++){
//		for ( int Idim=0; Idim<l; Idim++){
//			this->full_features(Itime, Idim) = _allfeatures[Itime]->getFeatureElement(Idim);
//		}
//	}
//	
//	// get the limits BETWEEN segments as integers
//	// usually does not contain 0 nor the last index
//	std::vector<int> segments_limits = this->_segment();
//
//	//transform them into ACMedia*
//	vector<ACMedia*> segments;
//
//	int Nseg = segments_limits.size();
//	if (Nseg == 0) {
//		cerr << "< ACBicSegmentationPlugin::segment> : no segments" << endl;
//		return segments; // XS check this
//	}
//	
//	// add the beginning of first segment (0)
////	if (segments_limits[0] != 0) segments_limits.push_front(0);
////	if (segments_limits[Nseg] !=
//		
//
////	for (int i = 0; i < seg_m.n_rows; i++){
////		ACMedia* media = ACMediaFactory::getInstance()->create(theAudio);
////		media->setParentId(theMedia->getId());
////		media->setStart(seg_m(i,0));
////		media->setEnd(seg_m(i,1));
////		segments.push_back(media);
////	}
//	
//	return segments;
//}
//
//std::vector<int> ACBicSegmentationPlugin::segment(const vector< vector<float> > & _allfeatures, \
//												  float _lambda, \
//												  int _samplingrate){
//	this->lambda = _lambda;
//	if (_samplingrate >0) this->sampling_rate = _samplingrate;
//	
//	// transforming vector< vector<> > into fmat
//	int c = _allfeatures.size();
//	int l = _allfeatures[0].size();
//	this->full_features = arma::fmat((int) c, (int) l); 
//	for ( int Itime=0; Itime< c; Itime++){
//		for ( int Idim=0; Idim<l; Idim++){
//			this->full_features(Itime, Idim) = _allfeatures[Itime][Idim];
//		}
//	}
//	
//	return (this->_segment());
//}

std::vector<int> ACBicSegmentationPlugin::segment(arma::fmat _M, \
												  float _lambda, \
												  int _samplingrate, \
												  int _Wmin, \
												  float _bic_thresh, \
												  int _jump_width, \
												  int _discard_borders  ){
	this->full_features = _M ; 
	
	this->lambda = _lambda;
	this->sampling_rate = _samplingrate;
	this->Wmin = _Wmin;
	this->bic_thresh = _bic_thresh;
	this->jump_width = _jump_width;
	this->discard_borders=_discard_borders;
	return (this->_segment());
}


std::vector<ACMedia*> ACBicSegmentationPlugin::segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia){
	//XS TODO: not efficient to transpose !! 
	// make this more coherent
	this->full_features = arma::trans (_MTF -> getValue());
	
	// get the limits BETWEEN segments as integers
	// usually does not contain 0 nor the last index
	std::vector<int> segments_limits = this->_segment();
	
	//transform them into ACMedia*
	vector<ACMedia*> segments;
	
	int Nseg = segments_limits.size();
	if (Nseg == 0) {
			cerr << "< ACBicSegmentationPlugin::segment> : no segments" << endl;
			return segments; // XS check this
	}
		
	// the beginning of first segment should be zero
	// no push_front for vectors (only for list)
	if (segments_limits[0] != 0) {
		vector<int>::iterator it;
		it = segments_limits.begin();
		it = segments_limits.insert ( it , 0 );
		Nseg++;
	}			
	
	for (int i = 0; i < Nseg-1; i++){
		//make sur the segment from the media have the proper type
		ACMedia* media = ACMediaFactory::getInstance()->create(_theMedia);
		media->setParentId(_theMedia->getId());
		media->setStart(segments_limits[i]);
		media->setEnd(segments_limits[i+1]);
		segments.push_back(media);
	}
		
	return segments;
}


std::vector<int> ACBicSegmentationPlugin::segment(std::vector <ACMediaTimedFeature*> _ACMTF, \
												  float _lambda, \
												  int _samplingrate, \
												  int _Wmin, \
												  float _bic_thresh, \
												  int _jump_width, \
												  int _discard_borders){
	this->full_features = arma::trans(vectorACMTF2fmat(_ACMTF)) ;
	
	this->lambda = _lambda;
	this->sampling_rate = _samplingrate;
	this->Wmin = _Wmin;
	this->bic_thresh = _bic_thresh;
	this->jump_width = _jump_width;
	this->discard_borders=_discard_borders;
	return (this->_segment());
}

std::vector<int> ACBicSegmentationPlugin::segmentDAC(arma::fmat _M, \
												  float _lambda, \
												  int _Wmin, \
												  float _bic_thresh, \
                                                                                                  int _discard_borders, \
                                                                                                  float _bic_thresh_DAC){
	this->full_features = _M ;

	this->lambda = _lambda;
	this->Wmin = _Wmin;
	this->bic_thresh = _bic_thresh;
        this->discard_borders=_discard_borders;
        this->bic_thresh_DAC=_bic_thresh_DAC;
	return (this->_segmentDAC());
}

std::vector<int> ACBicSegmentationPlugin::segmentDAC(std::vector <ACMediaTimedFeature*> _ACMTF, \
												  float _lambda, \
												  int _Wmin, \
												  float _bic_thresh, \
                                                                                                  int _discard_borders, \
                                                                                                  float _bic_thresh_DAC){
	this->full_features = arma::trans(vectorACMTF2fmat(_ACMTF)) ;

	this->lambda = _lambda;
	this->Wmin = _Wmin;
	this->bic_thresh = _bic_thresh;
        this->discard_borders=_discard_borders;
        this->bic_thresh_DAC=_bic_thresh_DAC;
	return (this->_segmentDAC());
}
	
//supposes we have defined:
// - this->lambda = _lambda;
// - this->sampling_rate = _samplingrate;
// - this->full_features = _M ; 

std::vector<int> ACBicSegmentationPlugin::_segment(){
	// XS test most significant segment (only one):
	// this->findSingleSegment(0,this->full_features.n_cols-1);
	
	// growing window algorithm (NB not efficient if long uniform segments)

	int seg_i = 0;
	int seg_f = Wmin;
	std::vector<int> segments_tmp;

	while (seg_f < int (this->full_features.n_cols)) {
		int s = this->findSingleSegment(seg_i, seg_f);
		if (s >= seg_i){ 
			// segment found
			segments_tmp.push_back(s);
			seg_i = s + jump_width; // JU: I believe we should always take all the frames in the BIC criterion (so, +1) even if we prevent for having segments shorter than Wmin: the first frames of the segment are relevant for its statistical modelling, hence finiding the segment end.
			seg_f = seg_i + Wmin; 
		}
		else
			// segment not yet found
			seg_f += sampling_rate;
	}
	
	cout << "found " << segments_tmp.size() << " segment(s)" << endl;

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
 	float R = S * N ; 
	
	for (int i=A+2; i<B; i++){ //JU: modified indexes so as to have at least 2 frames for each test
		N1=i-A;
		N2=B-i;
		// XS TODO : use running_stat_vec<type>(true) 
		S1 = detCovariance (A,i-1); // i-1 so frame i is not in both sides...
		S2 = detCovariance (i,B);
		deltaBICi = R - N1*S1 - N2*S2 -lP;
		//cout << deltaBICi << endl;
		if (deltaBICi > max_bic) {
			// store current maximum = most likely cut
			max_bic = deltaBICi;
			imax_bic = i;
		}
	}
	cout << "["<< A << "," << B << "] : Max BIC = " << max_bic << " at " << imax_bic << endl;

	if (max_bic > bic_thresh) {
		cout << "max bic : " << max_bic << " ; found at time : " << imax_bic << endl;
	}
        
        if(imax_bic<A+discard_borders||imax_bic>B-discard_borders) // JU: preventing from segmenting close to the borders of the BIC window: should we keep it, it should become an argument of the class
        {
            return 0;
        }
	return imax_bic;
}

float ACBicSegmentationPlugin::detCovariance(int _cinf, int _csup){
	//double d = abs(arma::det (arma::cov(this->full_features.cols(_cinf, _csup))));
	double d = abs(arma::det (arma::cov(arma::trans(this->full_features.cols(_cinf, _csup)))));
	double ld;
	if (d > 0) 
		ld = log(d)/2.0;
	else
		ld = 0;
	return float(ld);
}


std::vector<int> ACBicSegmentationPlugin::_segmentDAC(){
    segments_begin_i.clear();
    _segmentDAC(0,this->full_features.n_cols-1);
    return segments_begin_i;
}

void ACBicSegmentationPlugin::_segmentDAC(int begin_i, int end_i){
   //1) if window smaller than Nmin, return
    if(end_i-begin_i<Wmin)
    {
        return;
    }

    //2) find maximum Bic in window
    bool BIC_POSITIVE=false;
    int position;
    this->findSingleSegment(begin_i,end_i);
    if(imax_bic<begin_i+Wmin/2||imax_bic>end_i-Wmin/2)
    {
        return;
    }


    if(max_bic>bic_thresh_DAC&&imax_bic>0)
    {
        BIC_POSITIVE=true;
    }
    position=imax_bic;

    // 3) divide and solve sub-instances
    if(position>0)
    {
        _segmentDAC(begin_i,position-1);
        _segmentDAC(position, end_i);
    }

    // 4) Combine and validate segment
    if(BIC_POSITIVE)
    {
        segments_begin_i.push_back(position);
    }
    else
    {
        // find borders of analysis
        int b=begin_i, e=end_i;
        for(int i=0;i<segments_begin_i.size();i++)
        {
            if(segments_begin_i[i]<position&&segments_begin_i[i]>b)
            {
                b=segments_begin_i[i];
            }
            if(segments_begin_i[i]>position&&segments_begin_i[i]<e)
            {
                e=segments_begin_i[i]-1;
            }
        }
        // compute BIC value at "position", inside window [b,e]. If result > bic_thresh_combine, validate segmentation at "position"
        if(computeBIC(b,e,position)>bic_thresh_DAC)
        {
            segments_begin_i.push_back(position);
        }
    }


}

float ACBicSegmentationPlugin::computeBIC(int begin_i, int end_i, int position){
        int N = end_i-begin_i+1;
	int N1 = position-begin_i;
	int N2 = end_i-position+1;
        if(N<=0||N1<=0||N2<=0)
        {
            return (-1.0/0.0); // returning -Inf on purpose...
        }
	int d=this->full_features.n_rows;

	float P = .5 * (d + .5 * d * (d+1)) * log(N);
	float lP = lambda * P;

	float S = detCovariance (begin_i,end_i);
        float S1 = detCovariance (begin_i, position-1);
        float S2 = detCovariance (position, end_i);

        return (N*log(S)*0.5 - N1*log(S1)*0.5 - N2*log(S2)*0.5 -lP);
}