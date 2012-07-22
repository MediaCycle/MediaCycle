/*
 *  ACClusterPositionsPropellerPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 29/06/2012
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

#include "ACClusterPositionsPropellerPlugin.h"

#include <float.h> //FLT_MAX

static double compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
        assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
        int feature_count = obj1.size();

        double dis = 0.0;

        for (int f=0; f<feature_count; f++) {
                //		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f], obj2[f]);
                //		dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
                float temp=obj1[f]->getFeaturesVector().distance(obj2[f]->getFeaturesVector());
                dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
                //	delete E;
        }
        dis = sqrt(dis);

        return dis;
}

// this one is mostly used
// e.g., compute_distance(mLibrary->getMedia(i)->getAllPreProcFeaturesVectors(), mediaBrowser->getClusterCenter(j), featureWeights, false);

static double compute_distance(vector<ACMediaFeatures*> &obj1, const vector<FeaturesVector> &obj2, const vector<float> &weights, bool inverse_features)
{
    int s1=obj1.size();
    int s2=obj2.size();
    int s3=weights.size();

    double dis = 0.0;
    if( (obj1.size() != obj2.size()) || (obj1.size() != weights.size())){
        //std::cerr << "ACClusterPositionsPropellerPlugin::compute_distance: obj1.size() " << obj1.size() << " obj2.size() " << obj2.size() << " weights.size() " << weights.size() << std::endl;
        return dis;
    }

    //assert(obj1.size() == obj2.size()) ;
    //assert(obj1.size() == weights.size());

    int feature_count = obj1.size();

    for (int f=0; f<feature_count; f++) {
        //ACEuclideanDistance* E = new ACEuclideanDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
        //FeaturesVector tmp  = obj1[f]->getFeaturesVector();
        //	ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
        //	dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
        //	delete E;
        float temp=obj1[f]->getFeaturesVector().distance(obj2[f]);
        dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
    }
    dis = sqrt(dis);

    return dis;
}

ACClusterPositionsPropellerPlugin::ACClusterPositionsPropellerPlugin()
    : ACClusterPositionsPlugin(){
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Propeller";
    this->mDescription = "Propeller";
    this->mId = "";
}

ACClusterPositionsPropellerPlugin::~ACClusterPositionsPropellerPlugin() {
}


void ACClusterPositionsPropellerPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
    std::cout << "ACClusterPositionsPropellerPlugin::updateNextPositions" <<std::endl;
    ACMediaLibrary *library=mediaBrowser->getLibrary();

    if (library->isEmpty() ) return;
    int referenceNode = mediaBrowser->getReferenceNode();
    if (referenceNode < 0 || referenceNode >= mediaBrowser->getNumberOfMediaNodes()) return ;

    int clusterCount = mediaBrowser->getClusterCount();
    int navigationLevel = mediaBrowser->getNavigationLevel();
    std::vector<float> featureWeights = mediaBrowser->getFeatureWeights();

    int ci;
    float r, theta;
    double dt;
    ACPoint p;
    p.x = p.y = p.z = 0.0;
    double t = getTime();
    mediaBrowser->getMediaNode(referenceNode).setNextPosition(p, t);

    // srand(1234);

    // XS loop on MediaNodes.
    // each MediaNode has a MediaId by which we can access the Media

    float maxr = 0.0f;

    float *rmin = new float[clusterCount];
    float *rmax = new float[clusterCount];
    float *dtmin = new float[clusterCount];
    float *dtmax = new float[clusterCount];

    for (ci=0;ci<clusterCount;ci++) {
            rmin[ci] = FLT_MAX;
            rmax[ci] = 0;
            dtmin[ci] = FLT_MAX;
            dtmax[ci] = 0;
    }

    // SD 2011 may - normalization of radius and angle to use full available range
    for (ACMediaNodes::const_iterator node = mediaBrowser->getLoopAttributes().begin(); node != mediaBrowser->getLoopAttributes().end(); ++node) {

            if(node->getNavigationLevel() < navigationLevel) continue;

            ci = (*node).getClusterId();

            if(library->getMedia((*node).getMediaId())->getType() == library->getMediaType() && library->getMedia(referenceNode)->getType() == library->getMediaType()){//CF multimedia compatibility
                    r = compute_distance(library->getMedia(referenceNode)->getAllPreProcFeaturesVectors(),
                                                     library->getMedia((*node).getMediaId())->getAllPreProcFeaturesVectors(),
                                                             featureWeights, false) * 10.0;
                    if (r<rmin[ci]) {
                            rmin[ci] = r;
                    }
                    if (r>rmax[ci]) {
                            rmax[ci] = r;
                    }
            }

            if(library->getMedia((*node).getMediaId())->getType() == library->getMediaType()){//CF multimedia compatibility
                    dt = compute_distance(library->getMedia((*node).getMediaId())->getAllPreProcFeaturesVectors(), mediaBrowser->getClusterCenter(ci), featureWeights, false) / 2.0 * 10.0;

                    if (dt<dtmin[ci]) {
                            dtmin[ci] = dt;
                    }
                    if (dt>dtmax[ci]) {
                            dtmax[ci] = dt;
                    }
            }
    }

    for (ACMediaNodes::const_iterator node = mediaBrowser->getLoopAttributes().begin(); node != mediaBrowser->getLoopAttributes().end(); ++node) {
            if(node->getNavigationLevel() < navigationLevel) continue;

            int ci = (*node).getClusterId();

            // SD TODO - test both approaches
            r=1;
            if(library->getMedia((*node).getMediaId())->getType() == library->getMediaType() && library->getMedia(referenceNode)->getType() == library->getMediaType())//CF
                    r = compute_distance(library->getMedia(referenceNode)->getAllPreProcFeaturesVectors(),
                                                     library->getMedia((*node).getMediaId())->getAllPreProcFeaturesVectors(),
                                                     featureWeights, false) * 10.0;
            if (rmax[ci]>rmin[ci]) {
                    r = 0.01f + 0.89f * (r - rmin[ci])/(rmax[ci]-rmin[ci]);
            }
            else {
                    r = 0.5f;
            }
            r /= 2.0f;

            // dt = 1;
            if(library->getMedia((*node).getMediaId())->getType() == library->getMediaType())//CF multimedia compatibility
                    dt = compute_distance(library->getMedia((*node).getMediaId())->getAllPreProcFeaturesVectors(), mediaBrowser->getClusterCenter(ci), featureWeights, false) / 2.0 * 10.0;
            if (dtmax[ci]>dtmin[ci]) {
                    dt = -0.3f + 1.6f * (dt - dtmin[ci])/(dtmax[ci]-dtmin[ci]);
            }
            else {
                    dt = 0.5f;
            }
            theta = (ci + dt) * 2 * M_PI / (float)clusterCount;

            //p.x = 4*sin(theta)*r;//CF dirty trick to optimize the space, waiting for better ;)
            p.x = sin(theta)*r;
            //p.y = 4*cos(theta)*r;//CF dirty trick to optimize the space, waiting for better ;)
            p.y = cos(theta)*r;
            p.z = 0.0;

            //printf("computed next position: theta:%f,r=%f,  (%f %f %f)\n", theta, r, p.x, p.y, p.z);//CF free the console
            double t = getTime();
            mediaBrowser->getMediaNode( (*node).getNodeId() ).setNextPosition(p, t); // (*node) is const hence getMediaNode( getNodeId ) allows node modification

            maxr = max(maxr,p.x);
            maxr = max(maxr,p.y);
            #ifdef USE_DEBUG
            //cout<<"media n°"<<node->getMediaId()<<" cluster:"<<node->getClusterId()<<" x="<<p.x<<" y="<<p.y<<endl;
            #endif
    }

    p.x = p.y = p.z = 0.0;
    t = getTime();
    mediaBrowser->getMediaNode(referenceNode).setNextPosition(p, t);

    std::cout << "Max prop: " << maxr << std::endl;
    // printf("PROPELLER \n");

    delete[] rmin;
    delete[] rmax;
    delete[] dtmin;
    delete[] dtmax;

    mediaBrowser->setNeedsDisplay(true);

    if(!media_cycle) std::cerr << "ACClusterPositionsPropellerPlugin::updateNextPositions mediacycle not set" << std::endl;
}
