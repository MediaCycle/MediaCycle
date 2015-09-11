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

#include "ACAudioYaafeCorePlugin.h"
#include "ACAudio.h"
#include <vector>
#include <string>

using namespace YAAFE;
using namespace std;

#include <yaafe-io/io/FileUtils.h>
#include <sstream>

#include "ACYaafeOutputFormat.h"
#include "ACYaafeWriter.h"

#include "yaafe-io/io/CSVWriter.h"
//#ifdef WITH_SNDFILE
#include "yaafe-io/io/AudioFileReader.h"
//#endif
//#ifdef WITH_MPG123
#include "yaafe-io/io/MP3FileReader.h"
//#endif
#ifdef WITH_HDF5
#include "yaafe-io/io/H5DatasetWriter.h"
#endif

#include "yaafe-components/audio/Abs.h"
#include "yaafe-components/audio/AC2LPC.h"
#include "yaafe-components/audio/AmplitudeModulation.h"
#include "yaafe-components/audio/AutoCorrelation.h"
#include "yaafe-components/audio/AutoCorrelationPeaksIntegrator.h"
#include "yaafe-components/audio/Cepstrum.h"
#include "yaafe-components/audio/ComplexDomainFlux.h"
#include "yaafe-components/audio/Decrease.h"
#include "yaafe-components/audio/Derivate.h"
#include "yaafe-components/audio/Difference.h"
#include "yaafe-components/audio/Envelope.h"
#include "yaafe-components/audio/FFT.h"
#include "yaafe-components/audio/FilterSmallValues.h"
#include "yaafe-components/audio/Flatness.h"
#include "yaafe-components/audio/Flux.h"
#include "yaafe-components/audio/FrameTokenizer.h"
#include "yaafe-components/audio/HalfHannFilter.h"
#include "yaafe-components/audio/HistogramIntegrator.h"
#ifdef WITH_LAPACK
#include "yaafe-components/audio/LPC2LSF.h"
#endif
#include "yaafe-components/audio/SpecificLoudness.h"
#include "yaafe-components/audio/LoudnessSharpness.h"
#include "yaafe-components/audio/LoudnessSpread.h"
#include "yaafe-components/audio/MedianFilter.h"
#include "yaafe-components/audio/MelFilterBank.h"
#include "yaafe-components/audio/Normalize.h"
#include "yaafe-components/audio/OBSI.h"
#include "yaafe-components/audio/RMS.h"
#include "yaafe-components/audio/Rolloff.h"
#include "yaafe-components/audio/ShapeStatistics.h"
#include "yaafe-components/audio/Slope.h"
#include "yaafe-components/audio/SlopeIntegrator.h"
#include "yaafe-components/audio/SpectralCrestFactorPerBand.h"
#include "yaafe-components/audio/SpectralFlatnessPerBand.h"
#include "yaafe-components/audio/Sqr.h"
#include "yaafe-components/audio/StatisticalIntegrator.h"
#include "yaafe-components/audio/Sum.h"
#include "yaafe-components/audio/Variation.h"
#include "yaafe-components/audio/ZCR.h"

#include "yaafe-components/flow/Join.h"

#include "cba-yaafe-extension/audio/AccumulateSameValues.h"
#include "cba-yaafe-extension/audio/Binarization.h"
#include "cba-yaafe-extension/audio/DBConversion.h"
#include "cba-yaafe-extension/audio/DCOffsetFilter.h"
#include "cba-yaafe-extension/audio/DilationFilter.h"
#include "cba-yaafe-extension/audio/ErosionFilter.h"
#include "cba-yaafe-extension/audio/FrameSum.h"
#include "cba-yaafe-extension/audio/MathUtils.h"
#include "cba-yaafe-extension/audio/PeakDetection.h"
#include "cba-yaafe-extension/audio/SimpleNoiseGate.h"
#include "cba-yaafe-extension/audio/SimpleThresholdClassification.h"
#include "cba-yaafe-extension/audio/SubRunningAverage.h"
#include "cba-yaafe-extension/audio/WindowConvolution.h"
#include "cba-yaafe-extension/audio/WindowNormalize.h"

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACAudioYaafeCorePlugin::ACAudioYaafeCorePlugin() : ACTimedFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Yaafe Features";
    this->mDescription = "Audio feature extraction plugin using the Yaafe library";
    this->mId = "";
    //this->mtf_file_name = "";
    
    this->addNumberParameter("loadTimeFeatures",1,0,1,1,"load time features");
    
    dataflowLoaded = false;
    factoriesRegistered = false;
    
    //DataBlock::setPreferedBlockSize(1024);
    
    m_default_resample_rate = 22050;
    m_default_step_size = 256;
    m_default_block_size = 512;
    
    // Register the component and output format to use yaafe as MC plugin
    OutputFormat::registerFormat(new ACYaafeOutputFormat(this));
    ComponentFactory::instance()->registerPrototype(new ACYaafeWriter());
}

