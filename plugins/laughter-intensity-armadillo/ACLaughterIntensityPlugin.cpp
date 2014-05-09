/*
 *  ACLaughterIntensityPlugin.cpp
 *  MediaCycle
 *
 *  @author Jerome Urbain
 *  @date 29/04/14
 *  @copyright (c) 2014 – UMONS - Numediart
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

#include "ACLaughterIntensityPlugin.h"

#include <vector>
#include <string>

//class ACMedia;

//#define BUFFERIZED

ACLaughterIntensityPlugin::ACLaughterIntensityPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Laughter Intensity Armadillo";
    this->mDescription = "LaughterIntensity plugin";
    this->mId = "";
    this->mDistType=FeaturesVector::Euclidean;
    //this->mtf_file_name = "";
    //this->mfccNb = 13;
}

ACLaughterIntensityPlugin::~ACLaughterIntensityPlugin() {
}

ACFeatureDimensions ACLaughterIntensityPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Global intensity"] = 1;
    featureDimensions["Continuous intensity: Mean"] = 1;
    featureDimensions["Continuous intensity: Std"] = 1;
    return featureDimensions;
}


// JU
std::vector<ACMediaFeatures*> ACLaughterIntensityPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    // from MediaData
    return this->_calculate(theMedia->getFileName(),theMedia,_save_timed_feat);
}

// private method: JU: read the global and continuous intensity files and return global intensity, average continuous intensity and std continuous intensity
std::vector<ACMediaFeatures*> ACLaughterIntensityPlugin::_calculate(std::string aFileName, ACMedia* theMedia, bool _save_timed_feat){
    // JU: filenames: ideally this should be moved somewhere else...
    std::string features_dir="/media/Data/Data/Ilhaire/databases/Belfast_Storytelling/individual_laugh_segmentation/laughter_features/";
    std::string global_intensity_suffix="_intensity_global.txt";
    std::string continuous_intensity_suffix="_intensity.txt";
    

    
    // to have the name of the audio file without directory nor extension
    size_t pos=aFileName.find_last_of(".");
    size_t posSep = aFileName.find_last_of("/\\");

    // define names of intensity files
    std::string global_intensity_file;
    std::string continuous_intensity_file;
    if(posSep)
    {
      global_intensity_file=features_dir+aFileName.substr(posSep+1,pos-posSep-1)+global_intensity_suffix;
      continuous_intensity_file=features_dir+aFileName.substr(posSep+1,pos-posSep-1)+continuous_intensity_suffix;
    }
    else
    {
      global_intensity_file=features_dir+aFileName.substr(0,pos-1)+global_intensity_suffix;
      continuous_intensity_file=features_dir+aFileName.substr(0,pos-1)+continuous_intensity_suffix;
    }
    
    // open continuous intensity file
    arma::colvec continuous_intensity;
    continuous_intensity.load(continuous_intensity_file.c_str(),arma::raw_ascii);
    
        std::cout << "Opened file: " << continuous_intensity_file.c_str() << " n_elements: " << continuous_intensity.size() << std::endl;
    
    // open global intensity file
    arma::colvec global_intensity;
    global_intensity.load(global_intensity_file,arma::raw_ascii);
    
    // compute features, add them to desc and return 
    //std::vector<ACMediaTimedFeature*> descmf; // we could use that if we want to save again the continuous intensity
    std::vector<ACMediaFeatures*> desc;
    ACMediaFeatures *tmp, *tmp2, *tmp3;
    // add global intensity
    tmp=new ACMediaFeatures();
    tmp->setName("Global intensity");
    tmp->setDistanceType(FeaturesVector::Euclidean);
    tmp->addFeatureElement(global_intensity.at(0));
    /*tmp->addFeatureElement(10.0);
    tmp->addFeatureElement(20.0);
    tmp->addFeatureElement(30.0);*/
    tmp->setComputed();
    desc.push_back(tmp);
    std::cout << "Adding ACmediaFeatures: " << tmp->getName() << " n_elements: " << tmp->getFeaturesVector().size() << std::endl;
    //std::cout <<  tmp->getFeaturesVector()->vDense(0) << std::endl;
    
    
    // add average instantaneous intensity
    tmp2=new ACMediaFeatures();
    tmp2->setName("Continuous intensity: Mean");
    tmp2->setDistanceType(FeaturesVector::Euclidean);
    tmp2->addFeatureElement(arma::mean(continuous_intensity));
    tmp2->setComputed();
    desc.push_back(tmp2);
    std::cout << "Adding ACmediaFeatures: " << tmp2->getName() << " n_elements: " << tmp2->getFeaturesVector().size() << std::endl;
    //std::cout <<  tmp->getFeaturesVector()->vDense(0) << std::endl;
    
    // add std instantaneous intensity
    tmp3=new ACMediaFeatures();
    tmp3->setName("Continuous intensity: Std");
    tmp3->setDistanceType(FeaturesVector::Euclidean);
    tmp3->addFeatureElement(arma::stddev(continuous_intensity));
    tmp3->setComputed();
    desc.push_back(tmp3);
    std::cout << "Adding ACmediaFeatures: " << tmp3->getName() << " n_elements: " << tmp3->getFeaturesVector().size() << std::endl;
    //std::cout <<  tmp->getFeaturesVector()->vDense(0) << std::endl;
    /*delete tmp; 
    delete tmp2; 
    delete tmp3;*/

    return desc;
}

// the plugin should know internally where it saved the mtf
//ACMediaTimedFeature* ACLaughterIntensityPlugin::getTimedFeatures(){
//	if (mtf_file_name == ""){
//        cout << "<ACLaughterIntensityPlugin::getTimedFeatures> : missing file name "<<endl;
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
//ACMediaTimedFeature* ACLaughterIntensityPlugin::getTimedFeatures(){
//	if (mtf_file_names.size() == 0){
//        cout << "<ACLaughterIntensityPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}



