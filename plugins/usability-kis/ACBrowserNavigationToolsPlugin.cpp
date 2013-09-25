/**
 * @brief Plugin for adding browser navigation tools
 * @author Christian Frisson
 * @date 5/03/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACBrowserNavigationToolsPlugin.h"
#include<MediaCycle.h>

#include <iostream>

using namespace std;

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

/// CF std::maps automatically sort elements along their key by equality comparison
/// which might fail against "equal" floating point numbers
/// the following binary function circumvents the issue
class map_float_compare : public binary_function<double,double,bool>
{
public:
    map_float_compare( double arg_ = 1e-7 ) : epsilon(arg_) {}
    bool operator()( const double &left, const double &right  ) const
    {
        // you can choose other way to make decision
        // (The original version is: return left < right;)
        return (abs(left - right) > epsilon) && (left < right);
    }
    double epsilon;
};

ACBrowserNavigationToolsPlugin::ACBrowserNavigationToolsPlugin() : QObject(), ACPluginQt(), ACClientPlugin(){
    this->mName = "Browser Navigation Tools";
    this->mDescription ="Plugin for adding browser navigation tools";
    this->mMediaType = MEDIA_TYPE_ALL;

    /*discardClickedNodeAction = new ACInputActionQt(tr("Discard the clicked node"), this);
    discardClickedNodeAction->setToolTip(tr("Discard the clicked node"));
    discardClickedNodeAction->setShortcut(Qt::Key_H);
    discardClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    discardClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    //discardClickedNodeAction->setDeviceName(device);
    //discardClickedNodeAction->setDeviceEvent("button 09 pressed");
    connect(discardClickedNodeAction, SIGNAL(triggered()), this, SLOT(discardClickedNodeCallback()));*/

    discardClosestNodeAction = new ACInputActionQt(tr("Discard the closest node by hovering"), this);
    discardClosestNodeAction->setToolTip(tr("Discard the closest node by hovering"));
    discardClosestNodeAction->setShortcut(Qt::Key_J);
    discardClosestNodeAction->setKeyEventType(QEvent::KeyPress);
    discardClosestNodeAction->setMouseEventType(QEvent::MouseMove);
    //discardClosestNodeAction->setDeviceName(device);
    //discardClosestNodeAction->setDeviceEvent("button 09 pressed");
    connect(discardClosestNodeAction, SIGNAL(triggered()), this, SLOT(discardClosestNodeCallback()));

    /*togglediscardClosestNodeAction = new ACInputActionQt(tr("Toggle closest node hiding"), this);
    togglediscardClosestNodeAction->setToolTip(tr("Toggle closest node hiding"));
    togglediscardClosestNodeAction->setShortcut(Qt::Key_N);
    togglediscardClosestNodeAction->setKeyEventType(QEvent::KeyPress);
    //togglediscardClosestNodeAction->setMouseEventType(QEvent::MouseMove);
    //togglediscardClosestNodeAction->setDeviceName(device);
    //togglediscardClosestNodeAction->setDeviceEvent("button 09 pressed");
    connect(togglediscardClosestNodeAction, SIGNAL(triggered(bool)), this, SLOT(togglediscardClosestNodeCallback(bool)));*/

    discardNearestNodesAction = new ACInputActionQt(tr("Discard the nearest nodes by hovering"), this);
    discardNearestNodesAction->setToolTip(tr("Discard the nearest nodes by hovering"));
    discardNearestNodesAction->setShortcut(Qt::Key_N);
    discardNearestNodesAction->setKeyEventType(QEvent::KeyPress);
    discardNearestNodesAction->setMouseEventType(QEvent::MouseMove);
    //discardClosestNodeAction->setDeviceName(device);
    //discardClosestNodeAction->setDeviceEvent("button 09 pressed");
    connect(discardNearestNodesAction, SIGNAL(triggered()), this, SLOT(discardNearestNodesCallback()));

    undiscardAllNodesAction = new ACInputActionQt(tr("Undiscard all nodes"), this);
    undiscardAllNodesAction->setToolTip(tr("Undiscard all nodes"));
    undiscardAllNodesAction->setShortcut(Qt::Key_U);
    undiscardAllNodesAction->setKeyEventType(QEvent::KeyPress);
    //undiscardAllNodesAction->setDeviceName(device);
    //undiscardAllNodesAction->setDeviceEvent("button 09 pressed");
    connect(undiscardAllNodesAction, SIGNAL(triggered()), this, SLOT(undiscardAllNodesCallback()));

    this->addNumberParameter("Visual nearest number",1,0,20,1,"Number of visual nearest neighbors to remove by hovering"/*,boost::bind(&ACBrowserNavigationToolsPlugin::assignedFeaturesChanged,this)*/);
    this->addNumberParameter("Signal nearest threshold",15,0,100,1,"Distance threshold (%) for signal nearest neighbors to remove by hovering"/*,boost::bind(&ACBrowserNavigationToolsPlugin::assignedFeaturesChanged,this)*/);
}