ACAudioYaafeCorePlugin::~ACAudioYaafeCorePlugin() {
    //ComponentFactory::destroy();
}

bool ACAudioYaafeCorePlugin::registerFactories(){
    
    // Register yaafe-io components
    ComponentFactory::instance()->registerPrototype(new CSVWriter());
    //#ifdef USE_SNDFILE
    ComponentFactory::instance()->registerPrototype(new AudioFileReader());
    //#endif
    //#ifdef WITH_MPG123
    ComponentFactory::instance()->registerPrototype(new MP3FileReader());
    //#endif
#ifdef WITH_HDF5
    ComponentFactory::instance()->registerPrototype(new H5DatasetWriter());
#endif
    
    // Register yaafe-components components:
    ComponentFactory::instance()->registerPrototype(new AC2LPC());
    ComponentFactory::instance()->registerPrototype(new Abs());
    ComponentFactory::instance()->registerPrototype(new AmplitudeModulation());
    ComponentFactory::instance()->registerPrototype(new AutoCorrelation());
    ComponentFactory::instance()->registerPrototype(new AutoCorrelationPeaksIntegrator());
    ComponentFactory::instance()->registerPrototype(new Cepstrum());
    ComponentFactory::instance()->registerPrototype(new ComplexDomainFlux());
    ComponentFactory::instance()->registerPrototype(new Decrease());
    ComponentFactory::instance()->registerPrototype(new Derivate());
    ComponentFactory::instance()->registerPrototype(new Difference());
    ComponentFactory::instance()->registerPrototype(new Envelope());
    ComponentFactory::instance()->registerPrototype(new FFT());
    ComponentFactory::instance()->registerPrototype(new FilterSmallValues());
    ComponentFactory::instance()->registerPrototype(new Flatness());
    ComponentFactory::instance()->registerPrototype(new Flux());
    ComponentFactory::instance()->registerPrototype(new FrameTokenizer());
    ComponentFactory::instance()->registerPrototype(new HalfHannFilter());
    ComponentFactory::instance()->registerPrototype(new HistogramIntegrator());
#ifdef WITH_LAPACK
    ComponentFactory::instance()->registerPrototype(new LPC2LSF());
#endif
    ComponentFactory::instance()->registerPrototype(new SpecificLoudness());
    ComponentFactory::instance()->registerPrototype(new LoudnessSharpness());
    ComponentFactory::instance()->registerPrototype(new LoudnessSpread());
    ComponentFactory::instance()->registerPrototype(new MedianFilter());
    ComponentFactory::instance()->registerPrototype(new MelFilterBank());
    ComponentFactory::instance()->registerPrototype(new Normalize());
    ComponentFactory::instance()->registerPrototype(new OBSI());
    ComponentFactory::instance()->registerPrototype(new RMS());
    ComponentFactory::instance()->registerPrototype(new Rolloff());
    ComponentFactory::instance()->registerPrototype(new ShapeStatistics());
    ComponentFactory::instance()->registerPrototype(new Slope());
    ComponentFactory::instance()->registerPrototype(new SlopeIntegrator());
    ComponentFactory::instance()->registerPrototype(new Sqr());
    ComponentFactory::instance()->registerPrototype(new SpectralCrestFactorPerBand());
    ComponentFactory::instance()->registerPrototype(new SpectralFlatnessPerBand());
    ComponentFactory::instance()->registerPrototype(new StatisticalIntegrator());
    ComponentFactory::instance()->registerPrototype(new Sum());
    ComponentFactory::instance()->registerPrototype(new Variation());
    ComponentFactory::instance()->registerPrototype(new ZCR());
    ComponentFactory::instance()->registerPrototype(new Join());
    
    // Alternate version if yaafe-io and yaafe-components are built as modules
    /*setenv("YAAFbuild_path","/Volumes/data/Builds/numediart/audiocycle-osx10.6.8-qtcreator/3rdparty/yaafe/src_cpp/yaafe-io",1);
    int yaafe_io = 0;
    yaafe_io = ComponentFactory::instance()->loadLibrary("yaafe-io");
    if (yaafe_io)
        std::cerr << "Couldn't load yaafe-io" << std::endl;
        
    setenv("YAAFbuild_path","/Volumes/data/Builds/numediart/audiocycle-osx10.6.8-qtcreator/3rdparty/yaafe/src_cpp/yaafe-components",1);
    // The following is not necessary if the following line is on the dataflow file: useComponentLibrary yaafe-components
    //int yaafe_comp = 0;
    //yaafe_comp = ComponentFactory::instance()->loadLibrary("yaafe-components");
    //if (yaafe_comp)
    //    std::cerr << "Couldn't load yaafe_components" << std::endl;*/
    
    ComponentFactory::instance()->registerPrototype(new AccumulateSameValues());
    ComponentFactory::instance()->registerPrototype(new Binarization());
    ComponentFactory::instance()->registerPrototype(new DBConversion());
    ComponentFactory::instance()->registerPrototype(new DCOffsetFilter());
    ComponentFactory::instance()->registerPrototype(new DilationFilter());
    ComponentFactory::instance()->registerPrototype(new ErosionFilter());
    ComponentFactory::instance()->registerPrototype(new FrameSum());
    ComponentFactory::instance()->registerPrototype(new PeakDetection());
    ComponentFactory::instance()->registerPrototype(new SimpleNoiseGate());
    ComponentFactory::instance()->registerPrototype(new SimpleThresholdClassification());
    ComponentFactory::instance()->registerPrototype(new SubRunningAverage());
    ComponentFactory::instance()->registerPrototype(new WindowConvolution());
    ComponentFactory::instance()->registerPrototype(new WindowNormalize());
    
    return true;
}

