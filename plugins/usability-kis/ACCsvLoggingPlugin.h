/**
 * @brief Plugin for logging the user interface (pointers positions, media actions) in a csv file
 * @author Christian Frisson
 * @date 26/09/2013
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

#include <MediaCycle.h>
#include "ACPlugin.h"


#ifndef _ACCsvLoggingPlugin_
#define _ACCsvLoggingPlugin_

class ACCsvLoggingPlugin : public ACClientPlugin {
public:
    ACCsvLoggingPlugin();
    ~ACCsvLoggingPlugin();
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());

    virtual bool openLibrary(std::ofstream& file, std::string& filepath);
    virtual bool closeLibrary(std::ofstream& file);
    virtual bool isLibraryOpened(std::ofstream& file);

protected:
    std::ofstream pointers_file;
    std::string pointers_filepath;
    std::ofstream actions_file;
    std::string actions_filepath;
    std::ofstream positions_file;
    std::string positions_filepath;
    std::string delim;
    bool pointer_valid;
};

#endif	/* _ACCsvLoggingPlugin_ */
