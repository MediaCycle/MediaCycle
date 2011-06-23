/*
 *
 */

#include "ACPlugin.h"
#include "ACSparseCosKMeansPlugin.h"
#include "ACSparseCosClustPosition.h"
//#include "ACPositionsPluginRandom.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACSparseCosKMeansPlugin") {return new ACSparseCosKMeansPlugin();}
	if (namePlugin == "ACSparseCosClustPosition") {return new ACSparseCosClustPosition();}
	//if (namePlugin == "ACPositionsPluginRandom") {return new ACPositionsPluginRandom();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
	std::vector<std::string> listPlugin;
	listPlugin.push_back("ACSparseCosKMeansPlugin");
	listPlugin.push_back("ACSparseCosClustPosition");
	//listPlugin.push_back("ACPositionsPluginRandom");
	//listPlugin.push_back("...");
	return listPlugin;
}
