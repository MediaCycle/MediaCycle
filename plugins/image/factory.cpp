/*
 *
 */

#include "ACPlugin.h"
#include "ACImageColorMomentsPlugin.h"
#include "ACImageShapeHuMomentsPlugin.h"
#include "ACImageTextureWaveletsMomentsPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
    if (namePlugin == "ACImageColorMomentsPlugin") {return new ACImageColorMomentsPlugin();}
    if (namePlugin == "ACImageShapeHuMomentsPlugin") {return new ACImageShapeHuMomentsPlugin();}
    if (namePlugin == "ACImageTextureWaveletsMomentsPlugin") {return new ACImageTextureWaveletsMomentsPlugin();}
	
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
//    listPlugin.push_back("ACImagePlugin");
	// XS TODO: ajouter mécanisme pour vérifier que le plugin existe
    listPlugin.push_back("ACImageColorMomentsPlugin");
    listPlugin.push_back("ACImageShapeHuMomentsPlugin");
    listPlugin.push_back("ACImageTextureWaveletsMomentsPlugin");
    return listPlugin;
}
