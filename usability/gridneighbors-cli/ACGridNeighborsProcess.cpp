/**
 * @brief Process to compare proximity grids through MediaCycle commandline interfaces (CLI)
 * @author Christian Frisson
 * @date 11/03/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#include "ACGridNeighborsProcess.h"
#include<iostream>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACGridNeighborsProcess::ACGridNeighborsProcess(MediaCycle* _media_cycle)
    : ACAbstractProcessCLI(_media_cycle)
{
}

ACGridNeighborsProcess::~ACGridNeighborsProcess(){
}

void ACGridNeighborsProcess::run(int argc, char *argv[]){
    std::cout << "ACGridNeighborsProcess: number of input arguments " << argc << std::endl;
    //for(int arg=0; arg<argc;arg++)
    //    std::cout << "ACGridNeighborsProcess: argument " << arg << ": '" << argv[arg] << "'" << std::endl;

    fs::path cli_path(argv[0]);
    std::string cli_name("");
    cli_name = cli_path.stem().string();
    std::cout << "ACGridNeighborsProcess: cli name '" << cli_name << "'" << std::endl;

    bool recursive = false;
    bool forward_order=true;
    bool doSegment=false;
    int doSegmentArg = 0;
    bool _save_timed_feat=false;
    bool saveXML=false;
    std::string xml("");
    int saveXMLarg = 0;
    std::string path("");
    std::string segmentation_plugin("");

    if(argc < 2){
        std::cerr << "ACGridNeighborsProcess: expecting at least one argument: filename or directory" << std::endl;
        std::cerr << std::endl << "usage: " << cli_name << " [options] <path_to_folder>" << std::endl;
        std::cerr << "options:" << std::endl;
        std::cerr << "\t-r\tfor recursive" << std::endl;
        //std::cerr << "\t-mtf\tto save media timed features" << std::endl;
        //std::cerr << "\t-s [\"optional name\"]\tfor segment" << std::endl;
        //std::cerr << "\t-xml [\"optional name\"]\tto save as XML" << std::endl;
        std::cerr << std::endl;
        return;
    }

    for(int arg=0; arg<argc;arg++){
        std::cout << "ACGridNeighborsProcess: argument " << arg << ": '" << argv[arg] << "'" << std::endl;
        if( strcmp(argv[arg],"-r") == 0)
            recursive = true;
        else if( strcmp(argv[arg],"-s") == 0){
            doSegment = true;
            doSegmentArg = arg;
        }
        else if( strcmp(argv[arg],"-mtf") == 0)
            _save_timed_feat = true;
        else if( strcmp(argv[arg],"-xml") == 0){
            saveXML = true;
            saveXMLarg = arg;
        }
        else{
            // if the previous arg was "-xml" and the current arg isn't another flag or the last arg
            if( saveXMLarg != 0 && arg == saveXMLarg+1 && arg < argc-1 )
                xml = argv[arg];
            else if( doSegmentArg != 0 && arg == doSegmentArg+1 && arg < argc-1 )
                segmentation_plugin = argv[arg];
            else if (arg == argc-1)
                path = argv[arg];
        }
    }
    if(doSegment)
        _save_timed_feat = true;

    if(!media_cycle){
        std::cerr << "ACGridNeighborsProcess:: no mediacycle instance available " << std::endl;
        return;
    }

    if(path==""){
        std::cerr << "ACGridNeighborsProcess:: file or folder not specified " << std::endl;
        return;
    }

    fs::path folder_path(path.c_str());
    if ( !fs::exists( folder_path ) )
    {
        std::cerr << "ACGridNeighborsProcess:: file or folder '" << path << "' doesn't exist " << std::endl;
        return;
    }



    bool directory = fs::is_directory(folder_path);
    bool isXML = false;
    if(directory){
        std::cout << "ACGridNeighborsProcess:: importing folder '" << path << "'" << std::endl;
    }
    else{


//        std::cout << "ACGridNeighborsProcess:: requires a directory as path instead of '" << path << "'" << std::endl;
//        return;

        if ( !fs::is_regular( folder_path ) )
        {
            std::cerr << "ACGridNeighborsProcess:: file or folder '" << path << "' is not regular " << std::endl;
            return;
        }
        if( fs::file_size( folder_path ) <= 0 ){
            std::cerr << "ACGridNeighborsProcess:: file or folder '" << path << "' is of null size " << std::endl;
            return;
        }

        std::string ext = fs::extension( folder_path.string() );

        if(ext != ".xml"){
            std::cerr << "ACGridNeighborsProcess:: file '" << path << "' is not an "<< ext <<" file " << std::endl;
            //return;
        }

        isXML = true;

        std::cout << "ACGridNeighborsProcess:: importing file '" << path << "'" << std::endl;
    }

    std::string folder = folder_path.stem().string();
    std::cout << "ACGridNeighborsProcess:: importing folder '" << folder << "'" << std::endl;


    /*if(saveXML && xml==""){
        std::stringstream xml_path;
        std::string slash = "/";
#ifdef WIN32
        slash = "\\";
#endif
        xml_path << folder_path.parent_path().string() << slash << folder_path.stem().string();
        xml_path << ".xml";
        xml = xml_path.str();
        std::cout <<  "ACGridNeighborsProcess: no xml file specified, will save as '" << xml << "'" << std::endl;
    }*/
    if(doSegment){
        std::vector<std::string> segment_plugins = media_cycle->getAvailablePluginNames(PLUGIN_TYPE_SEGMENTATION,media_cycle->getMediaType());
        if(segment_plugins.size() == 0){
            std::cerr << "ACGridNeighborsProcess: no segment plugin available, turning off segmentation" << std::endl;
            doSegment = false;
        }
        else {
            if(segmentation_plugin==""){
                segmentation_plugin = segment_plugins.front();
                std::cerr << "ACGridNeighborsProcess: no segmentation plugin set, using first available plugin from the list: '" << segment_plugins.front() << "'" << std::endl;
            }
            if( std::find( segment_plugins.begin(),segment_plugins.end(),segmentation_plugin ) == segment_plugins.end()){
                std::cerr << "ACGridNeighborsProcess: couldn't find required segmentation plugin '" << segmentation_plugin << "', using first plugin available from the list: '" << segment_plugins.front() << "'" << std::endl;
                segmentation_plugin = segment_plugins.front();
            }
            else
                std::cerr << "ACGridNeighborsProcess: using segmentation plugin '" << segmentation_plugin << "'" << std::endl;
        }
        media_cycle->getPluginManager()->setActiveSegmentPlugin(segmentation_plugin);
    }

    std::string import_grid("MediaCycle Import Grid");
    ACPlugin* import_grid_plugin = media_cycle->getPlugin(import_grid);
    if(!import_grid_plugin){
        std::cerr << "Couldn't open plugin: " << import_grid << std::endl;
        return;
    }

    std::string pca_tsne("MediaCycle PCA t_Sne");
    ACPlugin* pca_tsne_plugin = media_cycle->getPlugin(pca_tsne);
    if(!pca_tsne_plugin){
        std::cerr << "Couldn't open plugin: " << pca_tsne << std::endl;
        return;
    }

    std::string proximity_grid("Proximity Grid");
    ACPlugin* proximity_grid_plugin = media_cycle->getPlugin(proximity_grid);
    if(!proximity_grid_plugin){
        std::cerr << "Couldn't open plugin: " << proximity_grid << std::endl;
        return;
    }

    std::string unispring("MediaCycle Unispring");
    ACPlugin* unispring_plugin = media_cycle->getPlugin(unispring);
    if(!unispring_plugin){
        std::cerr << "Couldn't open plugin: " << unispring << std::endl;
        return;
    }

    if(directory){
    media_cycle->importDirectory(
                path,
                recursive /*int recursive*/,
                forward_order /*bool forward_order=true*/,
                doSegment /*bool doSegment=false*/,
                _save_timed_feat /*bool _save_timed_feat=false*/
                );
    }
    else if(isXML){
        media_cycle->importXMLLibrary(path);
    }
    else
        return;


    // Normalize features
    media_cycle->normalizeFeatures(1);

    // Initialize media nodes
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();
    for (std::vector<long>::iterator id = ids.begin();id != ids.end(); id++)
        media_cycle->getBrowser()->initializeNode(*id);

    // Initialize weights
    media_cycle->initializeFeatureWeights();

    std::vector<float> weights = media_cycle->getWeightVector();
    int nbFeature = media_cycle->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    std::cout << "weights" << weights.size() << std::endl;
    std::cout << "nbFeatures" << nbFeature << std::endl;

    if(weights.size() == 0){
        weights = std::vector<float>(nbFeature,1.0f);
        media_cycle->setWeightVector(weights);
    }
    std::cout << "weights" << weights.size() << std::endl;
    std::cout << "nbFeatures" << nbFeature << std::endl;

    std::cout << "Computing the " << proximity_grid << " over the " <<  import_grid << std::endl;
    media_cycle->getBrowser()->setClustersPositionsPlugin(import_grid_plugin);
    media_cycle->getLibrary()->setTitle( folder+"-"+import_grid+"-"+proximity_grid );
    media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
    media_cycle->updateDisplay(false);

    exit(1);

    std::cout << "Computing the " << proximity_grid << " over the " <<  pca_tsne << std::endl;
    media_cycle->getBrowser()->setClustersPositionsPlugin(pca_tsne_plugin);
    media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+proximity_grid );
    //media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
    media_cycle->updateDisplay(false);

    arma::mat pca_tsne_pos = this->snapshotPositions();

    //Example of naming differently along the grid size
    /*
    {
        int min_grid = proximity_grid_plugin->getNumberParameterMin("Grid side");
        proximity_grid_plugin->setNumberParameterValue("Grid side",min_grid);
        std::cout << "Min grid size " << min_grid << std::endl;
        std::stringstream number;
        number << min_grid;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+proximity_grid+"-"+number.str());
    }
    media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);

    //media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
    {
        int max_grid = proximity_grid_plugin->getNumberParameterMax("Grid side");
        proximity_grid_plugin->setNumberParameterValue("Grid side",max_grid);
        std::cout << "Max grid size " << max_grid << std::endl;
        std::stringstream number;
        number << max_grid;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+proximity_grid+"-"+number.str());
    }
    media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
    */

    // Example of testing all grid sizes
    {
        media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
        int min_grid = proximity_grid_plugin->getNumberParameterMin("Grid side");
        int max_grid = proximity_grid_plugin->getNumberParameterMax("Grid side");
        std::cout << "Min grid size " << min_grid << std::endl;
        std::cout << "Max grid size " << max_grid << std::endl;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+proximity_grid);

        proximity_grid_plugin->setNumberParameterValue("Compact",1);

        //for (int g = min_grid; g <= max_grid; g++){
        for (int g = max_grid; g >= min_grid; g--){
            proximity_grid_plugin->setNumberParameterValue("Grid side",g);
            media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
            this->restorePositions(pca_tsne_pos);
        }
    }

    std::cout << "Computing the " << proximity_grid << " and " << unispring << " over the " <<  pca_tsne << std::endl;
    media_cycle->getBrowser()->setClustersPositionsPlugin(pca_tsne_plugin);
    media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+unispring+"-"+proximity_grid );
    //media_cycle->getBrowser()->setFilteringPlugin(unispring_plugin);
    //media_cycle->updateDisplay(false);

    //Example of naming differently along the grid size
    /*
    media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
    {
        int min_grid = proximity_grid_plugin->getNumberParameterMin("Grid side");
        proximity_grid_plugin->setNumberParameterValue("Grid side",min_grid);
        std::cout << "Min grid size " << min_grid << std::endl;
        std::stringstream number;
        number << min_grid;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+unispring+"-"+proximity_grid+"-"+number.str());
    }
    media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
    //media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
    {
        int max_grid = proximity_grid_plugin->getNumberParameterMax("Grid side");
        proximity_grid_plugin->setNumberParameterValue("Grid side",max_grid);
        std::cout << "Max grid size " << max_grid << std::endl;
        std::stringstream number;
        number << max_grid;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+unispring+"-"+proximity_grid+"-"+number.str());
    }
    media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
    */

    // Example of testing all grid sizes
    // Unispring
