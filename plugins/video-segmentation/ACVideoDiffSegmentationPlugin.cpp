//
//  ACVideoDiffSegmentationPlugin.cpp
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 8/11/12
//
//

#include "ACVideoDiffSegmentationPlugin.h"
#include "cv.h"
#include "highgui.h"
#include <Armadillo-utils.h>
using namespace arma;

ACVideoDiffSegmentationPlugin::ACVideoDiffSegmentationPlugin() : ACSegmentationPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    // this->mPluginType = PLUGIN_TYPE_SEGMENTATION;
    this->mName = "Video Diff Segmentation";
    this->mDescription = "Video Diff Segmentation plugin";
    this->mId = "";
    this->method = 0;
    this->addNumberParameter("threshold",0.4,0,1,0.05,"pike detection threshold");
    //this->addStringParameter("method",methods[method],methods,"method");
}

ACVideoDiffSegmentationPlugin::~ACVideoDiffSegmentationPlugin() {
}

std::vector<ACMedia*> ACVideoDiffSegmentationPlugin::segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia){
    //return this->_segment( _MTF, _theMedia);
    return this->_segment(_theMedia);
}

std::vector<ACMedia*> ACVideoDiffSegmentationPlugin::_segment(ACMedia* theMedia) {
    ACMediaData* Video_data = theMedia->getMediaData();
    return this->segment(Video_data, theMedia);
}

std::vector<ACMedia*> ACVideoDiffSegmentationPlugin::segment(ACMediaData* Video_data, ACMedia* theMedia) {
    
    threshold=this->getNumberParameterValue("threshold");
    
    vector<ACMedia*> segments;
    cv::VideoCapture cvVideo(theMedia->getFileName());
    if (cvVideo.isOpened()&&cvVideo.get(CV_CAP_PROP_FRAME_COUNT)>4){
        int lMax=cvVideo.get(CV_CAP_PROP_FRAME_COUNT);
        float fps=cvVideo.get(CV_CAP_PROP_FPS);
        fcolvec diffImage(lMax);
        cv::Mat imgCurr,imgNext,imgTemp,imgTemp2;
        cvVideo>>imgTemp;
        if (imgTemp.channels()==3)
            cv::cvtColor(imgTemp, imgCurr, CV_BGR2GRAY);
        else
            imgCurr=imgTemp;
        {
            cv::Scalar totTemp=cv::sum(imgCurr);
            diffImage[0]=totTemp[0];
        }
        
        for (int cpt=1;cpt<lMax;cpt++){
            if (cvVideo.isOpened()==false)
                break;
            cvVideo>>imgTemp;
            if (imgTemp.empty())
                continue;
            if (imgTemp.channels()==3)
                cv::cvtColor(imgTemp, imgNext, CV_BGR2GRAY);
            else
                imgNext=imgTemp;
            cv::absdiff(imgNext,imgCurr,imgTemp2);
            cv::Scalar totTemp=cv::sum(imgTemp2);
           // std::cout<<totTemp[0]<<","<<totTemp[1]<<","<<totTemp[2]<<","<<totTemp[3]<<std::endl;
            
            diffImage[cpt]=totTemp[0];
            imgCurr=imgNext.clone();
            imgNext.release();
            imgTemp2.release();
            imgTemp.release();
        }
        fcolvec medVec(lMax);
        //std::cout<<diffImage<<std::endl;
        for (int cpt=0;cpt<lMax-2;cpt++){
            medVec[cpt]=median(diffImage.subvec(cpt,cpt+2));
        }
        //std::cout<<medVec<<std::endl;
        medVec[lMax-2]=medVec[lMax-3];
        medVec[lMax-1]=medVec[lMax-2];
        diffImage=abs(diffImage-medVec);
        float _max=max(diffImage);
        float _min=min(diffImage);
        float _std=stddev(diffImage);
        uvec index;
        if (stddev(diffImage)<threshold*max(diffImage)){
            diffImage=diffImage/max(diffImage);
            index=find(diffImage>threshold);
            std::cout<<diffImage<<std::endl;
            std::cout<<index<<std::endl;
        }
        cvVideo.release();
        
        // Retrieve the user-defined values
        std::vector<int> segments_limits ;
        for (int l=0;l<index.size();l++){
            segments_limits.push_back(index[l]);
        }
        
        int Nseg = segments_limits.size();
        if (Nseg == 0) {
            cerr << "< ACVideoDiffSegmentationPlugin::segment> : no segments" << endl;
            return segments; // XS check this
        }
        
        // the beginning of first segment should be zero
        // no push_front for vectors (only for list)
        if (segments_limits[0] != 0) {
            vector<int>::iterator it;
            it = segments_limits.begin();
            it = segments_limits.insert ( it , 0 );
            Nseg++;
        }
        if (segments_limits[Nseg-1] != lMax-1){
            segments_limits.push_back(lMax-1);
            Nseg++;
            
        }
        
        for (int i = 0; i < Nseg-1; i++){
                //make sure the segment from the media have the proper type
                cout << "Segment n° " << i+1 << endl;
                ACMedia* media = ACMediaFactory::getInstance().create(theMedia);
                media->setParentId(theMedia->getId());
                media->setStartInt(segments_limits[i]); // XS TODO : this is in frame number, not time code
                media->setEndInt(segments_limits[i+1]);
                cout << "begin:" << segments_limits[i]<< " end:" <<segments_limits[i+1]<<endl;
                //cout << "frameRate " << media->getFrameRate() << endl;
                //cout << "sampleRate " << media->getSampleRate() << endl;
                media->setStart((float)segments_limits[i]/fps); //TR, 29/03
                if (i<Nseg-2)
                    media->setEnd((float)segments_limits[i+1]/fps);
                else
                    media->setEnd(theMedia->getEnd());
                
                cout << "duration of the segment " << media->getDuration() << endl;
                
                media->setId(theMedia->getId()+i+1); // XS TODO check this, it is overlapping with another ID ?
                segments.push_back(media);
            }

    }
    else{//Only one segment
        cerr << "< ACVideoDiffSegmentationPlugin::segment> : no segments" << endl;
        return segments; // XS check this
    }
    
    
    
   
    return segments;
}
