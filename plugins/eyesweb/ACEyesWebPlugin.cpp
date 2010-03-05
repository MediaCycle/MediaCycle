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

vector<ACMediaFeatures*> ACEyesWebPlugin::calculate()
// TODO XS : do we really want this to return NULL ?
{
  vector<ACMediaFeatures*> raf;
  raf.resize(0);
  return raf;
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

vector<ACMediaFeatures*> ACEyesWebPlugin::calculate(std::string fileName){
  ACMediaTimedFeatures *mediaTimedFeatures = new ACMediaTimedFeatures();
  //fileName = video path (.mov, .avi, ...)
  string dataFile = changeExtension(changeLastFolder(fileName,"analyse/Front"),".ew.txt");
  string dataFile2 = changeExtension(changeLastFolder(fileName,"analyse/Top"),"imagefeatures.txt");
  //string dataFile = changeExtension(fileName,".txt");
  const int numDesc = 6;
  vector<ACMediaFeatures*> featureVec;
  const std::string descMeanNames[numDesc] = {"xBarycenterMean", "yBarycenterMean", "wRectMean", "hRectMean", "iqomMean", "ciMean"};
  const std::string descStdNames[numDesc] = {"xBarycenterStd", "yBarycenterStd", "wRectStd", "hRectStd", "iqomStd", "ciStd"};

  if (mediaTimedFeatures->readFile(dataFile) == 0) {    //the file is missing
    featureVec.resize(0);
    return featureVec;
  }
  
  ACMediaFeatures* mMediaFeatures;
  
  
  ACMediaFeatures* meanV = mediaTimedFeatures->mean();   //mean computation    
  //write meanV in the ACMediaFeature object
   
  for (int i=0; i<meanV->getSize(); i++){
    mMediaFeatures = new ACMediaFeatures();
    mMediaFeatures->resize(1);
    mMediaFeatures->setName(descMeanNames[i]);
    mMediaFeatures->setFeatureElement(0, meanV->getFeatureElement(i));
    featureVec.push_back(mMediaFeatures);
  }

  ACMediaFeatures* stdV = mediaTimedFeatures->std();     //std computation

  for (int i=0; i<stdV->getSize(); i++){
    mMediaFeatures = new ACMediaFeatures();
    mMediaFeatures->resize(1);
    mMediaFeatures->setName(descStdNames[i]);
    mMediaFeatures->setFeatureElement(0,stdV->getFeatureElement(i));
    featureVec.push_back(mMediaFeatures);
  }

  //-----------------------------------------------

//   fmat imagefeatures_m;
//   imagefeatures_m.load(dataFile2, raw_ascii);
//   if (imagefeatures_m.n_elem == 0)
//     {
//       mMediaFeatures = NULL;
//       return mMediaFeatures;
//     }
//   //mediaTimedFeatures->getValue().print("Value : ");
//   mMediaFeatures->resize(mMediaFeatures->size() + imagefeatures_m.n_cols);
//   for (int i=0; i<imagefeatures_m.n_cols; i++)
//     mMediaFeatures->setFeatureElement(mMediaFeatures->size()-imagefeatures_m.n_cols+i,imagefeatures_m(0,i));

//   mMediaFeatures->setComputed();

//   mMediaFeatures->dump();

    
  return featureVec;
}


