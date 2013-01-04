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

#include <Armadillo-utils.h> 
#include <MediaCycle.h>
#include <ACMediaTimedFeature.h>

class ACBicSegmentationPlugin : public ACSegmentationPlugin {
public:
    ACBicSegmentationPlugin();
    ~ACBicSegmentationPlugin();

    //  XS TODO clean API !!

    std::vector<ACMedia*> segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia);
    virtual std::vector<ACMedia*> segment(ACMedia*){return std::vector<ACMedia*>();}

    void setParameters(float _lambda = 1, int _samplingrate = 1, int _Wmin = 20, float _bic_thresh = 1, int _jump_width = 5, int _discard_borders = 5);

    arma::fmat get_features() {
        return full_features;
    }; //JU: added to ease the visualization during the tests

    // XS todo: synchronize default values between constructor and segment
    std::vector<int> testSegment(arma::fmat _M, float _lambda = 1, int _samplingrate = 1, int _Wmin = 20, float _bic_thresh = 1, int _jump_width = 5, int _discard_borders = 5);
    std::vector<int> testSegmentDAC(arma::fmat _M, float _lambda = 1, int _Wmin = 20, float _bic_thresh = (-1.0 / 0.0), int _discard_borders = 0, float _bic_thresh_DAC = 0);

private:
    //    std::vector<int> segment(std::vector <ACMediaTimedFeature*> _ACMTF, float _lambda = 1, int _samplingrate = 1, int _Wmin = 20, float _bic_thresh = 1, int _jump_width = 5, int discard_borders = 5);
    //    std::vector<int> segmentDAC(std::vector <ACMediaTimedFeature*> _ACMTF, float _lambda = 1, int _Wmin = 20, float _bic_thresh = (-1.0 / 0.0), int _discard_borders = 0, float _bic_thresh_DAC = 0);

    int findSingleSegment(int _A, int _B);
    float detCovariance(int _cinf, int _csup);
    std::vector<int> _segment();

    /// Divide and conquer method (algorithm 2, , Cheng, Wang, Fu)
    std::vector<int> _segmentDAC();
    void _segmentDAC(int begin_i, int end_i);
    std::vector<int> segments_begin_i; // vector containing the indexes of the beginning of each segment

    // Parameters WARNING these are not initialized!
    int imax_bic, max_bic;
    int bic_thresh_DAC;

    /// Basic linear method
    float computeBIC(int begin_i, int end_i, int position);

    // Parameters
    float lambda;
    int sampling_rate;
    int Wmin;
    float bic_thresh;
    int jump_width;
    int discard_borders;

    arma::fmat full_features;
};


#endif	// _ACBICSEGMENTATIONPLUGIN_H
