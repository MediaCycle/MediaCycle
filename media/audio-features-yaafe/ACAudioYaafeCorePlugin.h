/**
 * @brief Core MediaCycle plugin to extract Yaafe features
 * @author Christian Frisson
 * @date 31/03/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#ifndef _ACAudioYaafeCorePlugin_H
#define	_ACAudioYaafeCorePlugin_H


#include "MediaCycle.h"
#include<iostream>

#include <algorithm>
#include <iostream>
#include <string>
#include "argtable2.h"
#include <yaafe-core/DataFlow.h>
#include <yaafe-core/AudioFileProcessor.h>
#include <yaafe-core/OutputFormat.h>
#include <yaafe-core/Engine.h>
#include "yaafe-core/ComponentFactory.h"

class ACAudioYaafeCorePlugin : public ACTimedFeaturesPlugin {
    
public:
    ACAudioYaafeCorePlugin();
    virtual ~ACAudioYaafeCorePlugin();
    
    virtual std::string dataflowFilename()=0;
    
    std::vector<ACMediaFeatures*> calculate(ACMedia* theMedia, bool _save_timed_feat=false);
    
    bool addMediaTimedFeature(ACMediaTimedFeature* feature);
    bool addMediaFeature(ACMediaFeatures* feature);
    
    bool isMediaTimedFeatureStored(std::string name, std::string file);
    ACMediaTimedFeature* getMediaTimedFeatureStored(std::string name, std::string file);
    
    virtual ACFeatureDimensions getFeaturesDimensions();
    virtual ACFeatureDimensions getTimedFeaturesDimensions();
    
    void clearFeatures();
    void clearTimedFeatures();

protected:
    void loadDataflow();
    bool registerFactories();
    
private:
    std::map<std::string,ACMediaTimedFeature*> descmtf;
    std::map<std::string,ACMediaFeatures*> descmf;
    
    //std::vector<std::string> mtf_file_names;
    void listComponents();
    void describeComponent(const std::string component);
    void printOutputFormats();
    YAAFE::AudioFileProcessor processor;
    YAAFE::Engine engine;
    int m_default_resample_rate, m_default_step_size, m_default_block_size;
    bool dataflowLoaded;
    bool factoriesRegistered;
    
    ACFeatureDimensions featureDimensions;
    ACFeatureDimensions timedFeatureDimensions;
    
    std::vector<std::string> timedFeatureNames;
    std::vector<std::string> readableTimedFeatureNames;
    std::map<std::string,ACStatType> featuresStats;
};

#endif	/* _ACAudioYaafeCorePlugin_H */
