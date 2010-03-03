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
  virtual std::vector<ACMediaFeatures*> calculate();
  virtual std::vector<ACMediaFeatures*> calculate(std::string aFileName);
  std::vector<ACMediaFeatures*> calculate(ACMediaData* _data){};
	void updateClusters(ACMediaBrowser* ){};//not implemented
	void updateNextPositions(ACMediaBrowser* ){};//not implemented
	virtual void updateNeighborhoods(ACMediaBrowser* );
  virtual int start() {return 0;};
  virtual int stop() {return 0;};
protected:
};

#endif	/* _ACNEIGHBORHOODSPLUGINRANDOM_ */


