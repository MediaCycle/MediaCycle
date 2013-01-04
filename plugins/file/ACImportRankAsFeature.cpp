/**
 * @brief A plugin that wraps the import rank as feature.
 * @author Christian Frisson
 * @date 17/12/2012
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

#include "ACImportRankAsFeature.h"
using namespace std;

ACImportRankAsFeature::ACImportRankAsFeature() : ACFeaturesPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "Import Rank";
    this->mDescription = "Import Rank (convenient for the sort against the order of import)";
    this->mId = "";
    this->mDescriptorsList.push_back(this->mName);
    this->import_rank = 0;
}

ACImportRankAsFeature::~ACImportRankAsFeature() {
}

std::vector<ACMediaFeatures*> ACImportRankAsFeature::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;
    ACMediaFeatures* import_rank_feat;
    vector<float> import_rank_value;
    //import_rank_value.push_back( theMedia->getId()); // won't work, media ids are attributed after the import
    import_rank_value.push_back( this->import_rank++ ); //
    import_rank_feat = new ACMediaFeatures(import_rank_value, this->mName);
    desc.push_back(import_rank_feat);
    return desc;
}
