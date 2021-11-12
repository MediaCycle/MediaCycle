//
//  ACVideoDiffSegmentationPlugin.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 8/11/12
//
//

#ifndef __MediaCycle__ACVideoDiffSegmentationPlugin__
#define __MediaCycle__ACVideoDiffSegmentationPlugin__

#include <iostream>


#if defined (SUPPORT_VIDEO)

#include <MediaCycle.h>

#include <iostream>

class ACVideoDiffSegmentationPlugin : public ACSegmentationPlugin {
public:
    ACVideoDiffSegmentationPlugin();
    ~ACVideoDiffSegmentationPlugin();
    
    virtual std::vector<ACMedia*> segment(ACMediaData* _data, ACMedia*);
    std::vector<ACMedia*> segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia);
    
private:
    std::vector<ACMedia*> _segment(ACMedia* _theMedia);
    std::vector<ACMedia*> _segment(ACMediaTimedFeature* _MTF, ACMedia* _theMedia);
    int method;
    std::vector<std::string> methods;
    float threshold;
};

#endif //defined (SUPPORT_VIDEO)

#endif /* defined(__MediaCycle__ACVideoDiffSegmentationPlugin__) */
