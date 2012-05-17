/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioAcidPlugin.h"
#include "ACAudioFeaturesPlugin.h"
#include "ACAudioHaitsmaFingerprintPlugin.h"
#include "ACAudioGardenFeaturesPlugin.h"
#include "ACAudioYaafePlugin.h"

//#include "ACAudioFeaturesChromaRhythmPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
#ifdef USE_YAAFE
    if (namePlugin == "ACAudioYaafePlugin") {return new ACAudioYaafePlugin();}
#endif
    if (namePlugin == "ACAudioFeaturesPlugin") {return new ACAudioFeaturesPlugin();}
    //if (namePlugin == "ACAudioHaitsmaFingerprintPlugin") {return new ACAudioHaitsmaFingerprintPlugin();}
    //if (namePlugin == "ACAudioGardenFeaturesPlugin") {return new ACAudioGardenFeaturesPlugin();}
    //if (namePlugin == "ACAudioFeaturesChromaRhythmPlugin") {return new ACAudioFeaturesChromaRhythmPlugin();}
    if (namePlugin == "ACAudioAcidPlugin") {return new ACAudioAcidPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	#ifdef USE_DEBUG
	#ifdef USE_YAAFE
    listPlugin.push_back("ACAudioYaafePlugin");
	#endif
	#endif
    listPlugin.push_back("ACAudioFeaturesPlugin");
    //listPlugin.push_back("ACAudioHaitsmaFingerprintPlugin");
    //listPlugin.push_back("ACAudioGardenFeaturesPlugin");
    //listPlugin.push_back("ACAudioFeaturesChromaRhythmPlugin");
    //listPlugin.push_back("...");
    listPlugin.push_back("ACAudioAcidPlugin");
    return listPlugin;
}
