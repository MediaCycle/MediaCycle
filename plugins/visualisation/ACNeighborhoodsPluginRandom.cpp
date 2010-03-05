//#include <armadillo>
//#include "Armadillo-utils.h"
//#include "ACPlugin.h"
#include "ACNeighborhoodsPluginRandom.h"

//using namespace arma;
using namespace std;

ACNeighborhoodsPluginRandom::ACNeighborhoodsPluginRandom() {
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "RandomNeighborhoods";
    this->mDescription = "Plugin for the computation of random neighborhoods";
    this->mId = "";
	
    //local vars
}

ACNeighborhoodsPluginRandom::~ACNeighborhoodsPluginRandom() {
}

void ACNeighborhoodsPluginRandom::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	int _clickedloop = mediaBrowser->getClickedLoop();
	// XSrandom...
	
	// initialize -  node #0                      // node ID
	mediaBrowser->getUserLog()->addNode(0, 0, 0); // 0
	// clicked on node 0 at time 0
	mediaBrowser->getUserLog()->clickNode(0,0);
	
	// add his neighbors
	mediaBrowser->getUserLog()->addNode(0, 17, 0); // 1
	mediaBrowser->getUserLog()->addNode(0, 3, 0);  // 2
	mediaBrowser->getUserLog()->addNode(0, 14, 0); // 3
	mediaBrowser->getUserLog()->addNode(0, 22, 0); // 4
	mediaBrowser->getUserLog()->addNode(0, 8, 0);  // 5

	// clicked on node 3 at time 1
	mediaBrowser->getUserLog()->clickNode(3,1);  

	// add his neighbors
	mediaBrowser->getUserLog()->addNode(3, 0, 0);  // 6 == 0
	mediaBrowser->getUserLog()->addNode(3, 4, 0);  // 7
	mediaBrowser->getUserLog()->addNode(3, 13, 0); // 8
	mediaBrowser->getUserLog()->addNode(3, 21, 0); // 9
	mediaBrowser->getUserLog()->addNode(3, 9, 0) ; // 10	
}