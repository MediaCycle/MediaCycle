/*
 *
 */

#include "ACPlugin.h"
#include "ACVisualisationPlugin.h" // "ACPlugVizDancers.h"
#include "ACNeighborhoodsPluginRandom.h"
#include "ACNeighborhoodsPluginPareto.h"
#include "ACNeighborhoodsPluginEuclidean.h"
#include "ACVisPluginPCA.h"
#include "ACVisPlugin2Desc.h"
#include "ACVisPluginAudiogarden.h"
#include "ACPosPlugAudioGardenFlower.h"
//#if defined(SUPPORT_ARCHIPEL)
#include "ACPosPlugArchipelAtoll.h"
//#endif// defined(SUPPORT_ARCHIPEL)
#include "ACPositionsPluginNodeLinkTreeLayout.h"
#include "ACPositionsPluginRadialTreeLayout.h"
#include "ACEuclideanKMeansPlugin.h"
//#include "ACPositionsPluginRandom.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACVisualisationPlugin") {return new ACVisualisationPlugin();}
	if (namePlugin == "ACNeighborhoodsPluginRandom") {return new ACNeighborhoodsPluginRandom();}
	if (namePlugin == "ACNeighborhoodsPluginPareto") {return new ACNeighborhoodsPluginPareto();}
	if (namePlugin == "ACNeighborhoodsPluginEuclidean") {return new ACNeighborhoodsPluginEuclidean();}
	if (namePlugin == "ACPositionsPluginNodeLinkTreeLayout") {return new ACPositionsPluginNodeLinkTreeLayout();}
	if (namePlugin == "ACVisPluginPCA") {return new ACVisPluginPCA();}
	if (namePlugin == "ACVisPlugin2Desc") {return new ACVisPlugin2Desc();}
	if (namePlugin == "ACVisPluginAudiogarden") {return new ACVisPluginAudiogarden();}
	if (namePlugin == "ACPosPlugAudioGardenFlower") {return new ACPosPlugAudioGardenFlower();}
	//#if defined(SUPPORT_ARCHIPEL)
	if (namePlugin == "ACPosPlugArchipelAtoll") {return new ACPosPlugArchipelAtoll();}
	//#endif// defined(SUPPORT_ARCHIPEL)
	if (namePlugin == "ACPositionsPluginRadialTreeLayout") {return new ACPositionsPluginRadialTreeLayout();}
	if (namePlugin == "ACEuclideanKMeansPlugin") {return new ACEuclideanKMeansPlugin();}
	//if (namePlugin == "ACPositionsPluginRandom") {return new ACPositionsPluginRandom();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
	std::vector<std::string> listPlugin;
	listPlugin.push_back("ACVisualisationPlugin");
	listPlugin.push_back("ACPositionsPluginNodeLinkTreeLayout");
    #ifdef USE_DEBUG
	listPlugin.push_back("ACNeighborhoodsPluginRandom");
    #endif
	listPlugin.push_back("ACNeighborhoodsPluginPareto");
	listPlugin.push_back("ACNeighborhoodsPluginEuclidean");
	listPlugin.push_back("ACVisPluginPCA");
	listPlugin.push_back("ACVisPlugin2Desc");
	listPlugin.push_back("ACVisPluginAudiogarden");
	listPlugin.push_back("ACPosPlugAudioGardenFlower");
	//#if defined(SUPPORT_ARCHIPEL)
	listPlugin.push_back("ACPosPlugArchipelAtoll");
	//#endif// defined(SUPPORT_ARCHIPEL)
	listPlugin.push_back("ACPositionsPluginRadialTreeLayout");
	listPlugin.push_back("ACEuclideanKMeansPlugin");
	//listPlugin.push_back("ACPositionsPluginRandom");
	//listPlugin.push_back("...");
	return listPlugin;
}
