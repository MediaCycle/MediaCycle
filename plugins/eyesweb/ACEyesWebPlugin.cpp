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

string ACEyesWebPlugin::extractDirectory(string path)
{
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\');
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(0, index + 1);
}

string ACEyesWebPlugin::extractFilename(string path)
{
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\' );
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(index + 1);
}

string ACEyesWebPlugin::changeExtension(string path, string ext)
{
  string filename = extractFilename(path);
  return extractDirectory(path) + filename.substr(0, filename.find_last_of('.')) + ext;
}

ACMediaFeatures* ACEyesWebPlugin::calculate(std::string fileName)
{

    ACMediaTimedFeatures *mediaTimedFeatures = new ACMediaTimedFeatures();
    mediaTimedFeatures->setName("Descriptor");

    //fileName = video path (.mov, .avi, ...)
    string dataFile = changeExtension(fileName,".desc.txt");
    string segmentFile = changeExtension(fileName,".seg.txt");
 
    mediaTimedFeatures->readFile(dataFile); 
    cout << "Reading : " << dataFile << endl;
    //use of mean()
    fmat mean = mediaTimedFeatures->mean();
    //mediaTimedFeatures->getValue().print("Value");
    mean.print("Mean : ");

    fmat stdDev = mediaTimedFeatures->standardDeviation();
    stdDev.print("STD : ");

    //use of meanAsVector()
    vector<float> meanV = mediaTimedFeatures->meanAsVector();

    //write the mean vector in the ACMediaFeature object
    this->mMediaFeatures->resize(meanV.size());
    for (int i=0; i<meanV.size(); i++)
        this->mMediaFeatures->setFeature(i,meanV[i]);
    this->mMediaFeatures->setComputed();

    /*//Display meanAsVector()
    cout << "Mean vector (float) : "<<endl;
    this->mMediaFeatures->dump();*/

    umat resultHist = mediaTimedFeatures->hist(10);     //10 bins
    resultHist.print("Histo : ");       //ok, checked with Matlab

    //fmat similarity = mediaTimedFeatures->similarity();
    //similarity.print("Similarity matrix : ");

    cout << "Reading : " << segmentFile << endl;
    mediaTimedFeatures->importSegmentsFromFile(segmentFile);

    //vector<float> seg = mediaTimedFeatures->getSegments();
    //for (int i=0;i<seg.size();i++) cout<<"Segment : "<<seg[i]<<endl;

    ACMediaTimedFeatures *resultseg = mediaTimedFeatures->meanSegment();
    resultseg->getTime().print("Time : ");
    resultseg->getValue().print("Value : ");
    
    return this->mMediaFeatures;
}

