/*
 *
 */

#include "ACPlugin.h"
#include "ACPcaPreprocessPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACPcaPreprocessPlugin") {return new ACPcaPreprocessPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	listPlugin.push_back("ACPcaPreprocessPlugin");
    //listPlugin.push_back("...");
    return listPlugin;
}
