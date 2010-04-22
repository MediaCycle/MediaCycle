/*
 *
 */

#include "ACPlugin.h"
#include "ACVisualisationPlugin.h" // "ACPlugVizDancers.h"
#include "ACNeighborhoodsPluginRandom.h"
//#include "ACNeighborhoodsPluginPareto.h"
#include "ACNeighborhoodsPluginEuclidean.h"
#include "ACVisPluginPCA.h"
#include "ACPositionsPluginNodeLinkTreeLayout.h"
//#include "ACPositionsPluginRandom.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACVisualisationPlugin") {return new ACVisualisationPlugin();}
	if (namePlugin == "ACNeighborhoodsPluginRandom") {return new ACNeighborhoodsPluginRandom();}
//	if (namePlugin == "ACNeighborhoodsPluginPareto") {return new ACNeighborhoodsPluginPareto();}
	if (namePlugin == "ACNeighborhoodsPluginEuclidean") {return new ACNeighborhoodsPluginEuclidean();}
	if (namePlugin == "ACPositionsPluginNodeLinkTreeLayout") {return new ACPositionsPluginNodeLinkTreeLayout();}
	if (namePlugin == "ACVisPluginPCA") {return new ACVisPluginPCA();}
	//if (namePlugin == "ACPositionsPluginRandom") {return new ACPositionsPluginRandom();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
	std::vector<std::string> listPlugin;
	listPlugin.push_back("ACVisualisationPlugin");
	listPlugin.push_back("ACPositionsPluginNodeLinkTreeLayout");
	listPlugin.push_back("ACNeighborhoodsPluginRandom");
//	listPlugin.push_back("ACNeighborhoodsPluginPareto");
	listPlugin.push_back("ACNeighborhoodsPluginEuclidean");
	listPlugin.push_back("ACVisPluginPCA");
	//listPlugin.push_back("ACPositionsPluginRandom");
	//listPlugin.push_back("...");
	return listPlugin;
}
