/*
 *
 */

#include "ACEyesWebPlugin.h"

using namespace arma;
using namespace std;

ACEyesWebPlugin::ACEyesWebPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "EyesWeb";
    this->mDescription = "EyesWeb plugin";
    this->mId = "";

    //local vars
    this->mMediaFeatures = new ACMediaFeatures();
}

ACEyesWebPlugin::~ACEyesWebPlugin()
{
    delete this->mMediaFeatures;
}

int ACEyesWebPlugin::initialize()
{
    std::cout << "ACEyesWebPlugin initialized" << std::endl;
    return 0;
}

ACMediaFeatures* ACEyesWebPlugin::calculate()
{
    return NULL;
}

ACMediaFeatures* ACEyesWebPlugin::calculate(std::string aFileName)
{
    ACMediaTimedFeatures *mediaTimedFeatures = new ACMediaTimedFeatures();

    mediaTimedFeatures->readFile(aFileName);
    /*
    fmat mean = mediaTimedFeatures->mean();
    mediaTimedFeatures->getValue().print("Value");
    mean.print("Mean");
    */
    //use of meanAsVector()
    vector<float> meanV = mediaTimedFeatures->meanAsVector();

    //write the mean vector in the ACMediaFeature object
    this->mMediaFeatures->resize(meanV.size());
    for (int i=0; i<meanV.size(); i++)
        this->mMediaFeatures->setFeature(i,meanV[i]);
    this->mMediaFeatures->setComputed();
    
    return this->mMediaFeatures;
}
