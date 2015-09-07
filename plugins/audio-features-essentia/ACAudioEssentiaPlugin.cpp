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
    return featureDimensions;
}

ACFeatureDimensions ACAudioEssentiaPlugin::getTimedFeaturesDimensions(){
    return timedFeatureDimensions;
}

void ACAudioEssentiaPlugin::clearFeatures(){
    /*std::map<std::string,ACMediaTimedFeature*>::iterator mf;
    for(mf=descmf.begin();mf!=descmf.end();mf++)
        //delete (*mf);*/
    descmf.clear();
}

void ACAudioEssentiaPlugin::clearTimedFeatures(){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf;
    for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++)
        (*mtf).second = 0;//delete (*mtf);
    descmtf.clear();
}

std::vector<ACMediaFeatures*> ACAudioEssentiaPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;
    this->clearFeatures();
    this->clearTimedFeatures();
    
    bool storeDimensions = false;
    if(featureDimensions.empty() && timedFeatureDimensions.empty()){
        storeDimensions = true;
        featureDimensions.clear();
        timedFeatureDimensions.clear();
    }
    
    _save_timed_feat = true; // forcing saving features so that we don't have to calculate them all the time
    
    // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
    bool save_binary = true;//CF was true
    string mtf_file_name; // file(s) in which feature(s) will be saved
    string mf_file_name; 
    string binary("_b");
    string mtf_file_ext =  ".mtf";
    string mf_file_ext =  ".mf";
    if(!save_binary)
        binary =  "_t";
    string aFileName = theMedia->getFileName();
    string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
    
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf;
    std::map<std::string,ACMediaFeatures*>::iterator mf;
    
    // Trying to open previously-computed files for parent files (TODO should test/compare parameters and plugin versions)
    bool featuresAvailable = false;
    //#ifdef USE_DEBUG
    _save_timed_feat = true; // forcing saving features so that we don't have to calculate them all the time
    if(!storeDimensions && theMedia->getParentId()==-1){
        featuresAvailable = true;
        
        // For each timed feature name, try to load it
        for(ACFeatureDimensions::iterator timedFeatureDim = timedFeatureDimensions.begin(); timedFeatureDim != timedFeatureDimensions.end();timedFeatureDim++){
            ACMediaTimedFeature* feature = 0;
            feature = new ACMediaTimedFeature();
            bool featureAvailable = false;
            std::string featureName((*timedFeatureDim).first);
            std::replace( featureName.begin(), featureName.end(), '_', ' ');
            std::string mtfFeatureName((*timedFeatureDim).first);
            std::replace( mtfFeatureName.begin(), mtfFeatureName.end(), ' ', '_');
            mtf_file_name = aFileName_noext + "_" + mtfFeatureName + binary+ mtf_file_ext;
            //std::cout << "ACAudioEssentiaPlugin: trying to load feature named '" << mtf_file_name << "'... " << std::endl;
            featureAvailable = feature->loadFromFile(mtf_file_name,save_binary);
            if(featureAvailable && feature){
                feature->setName(featureName);
                featureAvailable = this->addMediaTimedFeature(feature);
            }
            if(!featureAvailable){
                std::cout << "ACAudioEssentiaPlugin: feature named '" << mtf_file_name  << "' NOT loaded" << std::endl;
            }
            /*else{
                std::cout << "ACAudioEssentiaPlugin: feature named '" << mtf_file_name  << "' NOT loaded" << std::endl;
            }*/
            featuresAvailable *= featureAvailable;
        }
        if(descmtf.size()==0){
            featuresAvailable = false;
            std::cout << "ACAudioEssentiaPlugin: features weren't calculated previously" << std::endl;
        }
        else if(timedFeatureDimensions.size()==0){
            featuresAvailable = false;
            std::cerr << "ACAudioEssentiaPlugin: loaded features are empty" << std::endl;
        }
        else if(timedFeatureDimensions.size()!=descmtf.size()){
            featuresAvailable = false;
            std::cerr << "ACAudioEssentiaPlugin: some features weren't calculated previously" << std::endl;
        }
        if(!featuresAvailable){
            for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++)
                delete (*mtf).second;
            descmtf.clear();
            std::cerr << "ACAudioEssentiaPlugin: error while loading features, now recalculating them..." << std::endl;
        }
        
        // For each feature name, try to load it
        for(ACFeatureDimensions::iterator featureDim = featureDimensions.begin(); featureDim != featureDimensions.end();featureDim++){
            ACMediaFeatures* feature = 0;
            feature = new ACMediaFeatures();
            bool featureAvailable = false;
            std::string featureName((*featureDim).first);
            std::replace( featureName.begin(), featureName.end(), '_', ' ');
            std::string mfFeatureName((*featureDim).first);
            std::replace( mfFeatureName.begin(), mfFeatureName.end(), ' ', '_');
            mf_file_name = aFileName_noext + "_" + mfFeatureName + binary+ mf_file_ext;
            //std::cout << "ACAudioEssentiaPlugin: trying to load feature named '" << mf_file_name << "'... " << std::endl;
            featureAvailable = feature->loadFromFile(mf_file_name,save_binary);
            if(featureAvailable && feature){
                feature->setName(featureName);
                featureAvailable = this->addMediaFeature(feature);
            }
            if(!featureAvailable){
                std::cout << "ACAudioEssentiaPlugin: feature named '" << mf_file_name  << "' NOT loaded" << std::endl;
            }
            /*else{
                std::cout << "ACAudioEssentiaPlugin: feature named '" << mf_file_name  << "' loaded" << std::endl;
            }*/
            featuresAvailable *= featureAvailable;
        }
        if(descmf.size()==0){
            featuresAvailable = false;
            std::cout << "ACAudioEssentiaPlugin: features weren't calculated previously" << std::endl;
        }
        else if(featureDimensions.size()==0){
            featuresAvailable = false;
            std::cerr << "ACAudioEssentiaPlugin: loaded features are empty" << std::endl;
        }
        else if(featureDimensions.size()!=descmf.size()){
            featuresAvailable = false;
            std::cerr << "ACAudioEssentiaPlugin: some features weren't calculated previously" << std::endl;
        }
        if(!featuresAvailable){
            for(mf=descmf.begin();mf!=descmf.end();mf++)
                delete (*mf).second;
            descmf.clear();
            std::cerr << "ACAudioEssentiaPlugin: error while loading features, now recalculating them..." << std::endl;
        }
    }
    //#endif
    // CF TODO check if the features length matches the expected length based on the file length and block/step sizes and resample rate
    // CF TODO if some features weren't previously calcultated, only recalculate these
    
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
    
    bool computed = false;
    
    if(descmtf.size()==0 && descmf.size()==0){
        
        try {
            
            compute(aFileName, yaml_file_name, neqloudPool, eqloudPool, options);
            computed = true;
            
        }
        catch (EssentiaException& e) {
            cout << e.what() << endl;
            //throw;
            computed = false;
        }
    }
    else{
        bool reloading = true;
        std::cout << reloading << std::endl; 
    }
    
    for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++){
        std::string featureName((*mtf).second->getName());
        std::replace( featureName.begin(), featureName.end(), ' ', '_');
        mtf_file_name = aFileName_noext + "_" + featureName   + binary+ mtf_file_ext;;
        
        /// Save each timed feature in a *.mtf file if it has just been computed (not if loaded)
        if(computed){
            bool saved = (*mtf).second->saveInFile(mtf_file_name, save_binary);
            if(!saved)
                std::cerr << "ACAudioEssentiaPlugin: couldn't save feature named '" << mtf_file_name << "'" << std::endl;
        }
        
        if(storeDimensions){
            std::replace( featureName.begin(), featureName.end(), '_', ' ');
            timedFeatureDimensions[ featureName ] = (*mtf).second->getDim();
        }
        
        /// Inform the media of its available timed features
        theMedia->addTimedFileNames(mtf_file_name);
    }
    
    for(mf=descmf.begin();mf!=descmf.end();mf++){
        std::string featureName((*mf).second->getName());
        std::replace( featureName.begin(), featureName.end(), ' ', '_');
        std::string mf_file_name = aFileName_noext + "_" + featureName  + binary+ mf_file_ext;
        
        /// Save each feature in a *.mf file if it has just been computed (not if loaded)
        if(computed){
            bool saved = (*mf).second->saveInFile(mf_file_name, save_binary);
            if(!saved)
                std::cerr << "ACAudioEssentiaPlugin: couldn't save feature named '" << mf_file_name << "'" << std::endl;
        }
        
        if(storeDimensions){
            std::replace( featureName.begin(), featureName.end(), '_', ' ');
            featureDimensions[ featureName ] = (*mf).second->getSize();
        }
        
        /// Build a list of features extracted
        desc.push_back((*mf).second);
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
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf = descmtf.find(feature->getName());
    if(mtf!=descmtf.end()){
        //std::cout << "ACAudioEssentiaPlugin: appended feature: " << feature->getName() << " of length " << feature->getLength() << "/" << (*mf).second->getLength() << " and dim " << feature->getDim() << " vs " << (*mf).second->getDim() << " for file " << file << std::endl;
        return (*mtf).second->appendTimedFeatureAlongTime(feature);
    }
    else{
        //std::cout << "ACAudioEssentiaPlugin: new feature: " << feature->getName() << " of length " << feature->getLength() << " and dim " << feature->getDim() << " for file " << file << std::endl;
        descmtf.insert( pair<std::string,ACMediaTimedFeature*>(feature->getName(),feature) );
        return true;
    }
}

bool ACAudioEssentiaPlugin::addMediaFeature(ACMediaFeatures* feature){
    descmf.insert( pair<std::string,ACMediaFeatures*>(feature->getName(),feature) );
    return true;
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
        std::map<std::string,ACMediaTimedFeature*> _descmtf = mtf_output->getTimedFeatures();
        std::map<std::string,ACMediaFeatures*> _descmf = mtf_output->getFeatures();
        
        for(std::map<std::string,ACMediaTimedFeature*>::iterator mtf = _descmtf.begin();mtf!=_descmtf.end();mtf++){
            this->addMediaTimedFeature(mtf->second);
        }
        
        for(std::map<std::string,ACMediaFeatures*>::iterator mf = _descmf.begin(); mf!=_descmf.end();mf++){
            this->addMediaFeature(mf->second);
        }
        
        mtf_output->clearFeatures();
        mtf_output->clearTimedFeatures();
    }
    
    delete output;
}
