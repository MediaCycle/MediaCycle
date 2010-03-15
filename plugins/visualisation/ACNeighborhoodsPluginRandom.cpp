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

int ACNeighborhoodsPluginRandom::initialize()
{
    std::cout << "ACNeighborhoodsPluginRandom initialized" << std::endl;
    return 0;
}

void ACNeighborhoodsPluginRandom::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	//int _clickedloop = mediaBrowser->getClickedLoop();
	std::cout << "ACNeighborhoodsPluginRandom::updateNeighborhoods" << std::endl;
	if (mediaBrowser->getUserLog()->getLastClickedNodeId() == -1 && mediaBrowser->getNumberOfMediaNodes() >= 19) { //CF: 19 audio samples on the mercurialized dataset
		// XSrandom...
		std::cout << "ACNeighborhoodsPluginRandom: filling tree..." << std::endl;
		// initialize -  node #0                      // node ID
		mediaBrowser->getUserLog()->addRootNode(0, 0); // 0
		// clicked on node 0 at time 0
		mediaBrowser->getUserLog()->clickNode(0,0);
	
		// add his neighbors
		mediaBrowser->getUserLog()->addNode(0, 1, 0); // 1
		mediaBrowser->getUserLog()->addNode(0, 2, 0);  // 2
		mediaBrowser->getUserLog()->addNode(0, 3, 0); // 3
		mediaBrowser->getUserLog()->addNode(0, 4, 0); // 4
		mediaBrowser->getUserLog()->addNode(0, 5, 0);  // 5

		// clicked on node 3 at time 1
		mediaBrowser->getUserLog()->clickNode(3,1);  

		// add his neighbors
		//CF: impossible to display currently an ACMedia assigned to more than one ACLoopAttribute
		//mediaBrowser->getUserLog()->addNode(3, 0, 0);  // 6 == 0
		mediaBrowser->getUserLog()->addNode(3, 6, 0); // 6
		//
		mediaBrowser->getUserLog()->addNode(3, 7, 0);  // 7
		mediaBrowser->getUserLog()->addNode(3, 8, 0); // 8
		mediaBrowser->getUserLog()->addNode(3, 9, 0); // 9
		mediaBrowser->getUserLog()->addNode(3, 10, 0) ; // 10

		mediaBrowser->getUserLog()->addNode(1, 11, 0); // 11
		mediaBrowser->getUserLog()->addNode(1, 12, 0);  // 12

		mediaBrowser->getUserLog()->addNode(2, 13, 0); // 13
		mediaBrowser->getUserLog()->addNode(2, 14, 0); // 14
		mediaBrowser->getUserLog()->addNode(2, 15, 0) ; // 15

		mediaBrowser->getUserLog()->addNode(13, 16, 0); // 16
		mediaBrowser->getUserLog()->addNode(13, 17, 0); // 17
		mediaBrowser->getUserLog()->addNode(13, 18, 0) ; // 18
		
	}	
}
