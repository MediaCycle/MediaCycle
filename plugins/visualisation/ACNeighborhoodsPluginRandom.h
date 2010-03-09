//#include <armadillo>

#include "ACPlugin.h"
#include "ACMediaBrowser.h"
//#include "ACMediaBrowser.h"
//#include "ACMediaTimedFeature.h"

//using namespace arma;

#ifndef _ACNEIGHBORHOODSPLUGINRANDOM_
#define _ACNEIGHBORHOODSPLUGINRANDOM_

class ACNeighborhoodsPluginRandom : public ACPlugin {
public:
  ACNeighborhoodsPluginRandom();
  ~ACNeighborhoodsPluginRandom();
	virtual int initialize();
	void updateClusters(ACMediaBrowser* ){};//not implemented
	void updateNextPositions(ACMediaBrowser* ){};//not implemented
	virtual void updateNeighborhoods(ACMediaBrowser* );
protected:
};

#endif	/* _ACNEIGHBORHOODSPLUGINRANDOM_ */


