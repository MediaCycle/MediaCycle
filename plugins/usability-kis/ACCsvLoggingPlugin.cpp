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

#include "ACCsvLoggingPlugin.h"

#include <vector>
#include <string>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;

ACCsvLoggingPlugin::ACCsvLoggingPlugin() : ACClientPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle UI Logging";
    this->mDescription = "Plugin for logging the user interface (pointers positions, media actions) in a csv file";
    this->mId = "";
    this->actions_filepath = "";
    this->pointers_filepath = "";
}

ACCsvLoggingPlugin::~ACCsvLoggingPlugin()
{
    if(isLibraryOpened(actions_file)){
        closeLibrary(actions_file);
    }
    if(isLibraryOpened(pointers_file)){
        closeLibrary(pointers_file);
    }
}

bool ACCsvLoggingPlugin::openLibrary(std::ofstream& file, std::string& filepath){
    file.open(filepath.c_str());
    return file.is_open();
}

bool ACCsvLoggingPlugin::closeLibrary(std::ofstream& file){
    file.close();
    return !file.is_open();
}

bool ACCsvLoggingPlugin::isLibraryOpened(std::ofstream& file){
    return file.is_open();
}

bool ACCsvLoggingPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments)
{
    if(!isLibraryOpened(pointers_file)){

        time_t t = time(0);
        struct tm * now = localtime( & t );
        std::stringstream filename;
        filename << getExecutablePath() << "MediaCyclePointersLog-" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << '-' << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << ".csv";
        pointers_filepath = filename.str();
        std::cout << "Logging pointers on file: " << pointers_filepath << std::endl;

        openLibrary(pointers_file,pointers_filepath);
        pointers_file << "\"Time\",\"Action\",\"ID\",\"x\",\"y\"" << std::endl;

    }
    if(!isLibraryOpened(actions_file)){

        time_t t = time(0);
        struct tm * now = localtime( & t );
        std::stringstream filename;
        filename << getExecutablePath() << "MediaCycleActionsLog-" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << '-' << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << ".csv";
        actions_filepath = filename.str();
        std::cout << "Logging actions on file: " << actions_filepath << std::endl;

        openLibrary(actions_file,actions_filepath);
        actions_file << "\"Time\",\"Action\",\"Media ID\",\"Arguments\",\"1\",\"2\",\"3\"" << std::endl;

    }

    time_t t = time(0);
    struct tm * now = localtime( & t );
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    gettimeofday(&tv, &tz);

    if(action == "hover pointer index" || action == "hover pointer id"){
        pointers_file << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << '-' << tv.tv_usec/1000.0f << ',';
        pointers_file << action;
        int arg=0;
        for(int arg=0;arg<arguments.size();arg++){
            std::string new_value = "";
            pointers_file << ',';
            try{
                new_value = boost::any_cast<std::string>(arguments[arg]);
                pointers_file << new_value;
            }
            catch(const boost::bad_any_cast &){
                //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to string, aborting..."<< std::endl;
                float float_arg (0.0f);
                try{
                    float_arg = boost::any_cast<float>(arguments[arg]);
                    pointers_file << float_arg;
                }
                catch(const boost::bad_any_cast &){
                    //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to float, aborting..."<< std::endl;
                    int int_arg (-1);
                    try{
                        int_arg = boost::any_cast<int>(arguments[arg]);
                        pointers_file << int_arg;
                    }
                    catch(const boost::bad_any_cast &){
                        //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to int, aborting..."<< std::endl;
                    }
                }
            }
        }
        pointers_file << std::endl;

    }
    else{
        actions_file << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << '-' << tv.tv_usec/1000.0f << ',';
        actions_file << action << ',' << mediaId << ',';
        actions_file << arguments.size();
        int arg=0;
        for(int arg=0;arg<arguments.size();arg++){
            std::string new_value = "";
            actions_file << ',';
            try{
                new_value = boost::any_cast<std::string>(arguments[arg]);
                actions_file << new_value;
            }
            catch(const boost::bad_any_cast &){
                //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to string, aborting..."<< std::endl;
                float float_arg (0.0f);
                try{
                    float_arg = boost::any_cast<float>(arguments[arg]);
                    actions_file << float_arg;
                }
                catch(const boost::bad_any_cast &){
                    //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to float, aborting..."<< std::endl;
                    int int_arg (-1);
                    try{
                        int_arg = boost::any_cast<int>(arguments[arg]);
                        actions_file << int_arg;
                    }
                    catch(const boost::bad_any_cast &){
                        //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to int, aborting..."<< std::endl;
                    }
                }
            }
        }
        while(arg<3){
            actions_file << ',';
            arg++;
        }
        actions_file << std::endl;
    }


    return true;
}

