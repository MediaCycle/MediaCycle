/**
 * @brief A plugin that wraps the file size and last write time and directory depth as features.
 * @author Christian Frisson
 * @date 6/09/2012
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

#include "ACFileFeaturesPlugin.h"

#include <vector>
#include <string>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACFileFeaturesPlugin::ACFileFeaturesPlugin() : ACFeaturesPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "File Features";
    this->mDescription = "File Features Plugin (Size, Last Write Time, Directory Depth)";
    this->mId = "";
}

ACFileFeaturesPlugin::~ACFileFeaturesPlugin() {	
}

ACFeatureDimensions ACFileFeaturesPlugin::getFeaturesDimensions(){
    ACFeatureDimensions featureDimensions;
    featureDimensions["File Size"] = 1;
    featureDimensions["File Last Write Time"] = 1;
    featureDimensions["File Directory Depth"] = 1;
    return featureDimensions;
}

std::vector<ACMediaFeatures*> ACFileFeaturesPlugin::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    std::vector<ACMediaFeatures*> desc;

    std::string file_name = theMedia->getFileName();
    fs::path p( file_name.c_str());// , fs::native );

    if ( !fs::exists( p ) )
    {
        std::cout << "ACFileFeaturesPlugin::calculate: file not found: " << file_name << std::endl;
        return desc;
    }

    if ( !fs::is_regular( p ) )
    {
        std::cout << "ACFileFeaturesPlugin::calculate: file is not a regular file: " << file_name << std::endl;
        return desc;
    }

    std::cout << "ACFileFeaturesPlugin::calculate:  size of " << file_name << " is " << fs::file_size( p ) << std::endl;

    ACMediaFeatures* size_feat;
    vector<float> size_value;
    size_value.push_back(fs::file_size( p ));
    size_feat = new ACMediaFeatures(size_value, "File Size");
    desc.push_back(size_feat);

    ACMediaFeatures* last_write_time_feat;
    vector<float> last_write_time_value;
    last_write_time_value.push_back(fs::last_write_time( p ));
    last_write_time_feat = new ACMediaFeatures(last_write_time_value, "File Last Write Time");
    desc.push_back(last_write_time_feat);

    char slash('/');
    #ifdef __WIN32__
    slash = '\\';
    #endif
    ACMediaFeatures* directory_depth_feat;
    vector<float> directory_depth_value;
    directory_depth_value.push_back( (int) std::count(file_name.begin(), file_name.end(), slash) );
    directory_depth_feat = new ACMediaFeatures(directory_depth_value, "File Directory Depth");
    desc.push_back(directory_depth_feat);

    return desc;
}

