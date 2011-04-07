//#include <armadillo>

#include "ACPlugin.h"
#include "ACMediaBrowser.h"
//#include "ACMediaBrowser.h"
//#include "ACMediaTimedFeature.h"

//using namespace arma;

#ifndef _ACNEIGHBORHOODSPLUGIN_
#define _ACNEIGHBORHOODSPLUGIN_

class ACNeighborhoodsPlugin : public ACNeighborMethodPlugin {
public:
  ACNeighborhoodsPlugin();
  ~ACNeighborhoodsPlugin();
	virtual void updateNeighborhoods(ACMediaBrowser* );
protected:
};

#endif	/* _ACNEIGHBORHOODSPLUGIN_ */


