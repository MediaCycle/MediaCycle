/**
 * @brief ACPosPlugAudioGardenGramoPhone.cpp
 * @author Christian Frisson
 * @date 03/08/2012
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
#include "ACPosPlugAudioGardenGramoPhone.h"

using namespace arma;
using namespace std;

ACPosPlugAudioGardenGramoPhone::ACPosPlugAudioGardenGramoPhone()
{
    //vars herited from ACPlugin
    // XS TODO: are these general enough ? can we use this only for audio ??
    this->mMediaType = MEDIA_TYPE_AUDIO;
    //this->mPluginType = PLUGIN_TYPE_CLUSTERS_POSITIONS;
    this->mName = "AudioGarden GramoPhone";
    this->mDescription = "Audiogarden \"GramoPhone\" Visualisation plugin";
    this->mId = "";
    this->featureList.push_back("Mean of MFCC");
    //this->featureList.push_back("Mean of Spectral Flatness");
    //this->featureList.push_back("Interpolated Energy");
}

ACPosPlugAudioGardenGramoPhone::~ACPosPlugAudioGardenGramoPhone()
{
}


void ACPosPlugAudioGardenGramoPhone::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;

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
    int r = 0;
    for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++) {
        r_v(r++) = 1./log(1+media->second->getDuration());
    }
    r_v = r_v/max(r_v) *.1;

    desc_m = extractDescMatrix(mediaBrowser->getLibrary(), featureList);
    mat coef_m;
    mat proj_m;
    princomp(coef_m, proj_m, desc_m);
    theta_v = proj_m.col(0);
    theta_v = (theta_v - min(theta_v))/(max(theta_v)-min(theta_v)) * 2 * arma::math::pi();
    //	std::cout << theta_v << std::endl;


    mediaBrowser->setNumberOfDisplayedNodes(desc_m.n_rows);

    posDisp_m.col(0) = r_v % cos(theta_v);
    posDisp_m.col(1) = r_v % sin(theta_v);
    posDisp_m = zscore(posDisp_m)*.1;

    ACPoint p;
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    for (int i=0; i<ids.size(); i++){
        mediaBrowser->setMediaNodeDisplayed(ids[i], true);
        // TODO: make sure you meant next
        p.x = posDisp_m(i,0);
        p.y = posDisp_m(i,1);
        p.z = 0;
        mediaBrowser->setNodeNextPosition(ids[i], p);
    }
    //   for (int i=0; i<ids.size(); i++){
    //     mediaBrowser->setMediaNodeDisplayed(ids[i], true);
    // 	  // TODO: make sure you meant next
    // 		mediaBrowser->setNodeNextPosition(ids[i], r_v(i) * cos(theta_v(i)), r_v(i) * sin(theta_v(i)));
    //   }
    ////////////////////////////////////////////////////////////////
}

mat ACPosPlugAudioGardenGramoPhone::extractDescMatrix(ACMediaLibrary* lib, string featureName){
    ACMedias medias = lib->getAllMedia();
    int nbMedia = medias.size();
    int featureId = 0;
    int featureSize = 0;

    int nbFeature = lib->getFirstMedia()->getNumberOfFeaturesVectors();

    for(int f=0; f< nbFeature; f++){
        if (lib->getFirstMedia()->getPreProcFeaturesVector(f)->getName() == featureName){
            featureId = f;
        }
    }

    featureSize = lib->getFirstMedia()->getPreProcFeaturesVector(featureId)->getSize();

    mat desc_m(medias.size(),featureSize);

    mat pos_m(nbMedia,2);

    int i = 0;
    for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++) {
        for(int d=0; d < featureSize; d++){
            desc_m(i, d) = media->second->getPreProcFeaturesVector(featureId)->getFeatureElement(d);
        }
        i++;
    }
    return desc_m;
}


mat ACPosPlugAudioGardenGramoPhone::extractDescMatrix(ACMediaLibrary* lib, vector<string> featureList){
    mat desc_m;
    mat tmpDesc_m;

    desc_m = extractDescMatrix(lib, featureList[0]);

    for (int i=1; i<featureList.size(); i++){
        tmpDesc_m = extractDescMatrix(lib, featureList[i]);
        desc_m = join_rows(desc_m, tmpDesc_m);
    }
    return desc_m;
}

