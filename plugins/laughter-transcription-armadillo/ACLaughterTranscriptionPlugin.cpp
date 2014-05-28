/*
 *  ACLaughterTranscriptionPlugin.cpp
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

#include "ACLaughterTranscriptionPlugin.h"

#include <vector>
#include <string>

//class ACMedia;

//#define BUFFERIZED

ACLaughterTranscriptionPlugin::ACLaughterTranscriptionPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "Laughter Transcription Armadillo";
    this->mDescription = "LaughterTranscription plugin";
    this->mId = "";
    this->mDistType=FeaturesVector::Cosinus;
    //this->mtf_file_name = "";
    //this->mfccNb = 13;
    this->mnPhones=16;
}

ACLaughterTranscriptionPlugin::~ACLaughterTranscriptionPlugin() {
}

ACFeatureDimensions ACLaughterTranscriptionPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Phones"] = mnPhones;
    return featureDimensions;
}

// JU:
std::vector<float> parse_csv_line(std::string line, std::string delimiter)
{
    vector <float> values;
    float val;
    size_t pos;
    
    while ((pos = line.find(delimiter)) != std::string::npos) {
        std::stringstream ss(line.substr(0, pos));
        ss >> val;
        values.push_back(val);
        line.erase(0, pos + delimiter.length());
    }
    std::stringstream ss(line.substr(0, pos));
    ss >> val;
    values.push_back(val);
    return values;
}


// JU
std::vector<ACMediaFeatures*> ACLaughterTranscriptionPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    // from MediaData
    return this->_calculate(theMedia->getFileName(),theMedia,_save_timed_feat);
}

// private method
std::vector<ACMediaFeatures*> ACLaughterTranscriptionPlugin::_calculate(std::string aFileName, ACMedia* theMedia, bool _save_timed_feat){
     // JU: filenames: ideally this should be moved somewhere else...
    std::string features_dir="/media/Data/Data/Ilhaire/databases/Belfast_Storytelling/individual_laugh_segmentation/laughter_features/";
    std::string transcription_suffix="_transcription_features.csv";
    
    // to have the name of the audio file without directory nor extension
    size_t pos=aFileName.find_last_of(".");
    size_t posSep = aFileName.find_last_of("/\\");

    // define name of transcription files
    std::string transcription_file;
    if(posSep)
    {
      transcription_file=features_dir+aFileName.substr(posSep+1,pos-posSep-1)+transcription_suffix;
    }
    else
    {
      transcription_file=features_dir+aFileName.substr(0,pos-1)+transcription_suffix;
    }
    
    // open transcription file and get features.
    std::string line, header, delimiter=";";
    vector <float> count_phone;
    vector <float> duration_phone;;
    vector <float> intensity_phone;
    vector <std::string> phones;
    ifstream fs;
    fs.open(transcription_file.c_str(),ios::in);
    while(getline(fs,line))
    {
        pos = line.find(delimiter);
        if(pos)
        {
            header=line.substr(0,pos);
            if(!strcmp(header.c_str(),"phones"))
            {
                line.erase(0, pos + delimiter.length());
                while ((pos = line.find(delimiter)) != std::string::npos) {
                  phones.push_back(line.substr(0, pos));
                  //std::cout << line.substr(0, pos) << std::endl;
                  line.erase(0, pos + delimiter.length());
                }
                phones.push_back(line);
            }
            if(!strcmp(header.c_str(),"count"))
            {
                line.erase(0, pos + delimiter.length());
                count_phone=parse_csv_line(line, delimiter);
            }
            if(!strcmp(header.c_str(),"duration"))
            {
                line.erase(0, pos + delimiter.length());
                duration_phone=parse_csv_line(line, delimiter);
            }
            if(!strcmp(header.c_str(),"intensity"))
            {
                line.erase(0, pos + delimiter.length());
                intensity_phone=parse_csv_line(line, delimiter);
            }
        }

    }
    fs.close();
    
    // here we only use the count_phones, this code could be copy-pasted for duration and intensity
    // check we have the correct number of values
    if(count_phone.size()-mnPhones)
    {
        std::cout << "Problem with number of read phones in intensity file: " << transcription_file << "\nread count phones: " << count_phone.size() << " instead of " << mnPhones << std::endl;
    }
    
    //fill in features
    std::vector<ACMediaFeatures*> desc;
    ACMediaFeatures* tmp;
    tmp=new ACMediaFeatures();
    tmp->setName("PhoneticTranscriptionCount");
    tmp->setDistanceType(FeaturesVector::Cosinus);
    tmp->setComputed();
    for(int i=0;i<mnPhones;i++)
    {
        tmp->addFeatureElement(count_phone.at(i));
    }
    desc.push_back(tmp);
    //delete tmp;
    return desc;
}

// the plugin should know internally where it saved the mtf
//ACMediaTimedFeature* ACLaughterTranscriptionPlugin::getTimedFeatures(){
//	if (mtf_file_name == ""){
//        cout << "<ACLaughterTranscriptionPlugin::getTimedFeatures> : missing file name "<<endl;
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
//ACMediaTimedFeature* ACLaughterTranscriptionPlugin::getTimedFeatures(){
//	if (mtf_file_names.size() == 0){
//        cout << "<ACLaughterTranscriptionPlugin::getTimedFeatures> : missing file name "<<endl;
//		return 0;
//	}
//	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
//	if (ps_mtf->loadFromFile(mtf_file_name) <= 0){
//		return 0;
//	}
//	return ps_mtf;
//}



