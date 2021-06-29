/*
 *  ACArchipelXmlFile.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 16/11/10
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include "ACArchipelXmlFile.h"
#include <iostream>
#include <fstream>
#include "ACArchipelReader.h"
using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

string* ACArchipelXmlFileRead(string filePath){
    string ext=fs::extension(filePath);
    if (ext==string(".xml")){
        ACArchipelReader doc(filePath);
        return new string(doc.getText());
    }
    else
        return NULL;
}

string ACArchipelLabelFileRead(string filePath){
    string ext=fs::extension(filePath);
    if (ext==string(".xml")){
        ACArchipelReader doc(filePath);
        return (doc.getArtist()+string("/")+doc.getAlbumName()) ;
    }
    else
        return string("");
}

cv::Mat ACArchipelThumbFileRead(std::string filePath){
    cv::Mat thumb = 0;
    string ext=fs::extension(filePath);
    if (ext==string(".xml")){
        ACArchipelReader doc(filePath);
        std::cout << "Thumbnail: " << fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath() << std::endl;

        bool verbose = true;
        if(verbose){
            std::vector<std::string> ilots = doc.getIlot();
            std::cout << "Ilot(s) :";
            for(std::vector<std::string>::iterator ilot = ilots.begin();ilot!=ilots.end();++ilot)
                std::cout << " " << *ilot;
            std::cout << std::endl;

            std::vector<std::string> tags = doc.getGlossaire();
            std::cout << "Glossaire :";
            for(std::vector<std::string>::iterator tag = tags.begin();tag!=tags.end();++tag)
                std::cout << " " << *tag;
            std::cout << std::endl;

            std::cout << "Text: " << doc.getText();
            std::cout << "Artist: " << doc.getArtist();
            std::cout << "ArtistType: " << doc.getArtistType();
            std::cout << "AlbumName: " << doc.getAlbumName();
            std::cout << "Reference: " << doc.getReference();

            std::vector<std::string> track_titles = doc.getTrackTitle();
            std::vector<std::string> track_paths = doc.getTrackPath();
            std::cout << "Tracks :" << std::endl;
            std::vector<std::string>::iterator track_path = track_paths.begin();
            std::vector<std::string>::iterator track_title = track_titles.begin();
            for(;track_title!=track_titles.end() && track_path!=track_paths.end() ;++track_title){
                std::cout << "   title: " << *track_title << " path '" << *track_path << "'" << std::endl;
                ++track_path;
            }

            std::cout << "ThumbPath: " << doc.getThumbPath();
            std::cout << "Year: " << doc.getYear();
            std::cout << "MediathequeHyperlink: " << doc.getMediathequeHyperlink();
            std::cout << "Producer: " << doc.getProducer();
            std::cout << "Extra: " << doc.getExtra();
        }

        thumb = cv::imread((fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath()).c_str(), cv::IMREAD_COLOR);
        try {
            if (!thumb) {
                cerr << "Check file name : " << fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath() << endl;
                throw(string(" <ACArchipelThumbFileRead> cv::IMREAD_COLOR : not a color image !"));
            }
        }
        catch (const string& not_image_file) {
            cerr << not_image_file << endl;
        }
        return thumb;
    }
    else
        return 0;
}

string ACArchipelGetThumbFileName(std::string filePath){
    string ext=fs::extension(filePath);
    if (ext==string(".xml")){
        ACArchipelReader doc(filePath);
        return fs::path(filePath).parent_path().string()+string("/")+doc.getThumbPath();
    }
    else
        return "";
}
