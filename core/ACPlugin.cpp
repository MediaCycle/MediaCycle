/*
 *  ACPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 24/03/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACPlugin.h"
#include "ACMedia.h"

#include <iostream>
using std::cout;
using std::endl;
using std::string;

ACPlugin::ACPlugin() {
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mMediaType = MEDIA_TYPE_NONE;
}

bool ACPlugin::implementsPluginType(ACPluginType pType) {
    int test = mPluginType&pType;
    if (test == 0)
        return false;
    else
        return true;
}

bool ACPlugin::mediaTypeSuitable(ACMediaType pType) {
    int test = mMediaType&pType;
    if (test == 0)
        return false;
    else
        return true;
}

ACFeaturesPlugin::ACFeaturesPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_FEATURES;
}

// XS TODO : add (?)
// this->mPluginType=mPluginType|PLUGIN_TYPE_TIMED_FEATURES;

ACTimedFeaturesPlugin::ACTimedFeaturesPlugin() : ACFeaturesPlugin() {
    //this->mtf_file_name = "";
}

// the plugin knows internally where it saved the mtf
// thanks to mtf_file_name

ACMediaTimedFeature* ACTimedFeaturesPlugin::getTimedFeatures(std::string mtf_file_name) {
    if (mtf_file_name == "") {
        cout << "<ACTimedFeaturesPlugin::getTimedFeatures> : missing file name " << endl;
        cout << "                                            in plugin : " << this->getName() << endl;
        return 0;
    }
    ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
    if (ps_mtf->loadFromFile(mtf_file_name) <= 0) {
        return 0;
    }
    return ps_mtf;
}

// saving timed features on disk (if _save_timed_feat flag is on)
// try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
// XS TODO add checks and setup/use default params
std::string ACTimedFeaturesPlugin::saveTimedFeatures(ACMediaTimedFeature* mtf, string aFileName, bool _save_timed_feat, bool _save_binary) {
    if (_save_timed_feat) {
		std::string mtf_file_name;
        string file_ext = "_b.mtf";
        string aFileName_noext = aFileName.substr(0, aFileName.find_last_of('.'));
        mtf_file_name = aFileName_noext + "_" + this->mDescription + file_ext;
        if  (mtf->saveInFile(mtf_file_name, _save_binary)) // error message if failed
			return mtf_file_name;
    }
    return string(""); // nothing to do
}

ACSegmentationPlugin::ACSegmentationPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_SEGMENTATION;
}

ACClusterMethodPlugin::ACClusterMethodPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_CLUSTERS_METHOD;
}

ACNeighborMethodPlugin::ACNeighborMethodPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NEIGHBORS_METHOD;
}

ACPositionsPlugin::ACPositionsPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_POSITIONS;
}

ACClusterPositionsPlugin::ACClusterPositionsPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_CLUSTERS_POSITIONS;
}

ACNeighborPositionsPlugin::ACNeighborPositionsPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NEIGHBORS_POSITIONS;
}

ACNoMethodPositionsPlugin::ACNoMethodPositionsPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NOMETHOD_POSITIONS;
}

ACPreProcessPlugin::ACPreProcessPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_PREPROCESS;
}

ACMediaReaderPlugin::ACMediaReaderPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_MEDIAREADER;
}

