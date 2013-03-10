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


ACAudioYaafeCorePlugin::ACAudioYaafeCorePlugin() : ACTimedFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Yaafe Features";
    this->mDescription = "Audio feature extraction plugin using the Yaafe library";
    this->mId = "";
    //this->mtf_file_name = "";

    dataflowLoaded = false;
    factoriesRegistered = false;

    with_min = true;
    with_mean = true;
    with_max = true;
    /*with_centroid = true;
    with_spread = true;
    with_skewness = true;
    with_kurtosis = true;*/
    with_centroid = false;
    with_spread = false;
    with_skewness = false;
    with_kurtosis = false;

    //DataBlock::setPreferedBlockSize(1024);

    m_default_resample_rate = 20050;
    m_default_step_size = 256;
    m_default_block_size = 512;

    // Register the component and output format to use yaafe as MC plugin
    OutputFormat::registerFormat(new ACYaafeOutputFormat(this));
    ComponentFactory::instance()->registerPrototype(new ACYaafeWriter());
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
    /*setenv("YAAFE_PATH","/Volumes/data/Builds/numediart/audiocycle-osx10.6.8-qtcreator/3rdparty/yaafe/src_cpp/yaafe-io",1);
    int yaafe_io = 0;
    yaafe_io = ComponentFactory::instance()->loadLibrary("yaafe-io");
    if (yaafe_io)
        std::cerr << "Couldn't load yaafe-io" << std::endl;

    setenv("YAAFE_PATH","/Volumes/data/Builds/numediart/audiocycle-osx10.6.8-qtcreator/3rdparty/yaafe/src_cpp/yaafe-components",1);
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
    boost::filesystem::path c_path( __FILE__ );
    ///dataflow_file += s_path.parent_path().string() + "/" + this->dataflowFilename();
    std::string library_path("");
    if(media_cycle)
        library_path = media_cycle->getLibraryPathFromPlugin(this->mName);
    if(library_path == ""){
        std::cerr << "ACAudioYaafeCorePlugin::loadDataflow: couldn't get the library path" << std::endl;
        return;
    }
    boost::filesystem::path b_path( library_path );
    dataflow_file = c_path.parent_path().parent_path().parent_path().string() + "/" + b_path.parent_path().stem().string() + "/" + b_path.stem().string() + "/" + this->dataflowFilename();
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
        cerr << "ERROR: cannot load dataflow from file "<< endl;
        return;
    }
    //df.display();

    //Engine engine;
    if (!engine.load(df)) {
        cerr << "ERROR: cannot initialize dataflow engine" << endl;
        return;
    }



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

        size_t underscore = 0;
        while(underscore != std::string::npos){
            underscore = name.find_first_of("_",underscore);
            if(underscore != std::string::npos){
                name = name.replace(underscore,1," ");
            }
        }
        //std::cout << "ACAudioYaafeCorePlugin: adding descriptor: '" << name << "'" << std::endl;
        //std::cout << "ACAudioYaafeCorePlugin: feature " << *output << " has a dimension of " << dim << std::endl;

        if(with_min)
            featdims[name+": Min"] = dim;
        if(with_mean)
            featdims[name+": Mean"] = dim;
        if(with_max)
            featdims[name+": Max"] = dim;
        if(with_centroid)
            featdims[name+": Centroid"] = dim;
        if(with_spread)
            featdims[name+": Spread"] = dim;
        if(with_skewness)
            featdims[name+": Skewness"] = dim;
        if(with_kurtosis)
            featdims[name+": Kurtosis"] = dim;

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

ACAudioYaafeCorePlugin::~ACAudioYaafeCorePlugin() {
    //ComponentFactory::destroy();
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
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(name);
    if(mf!=descmf.end()){
        return true;
    }
    else{
        return false;
    }
}

ACMediaTimedFeature* ACAudioYaafeCorePlugin::getMediaTimedFeatureStored(std::string name, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(name);
    if(mf!=descmf.end()){
        return (*mf).second;
    }
    else{
        return 0;
    }
}


bool ACAudioYaafeCorePlugin::addMediaTimedFeature(ACMediaTimedFeature* feature, std::string file){
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

std::vector<ACMediaFeatures*> ACAudioYaafeCorePlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    descmf.clear();
    cout<<"ACAudioYaafeCorePlugin::calculate"<<endl;

    std::vector<ACMediaFeatures*> desc;

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
            std::cerr << "ACAudioYaafeCorePlugin: dataflow settings not loaded, can't calcutate features" << std::endl;
            return desc;
        }
    }

    //ACMediaFeatures* feat;
    ACAudio* theAudio = 0;

    // try {
    theAudio = static_cast <ACAudio*> (theMedia);
    if(!theAudio)
        //       throw runtime_error("<ACAudioYaafeCorePlugin::_calculate> problem with ACAudio cast");
        //}
        //catch (const exception& e)
    {
        //cerr << e.what() << endl;
        cerr <<"ACAudioYaafeCorePlugin::_calculate> problem with ACAudio cast" << endl;
        return desc;
    }

    boost::filesystem::path f_path( theMedia->getFileName() );

    // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
    bool save_binary = false;//CF was true
    string mtf_file_name; // file(s) in which feature(s) will be saved
    string file_ext =  "_b.mtf";
    if(!save_binary)
        file_ext =  "_t.mtf";
    string aFileName = theMedia->getFileName();
    string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
    std::map<std::string,ACMediaTimedFeature*>::iterator mf;

    // Trying to open previously-computed files for parent files (TODO should test/compare parameters and plugin versions)
#ifdef USE_DEBUG
    _save_timed_feat = true; // forcing saving features so that we don't have to calculate them all the time
    int feature_length = -1;
    if(theMedia->getParentId()==-1){
        bool featuresAvailable = true;
        for (ACFeatureDimensions::iterator featIt = featureDimensions.begin();featIt!=featureDimensions.end();featIt++){
            ACMediaTimedFeature* feature = 0;
            feature = new ACMediaTimedFeature();
            bool featureAvailable = false;
            //std::cout << "ACAudioYaafeCorePlugin: trying to load feature named '" << *feat << "'... " << std::endl;
            mtf_file_name = aFileName_noext + "_" + featIt->first + file_ext;
            featureAvailable = feature->loadFromFile(mtf_file_name,save_binary);
            if(featureAvailable && feature){
                feature->setName(featIt->first);
                featureAvailable = this->addMediaTimedFeature(feature, aFileName);
                if(featureAvailable){
                    if(feature_length != -1){
                        // Testing if all features have the same length
                        if( feature_length != feature->getLength())
                            featureAvailable = false;
                    }
                    feature_length = feature->getLength();
                    if(feature->getLength() == 0) // Testing if the length of any feature is null
                        featureAvailable = false;
                }
            }
            //if(featureAvailable){
            //    std::cout << "ACAudioYaafeCorePlugin: feature named '" << *feat  << "' loaded" << std::endl;
            //}
            //else{
            //    std::cout << "ACAudioYaafeCorePlugin: feature named '" << *feat  << "' NOT loaded" << std::endl;
            //}
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
        if(!featuresAvailable){
            for(mf=descmf.begin();mf!=descmf.end();mf++)
                delete (*mf).second;
            descmf.clear();
            std::cerr << "ACAudioYaafeCorePlugin: error while loading features, now recalculating them..." << std::endl;
        }
    }
#endif
    // CF TODO check if the features length matches the expected length based on the file length and block/step sizes and resample rate
    // CF TODO if some features weren't previously calcultated, only recalculate these

    // Compute the features if not mtf file is found
    if(descmf.size()==0){

        //AudioFileProcessor processor;
        {
            string formatStr = "MC";
            /*if (format->count)
                        formatStr = format->sval[0];*/
            //string outDirStr = f_path.parent_path().string();
            //std::cout << f_path.parent_path().string() << std::endl;
            string outDirStr = ".";
            /*if (outdir->count)
                        outDirStr = outdir->sval[0];*/
            ParameterMap params;
            /*for (int p=0;p<formatparams->count;p++) {
                        string str = formatparams->sval[p];
                        size_t keyEnd = str.find('=');
                        if (keyEnd==string::npos) {
                                cerr << "Malformed parameter " << formatparams->sval[p] << endl;
                                continue;
                        }
                        params[str.substr(0,keyEnd)] = str.substr(keyEnd+1,str.size()-keyEnd-1);
                }*/
            if (!processor.setOutputFormat(formatStr,outDirStr,params)) {
                //ComponentFactory::destroy();
                std::cerr << "ACAudioYaafeCorePlugin::calculate: couldn't set output format" << std::endl;
                return desc;
            }
        }

        //std::cout << "ACAudioYaafeCorePlugin::calculate: file " << theMedia->getFileName() << std::endl;
        /*for (int i=0;i<files->count; i++)
        {*/

        int res = 0;
        try{
            res = processor.processFile(engine, theMedia->getFileName(), theMedia->getStart(), theMedia->getEnd() );//files->filename[i]);
        }
        catch(...){
            res = -1;
        }
        if (res!=0) {
            cerr << "ERROR: error while processing " << theMedia->getFileName() << endl;//files->filename[i] << endl;
        }
        //}

        //std::cout << "ACAudioYaafeCorePlugin::calculate: file " << theMedia->getFileName() << " done " << std::endl;
    }

    // find the index of the feature named "Energy"
    /*int nrgIdx = 0;
        for (int i=0; i<descmf.size(); i++){
            if (descmf[i]->getName() == "Energy")
                nrgIdx = i;
        }*/
    // the feature named "Energy" does not need to be normalized
    for(mf=descmf.begin();mf!=descmf.end();mf++){
        if(with_min)
            desc.push_back((*mf).second->min());
        if(with_mean)
            desc.push_back((*mf).second->mean());
        if(with_max)
            desc.push_back((*mf).second->max());
        // CPL, statistics using boost library
        if(with_centroid)
            desc.push_back((*mf).second->centroid());
        if(with_spread)
            desc.push_back((*mf).second->spread());
        //std::map<std::string,ACMediaTimedFeature*>::iterator mf2;
        //for(mf2=descmf.begin();mf2!=descmf.end();mf2++){
        //    desc.push_back((*mf).second->cov(mf2->second));
        //}
        if(with_skewness)
            desc.push_back((*mf).second->skew());
        if(with_kurtosis)
            desc.push_back((*mf).second->kurto());

        //std::cout << "descmf " << (*mf).second->getName() << " of length " << (*mf).second->getLength() << " and dim " << (*mf).second->getDim() << " gives mean of size " << (*mf).second->mean()->getSize() << std::endl;
        /*if (i==nrgIdx){
                desc[i]->setNeedsNormalization(0);
            }*/
    }

    // saving timed features on disk (if _save_timed_feat flag is on)
    if (theMedia->getParentId()==-1 && _save_timed_feat) {
        for(mf=descmf.begin();mf!=descmf.end();mf++){
            mtf_file_name = aFileName_noext + "_" +(*mf).second->getName() + file_ext;
            (*mf).second->saveInFile(mtf_file_name, save_binary);
            theMedia->addTimedFileNames(mtf_file_name);
            //mtf_file_names.push_back(mtf_file_name); // keep track of saved features
        }
    }

    //std::vector<ACMediaTimedFeature*>::iterator mtf;
    for(mf=descmf.begin();mf!=descmf.end();mf++)
        (*mf).second = 0;//delete (*mf);
    descmf.clear();

    return desc;
}
