/*
 *
 */

#include "ACPlugin.h"
#ifdef SUPPORT_AUDIO
#include "ACAudioSegmentationPlugin.h"
#endif
#ifdef SUPPORT_VIDEO
#include "ACVideoDiffSegmentationPlugin.h"
#endif
#include "ACBicSegmentationPlugin.h"
#include "ACSelfSimSegmentationPlugin.h"

#include<iostream> 
//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACBicSegmentationPlugin") {return new ACBicSegmentationPlugin();}
	#ifdef SUPPORT_AUDIO
	else if (namePlugin == "ACAudioSegmentationPlugin") {return new ACAudioSegmentationPlugin();}
#endif
    else if (namePlugin == "ACSelfSimSegmentationPlugin") {return new ACSelfSimSegmentationPlugin();}
#ifdef SUPPORT_VIDEO
    else if (namePlugin == "ACVideoDiffSegmentationPlugin") {return new ACVideoDiffSegmentationPlugin();}
#endif
	std::cout << "unknown Plugin Type, returning NULL" << namePlugin << std::endl;
	return 0;
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
#ifdef SUPPORT_AUDIO
    listPlugin.push_back("ACAudioSegmentationPlugin");
#endif
#ifdef SUPPORT_VIDEO
    listPlugin.push_back("ACVideoDiffSegmentationPlugin");
#endif
	listPlugin.push_back("ACBicSegmentationPlugin");
	listPlugin.push_back("ACSelfSimSegmentationPlugin");
	//listPlugin.push_back("...");
    return listPlugin;
}
