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
}

ACEyesWebPlugin::~ACEyesWebPlugin()
{
}

int ACEyesWebPlugin::initialize()
{
    std::cout << "ACEyesWebPlugin initialized" << std::endl;
    return 0;
}

ACMediaFeatures* ACEyesWebPlugin::calculate()
// TODO XS : do we really want this to return NULL ?
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

string ACEyesWebPlugin::changeLastFolder(string path, string folder)
{
    int index = 0;
    int tmp = 0;
    string sep = "";
    string dir = extractDirectory(path);
    
    tmp = dir.substr(0,dir.size()-2).find_last_of('\\');
    int tmp2 = 0;
    tmp2 = dir.substr(0,dir.size()-2).find_last_of('/');
    if (tmp > tmp2)
    {
        index = tmp;
        sep = "\\";
    }
    else
    {
        index = tmp2;
        sep = "/";
    }
    return dir.substr(0,index+1) + folder + sep + extractFilename(path);
}

ACMediaFeatures* ACEyesWebPlugin::calculate(std::string fileName)
{
    ACMediaTimedFeatures *mediaTimedFeatures = new ACMediaTimedFeatures();
	ACMediaFeatures* mMediaFeatures = new ACMediaFeatures();
    //fileName = video path (.mov, .avi, ...)
    string dataFile = changeExtension(changeLastFolder(fileName,"Analyse/Front"),".ew.txt");
    string dataFile2 = changeExtension(changeLastFolder(fileName,"Analyse/Top"),"imagefeatures.txt");
    //string dataFile = changeExtension(fileName,".txt");

    if (mediaTimedFeatures->readFile(dataFile) == 0)    //the file is missing
    {
        mMediaFeatures = NULL;
        return mMediaFeatures;
    }

    vector<float> meanV = mediaTimedFeatures->meanAsVector();   //mean computation
    
    //write meanV in the ACMediaFeature object
    mMediaFeatures->resize(meanV.size());
    for (int i=0; i<meanV.size(); i++)
        mMediaFeatures->setFeature(i,meanV[i]);

    vector<float> stdV = mediaTimedFeatures->stdAsVector();     //std computation

    mMediaFeatures->resize(mMediaFeatures->size() + stdV.size());
    for (int i=0; i<stdV.size(); i++)
        mMediaFeatures->setFeature(mMediaFeatures->size()-stdV.size()+i,stdV[i]);

    //-----------------------------------------------

	fmat imagefeatures_m;
	imagefeatures_m.load(dataFile2, raw_ascii);
    if (imagefeatures_m.n_elem == 0)
    {
        mMediaFeatures = NULL;
        return mMediaFeatures;
    }
    //mediaTimedFeatures->getValue().print("Value : ");
    mMediaFeatures->resize(mMediaFeatures->size() + imagefeatures_m.n_cols);
    for (int i=0; i<imagefeatures_m.n_cols; i++)
        mMediaFeatures->setFeature(mMediaFeatures->size()-imagefeatures_m.n_cols+i,imagefeatures_m(0,i));

    mMediaFeatures->setComputed();

    mMediaFeatures->dump();

    //fmat mean = mediaTimedFeatures->mean();
    //mean.print("Mean : ");

    //fmat std = mediaTimedFeatures->std();
    //std.print("STD : ");

    //umat resultHist = mediaTimedFeatures->hist(10);     //10 bins
    //resultHist.print("Histo : ");       //ok, checked with Matlab

    //fmat similarity = mediaTimedFeatures->similarity();
    //similarity.print("Similarity matrix : ");
    //similarity.save("test.txt", raw_ascii);

    //mediaTimedFeatures->importSegmentsFromFile(dataFile2);

    //vector<float> seg = mediaTimedFeatures->getSegments();
    //for (int i=0;i<seg.size();i++) cout<<"Segment : "<<seg[i]<<endl;

    //ACMediaTimedFeatures *resultseg = mediaTimedFeatures->meanSegment();
    //resultseg->getTime().print("Time : ");
    //resultseg->getValue().print("Value : ");
    
    return mMediaFeatures;
}


