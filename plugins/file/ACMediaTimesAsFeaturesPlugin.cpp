/**
 * @brief A plugin that wraps the media start/end times and duration as features.
 * @author Christian Frisson
 * @date 10/10/2012
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

#include "ACMediaTimesAsFeaturesPlugin.h"
using namespace std;

ACMediaTimesAsFeaturesPlugin::ACMediaTimesAsFeaturesPlugin() : ACFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO | MEDIA_TYPE_AUDIO;
    this->mName = "Media Times as Features";
    this->mDescription = "Wraps the media start/end times and duration as features";
    this->mId = "";
    this->mDescriptorsList.push_back("Media Start Time");
    this->mDescriptorsList.push_back("Media End Time");
    this->mDescriptorsList.push_back("Media Duration");
}

ACMediaTimesAsFeaturesPlugin::~ACMediaTimesAsFeaturesPlugin() {
}

std::vector<ACMediaFeatures*> ACMediaTimesAsFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;

    ACMediaFeatures* start_feat;
    vector<float> start_value;
    start_value.push_back(theMedia->getStart());
    start_feat = new ACMediaFeatures(start_value, "Media Start Time");
    desc.push_back(start_feat);

    ACMediaFeatures* end_feat;
    vector<float> end_value;
    end_value.push_back(theMedia->getEnd());
    end_feat = new ACMediaFeatures(end_value, "Media End Time");
    desc.push_back(end_feat);

    ACMediaFeatures* duration_feat;
    vector<float> duration_value;
    duration_value.push_back( theMedia->getEnd() - theMedia->getStart());
    duration_feat = new ACMediaFeatures(duration_value, "Media Duration");
    desc.push_back(duration_feat);

    std::cout << "ACMediaTimesAsFeaturesPlugin::calculate: start " << theMedia->getStart() << " end " << theMedia->getEnd() << std::endl;

    return desc;
}
