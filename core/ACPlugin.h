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
#include "ACMediaData.h"
//#include "ACOsgBrowserRenderer.h"
//#include "ACMedia.h"
/* #include "ACMediaTimedFeature.h" */
//#include "ACMediaBrowser.h"

class ACMedia;
class ACMediaBrowser;

using std::vector;
using std::string;

enum ACPluginType {
	PLUGIN_TYPE_NONE,
	PLUGIN_TYPE_FEATURES,
	PLUGIN_TYPE_SEGMENTATION,
	PLUGIN_TYPE_SERVER,
	PLUGIN_TYPE_CLIENT,	
	PLUGIN_TYPE_CLUSTERS_METHOD,//CF updateClusters
	PLUGIN_TYPE_CLUSTERS_POSITIONS,//CF updatePositions for the Clusters mode
	PLUGIN_TYPE_CLUSTERS_PIPELINE,//CF updateClusters and updatePositions for the Clusters mode
	PLUGIN_TYPE_NEIGHBORS_METHOD,//CF updateNeighborhoods
	PLUGIN_TYPE_NEIGHBORS_POSITIONS,//CF updatePositions for the Neighbors mode
	PLUGIN_TYPE_NEIGHBORS_PIPELINE,//CF updateNeighborhoods and updatePositions for the Neighbors mode
	PLUGIN_TYPE_ANYMODE_POSITIONS,//CF updatePositions for the Clusters or Neighbors modes
	PLUGIN_TYPE_ALLMODES_PIPELINE//CF updateClusters and updateNeighborhoods and updatePositions for both modes
};

class ACPlugin {
public:
	ACPlugin() {}
	virtual ~ACPlugin() {mName.clear(); mDescription.clear(); mId.clear();}
	std::string getName() {return this->mName;}
	//virtual std::string getName() const = 0; -> error !
	std::string getIdentifier() {return this->mId;}
	std::string getDescription() {return this->mDescription;}
	ACMediaType getMediaType() {return this->mMediaType;}
	ACPluginType getPluginType() {return this->mPluginType;}

	virtual int initialize(){return 0;}

	virtual std::vector<ACMediaFeatures*> calculate(){};
	virtual std::vector<ACMediaFeatures*> calculate(std::string aFileName){};
	virtual std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia){};
	//XSCF TODO: should the plugin receive MediaCycle ?
	virtual void updateClusters(ACMediaBrowser*){};
	virtual void updateNextPositions(ACMediaBrowser*){};
	virtual void updateNeighborhoods(ACMediaBrowser*){};
	
	virtual std::vector<ACMedia*> segment(ACMediaData* audio_data, ACMedia*){};

	virtual int start(){return 0;}
	virtual int stop(){return 0;}
	//virtual void prepareLayout(ACOsgBrowserRenderer*, int start){};
	//virtual void updateLayout(ACOsgBrowserRenderer*, double ratio){};

    //virtual int readFile(std::string);
protected:
	string mName;
	string mId;
	string mDescription;
	ACMediaType mMediaType;
	ACPluginType mPluginType;
};

// the types of the class factories
typedef ACPlugin* createPluginFactory(std::string);
typedef void destroyPluginFactory(ACPlugin*);
typedef vector<string> listPluginFactory();

#endif	/* _ACPLUGIN_H */