void ACAudioYaafeCorePlugin::loadDataflow(){
    
    // Check the list of loaded components
    //listComponents();
    
    std::string dataflow_file = "";
#ifdef USE_DEBUG
    boost::filesystem::path source_path( __FILE__ );
    ///dataflow_file += s_path.parent_path().string() + "/" + this->dataflowFilename();
    std::string library_path("");
    if(media_cycle)
        library_path = media_cycle->getLibraryPathFromPlugin(this->mName);
    if(library_path == ""){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: couldn't get the library path" << std::endl;
        return;
    }
    boost::filesystem::path b_path( library_path );
#ifdef XCODE
    // Xcode adds Debug folders to each target
    dataflow_file = source_path.parent_path().parent_path().parent_path().string() + "/" + b_path.parent_path().parent_path().stem().string() + "/" + b_path.parent_path().stem().string() + "/" + this->dataflowFilename();
#else
    dataflow_file = source_path.parent_path().parent_path().parent_path().string() + "/" + b_path.parent_path().stem().string() + "/" + b_path.stem().string() + "/" + this->dataflowFilename();
#endif
    
    string slash = "/";
#ifdef __WIN32__
    slash = "\\";
    
    if(!media_cycle){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: mediacycle instance not set, will try reload the dataflow later" << std::endl;
        return;
    }
    std::string _path = this->media_cycle->getLibraryPathFromPlugin(this->mName);
    if(_path==""){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: plugin path not available" << std::endl;
        return;
    }
    
    boost::filesystem::path dataflow_path ( _path + slash + this->dataflowFilename());
    dataflow_file = dataflow_path.string();
    
#endif
#else
#ifdef __APPLE__
    dataflow_file = getExecutablePath() + "/" + this->dataflowFilename();
#elif __WIN32__
    dataflow_file = this->dataflowFilename();
#else
    dataflow_file = "/usr/share/mediacycle/plugins/audio/" + this->dataflowFilename();
#endif
#endif
    DataFlow df;
    if (!df.load(dataflow_file)) {
        cerr << "ACAudioYaafeCorePlugin::loadDataflow: cannot load dataflow from file " << dataflow_file << endl;
        return;
    }
    cout << "ACAudioYaafeCorePlugin::loadDataflow: loaded dataflow from file " << dataflow_file << endl;
    //df.display();
    
    //Engine engine;
    if (!engine.load(df)) {
        cerr << "ACAudioYaafeCorePlugin::loadDataflow: cannot initialize dataflow engine from file " << dataflow_file << endl;
        return;
    }
    cout << "ACAudioYaafeCorePlugin::loadDataflow: initialized dataflow engine from file " << dataflow_file << endl;
    
    /* std::cout << "ACAudioYaafeCorePlugin: inputs" << std::endl;
    std::vector<std::string> inputs = engine.getInputs();
    for (std::vector<std::string>::iterator input = inputs.begin();input!=inputs.end();input++){
        std::cout << "Input: " << (*input) << std::endl;
        ParameterMap inputparams = engine.getInputParams(*input);
        //for (ParameterMap::iterator inputparam = inputparams.begin();inputparam!=inputparams.end();inputparam++){
        //    std::cout << "Input Param: '" << inputparam->first << "' '" << inputparam->second << "'" << std::endl;
        //}
        ParameterMap::iterator inputparam = inputparams.find("SampleRate");
        if (inputparam != inputparams.end())
            std::cout << "SampleRate: '" << inputparam->second << "'" << std::endl;
    }*/
    
    dataflowLoaded = true;
    
    featureDimensions.clear();
    timedFeatureDimensions.clear();
    
    if(!factoriesRegistered)
        this->registerFactories();
    if(!factoriesRegistered)
        return;
}

