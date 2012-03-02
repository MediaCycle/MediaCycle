/*
 *
 */

#include "ACPlugin.h"
#include "ACImageColorMomentsPlugin.h"
#include "ACImageShapeHuMomentsPlugin.h"
#include "ACImageShapeFourierPolarMomentsPlugin.h"
#include "ACImageTextureWaveletsMomentsPlugin.h"
#include "ACImageNumberOfFacesPlugin.h"
#include "ACImageNumberOfHoughLinesPlugin.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
    if (namePlugin == "ACImageColorMomentsPlugin") {return new ACImageColorMomentsPlugin();}
    if (namePlugin == "ACImageShapeHuMomentsPlugin") {return new ACImageShapeHuMomentsPlugin();}
//	if (namePlugin == "ACImageShapeFourierPolarMomentsPlugin") {return new ACImageShapeFourierPolarMomentsPlugin();}
    if (namePlugin == "ACImageTextureWaveletsMomentsPlugin") {return new ACImageTextureWaveletsMomentsPlugin();}
    if (namePlugin == "ACImageNumberOfFacesPlugin") {return new ACImageNumberOfFacesPlugin();}
    if (namePlugin == "ACImageNumberOfHoughLinesPlugin") {return new ACImageNumberOfHoughLinesPlugin();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
	// XS TODO: ajouter m�canisme pour v�rifier que le plugin existe
	listPlugin.push_back("ACImageColorMomentsPlugin");
	listPlugin.push_back("ACImageShapeHuMomentsPlugin");
//	listPlugin.push_back("ACImageShapeFourierPolarMomentsPlugin");
    listPlugin.push_back("ACImageTextureWaveletsMomentsPlugin");
    #ifdef USE_DEBUG
    //listPlugin.push_back("ACImageNumberOfFacesPlugin");
    //listPlugin.push_back("ACImageNumberOfHoughLinesPlugin");
    #endif
   return listPlugin;
}
