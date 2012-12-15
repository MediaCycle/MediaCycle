//
//  ACNormalizePlugin.h
//  MediaCycle
//
//  @author Thierry Ravet
//  @date 5/12/12
//
//

#ifndef __MediaCycle__ACNormalizePlugin__
#define __MediaCycle__ACNormalizePlugin__

#include <iostream>

#include "ACPlugin.h"
#include "ACMediaBrowser.h"

class ACNormalizePlugin : public ACPreProcessPlugin {
public:
	ACNormalizePlugin();
	~ACNormalizePlugin();
    
    virtual preProcessInfo update(ACMedias media_library);
	
	virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia);
	
	virtual void freePreProcessInfo(preProcessInfo &info);
	
private:
	
protected:
};
#endif /* defined(__MediaCycle__ACNormalizePlugin__) */
