/*
 *  ACArchipelFeaturesPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/09/2012
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

#include "ACArchipelFeaturesPlugin.h"
#include "ACArchipelReaderPlugin.h"
#include "ACArchipelReader.h"

#include <vector>
#include <string>

ACArchipelFeaturesPlugin::ACArchipelFeaturesPlugin() {

    this->mMediaType = MEDIA_TYPE_MIXED;
    this->mName = "Archipel Features";
    this->mDescription = "Archipel Features Plugin (Year, First Atoll)";
    this->mId = "";
    known_atolls.push_back("unknown");
    known_atolls.push_back("micro/macro");
    known_atolls.push_back("recyclage");
    known_atolls.push_back("temps");
    known_atolls.push_back("aleas");
    known_atolls.push_back("bruits");
    known_atolls.push_back("utopie");
    known_atolls.push_back("temoins");
    known_atolls.push_back("silence");
    known_atolls.push_back("corps");
    known_atolls.push_back("bonjour!");
    known_atolls.push_back("espace");
}

ACArchipelFeaturesPlugin::~ACArchipelFeaturesPlugin() {
}

ACFeatureDimensions ACArchipelFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Year"] = 1;
    featureDimensions["First Atoll"] = 1;
    return featureDimensions;
}

std::vector<ACMediaFeatures*> ACArchipelFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;

    std::string filename("");
    filename = theMedia->getFileName();
    if(filename == ""){
        std::cerr << "ACArchipelFeaturesPlugin::calculate: couldn't find the related archipel document" << std::endl;
        return desc;
    }

    string extension = boost::filesystem::extension(filename);
    string path=boost::filesystem::path(filename).parent_path().string()+string("/");
    if (extension!=string(".xml"))
        return desc;

    ACArchipelReader* xmlDoc=new ACArchipelReader(filename);
    if (!xmlDoc->isArchipel())
        return desc;

    float year = (float)(xmlDoc->getYear());
    ACMediaFeatures* year_feat;
    vector<float> year_value;
    year_value.push_back(year);
    year_feat = new ACMediaFeatures(year_value, "Year");
    desc.push_back(year_feat);

    std::vector<std::string> media_atolls;
    float first_atoll_index = 0;
    media_atolls = xmlDoc->getIlot();
    std::vector<std::string>::iterator first_atoll_iter = std::find(known_atolls.begin(),known_atolls.end(),media_atolls.front());
    if(first_atoll_iter!=known_atolls.end())
        first_atoll_index = (float) std::distance(known_atolls.begin(),first_atoll_iter);
    else
        first_atoll_index = 0;

    ACMediaFeatures* first_atoll_feat;
    vector<float> first_atoll_value;
    first_atoll_value.push_back(first_atoll_index);
    first_atoll_feat = new ACMediaFeatures(first_atoll_value, "First Atoll");
    desc.push_back(first_atoll_feat);


    return desc;
}

