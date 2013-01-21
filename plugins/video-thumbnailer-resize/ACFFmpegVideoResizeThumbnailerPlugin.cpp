/**
 * @brief A plugin that reduces the video dimensions proportionally into a video thumbnail through the ffmpeg commandline
 * @author Christian Frisson
 * @date 16/12/2012
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

#include "ACFFmpegVideoResizeThumbnailerPlugin.h"

#include <vector>
#include <string>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACFFmpegVideoResizeThumbnailerPlugin::ACFFmpegVideoResizeThumbnailerPlugin() : ACThumbnailerPlugin() {
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mName = "Video resizing thumbnailer";
    this->mDescription = "Video resizing thumbnailer: reduces the video dimensions proportionally, the higher not exceeding 64 pixels";
    this->mId = "";
    this->max_dimension = 64; // pixels
}

ACFFmpegVideoResizeThumbnailerPlugin::~ACFFmpegVideoResizeThumbnailerPlugin(){
}

std::vector<ACMediaThumbnail*> ACFFmpegVideoResizeThumbnailerPlugin::summarize(ACMedia* theMedia)
{
    std::vector<ACMediaThumbnail*> thumbnails;

    std::string _media_filename = theMedia->getFileName();
    std::string _name = "Timeline Resized";

    std::stringstream thumbnail_path;
    boost::filesystem::path media_path(_media_filename.c_str());
    std::string thumbnail_suffix(_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    std::cout << _name << " converted to " << thumbnail_suffix << std::endl;
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
#ifdef __APPLE__
    thumbnail_path << media_path.parent_path().string() << slash << media_path.stem().string() << "_" << thumbnail_suffix << "_" << max_dimension << ".mp4";
#else // this seems required on ubuntu to compile...
    thumbnail_path << media_path.parent_path() << slash << media_path.stem() << "_" << thumbnail_suffix << "_" << max_dimension << ".mp4";
#endif
    std::string _thumbnail_filename = thumbnail_path.str();
    std::cout << "ACFFmpegVideoResizeThumbnailerPlugin: thumbnail filename '" << _thumbnail_filename << "'" << std::endl;

    int _width = theMedia->getWidth();
    int _height = theMedia->getHeight();

    float scale = (_width > _height) ? this->max_dimension/(float)_width : this->max_dimension/(float)_height;
    _width = (int)((float)_width * scale);
    _height = (int)((float)_height *scale);

    // Make dimensions divisible by 2 (required by ffmpeg)
    if( (int)((float)_height/2.0f) != ((float)_height/2.0f) )
        _height++;
    if( (int)((float)_width/2.0f) != ((float)_width/2.0f) )
        _width++;

    int fail = 1;

    fs::path p( _thumbnail_filename.c_str());// , fs::native );
    if ( fs::exists( p ) )
    {
        std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << _thumbnail_filename << std::endl;
        if ( fs::is_regular( p ) )
        {
            std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: file is regular: " << _thumbnail_filename << std::endl;
            if(fs::file_size( p ) > 0 ){
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: size of " << _thumbnail_filename << " is non-zero, not recomputing "<< std::endl;
                fail = 0;
            }
        }
    }

    if(fail == 1){
        std::stringstream command;
        command << ffmpeg_cmd << " -i '" << _media_filename << "' -an -s " << _width << "x" << _height << " '" << _thumbnail_filename << "'";
        std::cout << "Command '" << command.str() << "'" << std::endl;
        try {
            fail = system(command.str().c_str());
        }
        catch (const exception& e) {
            cout << "ACFFmpegVideoResizeThumbnailerPlugin: caught exception while trying resize video " << _media_filename << " with ffmpeg: " << e.what() << endl;
        }
    }

    if(fail == 0){
        ACMediaThumbnail* thumbnail = new ACMediaThumbnail();
        thumbnail->setFileName(_thumbnail_filename);
        thumbnail->setName(_name);
        thumbnail->setWidth(_width);
        thumbnail->setHeight(_height);
        thumbnail->setLength((int)(theMedia->getDuration() * theMedia->getSampleRate()));
        thumbnails.push_back(thumbnail);
    }

    return thumbnails;
}
