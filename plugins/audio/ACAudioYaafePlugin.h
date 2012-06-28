/*
 *  ACAudioYaafePlugin.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 31/03/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#ifndef _ACAudioYaafePlugin_H
#define	_ACAudioYaafePlugin_H

#ifdef USE_YAAFE

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
#include <yaafe-core/ComponentFactory.h>

class ACAudioYaafePlugin : public ACTimedFeaturesPlugin {

public:
    ACAudioYaafePlugin();
    ~ACAudioYaafePlugin();

    std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat=false);

    // XS TODO check if this works
    // ACMediaTimedFeature* getTimedFeatures();
    bool addMediaTimedFeature(ACMediaTimedFeature* feature, std::string file);
    bool isMediaTimedFeatureStored(std::string name, std::string file);
    ACMediaTimedFeature* getMediaTimedFeatureStored(std::string name, std::string file);

private:
    std::map<std::string,ACMediaTimedFeature*> descmf;
    //std::vector<std::string> mtf_file_names;
    void listComponents();
    void describeComponent(const std::string component);
    void printOutputFormats();
    YAAFE::AudioFileProcessor processor;
    YAAFE::Engine engine;
    int m_default_resample_rate, m_default_step_size, m_default_block_size;
};

#define ACYAAFEWRITER_ID "ACYaafeWriter"

namespace YAAFE{

class ACYaafeWriter: public ComponentBase<ACYaafeWriter> {
public:
    ACYaafeWriter();
    virtual ~ACYaafeWriter();

    const std::string getIdentifier() const {
        return ACYAAFEWRITER_ID;
    }

    virtual bool stateLess() const { return false; };

    virtual ParameterDescriptorList getParameterDescriptorList() const;

    virtual bool init(const ParameterMap& params, const Ports<StreamInfo>& in);
    virtual void reset();
    virtual bool process(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out);
    virtual void flush(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out);
    void setMediaCyclePlugin(ACAudioYaafePlugin* plugin){m_plugin = plugin;}

private:
    std::ofstream m_fout;
    std::string m_feature_name;
    int m_precision;
    std::string m_file;
    ACAudioYaafePlugin* m_plugin;
};


class ACYaafeOutputFormat : public YAAFE::OutputFormat {
public:
    ACYaafeOutputFormat(ACAudioYaafePlugin* plugin):m_plugin(plugin){}
    ~ACYaafeOutputFormat(){m_plugin = 0;}
    virtual const std::string getId() const { return "MC"; }
    virtual const std::string getDescription() const { return "Creates one MC file per feature and input file."; }
    virtual bool available() const;
    virtual const ParameterDescriptorList getParameters() const;
    virtual OutputFormat* clone() const { return new ACYaafeOutputFormat(m_plugin); }
    virtual Component* createWriter(
        const std::string& inputfile,
        const std::string& feature,
        const ParameterMap& featureParams,
        const Ports<StreamInfo>& featureStream);
private:
    ACAudioYaafePlugin* m_plugin;
};

}

#endif  //def USE_YAAFE
#endif	/* _ACAudioYaafePlugin_H */
