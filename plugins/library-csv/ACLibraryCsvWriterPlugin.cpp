/**
 * @brief A plugin that allows to save media libraries as csv files.
 * @author Christian Frisson
 * @date 20/06/2013
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

#include "ACLibraryCsvWriterPlugin.h"

#include <vector>
#include <string>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACLibraryCsvWriterPlugin::ACLibraryCsvWriterPlugin() : ACMediaLibraryWriterPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "CSV export";
    this->mDescription = "Plugin for saving media libraries as comma-separated values files";
    this->mId = "";

    with_media_id = true;
    this->addNumberParameter("Media ID",1,0,1,1,"Save the media ID",boost::bind(&ACLibraryCsvWriterPlugin::withMediaId,this));
}

ACLibraryCsvWriterPlugin::~ACLibraryCsvWriterPlugin() {
}

void ACLibraryCsvWriterPlugin::withMediaId(){
    this->with_media_id = this->getNumberParameterValue("Media ID");
}

bool ACLibraryCsvWriterPlugin::openLibrary(std::string filepath){
    file.open(filepath.c_str());
    return file.is_open();
}

bool ACLibraryCsvWriterPlugin::closeLibrary(std::string filepath){
    file.close();
    return !file.is_open();
}

bool ACLibraryCsvWriterPlugin::isLibraryOpened(std::string filepath){
    return file.is_open();
}

std::vector<std::string> ACLibraryCsvWriterPlugin::fileFormats(){
    std::vector<std::string> formats;
    formats.push_back(".csv");
    return formats;
}

bool ACLibraryCsvWriterPlugin::saveLibraryMetadata(){
    if(!file.is_open())
        return false;
    file << "\"Filename\"";
    if(with_media_id)
        file << ",\"Media ID\"";
    file << std::endl;
    return true;
}

bool ACLibraryCsvWriterPlugin::saveMedia(ACMedia* media){
    if(!file.is_open())
        return false;
    file << "\"" << media->getFileName() << "\"";
    if(with_media_id)
        file << ",\"" << media->getId() << "\"";
    file << std::endl;
    return true;
}

