/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioFeaturesPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACAudioFeaturesPlugin") {return new ACAudioFeaturesPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
    listPlugin.push_back("ACAudioFeaturesPlugin");
    //listPlugin.push_back("...");
    return listPlugin;
}