ACBrowserNavigationToolsPlugin::~ACBrowserNavigationToolsPlugin(){

}

void ACBrowserNavigationToolsPlugin::mediaCycleSet()
{
    // CF this will work only since/if we load feature plugins before this plugin...
    this->updateAvailableFeatures();
}

bool ACBrowserNavigationToolsPlugin::updateAvailableFeatures(){
    if(this->media_cycle){
        if(this->media_cycle->getPluginManager()){
            std::vector<std::string> feature_names;
            std::vector<std::string> names = this->media_cycle->getListOfActivePlugins();
            for (std::vector<std::string>::iterator name = names.begin();name != names.end();name++){
                feature_names.push_back(*name + " (dim 0)");
                //std::cout << "ACBrowserNavigationToolsPlugin: feature: " << *feature << std::endl;
            }
            if(feature_names.size()>=1){
                feature_names.push_back("None");
                if(this->hasStringParameterNamed("Nearest nodes computation feature"))
                    this->updateStringParameter("Nearest nodes computation feature",feature_names.front(),feature_names);
                else
                    this->addStringParameter("Nearest nodes computation feature",feature_names.front(),feature_names,"Feature to compute the nearest neighbors distance"/*,boost::bind(&ACBrowserNavigationToolsPlugin::assignedFeaturesChanged,this)*/);
                return true;
            }
        }
    }
    return false;
}

void ACBrowserNavigationToolsPlugin::discardClickedNodeCallback(){
    std::cout << "ACBrowserNavigationToolsPlugin::discardClickedNodeCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }

    int id = -1;
    if(media_cycle->getClickedNode() != -1)
        id = media_cycle->getClickedNode();
    else
        return;

    media_cycle->performActionOnMedia("discard clicked",id);
}

void ACBrowserNavigationToolsPlugin::discardClosestNodeCallback(){
    std::cout << "ACBrowserNavigationToolsPlugin::discardClosestNodeCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }

    int id = -1;
    if(media_cycle->getClosestNode() != -1)
        id = media_cycle->getClosestNode();
    else
        return;

    media_cycle->performActionOnMedia("discard closest",id);
}

void ACBrowserNavigationToolsPlugin::togglediscardClosestNodeCallback(bool pressed){
    std::cout << "ACBrowserNavigationToolsPlugin::togglediscardClosestNodeCallback" << std::endl;
    if(pressed)
        return;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }
    bool auto_discard = media_cycle->getAutoDiscard();
    media_cycle->setAutoDiscard(!auto_discard);
}

void ACBrowserNavigationToolsPlugin::discardNearestNodesCallback(){
    std::cout << "ACBrowserNavigationToolsPlugin::discardNearestNodesCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }

    int id = -1;
    if(media_cycle->getClosestNode() != -1)
        id = media_cycle->getClosestNode();
    else
        return;

    media_cycle->performActionOnMedia("discard nearest nodes",id);
}

void ACBrowserNavigationToolsPlugin::undiscardAllNodesCallback(){
    std::cout << "ACBrowserNavigationToolsPlugin::undiscardAllNodesCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }

    ACMedias medias = media_cycle->getLibrary()->getAllMedia();
    for(ACMedias::iterator media = medias.begin(); media!= medias.end(); media++){
        if(media->second)
            media->second->setDiscarded(false);
    }

    media_cycle->performActionOnMedia("undiscard all nodes",-1);
}

bool ACBrowserNavigationToolsPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    //std::cout << "ACBrowserNavigationToolsPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << std::endl;
    if(!media_cycle){
        std::cerr << "ACBrowserNavigationToolsPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(action == "discard clicked" || action == "discard closest"){
        /*ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        if(!node)
            return false;*/
        ACMedia* media = media_cycle->getLibrary()->getMedia(mediaId);
        if(!media)
            return false;
        media->setDiscarded(true);
        return true;
    }
//    else if(action == "hover closest node"){
//        std::cout << "Hovering closest node "<< mediaId << std::endl;
//    }
    else if(action == "discard nearest nodes"){
        this->discardNearestNodes(mediaId);
    }
    return false;
}

