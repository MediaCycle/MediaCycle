/*
 *
 */

#include "ACPlugin.h"
#include "ACPlugVizDancers.h"
#include "ACNeighborhoodsPluginRandom.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
    if (namePlugin == "ACPlugVizDancers") {return new ACPlugVizDancers();}
	if (namePlugin == "ACNeighborhoodsPluginRandom") {return new ACNeighborhoodsPluginRandom();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
    listPlugin.push_back("ACVisualisationPlugin");
	listPlugin.push_back("ACNeighborhoodsPluginRandom");
    //listPlugin.push_back("...");
    return listPlugin;
}
