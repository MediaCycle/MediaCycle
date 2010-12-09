/*
 *
 */

#include "ACPlugin.h"
#include "ACAudioSegmentationPlugin.h"
#include "ACBicSegmentationPlugin.h"

#include<iostream> 
//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
	if (namePlugin == "ACAudioSegmentationPlugin") {return new ACAudioSegmentationPlugin();}
	else if (namePlugin == "ACBicSegmentationPlugin") {return new ACBicSegmentationPlugin();}
	std::cout << "unknown Plugin Type, returning NULL" << namePlugin << std::endl;
	return NULL;
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	listPlugin.push_back("ACAudioSegmentationPlugin");
	listPlugin.push_back("ACBicSegmentationPlugin");
	//listPlugin.push_back("...");
    return listPlugin;
}
