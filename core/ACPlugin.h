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
#define _ACPLUGIN_H

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
const ACPluginType	PLUGIN_TYPE_NONE				=	0x0000;
const ACPluginType	PLUGIN_TYPE_FEATURES			=	0x0001;
const ACPluginType	PLUGIN_TYPE_SEGMENTATION		=	0x0002;
const ACPluginType	PLUGIN_TYPE_SERVER				=	0x0004;
const ACPluginType	PLUGIN_TYPE_CLIENT				=	0x0008;	
const ACPluginType	PLUGIN_TYPE_CLUSTERS_METHOD		=	0x0010;//CF updateClusters
const ACPluginType	PLUGIN_TYPE_CLUSTERS_POSITIONS	=	0x0020;//CF updatePositions for the Clusters mode
const ACPluginType	PLUGIN_TYPE_CLUSTERS_PIPELINE	=	0x0040;//CF updateClusters and updatePositions for the Clusters mode
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_METHOD	=	0x0080;//CF updateNeighborhoods
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_POSITIONS	=	0x0100;//CF updatePositions for the Neighbors mode
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_PIPELINE	=	0x0200;//CF updateNeighborhoods and updatePositions for the Neighbors mode
const ACPluginType	PLUGIN_TYPE_POSITIONS			=	0x0400;//TR todo 
const ACPluginType	PLUGIN_TYPE_NOMETHOD_POSITIONS	=	0x0800;//CF updatePositions for the Clusters or Neighbors modes
const ACPluginType	PLUGIN_TYPE_ALLMODES_PIPELINE	=	0x1000;//CF updateClusters and updateNeighborhoods and updatePositions for both modes
const ACPluginType	PLUGIN_TYPE_PREPROCESS			=	0x2000;//CF updateClusters and updateNeighborhoods and updatePositions for both modes
const ACPluginType	PLUGIN_TYPE_MEDIAREADER			=	0x4000;//CF updateClusters and updateNeighborhoods and updatePositions for both modes


class ACPlugin {
public:
	ACPlugin();
	bool implementsPluginType(ACPluginType pType);
	bool mediaTypeSuitable(ACMediaType);
	virtual ~ACPlugin() {mName.clear(); mDescription.clear(); mId.clear();};
	std::string getName() {return this->mName;};
	std::string getIdentifier() {return this->mId;};
	std::string getDescription() {return this->mDescription;};
	ACMediaType getMediaType() {return this->mMediaType;};
	ACPluginType getPluginType() {return this->mPluginType;};


		//XSCF TODO: should the plugin receive MediaCycle ?
//	virtual void updateClusters(ACMediaBrowser*){};
//	virtual void updateNextPositions(ACMediaBrowser*){};
//	virtual void updateNeighborhoods(ACMediaBrowser*){};
	
	// e.g., for audioSegmentationPLugin
	
	// timedFeatures: e.g., for BicSegmentationPlugin

protected:
	std::string mName;
	std::string mId;
	std::string mDescription;
	ACPluginType mPluginType;
	ACMediaType mMediaType;
};

// XS TODO : separate time & space plugins ?
// getTimedFeatures has no sense for image
class ACFeaturesPlugin: virtual public ACPlugin
{
protected:
	ACFeaturesPlugin();
	
public:
	virtual std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat=false)=0;
	std::vector<std::string> getDescriptorsList() {return this->mDescriptorsList;}
	// XS TODO is this the best way to proceed when no timed features ?
	virtual ACMediaTimedFeature* getTimedFeatures(){return 0;}
protected:	
	std::vector<std::string> mDescriptorsList;
};

// separate time-dependent plugins from other
// e.g.,  getTimedFeatures has no sense for image
class ACTimedFeaturesPlugin: virtual public ACFeaturesPlugin{
protected:
	ACTimedFeaturesPlugin();
	// XS TODO add a protected setmftfilename instead of making mtf_file_name protected
	std::string mtf_file_name; // file in which features have been saved
public:
	std::string getSavedFileName(){return mtf_file_name;}
	virtual ACMediaTimedFeature* getTimedFeatures();
};

class ACSegmentationPlugin: virtual public ACPlugin
{
protected:
public:
	ACSegmentationPlugin();
	virtual std::vector<ACMedia*> segment(ACMediaTimedFeature* _mtf, ACMedia*)=0;
	virtual std::vector<ACMedia*> segment(ACMediaData* _data, ACMedia*)=0;
};

class ACNeighborMethodPlugin : virtual public ACPlugin {
public:
	ACNeighborMethodPlugin();
	virtual void updateNeighborhoods(ACMediaBrowser* )=0;
protected:
};


class ACClusterMethodPlugin : virtual public ACPlugin {
public:
	ACClusterMethodPlugin();
	virtual void updateClusters(ACMediaBrowser* mediaBrowser ,bool needsCluster=true)=0;//updateClustersKMeans(animate, needsCluster)
protected:
};


class ACPositionsPlugin : virtual public ACPlugin {
public:
	ACPositionsPlugin();
	virtual void updateNextPositions(ACMediaBrowser* )=0;
protected:
};

//TODO TR These three plugin has the same interface but not the same constructor. This should be replaced by a versus without ACMediaBrowser access
class ACClusterPositionsPlugin : virtual public ACPlugin {
public:
	ACClusterPositionsPlugin();
	virtual void updateNextPositions(ACMediaBrowser* )=0;
protected:
};

class ACNeighborPositionsPlugin : virtual public ACPlugin {
public:
	ACNeighborPositionsPlugin();
	virtual void updateNextPositions(ACMediaBrowser* )=0;
protected:
};

class ACNoMethodPositionsPlugin : virtual public ACPlugin {
public:
	ACNoMethodPositionsPlugin();
	virtual void updateNextPositions(ACMediaBrowser* )=0;
protected:
};

typedef void* preProcessInfo;

class ACPreProcessPlugin : virtual public ACPlugin {
public:
	ACPreProcessPlugin();
	virtual preProcessInfo update(std::vector<ACMedia*> media_library)=0;
	virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia)=0;
	virtual void freePreProcessInfo(preProcessInfo &info)=0;
protected:
};

class ACMediaReaderPlugin:virtual public ACPlugin{
public:
	ACMediaReaderPlugin();
	virtual ACMedia* mediaFactory(ACMediaType mediaType)=0;
};

// the types of the class factories
typedef ACPlugin* createPluginFactory(std::string);
typedef void destroyPluginFactory(ACPlugin*);
typedef std::vector<std::string> listPluginFactory();

#endif	/* _ACPLUGIN_H */

