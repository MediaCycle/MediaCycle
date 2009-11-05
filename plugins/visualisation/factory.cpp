/*
 *
 */

#include "ACPlugin.h"
#include "ACVisualisationPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
    if (namePlugin == "ACVisualisationPlugin") {return new ACVisualisationPlugin();}
    if (namePlugin == "ACVisualisationPlugin2") {return new ACVisualisationPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
    listPlugin.push_back("ACVisualisationPlugin");
    listPlugin.push_back("ACVisualisationPlugin2");
    //listPlugin.push_back("...");
    return listPlugin;
}