/*
    {
        this->restorePositions(pca_tsne_pos);
        media_cycle->getBrowser()->setFilteringPlugin(unispring_plugin);
        media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
        arma::mat unispring_pos = this->snapshotPositions();


        media_cycle->getBrowser()->setFilteringPlugin(proximity_grid_plugin);
        int min_grid = proximity_grid_plugin->getNumberParameterMin("Grid side");
        int max_grid = proximity_grid_plugin->getNumberParameterMax("Grid side");
        std::cout << "Min grid size " << min_grid << std::endl;
        std::cout << "Max grid size " << max_grid << std::endl;
        media_cycle->getLibrary()->setTitle( folder+"-"+pca_tsne+"-"+unispring+"-"+proximity_grid);

        proximity_grid_plugin->setNumberParameterValue("Compact",1);

        //for (int g = min_grid; g <= max_grid; g++){
        for (int g = max_grid; g >= min_grid; g--){

            proximity_grid_plugin->setNumberParameterValue("Grid side",g);
            media_cycle->getBrowser()->commitPositions(); // if just changing the filtering //media_cycle->updateDisplay(false);
            this->restorePositions(unispring_pos);
        }
    }
*/


    /*std::cout <<  "ACGridNeighborsProcess: saving as '" << xml << "'" << std::endl;
    media_cycle->saveXMLConfigFile(xml);

    fs::path xml_path(xml.c_str());
    if ( !fs::exists( xml_path ) )
    {
        std::cerr << "ACGridNeighborsProcess:: file '" << xml << "' doesn't exist " << std::endl;
        return;
    }

    if ( !fs::is_regular( xml_path ) )
    {
        std::cerr << "ACGridNeighborsProcess:: file or folder '" << xml << "' is not regular " << std::endl;
        return;
    }

    if( fs::file_size( xml_path ) <= 0 ){
        std::cerr << "ACGridNeighborsProcess:: file or folder '" << xml << "' is of null size " << std::endl;
        return;
    }*/
}

arma::mat ACGridNeighborsProcess::snapshotPositions(){
    arma::mat pos;
    if(!this->media_cycle) return pos;
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();
    pos.set_size(ids.size(),2);

    for (int i=0; i<ids.size(); i++){
        ACMediaNode* node = media_cycle->getMediaNode(ids[i]);
        if(node){
            ACPoint p = node->getNextPosition();
            pos(i,0) = p.x;
            pos(i,1) = p.y;
        }
        else{
            std::cerr << "Problem accessing node " << ids[i] << std::endl;
            exit(1);
        }
    }

    return pos;
}
void ACGridNeighborsProcess::restorePositions(arma::mat pos){

    if(!this->media_cycle) return;
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(pos.n_rows != ids.size()){
        std::cerr << "Positions and media ids counts are different" << std::endl;
        exit(1);
    }

    for (int i=0; i<ids.size(); i++){
        ACMediaNode* node = media_cycle->getMediaNode(ids[i]);
        if(node){
            ACPoint p;
            p.x = pos(i,0);
            p.y = pos(i,1);
            node->setCurrentPosition(p);
            node->setNextPosition(p,0);
        }
        else{
            std::cerr << "Problem accessing node " << ids[i] << std::endl;
            exit(1);
        }

    }
}
