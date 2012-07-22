//#include <armadillo>

#include "ACPlugin.h"
#include "ACMediaBrowser.h"
//#include "ACMediaBrowser.h"
//#include "ACMediaTimedFeature.h"

//using namespace arma;

#ifndef _ACNEIGHBORHOODSPLUGINRANDOM_
#define _ACNEIGHBORHOODSPLUGINRANDOM_

class ACNeighborhoodsPluginRandom : public ACNeighborMethodPlugin {
public:
    ACNeighborhoodsPluginRandom();
    ~ACNeighborhoodsPluginRandom();
    virtual void updateNeighborhoods(ACMediaBrowser* );
};

#endif	/* _ACNEIGHBORHOODSPLUGINRANDOM_ */


