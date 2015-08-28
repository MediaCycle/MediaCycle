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

#include "ACAudioEssentiaPlugin.h"
#include "ACAudioEssentiaMtfOutput.h"

#include <essentia/essentiautil.h>

// helper functions
#include "streaming_extractorutils.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

ACAudioEssentiaPlugin::ACAudioEssentiaPlugin()
    :ACTimedFeaturesPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Essentia basic features";
    this->mDescription = "Audio feature extraction plugin using the Essentia library";
    this->mId = "";
    
    // Register the algorithms in the factory(ies)
    essentia::init();
    
    essentia::standard::AlgorithmFactory::Registrar<ACAudioEssentiaMtfOutput> regACAudioEssentiaMtfOutput;
    //AlgorithmFactory::Registrar<essentia::standard::ACAudioEssentiaMtfOutput, essentia::standard::ACAudioEssentiaMtfOutput> regACAudioEssentiaMtfOutput;
    
    std::string profileFilename;
    // set configuration from file or otherwise use default settings:
    setOptions(options, profileFilename);
}

ACAudioEssentiaPlugin::~ACAudioEssentiaPlugin(){
    essentia::shutdown();
}

ACFeatureDimensions ACAudioEssentiaPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featdims;
    return featdims;
}

ACFeatureDimensions ACAudioEssentiaPlugin::getTimedFeaturesDimensions(){
    ACFeatureDimensions featdims;
    return featdims;
}

void ACAudioEssentiaPlugin::clearFeatures(){
    /*std::vector<ACMediaTimedFeature*>::iterator mf;
    for(mf=desc.begin();mf!=desc.end();mf++)
        //delete (*mf);*/
    desc.clear();
}

void ACAudioEssentiaPlugin::clearTimedFeatures(){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf;
    for(mf=descmf.begin();mf!=descmf.end();mf++)
        (*mf).second = 0;//delete (*mf);
    descmf.clear();
}

std::vector<ACMediaFeatures*> ACAudioEssentiaPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    this->clearFeatures();
    this->clearTimedFeatures();
    
    
    //f_path.parent_path().string();
    _save_timed_feat = true; // forcing saving features so that we don't have to calculate them all the time
    
    // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
    bool save_binary = true;//CF was true
    string mtf_file_name; // file(s) in which feature(s) will be saved
    string file_ext =  "_b.mtf";
    if(!save_binary)
        file_ext =  "_t.mtf";
    string aFileName = theMedia->getFileName();
    string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
    
    mtf_file_name = aFileName_noext + "_" + "essentia"  + file_ext;
    string yaml_file_name = aFileName_noext + "_" + "essentia.xml";
    
    // pool containing profile (configure) options:
    //essentia::Pool options;
    // pools for storing results
    essentia::Pool neqloudPool; // non equal loudness pool
    essentia::Pool eqloudPool; // equal loudness pool
    
    
    
    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud =  options.value<Real>("equalLoudness")  != 0;
    
    if (!eqloud && !neqloud) {
        throw EssentiaException("Configuration for both equal loudness and non\
                                equal loudness is set to false. At least one must set to true");
    }
    
    try {
        
        compute(aFileName, yaml_file_name, neqloudPool, eqloudPool, options);
    }
    catch (EssentiaException& e) {
        cout << e.what() << endl;
        //throw;
    }
    
    std::map<std::string,ACMediaTimedFeature*>::iterator mf;
    for(mf=descmf.begin();mf!=descmf.end();mf++){
        std::string featureName((*mf).second->getName());
        std::replace( featureName.begin(), featureName.end(), ' ', '_');
        mtf_file_name = aFileName_noext + "_" + featureName  + file_ext;
        std::cout << "ACAudioEssentiaPlugin: trying to save feature named '" << mtf_file_name << "'... " << std::endl;
        //if(!featuresAvailable){
            bool saved = (*mf).second->saveInFile(mtf_file_name, save_binary);
            if(!saved)
                std::cerr << "ACAudioEssentiaPlugin: couldn't save feature named '" << mtf_file_name << "'" << std::endl;
        //}
        theMedia->addTimedFileNames(mtf_file_name);
        //mtf_file_names.push_back(mtf_file_name); // keep track of saved features
    }
    
    this->clearTimedFeatures();
    
    return desc;
}

