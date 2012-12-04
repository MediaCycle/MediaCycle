/*
 *  ACAudioSelfSimSegmentationPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson & Cécile Picard-Limpens
 *  @date 3/12/2012
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

#include "ACAudioSelfSimSegmentationPlugin.h"

#include <vector>
#include <string>

ACAudioSelfSimSegmentationPlugin::ACAudioSelfSimSegmentationPlugin() : ACSegmentationPlugin(),ACSelfSimSegmentation(){
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Self-Similarity Segmentation";
    this->mDescription = "Self-Similarity Segmentation plugin for audio";
    this->mId = "";

    this->Wmin = 128;
    this->SelfSimThresh = 0.01;
    this->L = 2;
    this->KernelType = SELFSIMGAUSSIAN;
    this->DistanceType = COSINE;

    this->addNumberParameter("minimum window size",Wmin,1,1024,1,"minimum length of the window in which to search for segment change (in number of frames)"); // Wmin
    this->addNumberParameter("threshold",SelfSimThresh,0,1,0.1,"threshold under which peaks are not considered as segmentation points"); // SelfSimThresh
    this->addNumberParameter("kernel size",L,0,16,1,"width of the kernel = range of the self-similarity matrix to compute"); // L
    this->addStringParameter("kernel type",kernel_types[SELFSIMGAUSSIAN],kernel_types,"kernel type"); // KernelType
    this->addStringParameter("distance type",distance_types[COSINE],distance_types,"distance type"); // DistanceType
}

ACAudioSelfSimSegmentationPlugin::~ACAudioSelfSimSegmentationPlugin() {
}

std::vector<ACMedia*> ACAudioSelfSimSegmentationPlugin::segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia){

    // Retrieve the user-defined values
    this->Wmin = this->getNumberParameterValue("minimum window size");
    this->SelfSimThresh = this->getNumberParameterValue("threshold");
    this->L = this->getNumberParameterValue("kernel size");
    this->KernelType = (SelfSimKernelType)(this->getStringParameterValueIndex("kernel type"));
    this->DistanceType = (SelfSimDistance)(this->getStringParameterValueIndex("distance type"));

    cout << "ACAudioSelfSimSegmentationPlugin::segment: user-defined parameters for segmentation: " <<  this->KernelType << " / " << this->DistanceType << " / " << this->Wmin << " / " << this->SelfSimThresh << " / " << this->L << endl; //CPL

    return this->_segment(_MTF,_theMedia);
}