ACFeatureDimensions ACAudioYaafeCorePlugin::getFeaturesDimensions(){
    ACFeatureDimensions featdims;
    
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered){
            std::cerr << "ACAudioYaafeCorePlugin::getFeaturesDimensions: couldn't register factories " << std::endl;
            return featdims;
        }
    }
    
    if(!dataflowLoaded){
        this->loadDataflow();
        if(!dataflowLoaded){
            std::cerr << "ACAudioYaafeCorePlugin::getFeaturesDimensions: couldn't load dataflow " << this->dataflowFilename() << std::endl;
            return featdims;
        }
    }
    
    //std::cout << "ACAudioYaafeCorePlugin: outputs" << std::endl;
    std::vector<std::string> outputs = engine.getOutputs();
    for (std::vector<std::string>::iterator output = outputs.begin();output!=outputs.end();output++){
        //std::cout << "Output: " << (*output) << std::endl;
        ParameterMap outputparams = engine.getOutputParams(*output);
        /*for(ParameterMap::iterator outputparam = outputparams.begin();outputparam!=outputparams.end();outputparam++){
            std::cout << "ACAudioYaafeCorePlugin: " << *output << ": '" << outputparam->first << "' '" << outputparam->second << "'" << std::endl;
        }*/
        
        std::string name = (*output);
        
        int dim = 0;
        std::string dimensions = outputparams["dimensions"];
        if(dimensions == ""){
            std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: dataflow malformed, feature " << name << " dimensions are not set " << std::endl;
            featdims.clear();
            return featdims;
        }
        else{
            std::stringstream _dims;
            _dims << dimensions;
            _dims >> dim;
        }
        
        if(dim<=0){
            std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: dataflow malformed, feature " << name << " dimension is null " << std::endl;
            featdims.clear();
            return featdims;
        }
        
        if(std::find(timedFeatureNames.begin(),timedFeatureNames.end(),name) == timedFeatureNames.end())
            timedFeatureNames.push_back(name);
        
        size_t underscore = 0;
        while(underscore != std::string::npos){
            underscore = name.find_first_of("_",underscore);
            if(underscore != std::string::npos){
                name = name.replace(underscore,1," ");
            }
        }
        
        if(std::find(readableTimedFeatureNames.begin(),readableTimedFeatureNames.end(),name) == readableTimedFeatureNames.end())
            readableTimedFeatureNames.push_back(name);
        
        //std::cout << "ACAudioYaafeCorePlugin: adding descriptor: '" << name << "'" << std::endl;
        //std::cout << "ACAudioYaafeCorePlugin: feature " << *output << " has a dimension of " << dim << std::endl;
        
        
        std::string statistics = outputparams["statistics"];
        if(statistics == ""){
            std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: dataflow malformed, feature " << name << " statistics are not set " << std::endl;
            featdims.clear();
            return featdims;
        }
        
        if(statistics.find("min")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_MIN;
            featdims[name+" Min"] = dim;
        }
        if(statistics.find("mean")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_MEAN;
            featdims[name+" Mean"] = dim;
        }
        if(statistics.find("max")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_MAX;
            featdims[name+" Max"] = dim;
        }
        if(statistics.find("centroid")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_CENTROID;
            featdims[name+" Centroid"] = dim;
        }
        if(statistics.find("spread")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_SPREAD;
            featdims[name+" Spread"] = dim;
        }
        if(statistics.find("skewness")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_SKEWNESS;
            featdims[name+" Skewness"] = dim;
        }
        if(statistics.find("kurtosis")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_KURTOSIS;
            featdims[name+" Kurtosis"] = dim;
        }
        if(statistics.find("cov")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_COV;
            featdims[name+" Covariance"] = dim;
        }
        if(statistics.find("corr")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_CORR;
            featdims[name+" Corr"] = dim;
        }
        if(statistics.find("modulation")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_MODULATION;
            featdims[name+" Modulation"] = dim;
        }
        if(statistics.find("logcentr")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_LOG_CENTROID;
            featdims[name+" Log Centroid"] = dim;
        }
        if(statistics.find("logspr")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_LOG_SPREAD;
            featdims[name+" Log Spread"] = dim;
        }
        if(statistics.find("logskew")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_LOG_SKEWNESS;
            featdims[name+" Log Skewness"] = dim;
        }
        if(statistics.find("logkurto")!=std::string::npos){
            featuresStats[name] |= STAT_TYPE_LOG_KURTOSIS;
            featdims[name+" Log Kurtosis"] = dim;
        }
        
        /*std::string fname = (*output);
        size_t funderscore = 0;
        while(funderscore != std::string::npos){
            funderscore = fname.find_first_of("_",funderscore);
            if(funderscore != std::string::npos){
                fname = fname.erase(funderscore,1);
            }
        }
        std::cout << "Yaafe name: '" << fname << "'" << std::endl;
        
        if(ComponentFactory::instance()->exists(*output)){
            ParameterDescriptorList defaultparams = ComponentFactory::instance()->getPrototype(*output)->getParameterDescriptorList();
            for(ParameterDescriptorList::iterator defaultparam=defaultparams.begin();defaultparam!=defaultparams.end();defaultparam++)
                std::cout << "Default parameter '" << (*defaultparam).m_identifier << "' value " << (*defaultparam).m_defaultValue << "'" << std::endl;
            //std::vector<ParameterDescriptor> ParameterDescriptorList
        }
        
        //for (ParameterMap::iterator outputparam = outputparams.begin();outputparam!=outputparams.end();outputparam++){
        //    std::cout << "Output Param: '" << outputparam->first << "' '" << outputparam->second << "'" << std::endl;
        //}
        ParameterMap::iterator outputparam = outputparams.find("yaafedefinition");
        if (outputparam != outputparams.end()){
            //std::cout << "yaafedefinition '" << outputparam->second << "'" << std::endl;
            size_t name = outputparam->second.find(*output);
            if(name != std::string::npos){
                std::string params = outputparam->second.substr(name + (*output).size()+1 );
                //std::cout << "yaafedefinition '" << params << "'" << std::endl;
                std::string blockSizeName("blockSize=");
                size_t blockSizePos = params.find(blockSizeName);
                if(blockSizePos != std::string::npos){
                    size_t blockSizeEnd = params.find_first_of(" ",blockSizePos);
                    std::cout << "blockSize '" << params.substr(blockSizePos + blockSizeName.size(), blockSizeEnd - blockSizePos - blockSizeName.size()) << "'" << std::endl;
                }
                std::string stepSizeName("stepSize=");
                size_t stepSizePos = params.find(stepSizeName);
                if(stepSizePos != std::string::npos){
                    size_t stepSizeEnd = params.find_first_of(" ",stepSizePos);
                    std::cout << "stepSize '" << params.substr(stepSizePos + stepSizeName.size(), stepSizeEnd - stepSizePos - stepSizeName.size()) << "'" << std::endl;
                }
            }
        }*/
    }
    featureDimensions = featdims;
    return featdims;
}

