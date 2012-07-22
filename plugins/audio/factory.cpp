/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioAcidPlugin.h"
#include "ACAudioFeaturesPlugin.h"
#include "ACAudioHaitsmaFingerprintPlugin.h"
#include "ACAudioGardenFeaturesPlugin.h"
#include "ACAudioYaafePlugin.h"
#if defined(USE_OCTAVE) && defined(USE_MAKAM)
#include "ACAudioMakamFeaturesPlugin.h"
#include "ACAudioMakamClassifierPlugin.h"
#endif
//#include "ACAudioFeaturesChromaRhythmPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
#if defined(USE_YAAFE)
    if (namePlugin == "ACAudioYaafePlugin") {return new ACAudioYaafePlugin();}
#endif
    if (namePlugin == "ACAudioFeaturesPlugin") {return new ACAudioFeaturesPlugin();}
    //if (namePlugin == "ACAudioHaitsmaFingerprintPlugin") {return new ACAudioHaitsmaFingerprintPlugin();}
    //if (namePlugin == "ACAudioGardenFeaturesPlugin") {return new ACAudioGardenFeaturesPlugin();}
    //if (namePlugin == "ACAudioFeaturesChromaRhythmPlugin") {return new ACAudioFeaturesChromaRhythmPlugin();}
    if (namePlugin == "ACAudioAcidPlugin") {return new ACAudioAcidPlugin();}
#if defined(USE_OCTAVE) && defined(USE_MAKAM)
    if (namePlugin == "ACAudioMakamFeaturesPlugin") {return new ACAudioMakamFeaturesPlugin();}
    if (namePlugin == "ACAudioMakamClassifierPlugin") {return new ACAudioMakamClassifierPlugin();}
#endif
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
#if defined(USE_YAAFE)
    listPlugin.push_back("ACAudioYaafePlugin");
#else
    listPlugin.push_back("ACAudioFeaturesPlugin");
#endif
    //listPlugin.push_back("ACAudioHaitsmaFingerprintPlugin");
    //listPlugin.push_back("ACAudioGardenFeaturesPlugin");
    //listPlugin.push_back("ACAudioFeaturesChromaRhythmPlugin");
    listPlugin.push_back("ACAudioAcidPlugin");
#if defined(USE_OCTAVE) && defined(USE_MAKAM)
    listPlugin.push_back("ACAudioMakamFeaturesPlugin");
    listPlugin.push_back("ACAudioMakamClassifierPlugin");
#endif
    return listPlugin;
}
