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

ACCsvLoggingPlugin::ACCsvLoggingPlugin() : QObject(), ACPluginQt(), ACClientPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle UI Logging";
    this->mDescription = "Plugin for logging the user interface (pointers positions, media actions) in a csv file";
    this->mId = "";
    this->actions_filepath = "";
    this->pointers_filepath = "";
    this->positions_filepath = "";
    this->delim =",";
    this->pointer_valid = false;
}

ACCsvLoggingPlugin::~ACCsvLoggingPlugin()
{
    if(isLibraryOpened(actions_file)){
        closeLibrary(actions_file);
    }
    if(isLibraryOpened(pointers_file)){
        closeLibrary(pointers_file);
    }
    if(isLibraryOpened(positions_file)){
        closeLibrary(positions_file);
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
    if(action == "reset"){
        pointers_file.close();
        actions_file.close();
        positions_file.close();
        return true;
    }

    if(!isLibraryOpened(pointers_file)){

        time_t t = time(0);
        struct tm * now = localtime( & t );
        std::stringstream filename;
        filename << getExecutablePath() << "MediaCyclePointersLog-" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << '-' << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << ".csv";
        pointers_filepath = filename.str();
        std::cout << "Logging pointers on file: " << pointers_filepath << std::endl;

        openLibrary(pointers_file,pointers_filepath);
        //pointers_file << "\"Time\"" << delim << "\"Action\"" << delim << "\"ID\"" << delim << "\"x\"" << delim << "\"y\"" << std::endl;
        pointers_file << "\"h\"" << delim << "\"min\"" << delim << "\"s\"" << delim << "\"ms\"" << delim << "\"x\"" << delim << "\"y\"" << std::endl;
    }
    if(!isLibraryOpened(actions_file)){

        time_t t = time(0);
        struct tm * now = localtime( & t );
        std::stringstream filename;
        filename << getExecutablePath() << "MediaCycleActionsLog-" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << '-' << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << ".csv";
        actions_filepath = filename.str();
        std::cout << "Logging actions on file: " << actions_filepath << std::endl;

        openLibrary(actions_file,actions_filepath);
        //actions_file << "\"Time\"" << delim << "\"Action\"" << delim << "\"Media ID\"" << delim << "\"Arguments\"" << delim << "\"1\"" << delim << "\"2\"" << delim << "\"3\"" << std::endl;
        actions_file << "\"h\"" << delim << "\"min\"" << delim << "\"s\"" << delim << "\"ms\"" << delim<< "\"Action\"" << delim << "\"Media ID\"" << std::endl;
    }
    if(!isLibraryOpened(positions_file)){

        time_t t = time(0);
        struct tm * now = localtime( & t );
        std::stringstream filename;
        filename << getExecutablePath() << "MediaCyclePositionsLog-" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << '-' << now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec << ".csv";
        positions_filepath = filename.str();
        std::cout << "Logging positions on file: " << positions_filepath << std::endl;

        openLibrary(positions_file,positions_filepath);
        positions_file << "\"Media ID\"" << delim << "\"Filename\"" << delim << "\"x\"" << delim << "\"y\"" << std::endl;
    }

    time_t t = time(0);
    struct tm * now = localtime( & t );
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    gettimeofday(&tv, &tz);

    if(action == "xml loaded"){
        std::string filename = "";
        try{
            filename = boost::any_cast<std::string>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "Couldn't retrieve xml filename from arguments" << std::endl;
            return false;
        }

        positions_file << "xml" << delim << filename << delim << 0 << delim << 0 << std::endl;
        actions_file << now->tm_hour << delim << now->tm_min << delim << now->tm_sec << delim << tv.tv_usec/1000.0f << delim << "xml loaded" << delim << filename << std::endl;
        pointer_valid = false;
    }

    if(action == "hover pointer index" || action == "hover pointer id"){

        std::vector<float> args = std::vector<float>(3,0.0f);

        for(int arg=1/*0*/;arg<arguments.size();arg++){
            std::string new_value = "";
            //pointers_file << delim;
            try{
                new_value = boost::any_cast<std::string>(arguments[arg]);
                //pointers_file << new_value;
            }
            catch(const boost::bad_any_cast &){
                //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to string, aborting..."<< std::endl;
                float float_arg (0.0f);
                try{
                    float_arg = boost::any_cast<float>(arguments[arg]);
                    //pointers_file << float_arg;
                    args[arg] = float_arg;
                }
                catch(const boost::bad_any_cast &){
                    //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to float, aborting..."<< std::endl;
                    int int_arg (-1);
                    try{
                        int_arg = boost::any_cast<int>(arguments[arg]);
                        //pointers_file << int_arg;
                        args[arg] = (float) int_arg;
                    }
                    catch(const boost::bad_any_cast &){
                        //std::cerr << "ACCsvLoggingPlugin::mediaActionPerformed: couldn't convert to int, aborting..."<< std::endl;
                    }
                }
            }
        }

        if(args[1] != 0.0f || args[2] != 0.0f){

            if(!pointer_valid){
                std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();
                for (int i=0; i<ids.size(); i++){
                    ACMediaNode* node = media_cycle->getMediaNode(i);
                    std::string filename = media_cycle->getMediaFileName(i);
                    ACPoint pos = node->getCurrentPosition();
                    if(this->browser_renderer){
                        pos = this->browser_renderer->getScreenCoordinates(pos);
                    }
                    if(node){
                        positions_file << node->getMediaId() << delim << filename << delim << pos.x << delim << pos.y << std::endl;
                    }
                }
            }
            pointer_valid = true;
        }
        if(pointer_valid){
            pointers_file << now->tm_hour << delim << now->tm_min << delim << now->tm_sec << delim << tv.tv_usec/1000.0f;
            //pointers_file << delim << action;
            pointers_file << delim << args[1];
            pointers_file << delim << args[2];
            pointers_file << std::endl;
        }

    }
    else{
        // Skipping unnecessarry logging
        if(action != "loop" && action != "hear" && action != "submit"  && action != "target"  && action != "success")
            return true;

        actions_file << now->tm_hour << delim << now->tm_min << delim << now->tm_sec << delim << tv.tv_usec/1000.0f << delim;
        actions_file << action << delim << mediaId;
		/*actions_file << delim;
        actions_file << arguments.size();
        int arg=0;
        for(int arg=0;arg<arguments.size();arg++){
            std::string new_value = "";
            actions_file << delim;
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
            actions_file << delim;
            arg++;
        }*/
        actions_file << std::endl;
    }


    return true;
}