void ACBrowserNavigationToolsPlugin::discardNearestNodes(int mediaId){
    //float start = getTime();
    if(media_cycle && media_cycle->hasBrowser()){
        ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        ACMedia* media = media_cycle->getLibrary()->getMedia(mediaId);
        vector<float> featureWeights= media_cycle->getBrowser()->getFeatureWeights();
        if(node && media && featureWeights.size()>0){
            //std::list<int> neighbors_ids;
            ACMediaNodes media_nodes = media_cycle->getBrowser()->getMediaNodes();

            ACPoint p = node->getNextPosition();// getCurrentPosition();
            std::map<float,int,map_float_compare> visual_distances;

            for(ACMediaNodes::iterator media_node = media_nodes.begin(); media_node != media_nodes.end();media_node++){
                if(media_node->second){
                    ACPoint n = media_node->second->getNextPosition();// getCurrentPosition();
                    visual_distances[ sqrt((n.x-p.x)*(n.x-p.x) + (n.y-p.y)*(n.y-p.y)) ] = media_node->first; // for optimization, sqrt isn't calculated
                }
                else
                    std::cerr << "Error node " << media_node->second << std::endl;
            }

            /*int s = 0;
            int m = 20;
            std::map<float,int,map_float_compare>::iterator vd = visual_distances.begin();
            std::cout << m << " first distances: ";
            while( s<m && s<visual_distances.size() && vd != visual_distances.end() ){
                std::cout << vd->first << " ";
                s++;
                vd++;
            }
            std::cout << std::endl;*/

            int visual_threshold = this->getNumberParameterValue("Visual nearest number");
            float signal_threshold = this->getNumberParameterValue("Signal nearest threshold")/100.0f;
            int k = 1;
            std::map<float,int,map_float_compare> signal_distances;
            std::map<float,int,map_float_compare>::iterator visual_distance = visual_distances.begin();
            //visual_distance++; // the first distance is the identity of the node
            std::vector<ACMediaFeatures*> features_vectors = media->getAllPreProcFeaturesVectors();
            std::cout << "Nearest visual neighbors are media ids: ";
            float max_signal_distance = 0.0f;
            while(k <= visual_threshold && k < visual_distances.size() && visual_distance!=visual_distances.end()){
                std::cout << visual_distance->second << " ";
                std::cout << " (vd2=" << visual_distance->first;
                //neighbors_ids.push_back(visual_distance->second);
                if(media_cycle->getLibrary()->getMedia(visual_distance->second)){
                    float sd = compute_distance(media_cycle->getLibrary()->getMedia(visual_distance->second)->getAllPreProcFeaturesVectors(), features_vectors, featureWeights, false);
                    //if(sd < signal_threshold)
                        signal_distances[ sd ] = visual_distance->second;
                        if(sd>max_signal_distance)
                            max_signal_distance = sd;
                    std::cout << " sd2=" << sd;
                }
                else
                    std::cerr << "Error media " << visual_distance->second << std::endl;
                std::cout << ") " ;
                visual_distance++;
                k++;
            }
            std::cout << "Max signal distance: " << max_signal_distance << std::endl;

            for(std::map<float,int,map_float_compare>::iterator signal_distance = signal_distances.begin();signal_distance!=signal_distances.end();signal_distance++){
                ACMedia* _media = media_cycle->getLibrary()->getMedia(signal_distance->second);
                if(_media && signal_distance->first > max_signal_distance*signal_threshold/100.0f){
                    _media->setDiscarded(true);
                }
                else
                    std::cerr << "Error media " << signal_distance->second << std::endl;
            }
        }
    }
    //std::cout << "ACBrowserNavigationToolsPlugin::discardNearestNodes took " << getTime()-start << std::endl;
}

std::map<std::string,ACMediaType> ACBrowserNavigationToolsPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    //media_actions["discard clicked"] = MEDIA_TYPE_ALL;
    media_actions["discard closest"] = MEDIA_TYPE_ALL;
    //media_actions["toggle closest node hiding by hovering"] = MEDIA_TYPE_ALL;
    media_actions["discard nearest nodes"] = MEDIA_TYPE_ALL;
    media_actions["undiscard all nodes"] = MEDIA_TYPE_ALL;
    return media_actions;
}

std::vector<ACInputActionQt*> ACBrowserNavigationToolsPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    //actions.push_back(discardClickedNodeAction);
    actions.push_back(discardClosestNodeAction);
    //actions.push_back(togglediscardClosestNodeAction);
    actions.push_back(discardNearestNodesAction);
    actions.push_back(undiscardAllNodesAction);
    return actions;
}
