/**
 * @brief A plugin that wraps the media width, heigth and area as features.
 * @author Christian Frisson
 * @date 10/06/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACMediaSizesAsFeaturesPlugin.h"
using namespace std;

ACMediaSizesAsFeaturesPlugin::ACMediaSizesAsFeaturesPlugin() : ACFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO | MEDIA_TYPE_IMAGE | MEDIA_TYPE_MIXED;
    this->mName = "Media Sizes as Features";
    this->mDescription = "Wraps the media start/end sizes and duration as features";
    this->mId = "";
}

ACMediaSizesAsFeaturesPlugin::~ACMediaSizesAsFeaturesPlugin() {
}

ACFeatureDimensions ACMediaSizesAsFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["Media Area"] = 1;
    featureDimensions["Media Height"] = 1;
    featureDimensions["Media Width"] = 1;
    return featureDimensions;
}

std::vector<ACMediaFeatures*> ACMediaSizesAsFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;

    float width = theMedia->getWidth();
    float height = theMedia->getHeight();

    ACMediaFeatures* area_feat;
    vector<float> area_value;
    area_value.push_back(width*height);
    area_feat = new ACMediaFeatures(area_value, "Media Area");
    desc.push_back(area_feat);

    ACMediaFeatures* height_feat;
    vector<float> height_value;
    height_value.push_back(height);
    height_feat = new ACMediaFeatures(height_value, "Media Height");
    desc.push_back(height_feat);

    ACMediaFeatures* width_feat;
    vector<float> width_value;
    width_value.push_back(width);
    width_feat = new ACMediaFeatures(width_value, "Media Width");
    desc.push_back(width_feat);

    std::cout << "ACMediaSizesAsFeaturesPlugin::calculate: area " << width*height << " height " << height << " width " << width << std::endl;

    return desc;
}
