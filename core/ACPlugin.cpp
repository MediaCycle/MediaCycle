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
#include <iostream>
using std::cout;
using std::endl;

ACPlugin::ACPlugin()
{
	cout<<"ACPlugin Constructor\n";
	this->mPluginType=PLUGIN_TYPE_NONE;
	this->mMediaType=MEDIA_TYPE_NONE;
}

bool ACPlugin::implementsPluginType(ACPluginType pType)
{
	int test=mPluginType&pType;
	if (test==0)
		return false;
	else 
		return true;
}
bool ACPlugin::mediaTypeSuitable(ACMediaType pType){
	int test=mMediaType&pType;
	if (test==0)
		return false;
	else 
		return true;
}

ACFeaturesPlugin::ACFeaturesPlugin()
{
	cout<<"ACPlugin feature Constructor\n";
	this->mPluginType=mPluginType|PLUGIN_TYPE_FEATURES;
}

// XS TODO : add (?)
// this->mPluginType=mPluginType|PLUGIN_TYPE_TIMED_FEATURES;
ACTimedFeaturesPlugin::ACTimedFeaturesPlugin() : ACFeaturesPlugin(){
	
	cout<<"ACPlugin Timed feature Constructor" << endl;
	this->mtf_file_name = "";
}

// the plugin knows internally where it saved the mtf
// thanks to mtf_file_name
ACMediaTimedFeature* ACTimedFeaturesPlugin:: getTimedFeatures(){
	if (mtf_file_name == ""){
        cout << "<ACTimedFeaturesPlugin::getTimedFeatures> : missing file name "<<endl;
		cout << "                                            in plugin : " << this->getName() << endl;
		return 0;
	}
	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
		return 0;
	}
	return ps_mtf;
}

ACSegmentationPlugin::ACSegmentationPlugin()
{
	cout<<"ACPlugin segmentation Constructor\n";
	this->mPluginType=mPluginType|PLUGIN_TYPE_SEGMENTATION;
}
ACClusterMethodPlugin::ACClusterMethodPlugin()
{
	cout<<"ACPlugin Cluster Constructor\n";
	this->mPluginType=mPluginType|PLUGIN_TYPE_CLUSTERS_METHOD;
	
	
}

ACNeighborMethodPlugin::ACNeighborMethodPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_NEIGHBORS_METHOD;
	
    //local vars
}
ACPositionsPlugin::ACPositionsPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_POSITIONS;
	
    //local vars
}
ACClusterPositionsPlugin::ACClusterPositionsPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_CLUSTERS_POSITIONS;
	
    //local vars
}
ACNeighborPositionsPlugin::ACNeighborPositionsPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_NEIGHBORS_POSITIONS;
	
    //local vars
}
ACNoMethodPositionsPlugin::ACNoMethodPositionsPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_NOMETHOD_POSITIONS;
	
    //local vars
}

ACPreProcessPlugin::ACPreProcessPlugin() {
	
	this->mPluginType=mPluginType|PLUGIN_TYPE_PREPROCESS;
	
    //local vars
}
ACMediaReaderPlugin::ACMediaReaderPlugin(){
	this->mPluginType=mPluginType|PLUGIN_TYPE_MEDIAREADER;

}

