/**
 * @brief Process to extract video features through MediaCycle commandline interfaces (CLI)
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

#include "ACImportProcess.h"
#include<iostream>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACImportProcess::ACImportProcess(MediaCycle* _media_cycle)
    : ACAbstractProcessCLI(_media_cycle)
{
}

ACImportProcess::~ACImportProcess(){
}

void ACImportProcess::run(int argc, char *argv[]){
    std::cout << "ACImportProcess: number of input arguments " << argc << std::endl;
    //for(int arg=0; arg<argc;arg++)
    //    std::cout << "ACImportProcess: argument " << arg << ": '" << argv[arg] << "'" << std::endl;

    fs::path cli_path(argv[0]);
    std::string cli_name("");
    cli_name = cli_path.stem().string();
    std::cout << "ACImportProcess: cli name '" << cli_name << "'" << std::endl;

    bool recursive = false;
    bool forward_order=true;
    bool doSegment=false;
    int doSegmentArg = 0;
    bool _save_timed_feat=false;
    bool saveCSV=false;
    std::string csv("");
    int saveCSVarg = 0;
    bool saveXML=false;
    std::string xml("");
    int saveXMLarg = 0;
    bool saveJSON=false;
    std::string json("");
    int saveJSONarg = 0;
    std::string path("");
    std::string segmentation_plugin("");

    if(argc < 2){
        std::cerr << "ACImportProcess: expecting at least one argument: filename or directory" << std::endl;
        std::cerr << std::endl << "usage: " << cli_name << " [options] <path_to_folder>" << std::endl;
        std::cerr << "options:" << std::endl;
        std::cerr << "\t-r\tfor recursive" << std::endl;
        std::cerr << "\t-mtf\tto save media timed features" << std::endl;
        std::cerr << "\t-s [\"optional name\"]\tfor segment" << std::endl;
        std::cerr << "\t-csv [\"optional name\"]\tto save as CSV" << std::endl;
        std::cerr << "\t-xml [\"optional name\"]\tto save as XML" << std::endl;
        std::cerr << "\t-json [\"optional name\"]\tto save as JSON" << std::endl;
        std::cerr << std::endl;
        return;
    }

    for(int arg=0; arg<argc;arg++){
        std::cout << "ACImportProcess: argument " << arg << ": '" << argv[arg] << "'" << std::endl;
        if( strcmp(argv[arg],"-r") == 0)
            recursive = true;
        else if( strcmp(argv[arg],"-s") == 0){
            doSegment = true;
            doSegmentArg = arg;
        }
        else if( strcmp(argv[arg],"-mtf") == 0)
            _save_timed_feat = true;
        else if( strcmp(argv[arg],"-csv") == 0){
            saveCSV = true;
            saveCSVarg = arg;
        }
        else if( strcmp(argv[arg],"-xml") == 0){
            saveXML = true;
            saveXMLarg = arg;
        }
        else if( strcmp(argv[arg],"-json") == 0){
            saveJSON = true;
            saveJSONarg = arg;
        }
        else{
            // if the previous arg was "-xml" and the current arg isn't another flag or the last arg
            if( saveCSVarg != 0 && arg == saveCSVarg+1 && arg < argc-1 )
                csv = argv[arg];
            else if( saveXMLarg != 0 && arg == saveXMLarg+1 && arg < argc-1 )
                xml = argv[arg];
            else if( saveJSONarg != 0 && arg == saveJSONarg+1 && arg < argc-1 )
                json = argv[arg];
            else if( doSegmentArg != 0 && arg == doSegmentArg+1 && arg < argc-1 )
                segmentation_plugin = argv[arg];
            else if (arg == argc-1)
                path = argv[arg];
        }
    }
    if(doSegment)
        _save_timed_feat = true;

    if(!media_cycle){
        std::cerr << "ACImportProcess:: no mediacycle instance available " << std::endl;
        return;
    }

    if(path==""){
        std::cerr << "ACImportProcess:: file or folder not specified " << std::endl;
        return;
    }

    fs::path folder_path(path.c_str());
    if ( !fs::exists( folder_path ) )
    {
        std::cerr << "ACImportProcess:: file or folder '" << path << "' doesn't exist " << std::endl;
        return;
    }

    bool directory = fs::is_directory(folder_path);
    if(directory){
        std::cout << "ACImportProcess:: importing folder '" << path << "'" << std::endl;
    }
    else{
        if ( !fs::is_regular( folder_path ) )
        {
            std::cerr << "ACImportProcess:: file or folder '" << path << "' is not regular " << std::endl;
            return;
        }
        if( fs::file_size( folder_path ) <= 0 ){
            std::cerr << "ACImportProcess:: file or folder '" << path << "' is of null size " << std::endl;
            return;
        }
        std::cout << "ACImportProcess:: importing file '" << path << "'" << std::endl;
    }

    if(saveCSV && csv==""){
        std::stringstream csv_path;
        std::string slash = "/";
#ifdef WIN32
        slash = "\\";
#endif
        csv_path << folder_path.parent_path().string() << slash << folder_path.stem().string();
        csv_path << ".csv";
        csv = csv_path.str();
        std::cout <<  "ACImportProcess: no csv file specified, will save as '" << csv << "'" << std::endl;
    }
    if(saveXML && xml==""){
        std::stringstream xml_path;
        std::string slash = "/";
#ifdef WIN32
        slash = "\\";
#endif
        xml_path << folder_path.parent_path().string() << slash << folder_path.stem().string();
        xml_path << ".xml";
        xml = xml_path.str();
        std::cout <<  "ACImportProcess: no xml file specified, will save as '" << xml << "'" << std::endl;
    }
    if(saveJSON && json==""){
        std::stringstream json_path;
        std::string slash = "/";
#ifdef WIN32
        slash = "\\";
#endif
        json_path << folder_path.parent_path().string() << slash << folder_path.stem().string();
        json_path << ".json";
        json = json_path.str();
        std::cout <<  "ACImportProcess: no JSON file specified, will save as '" << json << "'" << std::endl;
    }
    if(doSegment){
        std::vector<std::string> segment_plugins = media_cycle->getAvailablePluginNames(PLUGIN_TYPE_SEGMENTATION,media_cycle->getMediaType());
        if(segment_plugins.size() == 0){
            std::cerr << "ACImportProcess: no segment plugin available, turning off segmentation" << std::endl;
            doSegment = false;
        }
        else {
            if(segmentation_plugin==""){
                segmentation_plugin = segment_plugins.front();
                std::cerr << "ACImportProcess: no segmentation plugin set, using first available plugin from the list: '" << segment_plugins.front() << "'" << std::endl;
            }
            if( std::find( segment_plugins.begin(),segment_plugins.end(),segmentation_plugin ) == segment_plugins.end()){
                std::cerr << "ACImportProcess: couldn't find required segmentation plugin '" << segmentation_plugin << "', using first plugin available from the list: '" << segment_plugins.front() << "'" << std::endl;
                segmentation_plugin = segment_plugins.front();
            }
            else
                std::cerr << "ACImportProcess: using segmentation plugin '" << segmentation_plugin << "'" << std::endl;
        }
        media_cycle->getPluginManager()->setActiveSegmentPlugin(segmentation_plugin);
    }

    media_cycle->importDirectory(
                path,
                recursive /*int recursive*/,
                forward_order /*bool forward_order=true*/,
                doSegment /*bool doSegment=false*/,
                _save_timed_feat /*bool _save_timed_feat=false*/
                );

    if(saveCSV){
        std::cout <<  "ACImportProcess: saving as '" << csv << "'" << std::endl;

        try{
            media_cycle->saveLibrary(csv,"CSV export");
        }
        catch (const exception& e) {
            std::cerr << "ACImportProcess: couldn't save as '" << csv << "' due to error: " << e.what() << std::endl;
            return;
        }

        fs::path csv_path(csv.c_str());
        if ( !fs::exists( csv_path ) )
        {
            std::cerr << "ACImportProcess:: file '" << csv << "' doesn't exist " << std::endl;
            return;
        }

        if ( !fs::is_regular( csv_path ) )
        {
            std::cerr << "ACImportProcess:: file or folder '" << csv << "' is not regular " << std::endl;
            return;
        }

        if( fs::file_size( csv_path ) <= 0 ){
            std::cerr << "ACImportProcess:: file or folder '" << csv << "' is of null size " << std::endl;
            return;
        }
    }

    if(saveXML){
        std::cout <<  "ACImportProcess: saving as '" << xml << "'" << std::endl;
        media_cycle->saveXMLConfigFile(xml);

        fs::path xml_path(xml.c_str());
        if ( !fs::exists( xml_path ) )
        {
            std::cerr << "ACImportProcess:: file '" << xml << "' doesn't exist " << std::endl;
            return;
        }

        if ( !fs::is_regular( xml_path ) )
        {
            std::cerr << "ACImportProcess:: file or folder '" << xml << "' is not regular " << std::endl;
            return;
        }

        if( fs::file_size( xml_path ) <= 0 ){
            std::cerr << "ACImportProcess:: file or folder '" << xml << "' is of null size " << std::endl;
            return;
        }
    }

    if(saveJSON){
        std::cout <<  "ACImportProcess: saving as '" << json << "'" << std::endl;

        try{
            media_cycle->saveLibrary(json,"JSON export");
        }
        catch (const exception& e) {
            std::cerr << "ACImportProcess: couldn't save as '" << json << "' due to error: " << e.what() << std::endl;
            return;
        }

        fs::path json_path(json.c_str());
        if ( !fs::exists( json_path ) )
        {
            std::cerr << "ACImportProcess:: file '" << json << "' doesn't exist " << std::endl;
            return;
        }

        if ( !fs::is_regular( json_path ) )
        {
            std::cerr << "ACImportProcess:: file or folder '" << json << "' is not regular " << std::endl;
            return;
        }

        if( fs::file_size( json_path ) <= 0 ){
            std::cerr << "ACImportProcess:: file or folder '" << json << "' is of null size " << std::endl;
            return;
        }
    }
}
