/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioFeaturesPlugin.h"
#include "ACAudioGardenFeaturesPlugin.h"
//#include "ACAudioFeaturesChromaRhythmPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACAudioFeaturesPlugin") {return new ACAudioFeaturesPlugin();}
	//if (namePlugin == "ACAudioGardenFeaturesPlugin") {return new ACAudioGardenFeaturesPlugin();}
	//if (namePlugin == "ACAudioFeaturesChromaRhythmPlugin") {return new ACAudioFeaturesChromaRhythmPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	listPlugin.push_back("ACAudioFeaturesPlugin");
    //listPlugin.push_back("ACAudioGardenFeaturesPlugin");
	//listPlugin.push_back("ACAudioFeaturesChromaRhythmPlugin");
    //listPlugin.push_back("...");
    return listPlugin;
}
