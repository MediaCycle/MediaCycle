/*
 *  ACAudioYaafePlugin.cpp
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

#ifdef USE_YAAFE

//#define SAVE_CSV

#include "ACAudioYaafePlugin.h"
#include "ACAudio.h"
#include <vector>
#include <string>

using namespace YAAFE;
using namespace std;

#include <yaafe-io/io/FileUtils.h>
#include <sstream>

#include "yaafe-core/ComponentFactory.h"

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

#define BUFSIZED 64

using namespace std;

namespace YAAFE {

ACYaafeWriter::ACYaafeWriter() {
    m_feature_name = "";
    m_plugin = 0;
    m_file = "";
}

ACYaafeWriter::~ACYaafeWriter() {
    if (m_fout.is_open())
        m_fout.close();
    m_plugin = 0;
}

ParameterDescriptorList ACYaafeWriter::getParameterDescriptorList() const
{
    ParameterDescriptorList pList;
    ParameterDescriptor p;

    p.m_identifier = "File";
    p.m_description = "MC output filename";
    p.m_defaultValue = "";
    pList.push_back(p);

    p.m_identifier = "Attrs";
    p.m_description = "Metadata to be written to the output";
    p.m_defaultValue = "";
    pList.push_back(p);

    p.m_identifier = "Metadata";
    p.m_description = "If 'True' then write metadata as comments at the beginning of the MC file. If 'False', do not write metadata";
    p.m_defaultValue = "True";
    pList.push_back(p);

    p.m_identifier = "Precision";
    p.m_description = "precision of output floating point number.";
    p.m_defaultValue = "6";
    pList.push_back(p);

    return pList;
}

bool ACYaafeWriter::init(const ParameterMap& params, const Ports<StreamInfo>& inp)
{
    assert(inp.size()==1);
    const StreamInfo& in = inp[0].data;

    m_file = getStringParam("File", params);
    m_feature_name = getStringParam("Feature", params);

    // Replace underscores by spaces, human-readable, conforming with ACAudioFeatures
    size_t underscore = 0;
    while(underscore != std::string::npos){
        underscore = m_feature_name.find_first_of("_",underscore);
        if(underscore != std::string::npos)
            m_feature_name = m_feature_name.replace(underscore,1," ");
    }

    //std::cout << "ACYaafeWriter::init " << m_feature_name << " on " << m_file << std::endl;
    m_precision = getIntParam("Precision",params);
    if (m_precision > (BUFSIZED-10)) {
        cerr << "WARNING: precision is too large ! use precision " << BUFSIZE - 10 << endl;
        m_precision = BUFSIZED - 10;
    }

#ifdef SAVE_CSV
    int res = preparedirs(m_file.c_str());
    if (res!=0)
        return false;

    m_fout.open(m_file.c_str(), ios_base::trunc);
    if (!m_fout.is_open() || m_fout.bad())
        return false;

    if (getStringParam("Metadata",params)=="True") {
        // write metadata at the beginnig of the file
        string paramStr = getStringParam("Attrs",params);
        map<string,string> _params = decodeAttributeStr(paramStr);
        ostringstream oss;
        for (map<string,string>::const_iterator it=_params.begin();it!=_params.end();it++){
            oss << "% " << it->first << "=" << it->second << endl;
        }
        m_fout.write(oss.str().c_str(),oss.str().size());
    }
#endif

    return true;
}

void ACYaafeWriter::reset() {
    // nothing to do
    //std::cout << "ACYaafeWriter::reset" << std::endl;
}

bool ACYaafeWriter::process(Ports<InputBuffer*>& inp, Ports<OutputBuffer*>& outp)
{
    //std::cout << "ACYaafeWriter::process " << m_feature_name << " on " << m_file << std::endl;

    float time = 0;
    std::vector<float> times;
    std::vector< std::vector<float> > values;
    ACMediaTimedFeature* feat = 0;

    if(m_plugin){
        if(m_plugin->isMediaTimedFeatureStored(m_feature_name,m_file)){
            time = m_plugin->getMediaTimedFeatureStored(m_feature_name,m_file)->getTime( m_plugin->getMediaTimedFeatureStored(m_feature_name,m_file)->getLength()-1);
        }
    }

    assert(inp.size()==1);
    InputBuffer* in = inp[0].data;
    assert(outp.size()==0);
    char buf[BUFSIZED];
    //std::cout << "ACYaafeWriter::process starts" << std::endl;
    while (!in->empty())
    {
        values.push_back( std::vector<float>(in->info().size) );
        double* data = in->readToken();
        int strSize = sprintf(buf,"%0.*e",m_precision,data[0]);
        (values.back())[0] = ((float)(data[0]));
        //std::cout << " data[0] " << data[0] << std::endl;

#ifdef SAVE_CSV
        m_fout.write(buf,strSize);
#endif

        time += (float)in->info().sampleStep / (float)in->info().sampleRate;
        times.push_back(time);

        for (int i=1;i<in->info().size;i++)
        {
            strSize = sprintf(buf,",%0.*e",m_precision,data[i]);
#ifdef SAVE_CSV
            m_fout.write(buf,strSize);
#endif
            (values.back())[i] = ((float)(data[i]));
            //std::cout << "i " << i << " data[i] " << data[i] << std::endl;
        }
#ifdef SAVE_CSV
        m_fout << endl;
#endif
        in->consumeToken();
    }
    if(times.size()>0 && values.size()>0){
        feat = new ACMediaTimedFeature(times,values,m_feature_name);
        //std::cout << "ACYaafeWriter::process: feature " << m_feature_name << " times " << times.size() << " dims " << values.size() << "x" << values.front().size() << std::endl;
    }
    else{
        std::cerr <<  "ACYaafeWriter::process: empty times/values" << std::endl;
        return false;
    }

    if(m_plugin && feat){
        return m_plugin->addMediaTimedFeature(feat,m_file);
    }
    return true;
}

void ACYaafeWriter::flush(Ports<InputBuffer*>& in, Ports<OutputBuffer*>& out)
{
    //std::cout << "ACYaafeWriter::flush " << m_feature_name << " on " << m_file << std::endl;
    process(in,out);
#ifdef SAVE_CSV
    m_fout.close();
#endif
}

}


bool ACYaafeOutputFormat::available() const {
    return ComponentFactory::instance()->exists("ACYaafeWriter");
}

const ParameterDescriptorList ACYaafeOutputFormat::getParameters() const {
    const Component* ACw = ComponentFactory::instance()->getPrototype("ACYaafeWriter");
    if (ACw==NULL) {
        cerr << "WARNING: ACYaafeWriter not available ! cannot retrieve parameter list" << endl;
        return ParameterDescriptorList();
    }
    ParameterDescriptorList pList = ACw->getParameterDescriptorList();
    eraseParameterDescriptor(pList,"File");
    eraseParameterDescriptor(pList,"Attrs");
    return pList;
}

Component* ACYaafeOutputFormat::createWriter(
    const std::string& inputfile,
    const std::string& feature,
    const ParameterMap& featureParams,
    const Ports<StreamInfo>& featureStream)
{
    Component* writer = ComponentFactory::instance()->createComponent("ACYaafeWriter");
    ParameterMap writerParams = m_params;
    writerParams["Feature"] = feature;
    writerParams["File"] = filenameConcat(m_outDir,inputfile,"." + feature + ".csv");
    writerParams["Attrs"] = encodeParameterMap(featureParams);
    if (!writer->init(writerParams,featureStream)) {
        delete writer;
        cerr << "ERROR: cannot initialize ACYaafeWriter !" << endl;
        return NULL;
    }

    ACYaafeWriter* _writer = 0;
    try {
        _writer = static_cast <ACYaafeWriter*> (writer);
        if(!_writer)
            throw runtime_error("<ACYaafeOutputFormat::createWriter> problem with ACYaafeWriter cast");
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
    }
    if(_writer){
        _writer->setMediaCyclePlugin(m_plugin);
    }

    return writer;
}

ACAudioYaafePlugin::ACAudioYaafePlugin() : ACTimedFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "AudioYaafe";
    this->mDescription = "Audio feature extraction plugin usin the Yaafe library";
    this->mId = "";
    //this->mtf_file_name = "";

    //DataBlock::setPreferedBlockSize(1024);

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

    // Register the component and output format to use yaafe as MC plugin
    OutputFormat::registerFormat(new ACYaafeOutputFormat(this));
    ComponentFactory::instance()->registerPrototype(new ACYaafeWriter());

    // Check the list of loaded components
    //listComponents();

    std::string dataflow_file = "";
#ifdef USE_DEBUG
    boost::filesystem::path s_path( __FILE__ );
    //std::cout << "Current source path " << s_path.parent_path().string() << std::endl;
    dataflow_file += s_path.parent_path().string() + "/ACAudioYaafePluginSettings.txt";
#else
#ifdef __APPLE__
    dataflow_file = getExecutablePath() + "/ACAudioYaafePluginSettings.txt";
#elif __WIN32__
    dataflow_file = "ACAudioYaafePluginSettings.txt";
#else
    dataflow_file = "/usr/share/mediacycle/plugins/audio/ACAudioYaafePluginSettings.txt";
#endif
#endif
    DataFlow df;
    if (!df.load(dataflow_file)) {
        cerr << "ERROR: cannot load dataflow from file "<< endl;
    }
    //df.display();

    //Engine engine;
    if (!engine.load(df)) {
        cerr << "ERROR: cannot initialize dataflow engine" << endl;
    }

    m_default_resample_rate = 20050;
    m_default_step_size = 256;
    m_default_block_size = 512;

   /* std::cout << "ACAudioYaafePlugin: inputs" << std::endl;
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

    mDescriptorsList.clear();

    //std::cout << "ACAudioYaafePlugin: outputs" << std::endl;
    std::vector<std::string> outputs = engine.getOutputs();
    for (std::vector<std::string>::iterator output = outputs.begin();output!=outputs.end();output++){
        //std::cout << "Output: " << (*output) << std::endl;
        ParameterMap outputparams = engine.getOutputParams(*output);

        std::string name = (*output);
        size_t underscore = 0;
        while(underscore != std::string::npos){
            underscore = name.find_first_of("_",underscore);
            if(underscore != std::string::npos){
                name = name.replace(underscore,1," ");
            }
        }
        //std::cout << "ACAudioYaafePlugin: adding descriptor: '" << name << "'" << std::endl;
        mDescriptorsList.push_back(name);

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
}

ACAudioYaafePlugin::~ACAudioYaafePlugin() {
    //ComponentFactory::destroy();
}

void ACAudioYaafePlugin::listComponents()
{
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

void ACAudioYaafePlugin::describeComponent(const std::string component)
{
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

void ACAudioYaafePlugin::printOutputFormats()
{
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

bool ACAudioYaafePlugin::isMediaTimedFeatureStored(std::string name, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(name);
    if(mf!=descmf.end()){
        return true;
    }
    else{
        return false;
    }
}

ACMediaTimedFeature* ACAudioYaafePlugin::getMediaTimedFeatureStored(std::string name, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(name);
    if(mf!=descmf.end()){
        return (*mf).second;
    }
    else{
        return 0;
    }
}


bool ACAudioYaafePlugin::addMediaTimedFeature(ACMediaTimedFeature* feature, std::string file){
    std::map<std::string,ACMediaTimedFeature*>::iterator mf = descmf.find(feature->getName());
    if(mf!=descmf.end()){
        //std::cout << "ACAudioYaafePlugin: appended feature: " << feature->getName() << " of length " << feature->getLength() << "/" << (*mf).second->getLength() << " and dim " << feature->getDim() << " vs " << (*mf).second->getDim() << " for file " << file << std::endl;
        return (*mf).second->appendTimedFeatureAlongTime(feature);
    }
    else{
        //std::cout << "ACAudioYaafePlugin: new feature: " << feature->getName() << " of length " << feature->getLength() << " and dim " << feature->getDim() << " for file " << file << std::endl;
        descmf.insert( pair<std::string,ACMediaTimedFeature*>(feature->getName(),feature) );
        return true;
    }
}

std::vector<ACMediaFeatures*> ACAudioYaafePlugin::calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat) {
    descmf.clear();

    std::vector<ACMediaFeatures*> desc;

    if(mDescriptorsList.size()==0){
        std::cerr << "ACAudioYaafePlugin: dataflow settings not loaded, can't calcutate features" << std::endl;
        return desc;
    }

    ACMediaFeatures* feat;
    ACAudio* theAudio = 0;

    try {
        theAudio = static_cast <ACAudio*> (theMedia);
        if(!theAudio)
            throw runtime_error("<ACAudioYaafePlugin::_calculate> problem with ACAudio cast");
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return desc;
    }

    boost::filesystem::path f_path( theMedia->getFileName() );

    // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
    bool save_binary = false;//CF was true
    string mtf_file_name; // file(s) in which feature(s) will be saved
    string file_ext =  "_b.mtf";
    //if(!save_binary)
    //    file_ext =  "_t.mtf";
    string aFileName = theMedia->getFileName();
    string aFileName_noext = aFileName.substr(0,aFileName.find_last_of('.'));
    std::map<std::string,ACMediaTimedFeature*>::iterator mf;

    // Trying to open previousy-computed files for parent files (TODO should test/compare parameters and plugin versions)
    _save_timed_feat = true; // forcing saving features so that we don't have to calculate them all the time
    int feature_length = -1;
    if(theMedia->getParentId()==-1){
        bool featuresAvailable = true;
        for (std::vector<std::string>::iterator feat = mDescriptorsList.begin();feat!=mDescriptorsList.end();feat++){
            ACMediaTimedFeature* feature = 0;
            feature = new ACMediaTimedFeature();
            bool featureAvailable = false;
            //std::cout << "ACAudioYaafePlugin: trying to load feature named '" << *feat << "'... " << std::endl;
            mtf_file_name = aFileName_noext + "_" + (*feat) + file_ext;
            featureAvailable = feature->loadFromFile(mtf_file_name,save_binary);
            if(featureAvailable && feature){
                feature->setName(*feat);
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
            /*if(featureAvailable){
                std::cout << "ACAudioYaafePlugin: feature named '" << *feat  << "' loaded" << std::endl;
            }
            else{
                std::cout << "ACAudioYaafePlugin: feature named '" << *feat  << "' NOT loaded" << std::endl;
            }*/
            featuresAvailable *= featureAvailable;
        }
        if(descmf.size()==0){
            featuresAvailable = false;
            std::cout << "ACAudioYaafePlugin: features weren't calculated previously" << std::endl;
        }
        else if(mDescriptorsList.size()==0){
            featuresAvailable = false;
            std::cerr << "ACAudioYaafePlugin: loaded features are empty" << std::endl;
        }
        else if(mDescriptorsList.size()!=descmf.size()){
            featuresAvailable = false;
            std::cerr << "ACAudioYaafePlugin: some features weren't calculated previously" << std::endl;
        }
        if(!featuresAvailable){
            for(mf=descmf.begin();mf!=descmf.end();mf++)
                delete (*mf).second;
            descmf.clear();
            std::cerr << "ACAudioYaafePlugin: error while loading features, now recalculating them..." << std::endl;
        }
    }
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
                std::cerr << "ACAudioYaafePlugin::calculate: couldn't set output format" << std::endl;
                return desc;
            }
        }

        //std::cout << "ACAudioYaafePlugin::calculate: file " << theMedia->getFileName() << std::endl;
        /*for (int i=0;i<files->count; i++)
        {*/

        int res = processor.processFile(engine, theMedia->getFileName(), theMedia->getStart(), theMedia->getEnd() );//files->filename[i]);
        if (res!=0) {
            cerr << "ERROR: error while processing " << theMedia->getFileName() << endl;//files->filename[i] << endl;
        }
        //}

        //std::cout << "ACAudioYaafePlugin::calculate: file " << theMedia->getFileName() << " done " << std::endl;
    }

    // find the index of the feature named "Energy"
    /*int nrgIdx = 0;
        for (int i=0; i<descmf.size(); i++){
            if (descmf[i]->getName() == "Energy")
                nrgIdx = i;
        }*/
    // the feature named "Energy" does not need to be normalized
    for(mf=descmf.begin();mf!=descmf.end();mf++){
        desc.push_back((*mf).second->mean());
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

#endif //def USE_YAAFE
