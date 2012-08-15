/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioAcidPlugin.h"
#if defined(USE_AUDIOFEAT)
#include "ACAudioFeaturesPlugin.h"
#include "ACAudioHaitsmaFingerprintPlugin.h"
//#include "ACAudioFeaturesChromaRhythmPlugin.h"
#if defined(USE_AUDIOGARDEN)
#include "ACAudioGardenFeaturesPlugin.h"
#endif //defined(USE_AUDIOGARDEN)
#endif //defined(USE_AUDIOFEAT)
#if defined(USE_YAAFE)
#include "ACAudioYaafePlugin.h"
#endif //defined(USE_YAAFE)
#if defined(USE_OCTAVE) && defined(USE_MAKAM)
#include "ACAudioMakamFeaturesPlugin.h"
#include "ACAudioMakamClassifierPlugin.h"
#endif // defined(USE_OCTAVE) && defined(USE_MAKAM)


//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACAudioAcidPlugin") {return new ACAudioAcidPlugin();}
#if defined(USE_YAAFE)
    if (namePlugin == "ACAudioYaafePlugin") {return new ACAudioYaafePlugin();}
#endif //defined(USE_YAAFE)
#if defined(USE_AUDIOFEAT)
    if (namePlugin == "ACAudioFeaturesPlugin") {return new ACAudioFeaturesPlugin();}
    //if (namePlugin == "ACAudioHaitsmaFingerprintPlugin") {return new ACAudioHaitsmaFingerprintPlugin();}
    //if (namePlugin == "ACAudioFeaturesChromaRhythmPlugin") {return new ACAudioFeaturesChromaRhythmPlugin();}
#if defined(USE_AUDIOGARDEN)	
    //if (namePlugin == "ACAudioGardenFeaturesPlugin") {return new ACAudioGardenFeaturesPlugin();}
#endif //defined(USE_AUDIOGARDEN)	
#endif //defined(USE_AUDIOFEAT)	
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
// We use Yaafe if Yaafe and AudioFeatures are available	
#if defined(USE_YAAFE)
    listPlugin.push_back("ACAudioYaafePlugin");
#else
	#if defined(USE_AUDIOFEAT)
    listPlugin.push_back("ACAudioFeaturesPlugin");
	#endif //defined(USE_AUDIOFEAT)
#endif //defined(USE_YAAFE)
#if defined(USE_AUDIOFEAT)	
    //listPlugin.push_back("ACAudioHaitsmaFingerprintPlugin");
    //listPlugin.push_back("ACAudioFeaturesChromaRhythmPlugin");
#if defined(USE_AUDIOGARDEN)	
	//listPlugin.push_back("ACAudioGardenFeaturesPlugin");
#endif //defined(USE_AUDIOGARDEN)	
#endif //defined(USE_AUDIOFEAT)
#if defined(USE_OCTAVE) && defined(USE_MAKAM)
    listPlugin.push_back("ACAudioMakamFeaturesPlugin");
    listPlugin.push_back("ACAudioMakamClassifierPlugin");
#endif
    listPlugin.push_back("ACAudioAcidPlugin"); // We list the ACID plugin at the since (for now) it isn't content-based	
    return listPlugin;
}
