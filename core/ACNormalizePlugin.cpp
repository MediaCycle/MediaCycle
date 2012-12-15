//
//  ACNormalizePlugin.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 5/12/12
//
//

#include "ACNormalizePlugin.h"

using namespace std;


struct ACNormalizePreprocessInfo{
    std::vector< std::vector<double> > mean_features;
    std::vector< std::vector<double> > stdev_features;
};

ACNormalizePlugin::ACNormalizePlugin(): ACPreProcessPlugin(){
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Normalize";
    this->mDescription = "Plugin for normalizing preprocessing";
    this->mId = "";
}

ACNormalizePlugin::~ACNormalizePlugin(){}



preProcessInfo ACNormalizePlugin::update(ACMedias media_library){
    if (media_library.size()==0) return (preProcessInfo)0;
    ACNormalizePreprocessInfo* info=new ACNormalizePreprocessInfo;
    
    int n = media_library.size() ;
    int number_of_features = media_library.begin()->second->getNumberOfFeaturesVectors();
    int mt = 0; // number of media files of the library that have the same media type as the library (might be different in case of multimedia documents)
    ACMediaType media_type=media_library.begin()->second->getMediaType();
    // initialize to zero
    int i,j,k;
    for (i=0; i< number_of_features; i++) {
        //if (media_type != MEDIA_TYPE_MIXED || (media_type == MEDIA_TYPE_MIXED && media_library->getFirstMedia()->getMediaType() == media_type))
        { //CF
            vector<double> tmp_vect;
            for (j=0; j< media_library.begin()->second->getFeaturesVector(i)->getSize(); j++) {
                tmp_vect.push_back(0.0);
            }
            info->mean_features.push_back(tmp_vect);
            info->stdev_features.push_back(tmp_vect);
            mt++; //CF
        }
    }
    
    // computing sums
    for(ACMedias::iterator media = media_library.begin(); media!=media_library.end();media++){
        ACMedia* theMedia = media->second;
        //if (theMedia->getMediaType() == media_library->media_type)
        { //CF
            for(j=0; j<(int)info->mean_features.size(); j++){
                for(k=0; k<(int)info->mean_features[j].size(); k++){
                    double val = theMedia->getFeaturesVector(j)->getFeatureElement(k);
                    info->mean_features[j][k] += val;
                    info->stdev_features[j][k] += val * val;
                }
            }
        }
    }
    
    // before: divide by n --> biased variance estimator
    // now : divide by (n-1) -- unless n=1
    /*int nn;
     if (n==1) nn = n;
     else nn = n-1;*/
    int nn;
    if (n==1) nn = n;
    else nn = n-1;
    
    for(j=0; j<(int)info->mean_features.size(); j++) {
        cout << "calculating stats for feature" << j << endl;
        for(k=0; k<(int)info->mean_features[j].size(); k++) {
            info->mean_features[j][k] /= n;
            info->stdev_features[j][k] /= n;
            double tmp = info->stdev_features[j][k] - info->mean_features[j][k] * info->mean_features[j][k];
            if ( tmp < 0 )
                info->stdev_features[j][k] = 0;
            else {
                info->stdev_features[j][k] = sqrt( tmp*((1.0*n)/(nn)));
            }
            //printf("\t[%d] info->mean_features = %f, stddev = %f\n", k, info->mean_features[j][k], info->stdev_features[j][k]);//CF free the console
        }
    }
    return (preProcessInfo)info;
}

vector<ACMediaFeatures*> ACNormalizePlugin::apply(preProcessInfo info,ACMedia* theMedia){
    ACNormalizePreprocessInfo* locInfo=(ACNormalizePreprocessInfo*)info;
    ACMediaFeatures* feature;
    vector<ACMediaFeatures*> featureDest;
    std::vector<ACMediaFeatures*>::iterator iter;
	for (int i=0;i<theMedia->getNumberOfFeaturesVectors();i++) {
		ACMediaFeatures *tempFeat=new ACMediaFeatures;
		tempFeat->setComputed();
		tempFeat->setNeedsNormalization(theMedia->getFeaturesVector(i)->getNeedsNormalization());
		tempFeat->setName(theMedia->getFeaturesVector(i)->getName());
		tempFeat->resize(theMedia->getFeaturesVector(i)->getSize());
		featureDest.push_back(tempFeat);
	}
//    if (theMedia->getMediaType() == this->media_type)
    { //CF
            for(int j=0; j<locInfo->mean_features.size(); j++) {
                feature = theMedia->getFeaturesVector(j);
                if (feature->getNeedsNormalization()) {
                    for(int k=0; k<locInfo->mean_features[j].size(); k++) {
                        float old = feature->getFeatureElement(k);
                        featureDest[j]->setFeatureElement(k, (old - locInfo->mean_features[j][k]) / ( max(locInfo->stdev_features[j][k] , 0.00001)));//setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//CF TI_MAX(stdev_features[j][k] , 0.00001)));
                    }
                }
                else {
                    for(int k=0; k<locInfo->mean_features[j].size(); k++) {
                        float old = feature->getFeatureElement(k);
                        featureDest[j]->setFeatureElement(k, old);//setFeatureElement(k, (old - mean_features[j][k]) / ( max(stdev_features[j][k] , 0.00001)));//CF TI_MAX(stdev_features[j][k] , 0.00001)));                
                }
            }
        }
    }
    return featureDest;
}

void ACNormalizePlugin::freePreProcessInfo(preProcessInfo &info){
    delete ((ACNormalizePreprocessInfo*)info);
    info=0;
}