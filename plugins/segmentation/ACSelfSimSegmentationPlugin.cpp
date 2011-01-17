/*
 *  ACSelfSimSegmentationPlugin.cpp
 *  MediaCycle
 *
 *  @author Jerome Urbain
 *  @date 23/12/10
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

#include "ACSelfSimSegmentationPlugin.h"

#include <vector>
#include <string>

// L = width of the kernel = range of the self-similarity matrix to compute
// thresh = threshold under which peaks are not considered as segmentation points
 
ACSelfSimSegmentationPlugin::ACSelfSimSegmentationPlugin() : L(8), SelfSimThresh(0.8){
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mPluginType = PLUGIN_TYPE_SEGMENTATION;
    this->mName = "SelfSimSegmentation";
    this->mDescription = "SelfSimSegmentation plugin";
    this->mId = "";
}

ACSelfSimSegmentationPlugin::~ACSelfSimSegmentationPlugin() {
}

std::vector<ACMedia*> ACSelfSimSegmentationPlugin::segment(ACMediaData* _data, ACMedia* _theMedia) {
// XS TODO
	cout <<"todo" << endl;
}

std::vector<int> ACSelfSimSegmentationPlugin::segment(const vector< vector<float> > & _allfeatures, float _SelfSimThresh, int _L, int _Wmin, SelfSimKernelType _T, SelfSimDistance _D){
	this->SelfSimThresh = _SelfSimThresh;
        this->Wmin = _Wmin;
        this->L=_L;
        this->KernelType=_T;
        this->DistanceType=_D;
	
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

std::vector<int> ACSelfSimSegmentationPlugin::segment(arma::fmat _M, float _SelfSimThresh, int _L, int _Wmin, SelfSimKernelType _T, SelfSimDistance _D){
	this->SelfSimThresh = _SelfSimThresh;
        this->Wmin = _Wmin;
        this->L=_L;
	this->full_features = _M ;
        this->KernelType=_T;
        this->DistanceType=_D;

	return (this->_segment());
}

std::vector<int> ACSelfSimSegmentationPlugin::segment(ACMediaTimedFeature* _ACMTF, float _SelfSimThresh, int _L, int _Wmin, SelfSimKernelType _T, SelfSimDistance _D){
	this->SelfSimThresh = _SelfSimThresh;
        this->Wmin = _Wmin;
        this->L=_L;
	this->full_features = _ACMTF->getValue();
        this->KernelType=_T;
        this->DistanceType=_D;

	return (this->_segment());
}

std::vector<int> ACSelfSimSegmentationPlugin::segment(std::vector <ACMediaTimedFeature*> _ACMTF, float _SelfSimThresh, int _L, int _Wmin, SelfSimKernelType _T, SelfSimDistance _D){
	this->SelfSimThresh = _SelfSimThresh;
        this->Wmin = _Wmin;
        this->L=_L;
	this->full_features = arma::trans(vectorACMTF2fmat(_ACMTF));
        this->KernelType=_T;
        this->DistanceType=_D;

	return (this->_segment());
}
	
//supposes we have defined:
// - this->lambda = _lambda;
// - this->sampling_rate = _samplingrate;
// - this->full_features = _M ; 

std::vector<int> ACSelfSimSegmentationPlugin::_segment(){
	// XS test most significant segment (only one)
	// this->findSingleSegment(0,this->full_features.n_cols-1);

        //compute self-similarity
        arma::fmat SelfSim;
        int n_frames=int (this->full_features.n_cols);
        SelfSim.zeros(L,n_frames);
        int i,k,p;
        for(i=0;i<n_frames;i++)
        {
            for(k=0;k<L;k++)
            {
                if((i+k+1<n_frames) )
                {
                    //printf("i: %d; i+k: %d\n", i, i+k);
                    SelfSim(k,i) = computeDistance(i,i+k);
                }
            }
        }

        /*arma::fmat A=arma::trans(SelfSim);
        A.print();*/


        // building the kernel
        arma::fmat kernel;
        kernel = buildKernel();
        double sumkernel=arma::accu(arma::abs(kernel));

        kernel.print();

        // computing the novelty score
        arma::rowvec novelty;
        novelty.zeros(n_frames);
        for(i=L/2;i<n_frames-L/2;i++)
        {
            for(k=0;k<L;k++)
            {
                //taking advantage of the symmetry of both kernel and SelfSimilarity, we only compute on one side (and multiply by 2).
                for(p=0;p<k;p++)
                {
                    novelty(i)+=2*kernel(k,p)*SelfSim(k,i+p-L/2);
                }
                novelty(i)+=kernel(k,k)*SelfSim(k,i+k-L/2);
            }
            //novelty(i)*=2/sumkernel;
            novelty(i)/=sumkernel;
            //cout << "frame: " << i << "normalized novelty: " << novelty(i) << endl;
        }

        //find novelty peaks:
	std::vector<int> segments_tmp;
        for (i=Wmin;i<int (this->full_features.n_cols)-Wmin; i++)
        {
            if(novelty(i)==arma::max(novelty.cols(i-Wmin, i+Wmin)) && novelty(i)>= SelfSimThresh)
            {
                segments_tmp.push_back(i);
                i+=Wmin-1;
            }
        }




        /*int seg_i = 0;
	int seg_f = Wmin;

	while (seg_f < int (this->full_features.n_cols)) {
		int s = this->findSingleSegment(seg_i, seg_f);
		if (s > 0) {
			segments_tmp.push_back(s);
			seg_i = s + jump_width;
			seg_f = seg_i + Wmin;
		}
		seg_f += sampling_rate;
	}*/
	
	return segments_tmp;

}

