/**
 * @brief A plugin that provides a video slit-scan thumbnail with JPG as output using OpenCV.
 * @author Christian Frisson
 * @date 21/01/2011
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

#include "ACVideoOpenCVSlitScanThumbnailerPlugin.h"
#include "ACVideo.h"
#include <cmath>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

using namespace std;

ACVideoOpenCVSlitScanThumbnailerPlugin::ACVideoOpenCVSlitScanThumbnailerPlugin()
    :ACThumbnailerPlugin(),filename("")
{
    this->mName = "Video Slit-Scan Thumbnailer (OpenCV)";
    this->mDescription ="Plugin for summarizing video files in slit-scans (with OpenCV)";
    this->mMediaType = MEDIA_TYPE_VIDEO;
}

ACVideoOpenCVSlitScanThumbnailerPlugin::~ACVideoOpenCVSlitScanThumbnailerPlugin()
{
}

std::string ACVideoOpenCVSlitScanThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "";
}

std::vector<std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    // No features plugin is required
    return features_plugins;
}

std::vector<std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

std::vector<ACMediaThumbnail*> ACVideoOpenCVSlitScanThumbnailerPlugin::summarize(ACMedia* media){
    double start = getTime();
    std::vector<ACMediaThumbnail*> thumbnails;
    filename = media->getFileName();

    std::string _name = "Slit-scan (OpenCV)";

    std::stringstream thumbnail_path;
    boost::filesystem::path media_path(filename.c_str());
    std::string thumbnail_suffix(_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    //std::cout << _name << " converted to " << thumbnail_suffix << std::endl;
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
#ifdef __APPLE__
    thumbnail_path << media_path.parent_path().string() << slash << media_path.stem().string() << "_" << thumbnail_suffix << ".jpg";
#else // this seems required on ubuntu to compile...
    thumbnail_path << media_path.parent_path() << slash << media_path.stem() << "_" << thumbnail_suffix << ".jpg";
#endif
    std::string thumbnail_filename = thumbnail_path.str();

    std::cout << "Slit-scanning " << filename<< "..." << std::endl;
    //double slit_scan_in = getTime();
    if (this->computeSlitScan(thumbnail_filename)){
        ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
        thumbnail->setFileName(thumbnail_filename);
        thumbnail->setName(_name);
        //thumbnail->setWidth(thumbnail_specs->second.width);
        //thumbnail->setHeight(thumbnail_specs->second.height);
        //thumbnail->setLength(thumbnail_specs->second.length);
        thumbnails.push_back(thumbnail);
        //std::cout << "Done slit-scanning " << filename << " in " << getTime()-slit_scan_in << " sec." << std::endl;
    }
    std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::summarize took " << getTime()-start << std::endl;
    return thumbnails;
}

std::vector<std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> thumbnail_names;
    thumbnail_names.push_back("Slit-scan");
    return thumbnail_names;
}

std::map<std::string,ACMediaType> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["Slit-scan"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["Slit-scan"] = "Video slit-scan";
    return thumbnail_descriptions;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["Slit-scan"] = ".jpg";
    return extensions;
}

int ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan(std::string _thumbnail_filename){
    cv::VideoCapture* capture = new cv::VideoCapture(filename.c_str());
    if ( !capture) {
        cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> Could not open video..." << endl;
        return 0;
    }

    int width   = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
    int height  = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    //int fps     = (int) capture->get(CV_CAP_PROP_FPS);
    int total_frames = (int) capture->get(CV_CAP_PROP_FRAME_COUNT)-1; // XS -1 seems necessary in OpenCV 2.3

    cv::Mat slit_scan(height, 1, CV_8UC3);
    cv::Mat empty(height, 1, CV_8UC3);
    cv::Mat current_frame;
    bool init = false;
    for (unsigned int f=0;f<total_frames;f++){
        int c = (int)(capture->get(CV_CAP_PROP_POS_FRAMES));
        *capture >> current_frame;
        if (!current_frame.data) {
            if(!init){
                slit_scan = empty;
                init = true;
            }
            else
                cv::hconcat(slit_scan, empty, slit_scan);
            std::cerr << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan: frame " << f << " ("<< c <<") " << "/" << total_frames << " lost" << std::endl;
            break;
        }
        cv::Mat slit(current_frame, cv::Rect((int)(0.5f*width), 0, 1, height));

        if(!init){
            slit_scan = slit;
            init = true;
        }
        else{
            cv::Mat pre(slit_scan);
            slit_scan.release();
            cv::hconcat(pre, slit, slit_scan);
            pre.release();
        }

        slit.release();
        current_frame.release();
        std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan: frame " << f << " ("<< c <<") " << "/" << total_frames << std::endl;
    }
    cv::imwrite(_thumbnail_filename.c_str(), slit_scan);
    return 1;
}
