/*
 *
 */

#include "ACPlugin.h"
#include "ACVisualisationPlugin.h" // "ACPlugVizDancers.h"
#include "ACNeighborhoodsPluginRandom.h"
#include "ACNeighborhoodsPluginPareto.h"
#include "ACNeighborhoodsPluginEuclidean.h"
#include "ACVisPluginPCA.h"
#include "ACVisPluginPCA13.h"
#include "ACVisPlugin2Desc.h"
#include "ACPosPlugAudioGardenGramoPhone.h"
#include "ACPosPlugAudioGardenFlower.h"
#include "ACPosPlugMakamHistoGramoPhone.h"
#include "ACPosPlugClustersClock.h"
//#if defined(SUPPORT_ARCHIPEL)
#include "ACPosPlugArchipelAtoll.h"
//#endif// defined(SUPPORT_ARCHIPEL)
#include "ACPositionsPluginNodeLinkTreeLayout.h"
#include "ACPositionsPluginRadialTreeLayout.h"
//#include "ACPositionsPluginRandom.h"

//the factories

extern "C" ACPlugin* create(std::string namePlugin) {
    if (namePlugin == "ACVisualisationPlugin") {return new ACVisualisationPlugin();}
    if (namePlugin == "ACNeighborhoodsPluginRandom") {return new ACNeighborhoodsPluginRandom();}
    if (namePlugin == "ACNeighborhoodsPluginPareto") {return new ACNeighborhoodsPluginPareto();}
    if (namePlugin == "ACNeighborhoodsPluginEuclidean") {return new ACNeighborhoodsPluginEuclidean();}
    if (namePlugin == "ACPositionsPluginNodeLinkTreeLayout") {return new ACPositionsPluginNodeLinkTreeLayout();}
    if (namePlugin == "ACVisPluginPCA") {return new ACVisPluginPCA();}
    if (namePlugin == "ACVisPluginPCA13") {return new ACVisPluginPCA13();}
    if (namePlugin == "ACVisPlugin2Desc") {return new ACVisPlugin2Desc();}
    if (namePlugin == "ACPosPlugAudioGardenGramoPhone") {return new ACPosPlugAudioGardenGramoPhone();}
    if (namePlugin == "ACPosPlugAudioGardenFlower") {return new ACPosPlugAudioGardenFlower();}
    if (namePlugin == "ACPosPlugMakamHistoGramoPhone") {return new ACPosPlugMakamHistoGramoPhone();}
    if (namePlugin == "ACPosPlugClustersClock") {return new ACPosPlugClustersClock();}
    //#if defined(SUPPORT_ARCHIPEL)
    if (namePlugin == "ACPosPlugArchipelAtoll") {return new ACPosPlugArchipelAtoll();}
    //#endif// defined(SUPPORT_ARCHIPEL)
    if (namePlugin == "ACPositionsPluginRadialTreeLayout") {return new ACPositionsPluginRadialTreeLayout();}
    //if (namePlugin == "ACPositionsPluginRandom") {return new ACPositionsPluginRandom();}
}

extern "C" void destroy(ACPlugin* d) {
    delete d;
}

extern "C" std::vector<std::string> list() {    //returns a string vector containing the plugin names included in the DLL file
    std::vector<std::string> listPlugin;
    listPlugin.push_back("ACVisualisationPlugin");
    listPlugin.push_back("ACPositionsPluginNodeLinkTreeLayout");
    listPlugin.push_back("ACNeighborhoodsPluginPareto");
    listPlugin.push_back("ACNeighborhoodsPluginEuclidean");
/*#ifdef USE_DEBUG
    listPlugin.push_back("ACNeighborhoodsPluginRandom");
#endif*/ //CF just for testing neighbor position plugins
    listPlugin.push_back("ACVisPluginPCA");
    listPlugin.push_back("ACVisPluginPCA13");
    listPlugin.push_back("ACVisPlugin2Desc");
    listPlugin.push_back("ACPosPlugAudioGardenGramoPhone");
    listPlugin.push_back("ACPosPlugAudioGardenFlower");
#if defined(USE_MAKAM)
    listPlugin.push_back("ACPosPlugMakamHistoGramoPhone");
#endif// defined(USE_MAKAM)
    listPlugin.push_back("ACPosPlugClustersClock");
#if defined(SUPPORT_ARCHIPEL)
    listPlugin.push_back("ACPosPlugArchipelAtoll");
#endif// defined(SUPPORT_ARCHIPEL)
    listPlugin.push_back("ACPositionsPluginRadialTreeLayout");
    //listPlugin.push_back("ACPositionsPluginRandom");
    //listPlugin.push_back("...");
    return listPlugin;
}