arma::fmat ACSelfSimSegmentationPlugin::buildKernel()
{
    arma::fmat kernel;
     if (2*(int(L/2))!=L) { // maybe we can accept odd numbers
            printf("the size of the kernel (L=%u) must be an even number\n", L);
            L=2*(int(L/2));
            printf("L was modified to: %u\n", L);
        }

    kernel.ones(L,L);

    // Now only linear type (+1 and -1):
    int i,k;
    for(i=0;i<L/2;i++)
            {
                for(k=L/2;k<L;k++)
                {
                    kernel(i,k)=-1;
                    kernel(k,i)=-1;
                }
            }
    switch(this->KernelType)
    {
        case SELFSIMSTEP:
            // nothing else to do;
            break;
        case SELFSIMGAUSSIAN:
            double sigmasquare=L*L;
            double center=L/2-0.5;
            for(i=0;i<L;i++)
            {
                for(k=0;k<L;k++)
                {
                    kernel(i,k)*=exp(-((k-center)*(k-center)+(i-center)*(i-center))/sigmasquare)/(PI*sigmasquare);
                }
            }
            break;
    }
    

    return kernel;
}

double ACSelfSimSegmentationPlugin::computeDistance(int _a, int _b)
{
    //actually we compute the inverse of a distance (like cosine similarity): the larger the value, the more the vectors are similar
    //printf("_a: %d; _b: %d; produit: %f; norm_a: %f; norm_b: %f; cosdist: %f\n", _a, _b, arma::dot(this->full_features.col(_a),this->full_features.col(_b)),arma::norm(this->full_features.col(_a),2),arma::norm(this->full_features.col(_b),2),arma::dot(this->full_features.col(_a),this->full_features.col(_b))/(arma::norm(this->full_features.col(_a),2)*arma::norm(this->full_features.col(_b),2)));
    switch(this->DistanceType)
    {
        case COSINE:
            return (arma::dot(this->full_features.col(_a),this->full_features.col(_b))/(arma::norm(this->full_features.col(_a),2)*arma::norm(this->full_features.col(_b),2)));
            break;
        case EUCLIDEAN:
            return (-arma::norm(this->full_features.col(_a)-this->full_features.col(_b),2));
            break;
        case MANHATTAN:
            return (-arma::norm(this->full_features.col(_a)-this->full_features.col(_b),1));
            break;
        // default???
            // we can also allow any p-norm (p>2), simple call to armadillo norm witht he given parameter
    }


}


