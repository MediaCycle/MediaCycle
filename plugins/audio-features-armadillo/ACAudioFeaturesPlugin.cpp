/*
 *  ACAudioFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 22/09/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACAudioFeaturesPlugin.h"
#include "ACAudio.h"
#include "ACAudioData.h"

#include <vector>
#include <string>

//class ACMedia;

//#define BUFFERIZED

ACAudioFeaturesPlugin::ACAudioFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Audio Features Armadillo";
    this->mDescription = "AudioFeatures plugin";
    this->mId = "";
    //this->mtf_file_name = "";
    this->mfccNb = 13;
}

ACAudioFeaturesPlugin::~ACAudioFeaturesPlugin() {
}

ACFeatureDimensions ACAudioFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Spectral Centroid"] = 1;
    featureDimensions["Spectral Spread"] = 1;
    featureDimensions["Spectral Variation"] = 1;
    featureDimensions["Spectral Flatness"] = 1;
    featureDimensions["Spectral Flux"] = 1;
    featureDimensions["Spectral Decrease"] = 1;
    featureDimensions["MFCC"] = mfccNb;
    featureDimensions["DMFCC"] = mfccNb;
    featureDimensions["DDMFCC"] = mfccNb;
    featureDimensions["Zero Crossing Rate"] = 1;
    featureDimensions["Energy"] = 1;
    featureDimensions["Loudness"] = 1;
    featureDimensions["Log Attack Time"] = 1;
    featureDimensions["Energy Modulation Frequency"] = 1;
    featureDimensions["Energy Modulation Amplitude"] = 1;
    return featureDimensions;
}

/*
static starpu_codelet cl = {
    .where = STARPU_CPU,
    .cpu_func = cpu_func,
    .nbuffers = 2
}

static void cpu_func(void _*descr[], void *cl_args) {
    (ACAudioFeaturesPlugin*)cl_args.plugin->calculate(audio_data, theMedia, returndescriptor);
}

std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate_starpu(vector<ACMediaData*> audio_data, vector<ACMedia*> theMedia) {
    // create job list
    for () {
    }

    // return vector of decriptors

    // check that calculate is thread safe
}
*/

// CF
std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    // from MediaData
    return this->_calculate(theMedia->getFileName(),theMedia,_save_timed_feat);
}

// private method
std::vector<ACMediaFeatures*> ACAudioFeaturesPlugin::_calculate(std::string aFileName, ACMedia* theMedia, bool _save_timed_feat){
    bool extendSoundLimits = true;
    std::vector<ACMediaTimedFeature*> descmf;
    std::vector<ACMediaFeatures*> desc;

    ACAudio* theAudio = 0;

    try{
        theAudio = static_cast <ACAudio*> (theMedia);
        if(!theAudio)
            throw runtime_error("<ACAudioFeaturesPlugin::_calculate> problem with ACAudio cast");
    }catch (const exception& e) {
        cerr << e.what() << endl;
        return desc;
    }

#ifndef BUFFERIZED
    // XS TODO we are copying the data, unnecessary
    float* data = new float[theAudio->getNFrames() * theAudio->getChannels()];

    // SD replaced loop by more efficient memcpy
    memcpy(data, static_cast<float*>(theAudio->getSamples())+theAudio->getSampleStart()*theAudio->getChannels(),
           (theAudio->getSampleEnd()-theAudio->getSampleStart())*theAudio->getChannels()*sizeof(float));
    /*
     for (long i = theAudio->getSampleStart(); i< theAudio->getSampleEnd(); i++){
     for (long j = 0; j < theAudio->getChannels(); j++){
     data[index] = audio_data->getAudioData()[i*theAudio->getChannels()+j];
     index++;
     }
     }
     */


    // 	ofstream output("signal1.txt");
    // 	for(int i=0; i < (long) theAudio->getNFrames() * theAudio->getChannels(); i++){
    // 		output<<data[i]<<endl;
    // 	}
    descmf = computeFeatures(data, theAudio->getSampleRate(), theAudio->getChannels(), theAudio->getNFrames(), 32, this->mfccNb, 1024, extendSoundLimits);
#else
    descmf = computeFeaturesBuffered(theAudio, 32, this->mfccNb, 1024, extendSoundLimits);
#endif//def BUFFERIZED

    // find the index of the feature named "Energy"
    int nrgIdx = 0;
    for (int i=0; i<descmf.size(); i++){
        if (descmf[i]->getName() == "Energy")
            nrgIdx = i;
    }
    //std::cout << "nrgIdx = " << nrgIdx << std::endl;

    // the feature named "Energy" does not need to be normalized
    for (int i=0; i<descmf.size(); i++){
        desc.push_back(descmf[i]->mean());
        if (i==nrgIdx){
            desc[i]->setNeedsNormalization(1);
        }
    }

    // saving timed features on disk (if _save_timed_feat flag is on)
    // XS TODO add checks
    if (_save_timed_feat) {
        // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
        bool save_binary = false;//CF true
        string mtf_file_name; // file(s) in which feature(s) will be saved
        string file_ext =  "_b.mtf";
        if(!save_binary)
            file_ext =  "_t.mtf";
        string aFileName = theMedia->getFileName();
        string aFileName_noext = aFileName.substr(aFileName.find_last_of('/'),aFileName.find_last_of('.')-aFileName.find_last_of('/'));
        string aFileName_direct = aFileName.substr(0,aFileName.find_last_of('/'));
        boost::filesystem::path dir(aFileName_direct+"/mtf");
        boost::filesystem::create_directory(dir);
        for (int i=0; i<descmf.size(); i++){
            mtf_file_name = aFileName_direct+"/mtf"+aFileName_noext + "_" +descmf[i]->getName() + file_ext;
            descmf[i]->saveInFile(mtf_file_name, save_binary);
            theMedia->addTimedFileNames(mtf_file_name);
            //mtf_file_names.push_back(mtf_file_name); // keep track of saved features
        }
    }

    // XS TODO wtf ?
    //desc.push_back(descmf[nrgIdx]->interpN(10)->toMediaFeatures());

    // CF WARNING we save only the "Energy" feature that is expected for the AudioSegmentatinPlugin, until we can choose features associated to segmentation plugins
    /*if (_save_timed_feat) {
        // try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
        bool save_binary = true;
        string file_ext = "_b.mtf";
        string aFileName_noext = theMedia->getFileName().substr(0,theMedia->getFileName().find_last_of('.'));
        mtf_file_name = aFileName_noext + "_" +this->mDescription + file_ext; // mName instead of mDescription due its the space char, just in case?
        // CF we're saving the Energy feature as it is used for method 0 in ACAudioSegmentationPlugin...
        descmf[10]->saveInFile(mtf_file_name, save_binary);//CF just the nth MediaTimeFeature [0] saved!
    }*/

    for (int i=0; i<descmf.size(); i++){
        delete descmf[i];
    }
    descmf.clear();
#ifndef BUFFERIZED
    delete [] data;
#endif//def BUFFERIZED
    return desc;
}

// the plugin should know internally where it saved the mtf
//ACMediaTimedFeature* ACAudioFeaturesPlugin::getTimedFeatures(){
//	if (mtf_file_name == ""){
//        cout << "<ACAudioFeaturesPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}


// XS TODO !!!
// the plugin should know internally where it saved the mtf
// thanks to mtf_file_name
//ACMediaTimedFeature* ACAudioFeaturesPlugin::getTimedFeatures(){
//	if (mtf_file_names.size() == 0){
//        cout << "<ACAudioFeaturesPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}



