/**
 * @brief ACPosPlugAudioGardenFlower.cpp
 * @author Christian Frisson
 * @date 21/08/2012
 * @copyright (c) 2012 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACPlugin.h"
#include "ACPosPlugAudioGardenFlower.h"

using namespace arma;
using namespace std;

ACPosPlugAudioGardenFlower::ACPosPlugAudioGardenFlower()
{
    //vars herited from ACPlugin
    // XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_AUDIO;
    //this->mPluginType =    this->mPluginType|PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mName = "AudioGarden Flower";
    this->mDescription = "Audiogarden Flower Visualisation plugin";
    this->mId = "";

    //local vars
}

ACPosPlugAudioGardenFlower::~ACPosPlugAudioGardenFlower()
{
}
/*
int ACPosPlugAudioGardenFlower::initialize()
{
    std::cout << "ACPosPlugAudioGardenFlower initialized" << std::endl;
    return 0;
}*/


void ACPosPlugAudioGardenFlower::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;
    vector<string> featureList;
    int libSize = mediaBrowser->getLibrary()->getSize();
    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();

    mat desc_m, descD_m;
    mat posDisp_m(libSize, 2);
    colvec r_v(libSize);
#ifdef ARMADILLO_HAVE_RANDU
    colvec theta_v = arma::randu<colvec>(libSize);
#else
    colvec theta_v = arma::rand<colvec>(libSize);
#endif
    theta_v.ones(libSize); //theta_v * 2 * arma::math::pi();
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();

    featureList.push_back("Mean of MFCC");
    featureList.push_back("Mean of Spectral Flatness");
    //featureList.push_back("Interpolated Energy");

    vector<long> posParents;
    vector<long> posSegments;

    for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++) {
        if (media->second->getParentId() == -1)
            posParents.push_back(media->first);
        else
            posSegments.push_back(media->first);
    }
    //mat descS_m = extractDescMatrix(mediaBrowser->getLibrary(), featureList, posSegments);
    mat descP_m = extractDescMatrix(mediaBrowser->getLibrary(), featureList, posParents);
    mat coefP_m;
    mat projP_m;
    princomp(coefP_m, projP_m, descP_m);
    projP_m = zscore(projP_m)*.1;
    // affecting parents position
    for (int i=0; i<posParents.size();i++){
        posDisp_m(posParents[i],0) = projP_m(i,0);
        posDisp_m(posParents[i],1) = projP_m(i,1);
    }
    std::vector<ACMedia*> tmpSegments;
    float angle;
    for (long i=0; i<posParents.size(); i++){
        tmpSegments = medias[posParents[i]]->getAllSegments();
        for (int j=0; j<tmpSegments.size(); j++){
            angle = (2*arma::math::pi() / (float) tmpSegments.size()) * (float) j + (arma::math::pi()/2);
            posDisp_m(tmpSegments[j]->getId(),0) = .01 * cos(angle) + posDisp_m(posParents[i],0);
            posDisp_m(tmpSegments[j]->getId(),1) = .01 * sin(angle) + posDisp_m(posParents[i],1);
            std::cout << "angle = " << angle << std::endl;
            std::cout << "posDisp_m.row(tmpSegments[j]->getId())" << posDisp_m.row(tmpSegments[j]->getId()) << std::endl;
        }
    }

    mediaBrowser->setNumberOfDisplayedNodes(medias.size());

    ACPoint p;
    for (int i=0; i<libSize; i++){
        if(mediaBrowser->getMediaNode(i)){
            mediaBrowser->setMediaNodeDisplayed(i, true);
            p.x = posDisp_m(i,0);
            p.y = posDisp_m(i,1);
            p.z = 0;
            mediaBrowser->setNodeNextPosition(i, p);
        }
    }
    ////////////////////////////////////////////////////////////////
}

mat ACPosPlugAudioGardenFlower::extractDescMatrix(ACMediaLibrary* lib, vector<string> featureList, vector<long> mediaIds){
    mat desc_m;
    mat tmpDesc_m;

    desc_m = extractDescMatrix(lib, featureList[0], mediaIds);
    for (int i=1; i<featureList.size(); i++){
        tmpDesc_m = extractDescMatrix(lib, featureList[i], mediaIds);
        desc_m = join_rows(desc_m, tmpDesc_m);
    }
    return desc_m;
}

mat ACPosPlugAudioGardenFlower::extractDescMatrix(ACMediaLibrary* lib, string featureName, vector<long> mediaIds){
    ACMedias medias = lib->getAllMedia();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    int featureId = 0;
    int featureSize = 0;

    int nbFeature = lib->getFirstMedia()->getNumberOfFeaturesVectors();

    for(int f=0; f< nbFeature; f++){
        if (lib->getFirstMedia()->getPreProcFeaturesVector(f)->getName() == featureName){
            featureId = f;
        }
    }

    featureSize = lib->getFirstMedia()->getPreProcFeaturesVector(featureId)->getSize();

    mat desc_m(mediaIds.size(),featureSize);

    mat pos_m(nbMedia,2);

    for(int i=0; i<mediaIds.size(); i++) {
        for(int d=0; d < featureSize; d++){
            //desc_m(i, d) = medias[mediaIds[i]]->getPreProcFeaturesVector(featureId)->getFeatureElement(d);
            if(medias[mediaIds[i]]->getType() == lib->getMediaType())//CF or //if(medias[mediaIds[i]]->getPreProcFeaturesVector(featureId))
                desc_m(i, d) = medias[mediaIds[i]]->getPreProcFeaturesVector(featureId)->getFeatureElement(d);
        }
    }
    return desc_m;
}


