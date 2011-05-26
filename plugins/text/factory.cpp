/*
 *
 */

#include "ACPlugin.h"
#include "ACTextFeaturesPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACTextFeaturesPlugin") {return new ACTextFeaturesPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	listPlugin.push_back("ACTextFeaturesPlugin");
    //listPlugin.push_back("...");
    return listPlugin;
}