void ACAudioEssentiaPlugin::compute(const string& audioFilename, const string& outputFilename,
                                    Pool& neqloudPool, Pool& eqloudPool, const Pool& options) {
    
    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud = options.value<Real>("equalLoudness") != 0;
    
    if (neqloud) neqloudPool.set("metadata.audio_properties.equal_loudness", false);
    if (eqloud) eqloudPool.set("metadata.audio_properties.equal_loudness", true);
    
    // what to compute:
    bool lowlevel = options.value<Real>("lowlevel.compute")         ||
            options.value<Real>("average_loudness.compute") ||
            options.value<Real>("tonal.compute")            ||
            options.value<Real>("sfx.compute")              ||
            options.value<Real>("rhythm.compute");
    //bool beattrack = options.value<Real>("beattrack.compute");
    bool midlevel = options.value<Real>("tonal.compute") ||
            options.value<Real>("rhythm.compute");
    bool panning = options.value<Real>("panning.compute") != 0;
    
    // compute features for the whole song
    computeReplayGain(audioFilename, neqloudPool, eqloudPool, options);
    Real startTime = options.value<Real>("startTime");
    Real endTime = options.value<Real>("endTime");
    if (eqloud) {
        if (endTime > eqloudPool.value<Real>("metadata.audio_properties.length")) {
            endTime = eqloudPool.value<Real>("metadata.audio_properties.length");
        }
    }
    else {
        if (endTime > neqloudPool.value<Real>("metadata.audio_properties.length")) {
            endTime = neqloudPool.value<Real>("metadata.audio_properties.length");
        }
    }
    if (lowlevel)
        computeLowLevel(audioFilename, neqloudPool, eqloudPool, options, startTime, endTime);
    // outdated beat tracker
    //if (beattrack) {
    //  if (neqloud) computeBeatTrack(neqloudPool, options);
    //  if (eqloud) computeBeatTrack(eqloudPool, options);
    //}
    if (midlevel)
        computeMidLevel(audioFilename, neqloudPool, eqloudPool, options, startTime, endTime);
    if (panning)
        computePanning(audioFilename, neqloudPool, eqloudPool, options, startTime, endTime);
    if (neqloud) computeHighlevel(neqloudPool, options);
    if (eqloud) computeHighlevel(eqloudPool, options);
    
    vector<Real> segments;
    if (options.value<Real>("segmentation.compute") != 0) {
        computeSegments(audioFilename, neqloudPool, eqloudPool, options);
        segments = eqloudPool.value<vector<Real> >("segmentation.timestamps");
        for (int i=0; i<int(segments.size()-1); ++i) {
            Real start = segments[i];
            Real end = segments[i+1];
            cout << "\n**************************************************************************";
            cout << "\nSegment " << i << ": processing audio from " << start << "s to " << end << "s";
            cout << "\n**************************************************************************" << endl;
            
            // set segment name
            ostringstream ns;
            ns << "segment_" << i;
            string sn = ns.str();
            ns.str(""); ns << "segments." << sn << ".name";
            if (neqloud) neqloudPool.set(ns.str(), sn);
            if (eqloud) eqloudPool.set(ns.str(), sn);
            
            // set segment scope
            ns.str(""); ns << "segments." << sn << ".scope";
            vector<Real> scope(2, 0);
            scope[0] = start;
            scope[1] = end;
            if (neqloud) neqloudPool.set(ns.str(), scope);
            if (eqloud) eqloudPool.set(ns.str(), scope);
            
            // compute descriptors
            ns.str(""); ns << "segments.segment_" << i << ".descriptors";
            
            if (lowlevel)
                computeLowLevel(audioFilename, neqloudPool, eqloudPool, options, start, end, ns.str());
            // outdated beat tracker
            //if (beattrack) {
            //  if (neqloud) computeBeatTrack(neqloudPool, options);
            //  if (eqloud) computeBeatTrack(eqloudPool, options);
            //}
            if (midlevel)
                computeMidLevel(audioFilename, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (panning)
                computePanning(audioFilename, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (neqloud) computeHighlevel(neqloudPool, options);
            if (eqloud) computeHighlevel(eqloudPool, options);
        }
        cout << "\n**************************************************************************\n";
    }
    
    string baseFilename = outputFilename.substr(0, outputFilename.size()-4);
    string statsFilename = baseFilename + "_stats.xml";
    if (neqloud) {
        string neqOutputFilename = baseFilename + ".neq.sig";
        Pool stats = computeAggregation(neqloudPool, options, segments.size());
        //if (options.value<Real>("svm.compute") != 0) addSVMDescriptors(stats); //not available
        this->outputToFile(neqloudPool, neqOutputFilename, options);//CF added this for temporal features
        this->outputToFile(stats, statsFilename, options);
        neqloudPool.remove("metadata.audio_properties.downmix");
    }
    
    if (eqloud) {
        Pool stats = computeAggregation(eqloudPool, options, segments.size());
        if (options.value<Real>("svm.compute") != 0) addSVMDescriptors(stats);
        this->outputToFile(eqloudPool, outputFilename, options);//CF added this for temporal features
        this->outputToFile(stats, statsFilename, options);
        eqloudPool.remove("metadata.audio_properties.downmix");
    }
    return;
}


bool ACAudioEssentiaPlugin::addMediaTimedFeature(ACMediaTimedFeature* feature){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(feature->getName());
    if(mf!=descmf.end()){
        //std::cout << "ACAudioYaafeCorePlugin: appended feature: " << feature->getName() << " of length " << feature->getLength() << "/" << (*mf).second->getLength() << " and dim " << feature->getDim() << " vs " << (*mf).second->getDim() << " for file " << file << std::endl;
        return (*mf).second->appendTimedFeatureAlongTime(feature);
    }
    else{
        //std::cout << "ACAudioYaafeCorePlugin: new feature: " << feature->getName() << " of length " << feature->getLength() << " and dim " << feature->getDim() << " for file " << file << std::endl;
        descmf.insert( pair<std::string,ACMediaTimedFeature*>(feature->getName(),feature) );
        return true;
    }
}

bool ACAudioEssentiaPlugin::addMediaFeature(ACMediaFeatures* feature){
    std::vector<ACMediaFeatures*>::iterator mf = std::find(desc.begin(),desc.end(),feature);
    
    if(mf==desc.end()){
        desc.push_back(feature);
        return true;
    }
    return false;
}

void ACAudioEssentiaPlugin::outputToFile(Pool& pool, const string& outputFilename, const Pool& options) {
    
    cout << "Writing results to file " << outputFilename << endl;
    // some descriptors depend on lowlevel descriptors but it might be that the
    // config file was set lowlevel.compute: false. In this case, the ouput yaml
    // file should not contain lowlevel features. The rest of namespaces should
    // only be computed if they were set explicitly in the config file
    if (options.value<Real>("lowlevel.compute") == 0) pool.removeNamespace("lowlevel");
    
    // TODO: merge results pool with options pool so configuration is also
    // available in the output file
    mergeOptionsAndResults(pool, options);
    
    standard::Algorithm* output = standard::AlgorithmFactory::create("MtfOutput",
                                                                     "filename", outputFilename,
                                                                     "doubleCheck", true,
                                                                     "format", (options.value<Real>("outputJSON") != 0) ? "json" : "yaml");
    
    output->input("pool").set(pool);
    output->compute();
    
    ACAudioEssentiaMtfOutput* mtf_output = dynamic_cast<ACAudioEssentiaMtfOutput*>(output);
    if(mtf_output){
        std::map<std::string,ACMediaTimedFeature*> _descmf = mtf_output->getTimedFeatures();
        std::vector<ACMediaFeatures*> _desc = mtf_output->getFeatures();
        
        for(std::map<std::string,ACMediaTimedFeature*>::iterator mf = _descmf.begin();mf!=_descmf.end();mf++){
            this->addMediaTimedFeature(mf->second);
        }
        
        for(std::vector<ACMediaFeatures*>::iterator mf = _desc.begin(); mf!=_desc.end();mf++){
            this->addMediaFeature(*mf);
        }
        
        mtf_output->clearFeatures();
        mtf_output->clearTimedFeatures();
    }
    
    delete output;
}
