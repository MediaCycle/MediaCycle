/*
 *  ACSelfSimSegmentationPlugin.h
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

#ifndef _ACSelfSimSegmentationPlugin_H
#define	_ACSelfSimSegmentationPlugin_H

#include "Armadillo-utils.h" 
#include "MediaCycle.h"
#include "ACMediaTimedFeature.h"

#ifndef PI
#define PI 3.14159265
#endif

enum SelfSimKernelType {
    SELFSIMSTEP, // +1 and -1
    SELFSIMGAUSSIAN // gaussian kernel
};

enum SelfSimDistance {// actually it will be the inverse of a distance: the larger the number, the more the 2 objects are similar
    COSINE,
    EUCLIDEAN,
    MANHATTAN 
};

class ACSelfSimSegmentationPlugin : public ACSegmentationPlugin {
public:
    ACSelfSimSegmentationPlugin();
    ACSelfSimSegmentationPlugin(ACMediaType type);
    ~ACSelfSimSegmentationPlugin();

    virtual std::vector<ACMedia*> segment(ACMediaData* _data, ACMedia* _theMedia){};
    virtual std::vector<ACMedia*> segment(ACMediaTimedFeature* _mtf, ACMedia* _theMedia);

    //std::vector<int> testSegment(const vector< vector<float> > & _allfeatures, float _SelfSimThresh=0.8, int _L=8, int _Wmin=8, SelfSimKernelType _T=SELFSIMSTEP, SelfSimDistance _D=COSINE);
    std::vector<int> testSegment(arma::fmat _M, float _SelfSimThresh=0.8, int _L=8, int _Wmin=8, SelfSimKernelType _T=SELFSIMSTEP, SelfSimDistance _D=COSINE);
    std::vector<int> testSegment(ACMediaTimedFeature* _ACMTF, float _SelfSimThresh=0.8, int _L=8, int _Wmin=8, SelfSimKernelType _T=SELFSIMSTEP, SelfSimDistance _D=COSINE);
    std::vector<int> segment(std::vector <ACMediaTimedFeature*> _ACMTF, float _SelfSimThresh=0.8, int _L=8, int _Wmin=8, SelfSimKernelType _T=SELFSIMSTEP, SelfSimDistance _D=COSINE);

    arma::fmat get_features() {return full_features;};

private:
    std::vector<int> _segment();

    int Wmin;
    float SelfSimThresh;
    int L; //size of the kernel
    SelfSimKernelType KernelType;
    SelfSimDistance DistanceType;

    std::vector <std::string> kernel_types;
    std::vector <std::string> distance_types;

    arma::fmat full_features;
    arma::fmat buildKernel();

    double computeDistance(int _a, int _b);
    //arma::fmat vectorACMTF2fmat(std::vector <ACMediaTimedFeature*> _ACMTF);
};


#endif	// _ACSelfSimSegmentationPlugin_H
