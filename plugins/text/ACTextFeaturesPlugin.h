/*
 *  ACTextFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/10/10
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */
#ifndef _ACTEXTFEATURESPLUGIN_H
#define	_ACTEXTFEATURESPLUGIN_H

#include "ACTextFeatures.h"
#include "MediaCycle.h"
#include "ACMediaData.h"
#include "ACText.h"

#include "ACIndexModifier.h"

#include<iostream>

using namespace std;
using namespace lucene::index;

class ACTextFeaturesPlugin : public ACFeaturesPlugin, public ACPreProcessPlugin {
public:
    ACTextFeaturesPlugin();
    ~ACTextFeaturesPlugin();

    //	bool hasPostProcess(){return true;}

    virtual std::vector<ACMediaFeatures*> calculate(ACMedia* theMedia, bool _save_timed_feat=false);
    virtual ACFeatureDimensions getFeaturesDimensions();
    virtual preProcessInfo update(ACMedias media_library);

    virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia);

    virtual void freePreProcessInfo(preProcessInfo &info);

private:

    void clearIndexTerm(std::vector<TCHAR*> &pIndexTerms);
    std::vector<TCHAR*> indexTermsExtraction();
    std::vector<float> indexIdfExtraction();
    ACMediaFeatures* tfCalculate(ACText* pMedia);
    void createIndex(void);
    void closeIndex(void);
    void addMedia(ACText* theMedia);

protected:	
    bool mIndexValid;
    ACIndexModifier* mIndex;
    std::vector<TCHAR*> indexTerms;
    std::vector<float> indexIdf;
    //	lucene::analysis::SimpleAnalyzer* an;
    lucene::analysis::StopAnalyzer *an;//("/Users/ravet/Desktop/navimed/TMG_5.0R6/common_words - fr.txt");

    string pathIndex;
};

#endif	/* _ACTEXTFEATURESPLUGIN_H */