ACFeatureDimensions ACAudioYaafeCorePlugin::getTimedFeaturesDimensions(){
    ACFeatureDimensions featdims;
    
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered){
            std::cerr << "ACAudioYaafeCorePlugin::getFeaturesDimensions: couldn't register factories " << std::endl;
            return featdims;
        }
    }
    
    if(!dataflowLoaded){
        this->loadDataflow();
        if(!dataflowLoaded){
            std::cerr << "ACAudioYaafeCorePlugin::getFeaturesDimensions: couldn't load dataflow " << this->dataflowFilename() << std::endl;
            return featdims;
        }
    }
    
    //std::cout << "ACAudioYaafeCorePlugin: outputs" << std::endl;
    std::vector<std::string> outputs = engine.getOutputs();
    for (std::vector<std::string>::iterator output = outputs.begin();output!=outputs.end();output++){
        //std::cout << "Output: " << (*output) << std::endl;
        ParameterMap outputparams = engine.getOutputParams(*output);
        /*for(ParameterMap::iterator outputparam = outputparams.begin();outputparam!=outputparams.end();outputparam++){
            std::cout << "ACAudioYaafeCorePlugin: " << *output << ": '" << outputparam->first << "' '" << outputparam->second << "'" << std::endl;
        }*/
        
        std::string name = (*output);
        
        int dim = 0;
        std::string dimensions = outputparams["dimensions"];
        if(dimensions == ""){
            std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: dataflow malformed, feature " << name << " dimensions are not set " << std::endl;
            featdims.clear();
            return featdims;
        }
        else{
            std::stringstream _dims;
            _dims << dimensions;
            _dims >> dim;
        }
        
        if(dim<=0){
            std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: dataflow malformed, feature " << name << " dimension is null " << std::endl;
            featdims.clear();
            return featdims;
        }
        
        if(std::find(timedFeatureNames.begin(),timedFeatureNames.end(),name) == timedFeatureNames.end())
            timedFeatureNames.push_back(name);
        
        size_t underscore = 0;
        while(underscore != std::string::npos){
            underscore = name.find_first_of("_",underscore);
            if(underscore != std::string::npos){
                name = name.replace(underscore,1," ");
            }
        }
        
        if(std::find(readableTimedFeatureNames.begin(),readableTimedFeatureNames.end(),name) == readableTimedFeatureNames.end())
            readableTimedFeatureNames.push_back(name);
        
        //std::cout << "ACAudioYaafeCorePlugin: adding descriptor: '" << name << "'" << std::endl;
        //std::cout << "ACAudioYaafeCorePlugin: feature " << *output << " has a dimension of " << dim << std::endl;
        
        
        
        featdims[name] = dim;
    }
    timedFeatureDimensions = featdims;
    return featdims;
}

