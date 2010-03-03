//#include <armadillo>
//#include "Armadillo-utils.h"
//#include "ACPlugin.h"
#include "ACNeighborhoodsPluginRandom.h"

//using namespace arma;
using namespace std;

ACNeighborhoodsPluginRandom::ACNeighborhoodsPluginRandom()
{
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "RandomNeighborhoods";
    this->mDescription = "Plugin for the computation of random neighborhoods";
    this->mId = "";
	
    //local vars
}

ACNeighborhoodsPluginRandom::~ACNeighborhoodsPluginRandom()
{
}

int ACNeighborhoodsPluginRandom::initialize()
{
    std::cout << "ACNeighborhoodsPluginRandom initialized" << std::endl;
    return 0;
}

vector<ACMediaFeatures*> ACNeighborhoodsPluginRandom::calculate(){
	vector<ACMediaFeatures*> raf;
	raf.resize(0);
	return raf;
}

vector<ACMediaFeatures*> ACNeighborhoodsPluginRandom::calculate(std::string aFileName) {
	vector<ACMediaFeatures*> raf;
	raf.resize(0);
	return raf;
}

void ACNeighborhoodsPluginRandom::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	if ( mediaBrowser->getUserLog() == NULL )
		std::cout << "Need to create an user log " << std::endl;
}
