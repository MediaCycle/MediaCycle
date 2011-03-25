/* 
 * File:   ACPlugin.h
 * Author: Julien Dubois
 *
 * @date 27 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#ifndef _ACPLUGIN_H
#define	_ACPLUGIN_H

#include <iostream>
#include <string>
#include <vector>

#include "ACMediaTypes.h"
#include "ACMediaFeatures.h"
#include "ACMediaTimedFeature.h"
#include "ACMediaData.h"

//#include "ACOsgBrowserRenderer.h"
//#include "ACMedia.h"
//#include "ACMediaBrowser.h"

class ACMedia;
class ACMediaBrowser;
class ACMediaTimedFeature;

typedef		unsigned int ACPluginType;
#define		PLUGIN_TYPE_NONE				0x0000
#define		PLUGIN_TYPE_FEATURES			0x0001
#define		PLUGIN_TYPE_SEGMENTATION		0x0002
#define		PLUGIN_TYPE_SERVER				0x0004
#define		PLUGIN_TYPE_CLIENT				0x0008	
#define		PLUGIN_TYPE_CLUSTERS_METHOD		0x0010//CF updateClusters
#define		PLUGIN_TYPE_CLUSTERS_POSITIONS	0x0020//CF updatePositions for the Clusters mode
#define		PLUGIN_TYPE_CLUSTERS_PIPELINE	0x0040//CF updateClusters and updatePositions for the Clusters mode
#define		PLUGIN_TYPE_NEIGHBORS_METHOD	0x0080//CF updateNeighborhoods
#define		PLUGIN_TYPE_NEIGHBORS_POSITIONS	0x0100//CF updatePositions for the Neighbors mode
#define		PLUGIN_TYPE_NEIGHBORS_PIPELINE	0x0200//CF updateNeighborhoods and updatePositions for the Neighbors mode
#define		PLUGIN_TYPE_ANYMODE_POSITIONS	0x0400//CF updatePositions for the Clusters or Neighbors modes
#define		PLUGIN_TYPE_ALLMODES_PIPELINE	0x0800//CF updateClusters and updateNeighborhoods and updatePositions for both modes


class ACPlugin {
public:
	ACPlugin();
	bool isPlugintype(ACPluginType pType);
	virtual ~ACPlugin() {mName.clear(); mDescription.clear(); mId.clear();}
	std::string getName() {return this->mName;}
	std::string getIdentifier() {return this->mId;}
	std::string getDescription() {return this->mDescription;}
	ACMediaType getMediaType() {return this->mMediaType;}
	ACPluginType getPluginType() {return this->mPluginType;}


		//XSCF TODO: should the plugin receive MediaCycle ?
	virtual void updateClusters(ACMediaBrowser*){};
	virtual void updateNextPositions(ACMediaBrowser*){};
	virtual void updateNeighborhoods(ACMediaBrowser*){};
	
	// e.g., for audioSegmentationPLugin
	
	// timedFeatures: e.g., for BicSegmentationPlugin

protected:
	std::string mName;
	std::string mId;
	std::string mDescription;
	ACMediaType mMediaType;
	ACPluginType mPluginType;
};


class ACFeaturesPlugin: public ACPlugin
{
protected:
	ACFeaturesPlugin();
	
public:
	virtual std::vector<ACMediaFeatures*> calculate(std::string aFileName, bool _save_timed_feat=false)=0;
	virtual std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat=false)=0;
	virtual ACMediaTimedFeature* getTimedFeatures(){return 0;}
	std::vector<std::string> getDescriptorsList() {return this->mDescriptorsList;}
protected:	
	std::vector<std::string> mDescriptorsList;
};

class ACSegmentationPlugin:public ACPlugin
{
protected:
public:
	ACSegmentationPlugin();
	virtual std::vector<ACMedia*> segment(ACMediaTimedFeature* _mtf, ACMedia*)=0;
	virtual std::vector<ACMedia*> segment(ACMediaData* audio_data, ACMedia*)=0;

};


// the types of the class factories
typedef ACPlugin* createPluginFactory(std::string);
typedef void destroyPluginFactory(ACPlugin*);
typedef std::vector<std::string> listPluginFactory();

#endif	/* _ACPLUGIN_H */