void ACAudioYaafeCorePlugin::clearFeatures(){
    /*std::map<std::string,ACMediaTimedFeature*>::iterator mf;
    for(mf=descmf.begin();mf!=descmf.end();mf++)
        //delete (*mf);*/
    descmf.clear();
}

void ACAudioYaafeCorePlugin::clearTimedFeatures(){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf;
    for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++)
        (*mtf).second = 0;//delete (*mtf);
    descmtf.clear();
}

void ACAudioYaafeCorePlugin::listComponents()
{
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered)
            return;
    }
    
    vector<string> components;
    const vector<const Component*>& cList = ComponentFactory::instance()->getPrototypeList();
    for (size_t i = 0; i < cList.size(); i++)
    {
        components.push_back(cList[i]->getIdentifier());
    }
    sort(components.begin(),components.end());
    cout << "Available components : " << endl;
    for (vector<string>::const_iterator it = components.begin(); it
         != components.end(); it++)
        cout << " - " << *it << endl;
}

void ACAudioYaafeCorePlugin::describeComponent(const std::string component)
{
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered)
            return;
    }
    
    const Component* c = ComponentFactory::instance()->getPrototype(component);
    if (c)
    {
        cout << c->getIdentifier() << " : ";
        if (c->getDescription() != "")
            cout << c->getDescription();
        cout << endl;
        const ParameterDescriptorList& params = c->getParameterDescriptorList();
        for (ParameterDescriptorList::const_iterator it = params.begin(); it
             != params.end(); it++)
        {
            cout << " - " << it->m_identifier << " : " << it->m_description;
            cout << " (default=" << it->m_defaultValue << ")" << endl;
        }
        return;
    }
}

void ACAudioYaafeCorePlugin::printOutputFormats()
{
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered)
            return;
    }
    
    vector<string> formats = OutputFormat::availableFormats();
    printf("Available output formats are:\n");
    for (int i=0;i<formats.size();i++)
    {
        const OutputFormat* f = OutputFormat::get(formats[i]);
        printf("[%s] %s\n",formats[i].c_str(),f->getDescription().c_str());
        printf("     Parameters:\n");
        ParameterDescriptorList pList = f->getParameters();
        for (int p=0;p<pList.size();p++) {
            printf("     - %s: %s (default=%s)\n", pList[p].m_identifier.c_str(),
                   pList[p].m_description.c_str(), pList[p].m_defaultValue.c_str());
        }
        printf("\n");
    }
}

bool ACAudioYaafeCorePlugin::isMediaTimedFeatureStored(std::string name, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf = descmtf.find(name);
    if(mtf!=descmtf.end()){
        return true;
    }
    else{
        return false;
    }
}

ACMediaTimedFeature* ACAudioYaafeCorePlugin::getMediaTimedFeatureStored(std::string name, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf = descmtf.find(name);
    if(mtf!=descmtf.end()){
        return (*mtf).second;
    }
    else{
        return 0;
    }
}


bool ACAudioYaafeCorePlugin::addMediaTimedFeature(ACMediaTimedFeature* feature){
    std::map<std::string,ACMediaTimedFeature*>::iterator mtf = descmtf.find(feature->getName());
    if(mtf!=descmtf.end()){
        //std::cout << "ACAudioYaafeCorePlugin: appended feature: " << feature->getName() << " of length " << feature->getLength() << "/" << (*mf).second->getLength() << " and dim " << feature->getDim() << " vs " << (*mf).second->getDim() << " for file " << file << std::endl;
        return (*mtf).second->appendTimedFeatureAlongTime(feature);
    }
    else{
        //std::cout << "ACAudioYaafeCorePlugin: new feature: " << feature->getName() << " of length " << feature->getLength() << " and dim " << feature->getDim() << " for file " << file << std::endl;
        descmtf.insert( pair<std::string,ACMediaTimedFeature*>(feature->getName(),feature) );
        return true;
    }
}

