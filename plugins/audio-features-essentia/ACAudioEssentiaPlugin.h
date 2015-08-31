/**
 * @brief MediaCycle plugin to extract basic Essentia features
 * @author Christian Frisson
 * @date 27/08/2015
 * @copyright (c) 2015 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef _ACAudioEssentiaPlugin_H
#define	_ACAudioEssentiaPlugin_H

#include "MediaCycle.h"
#include<iostream>

#include "streaming_extractor.h"

class ACAudioEssentiaPlugin : public ACTimedFeaturesPlugin {
    
public:
    ACAudioEssentiaPlugin();
    virtual ~ACAudioEssentiaPlugin();
    
    virtual std::vector<ACMediaFeatures*> calculate(ACMedia* theMedia, bool _save_timed_feat=false);    
    // virtual ACMediaTimedFeature* getTimedFeatures(std::string mtf_file_name);
    virtual ACFeatureDimensions getFeaturesDimensions();
    virtual ACFeatureDimensions getTimedFeaturesDimensions();  
    
protected:
    // pool containing profile (configure) options:
    essentia::Pool options;
    /* // pools for storing results
  essentia::Pool neqloudPool; // non equal loudness pool
  essentia::Pool eqloudPool; // equal loudness pool*/
    
    void compute(const std::string& audioFilename, const std::string& outputFilename,
                 essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options);
    
    void outputToFile(essentia::Pool& pool, const std::string& outputFilename, const essentia::Pool& options);
    
    void clearFeatures();
    void clearTimedFeatures();

    bool addMediaTimedFeature(ACMediaTimedFeature* feature);
    bool addMediaFeature(ACMediaFeatures* feature);
    
    std::map<std::string,ACMediaTimedFeature*> descmtf;
    std::map<std::string,ACMediaFeatures*> descmf;
    
    /*std::vector<std::string> timedFeatureNames;
    std::vector<std::string> featureNames;*/
    
    ACFeatureDimensions featureDimensions;
    ACFeatureDimensions timedFeatureDimensions;
};


#endif	/* _ACAudioEssentiaPlugin_H */
