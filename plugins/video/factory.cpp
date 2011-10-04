/*
 *
 */

#include "ACPlugin.h"
#include "ACVideoDancersPlugin.h"
#include "ACVideoPixelSpeedPlugin.h"
#include "ACVideoColorPlugin.h"
#include "ACVideoMotionOrientationPlugin.h"

#include <ACOpenCVInclude.h>

#ifdef USE_STARPU 
#include "ACHuMomentsVideoPlugin.h"
#endif

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
 //   if (namePlugin == "ACVideoDancersPlugin") {return new ACVideoDancersPlugin();}
//#ifdef USE_STARPU //CF: temporary check to enable just this plugin only until we can select them for extraction, others might not have installled StarPU yet
//    if (namePlugin == "ACHuMomentsVideoPlugin") {return new ACHuMomentsVideoPlugin();}
//#endif//#else
	if (namePlugin == "ACVideoPixelSpeedPlugin") {return new ACVideoPixelSpeedPlugin();}
	if (namePlugin == "ACVideoColorPlugin") {return new ACVideoColorPlugin();}
//	#if CV_MIN_VERSION_REQUIRED(2,3,0)
	if (namePlugin == "ACVideoMotionOrientationPlugin") {return new ACVideoMotionOrientationPlugin();}
//	#endif //CV_MIN_VERSION_REQUIRED(2,3,0)
//#endif
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
//    listPlugin.push_back("ACVideoDancersPlugin");
//#ifdef USE_STARPU
//    listPlugin.push_back("ACHuMomentsVideoPlugin");
//#endif//#else
    listPlugin.push_back("ACVideoPixelSpeedPlugin");
	listPlugin.push_back("ACVideoColorPlugin");
//#endif
//	#if CV_MIN_VERSION_REQUIRED(2,3,0)
	listPlugin.push_back("ACVideoMotionOrientationPlugin");
//	#endif //CV_MIN_VERSION_REQUIRED(2,3,0)
    //listPlugin.push_back("...");
    return listPlugin;
}
