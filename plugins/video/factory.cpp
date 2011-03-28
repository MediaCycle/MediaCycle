/*
 *
 */

#include "ACPlugin.h"
#include "ACVideoDancersPlugin.h"
#include "ACVideoPixelSpeedPlugin.h"
#ifdef USE_STARPU 
#include "ACHuMomentsVideoPlugin.h"
#endif

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
//    if (namePlugin == "ACVideoDancersPlugin") {return new ACVideoDancersPlugin();}
#ifdef USE_STARPU //CF: temporary check to enable just this plugin only until we can select them for extraction, others might not have installled StarPU yet
    if (namePlugin == "ACHuMomentsVideoPlugin") {return new ACHuMomentsVideoPlugin();}
#else
	if (namePlugin == "ACVideoPixelSpeedPlugin") {return new ACVideoPixelSpeedPlugin();}
#endif
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
//    listPlugin.push_back("ACVideoDancersPlugin");
#ifdef USE_STARPU
    listPlugin.push_back("ACHuMomentsVideoPlugin");
#else
    listPlugin.push_back("ACVideoPixelSpeedPlugin");
#endif
    //listPlugin.push_back("...");
    //listPlugin.push_back("...");
    return listPlugin;
}
