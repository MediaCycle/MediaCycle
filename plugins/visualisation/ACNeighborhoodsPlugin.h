//#include <armadillo>

#include "ACPlugin.h"
#include "ACMediaBrowser.h"
//#include "ACMediaBrowser.h"
//#include "ACMediaTimedFeature.h"

//using namespace arma;

#ifndef _ACNEIGHBORHOODSPLUGIN_
#define _ACNEIGHBORHOODSPLUGIN_

class ACNeighborhoodsPlugin : public ACPlugin {
public:
  ACNeighborhoodsPlugin();
  ~ACNeighborhoodsPlugin();
	void updateClusters(ACMediaBrowser* ){};//not implemented
	void updateNextPositions(ACMediaBrowser* ){};//not implemented
	virtual void updateNeighborhoods(ACMediaBrowser* );
protected:
};

#endif	/* _ACNEIGHBORHOODSPLUGIN_ */


