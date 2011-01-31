/*
 *
 */

#include "ACPlugin.h"
#include "ACVideoDancersPlugin.h"
#include "ACVideoPixelSpeedPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
//    if (namePlugin == "ACVideoDancersPlugin") {return new ACVideoDancersPlugin();}
	if (namePlugin == "ACVideoPixelSpeedPlugin") {return new ACVideoPixelSpeedPlugin();}

}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
//    listPlugin.push_back("ACVideoDancersPlugin");
    listPlugin.push_back("ACVideoPixelSpeedPlugin");
    //listPlugin.push_back("...");
    return listPlugin;
}
