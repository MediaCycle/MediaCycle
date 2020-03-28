/**
 * @brief A plugin that allows to save media libraries as JSON files.
 * @author Christian Frisson
 * @date 3/12/2014
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

#include "ACLibraryJsonWriterPlugin.h"

#include <vector>
#include <string>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/serialization/string.hpp>
#include <boost/foreach.hpp>
using boost::property_tree::ptree;
using boost::property_tree::read_json;

//#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

ACLibraryJsonWriterPlugin::ACLibraryJsonWriterPlugin() : ACMediaLibraryWriterPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "JSON export";
    this->mDescription = "Plugin for saving media libraries as JSON files";
    this->mId = "";
    this->jsonfilename = "";

    //CF this should be moved to the core/factory if we need to spread the use of boost::locale
    //From the Boost locale FAQ: http://www.boost.org/doc/libs/master/libs/locale/doc/html/faq.html
    //Question: I try to use some Boost.Locale functions and I get an std::bad_cast exception thrown?
    //Answer: You probably try to use incorrect std::locale object. All Boost.Locale tools relay on std::locale object's facets. The locale object should be generated with generator class and then passed to the function or alternatively global locale should be set using std::locale::global() function such that global locale (and default created one) would have required facets to use.
//    boost::locale::generator gen;
//    std::locale loc = gen("");
//    std::locale::global(loc);
}

ACLibraryJsonWriterPlugin::~ACLibraryJsonWriterPlugin() {
}

bool ACLibraryJsonWriterPlugin::openLibrary(std::string filepath){
    jsonfilename = filepath;
    return true;
}

bool ACLibraryJsonWriterPlugin::closeLibrary(std::string filepath){
    jsonfilename = "";
    return true;
}

bool ACLibraryJsonWriterPlugin::isLibraryOpened(std::string filepath){
    return true;
}

std::vector<std::string> ACLibraryJsonWriterPlugin::fileFormats(){
    std::vector<std::string> formats;
    formats.push_back(".json");
    return formats;
}

bool ACLibraryJsonWriterPlugin::saveLibraryMetadata(){

    // containers to build a "flare.json"-like nested JSON filetree for d3js
    typedef std::set<std::string> edgecontainer; // note: a set only allows unique elements
    edgecontainer edgelist;

    // Min/max storage should be moved to the core for efficiency
    float feature_min = 0.0f;
    float feature_max = 0.0f;
    float feature_dim = 0.0f;
    std::string feature_name("Perceptual Sharpness Mean");
    std::string png_psr_thumbnail_name("Perceptual Sharpness Ring");

    if(media_cycle && media_cycle->getLibrarySize()>0){

        ACMedias medias = media_cycle->getLibrary()->getAllMedia();
        ACMedias::iterator _media = medias.begin();
        if(_media->second){

            ACMediaFeatures* _feature = _media->second->getFeaturesVector(feature_name);
            if(_feature){
                feature_min = _feature->getFeatureElement(feature_dim);
                feature_max = _feature->getFeatureElement(feature_dim);
            }
            for(;_media!=medias.end();_media++){
                if(_media->second){

                    // Min/max storage should be moved to the core for efficiency
                    _feature = _media->second->getFeaturesVector(feature_name);
                    if(_feature){
                        feature_min = min(feature_min,_feature->getFeatureElement(feature_dim));
                        feature_max = max(feature_max,_feature->getFeatureElement(feature_dim));
                    }
                    _feature = 0;

                    // d3js JSON: first we build the list of unique recursive parent paths for all files
                    std::string file_name = _media->second->getFileName();
                    fs::path p( file_name.c_str());// , fs::native );
                    if ( fs::exists( p ) && fs::is_regular( p ) )
                    {
                        edgelist.insert(file_name);
                        fs::path parent(p);
                        while (  fs::is_directory( parent.parent_path() )){
                            parent = parent.parent_path();
                            edgelist.insert(parent.string());
                        }
                    }
                }
            }
        }
    }

    // d3js JSON: second we populate a boost::property_tree by reverse-iterating on the set of paths (ordered by filename, thus files at each depth then recursive paths)
    ptree filetree;
    write_json(this->jsonfilename, filetree); // clear previous library
    fs::path previous;
    char slash('/');
#ifdef WIN32
    slash = '\\';
#endif
    std::map<int,ptree> folders;
    for(edgecontainer::reverse_iterator edgein = edgelist.rbegin(); edgein != edgelist.rend();edgein++){

        fs::path current ( (*edgein).c_str());
        bool is_current_directory = fs::is_directory( current );

        size_t current_depth = std::count((*edgein).begin(), (*edgein).end(), slash);
        if( (*edgein).size() == 1) current_depth = 0; // UNIX root '/'

        if(!is_current_directory){
            ptree fileproperties;
            fileproperties.put("name", current.stem().string());
            fileproperties.put("size", fs::file_size( current ));
            fileproperties.put("file", current.string());

            float x(0.5),y(0.5);
            int node_id = media_cycle->getLibrary()->getMediaIndex(current.string());
            ACMediaNode* node = media_cycle->getMediaNode(node_id);
            if(node){
                ACPoint pos = node->getNextPosition();
                //if(this->browser_renderer){
                //    pos = this->browser_renderer->getScreenCoordinates(pos);
                //}
                x = pos.x+0.5;//0.5*(pos.x+1);
                y = pos.y+0.5;//0.5*(pos.y+1);
                fileproperties.put("mcx", x);
                fileproperties.put("mcy", y);

                ACMedia* _media = media_cycle->getLibrary()->getMedia( node->getMediaId() );
                if(_media){
                    ACMediaFeatures* _feature = _media->getFeaturesVector(feature_name);
                    if(_feature){
                        float value(1.0);
                        value = _feature->getFeatureElement(0);
                        value = (value-feature_min)/(feature_max-feature_min);

                        std::string feature_name_camel_case(feature_name);
                        //boost::locale::to_title(feature_name_camel_case);
                        // boost::replace_all(feature_name_camel_case," ","");
                        //boost::replace_all(feature_name_camel_case,":","");
                        {
                            std::vector<std::string> strVec;
                            using boost::is_any_of;
                            using boost::algorithm::token_compress_on;
                            boost::algorithm::split(strVec, feature_name, is_any_of(":, "), token_compress_on);
                            std::vector<std::string>::iterator i;
                            for(i = strVec.begin(); i !=strVec.end(); ++i)
                            {
                                boost::to_lower(*i);
                                (*i)[0]=toupper((*i)[0]);
                                cout<<(*i)<<endl;
                                feature_name_camel_case += *i;
                            }
                            boost::trim_right(feature_name_camel_case);
                        }
                        if(feature_name_camel_case.length() > 1)
                            feature_name_camel_case[0] = std::tolower(feature_name_camel_case[0]);

                        //std::cout << feature_name_camel_case << std::endl;
                        fileproperties.put(feature_name_camel_case,value);
                    }
                    std::string png_psr_thumbnail_filename = _media->getThumbnailFileName(png_psr_thumbnail_name);
                    if(png_psr_thumbnail_filename != ""){
                        //fs::path png_psr_thumbnail_filepath(png_psr_thumbnail_filename);
                        //png_psr_thumbnail_filepath.wstring();
                        fileproperties.put("thumbnail"/*png_psr_thumbnail_name*/,png_psr_thumbnail_filename);
                    }
                }
            }

            folders[ current_depth ].push_back(std::make_pair("", fileproperties));
        }
        else {
            if(current_depth == 0){
                folders[ current_depth ].add("name", current.stem().string());
                folders[ current_depth ].add_child("children", folders[ current_depth+1 ] );
                folders[ current_depth+1 ].clear();
                write_json(this->jsonfilename, folders[ current_depth ]);
            }
            else {
                if(!previous.empty()){
                    bool is_previous_directory = fs::is_directory( current );
                    size_t previous_depth = std::count(previous.string().begin(), previous.string().end(), slash);
                    if( previous.string().size() <= 1) previous_depth = 0; // UNIX root '/'
                    ptree fileproperties;
                    fileproperties.add("name", current.stem().string());
                    fileproperties.add_child("children", folders[current_depth+1] );
                    folders[ current_depth ].push_back(std::make_pair("", fileproperties));
                    folders[ current_depth+1 ].clear();
                }
            }
        }
        previous = current;
    }

    return true;
}

bool ACLibraryJsonWriterPlugin::saveMedia(ACMedia* media){
    return true;
}