bool ACAudioYaafeCorePlugin::addMediaFeature(ACMediaFeatures* feature){
    descmf.insert( pair<std::string,ACMediaFeatures*>(feature->getName(),feature) );
    return true;
}

std::vector<ACMediaFeatures*> ACAudioYaafeCorePlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;
    this->clearFeatures();
    this->clearTimedFeatures();
    
    bool loadTimeFeatures = this->getNumberParameterValue("loadTimeFeatures");
    
    if(!factoriesRegistered){
        factoriesRegistered = this->registerFactories();
        if(!factoriesRegistered){
            std::cerr << "ACAudioYaafeCorePlugin::calculate: couldn't register factories " << std::endl;
            return desc;
        }
    }
    
    if(!dataflowLoaded){
        this->loadDataflow();
        if(!dataflowLoaded){
            std::cerr << "ACAudioYaafeCorePlugin::calculate: couldn't load dataflow " << this->dataflowFilename() << std::endl;
            return desc;
        }
    }
    
    if( featureDimensions.size()==0){
        featureDimensions = this->getFeaturesDimensions();
        if( featureDimensions.size()==0){
            std::cerr << "ACAudioYaafeCorePlugin: couldn't list feature dimensions, can't calcutate features" << std::endl;
            return desc;
        }
    }
    
    if( timedFeatureDimensions.size()==0){
        timedFeatureDimensions = this->getTimedFeaturesDimensions();
        if( timedFeatureDimensions.size()==0){
            std::cerr << "ACAudioYaafeCorePlugin: couldn't list timed feature dimensions, can't calcutate features" << std::endl;
            return desc;
        }
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
    if(theMedia->getParentId()==-1){
        featuresAvailable = true;
        
        if(loadTimeFeatures){
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
                //std::cout << "ACAudioYaafeCorePlugin: trying to load feature named '" << mtf_file_name << "'... " << std::endl;
                featureAvailable = feature->loadFromFile(mtf_file_name,save_binary);
                if(featureAvailable && feature){
                    feature->setName(featureName);
                    featureAvailable = this->addMediaTimedFeature(feature);
                }
                if(!featureAvailable){
                    std::cout << "ACAudioYaafeCorePlugin: feature named '" << mtf_file_name  << "' NOT loaded" << std::endl;
                }
                /*else{
                std::cout << "ACAudioYaafeCorePlugin: feature named '" << mtf_file_name  << "' NOT loaded" << std::endl;
            }*/
                featuresAvailable *= featureAvailable;
            }
            if(descmtf.size()==0){
                featuresAvailable = false;
                std::cout << "ACAudioYaafeCorePlugin: features weren't calculated previously" << std::endl;
            }
            else if(timedFeatureDimensions.size()==0){
                featuresAvailable = false;
                std::cerr << "ACAudioYaafeCorePlugin: loaded features are empty" << std::endl;
            }
            else if(timedFeatureDimensions.size()!=descmtf.size()){
                featuresAvailable = false;
                std::cerr << "ACAudioYaafeCorePlugin: some features weren't calculated previously" << std::endl;
            }
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
            //std::cout << "ACAudioYaafeCorePlugin: trying to load feature named '" << mf_file_name << "'... " << std::endl;
            featureAvailable = feature->loadFromFile(mf_file_name,save_binary);
            if(featureAvailable && feature){
                feature->setName(featureName);
                featureAvailable = this->addMediaFeature(feature);
            }
            if(!featureAvailable){
                std::cout << "ACAudioYaafeCorePlugin: feature named '" << mf_file_name  << "' NOT loaded" << std::endl;
            }
            /*else{
                std::cout << "ACAudioYaafeCorePlugin: feature named '" << mf_file_name  << "' loaded" << std::endl;
            }*/
            featuresAvailable *= featureAvailable;
        }
        if(descmf.size()==0){
            featuresAvailable = false;
            std::cout << "ACAudioYaafeCorePlugin: features weren't calculated previously" << std::endl;
        }
        else if(featureDimensions.size()==0){
            featuresAvailable = false;
            std::cerr << "ACAudioYaafeCorePlugin: loaded features are empty" << std::endl;
        }
        else if(featureDimensions.size()!=descmf.size()){
            featuresAvailable = false;
            std::cerr << "ACAudioYaafeCorePlugin: some features weren't calculated previously" << std::endl;
        }
    }
    
    if(!featuresAvailable){
        for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++)
            delete (*mtf).second;
        descmtf.clear();
        
        for(mf=descmf.begin();mf!=descmf.end();mf++)
            delete (*mf).second;
        descmf.clear();
        std::cerr << "ACAudioYaafeCorePlugin: error while loading features, now recalculating them..." << std::endl;
    }
    
    //#endif
    // CF TODO check if the features length matches the expected length based on the file length and block/step sizes and resample rate
    // CF TODO if some features weren't previously calcultated, only recalculate these
    
    // Compute the features if no mtf file is found
    bool computed = false;
    if(descmtf.size()==0 && descmf.size()==0){
        
        //AudioFileProcessor processor;
        {
            string formatStr = "MC";
            string outDirStr = ".";
            ParameterMap params;
            if (!processor.setOutputFormat(formatStr,outDirStr,params)) {
                std::cerr << "ACAudioYaafeCorePlugin::calculate: couldn't set output format" << std::endl;
                return desc;
            }
        }
        
        int res = -1;
        try{
            res = processor.processFile(engine, theMedia->getFileName(), theMedia->getStart(), theMedia->getEnd() );//files->filename[i]);
        }
        catch(...){
            res = -1;
        }
        if (res!=0) {
            cerr << "ERROR: error while processing " << theMedia->getFileName() << endl;//files->filename[i] << endl;
        }
        else{
            computed = true;
        }
        
        
        // find the index of the feature named "Energy"
        /*int nrgIdx = 0;
        for (int i=0; i<descmtf.size(); i++){
            if (descmtf[i]->getName() == "Energy")
                nrgIdx = i;
        }*/
        // the feature named "Energy" does not need to be normalized
        for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++){
            ACStatType mtfstats = featuresStats[(*mtf).second->getName()];
            if(mtfstats&STAT_TYPE_MIN){
                ACMediaFeatures* feat = (*mtf).second->min();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_MEAN){
                ACMediaFeatures* feat = (*mtf).second->mean();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_MAX){
                ACMediaFeatures* feat = (*mtf).second->max();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_CENTROID){
                ACMediaFeatures* feat = (*mtf).second->centroid();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_SPREAD){
                ACMediaFeatures* feat = (*mtf).second->spread();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_SKEWNESS){
                ACMediaFeatures* feat = (*mtf).second->skew();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_KURTOSIS){
                ACMediaFeatures* feat = (*mtf).second->kurto();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_COV){
                ACMediaFeatures* feat = (*mtf).second->cov(6);
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_CORR){
                ACMediaFeatures* feat = (*mtf).second->cor(12);
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_MODULATION){
                ACMediaFeatures* feat = (*mtf).second->modulation();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_LOG_CENTROID){
                ACMediaFeatures* feat = (*mtf).second->logCentroid();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_LOG_SPREAD){
                ACMediaFeatures* feat = (*mtf).second->logSpread();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_LOG_SKEWNESS){
                ACMediaFeatures* feat = (*mtf).second->logSkew();
                descmf[feat->getName()]=feat;
            }
            if(mtfstats&STAT_TYPE_LOG_KURTOSIS){
                ACMediaFeatures* feat = (*mtf).second->logKurto();
                descmf[feat->getName()]=feat;
            }
            
            //std::cout << "descmtf " << (*mtf).second->getName() << " of length " << (*mtf).second->getLength() << " and dim " << (*mtf).second->getDim() << " gives mean of size " << (*mtf).second->mean()->getSize() << std::endl;
            /*if (i==nrgIdx){
                desc[i]->setNeedsNormalization(0);
            }*/
        }
        
    }
    
    for(mtf=descmtf.begin();mtf!=descmtf.end();mtf++){
        std::string featureName((*mtf).second->getName());
        std::replace( featureName.begin(), featureName.end(), ' ', '_');
        mtf_file_name = aFileName_noext + "_" + featureName   + binary+ mtf_file_ext;;
        
        /// Save each timed feature in a *.mtf file if it has just been computed (not if loaded)
        if(computed){
            bool saved = (*mtf).second->saveInFile(mtf_file_name, save_binary);
            if(!saved)
                std::cerr << "ACAudioYaafeCorePlugin: couldn't save feature named '" << mtf_file_name << "'" << std::endl;
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
                std::cerr << "ACAudioYaafeCorePlugin: couldn't save feature named '" << mf_file_name << "'" << std::endl;
        }
        
        /// Build a list of features extracted
        desc.push_back((*mf).second);
    }
    
    this->clearTimedFeatures();
    
    return desc;
}
