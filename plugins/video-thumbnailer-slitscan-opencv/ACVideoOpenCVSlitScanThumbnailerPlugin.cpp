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

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;

ACVideoOpenCVSlitScanThumbnailerPlugin::ACVideoOpenCVSlitScanThumbnailerPlugin()
    :ACThumbnailerPlugin()
{
    this->mName = "Video Slit-Scan Thumbnailer (OpenCV)";
    this->mDescription ="Plugin for summarizing video files in slit-scans (with OpenCV)";
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->progress = 0;
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

std::string ACVideoOpenCVSlitScanThumbnailerPlugin::createFileName(std::string _filename, std::string _name, std::string _extension){
    std::stringstream thumbnail_path;
    boost::filesystem::path media_path(_filename.c_str());
    std::string thumbnail_suffix(_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    //std::cout << _name << " converted to " << thumbnail_suffix << std::endl;
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
#ifdef __APPLE__
    thumbnail_path << media_path.parent_path().string() << slash << media_path.stem().string() << "_" << thumbnail_suffix << _extension;
#else // this seems required on ubuntu to compile...
    thumbnail_path << media_path.parent_path() << slash << media_path.stem() << "_" << thumbnail_suffix << _extension;
#endif
    return thumbnail_path.str();
}

std::vector<ACMediaThumbnail*> ACVideoOpenCVSlitScanThumbnailerPlugin::summarize(ACMedia* media){
    this->progress = 0;
    double start = getTime();
    std::vector<ACMediaThumbnail*> thumbnails;
    thumbnails = this->computeSlitScans(media->getFileName());
    std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::summarize took " << getTime()-start << std::endl;
    this->progress = 1;
    return thumbnails;
}

std::vector<std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> thumbnail_names;
    thumbnail_names.push_back("Slit-scan");
    thumbnail_names.push_back("Circular slit-scan");
    return thumbnail_names;
}

std::map<std::string,ACMediaType> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["Slit-scan"] = MEDIA_TYPE_IMAGE;
    thumbnail_types["Circular slit-scan"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["Slit-scan"] = "Video slit-scan";
    thumbnail_descriptions["Circular slit-scan"] = "Circular video slit-scan with rotation warping";
    return thumbnail_descriptions;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["Slit-scan"] = ".png"; //CF: the jpg format has a maximum of 65000 px per side, unhandled in this plugin (slit-scans needs to be segmented in that case)
    extensions["Circular slit-scan"] = ".png";
    return extensions;
}

std::vector<ACMediaThumbnail*> ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans(std::string _filename){
    std::vector<ACMediaThumbnail*> thumbnails;

    bool abort = true;
    std::vector<std::string> thumbnail_names = this->getThumbnailNames();
    std::map<std::string,std::string> thumbnail_extensions = this->getThumbnailExtensions();
    for(std::vector<std::string>::iterator thumbnail_name = thumbnail_names.begin();thumbnail_name != thumbnail_names.end();thumbnail_name++){
        std::string _thumbnail_filename = this->createFileName(_filename,*thumbnail_name,thumbnail_extensions[*thumbnail_name]);
        compute[*thumbnail_name] = true;
        fs::path p( _thumbnail_filename.c_str());// , fs::native );
        if ( fs::exists( p ) ){
            std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << _thumbnail_filename << std::endl;
            if ( fs::is_regular( p ) )
            {
                std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::summarize: file is regular: " << _thumbnail_filename << std::endl;
                if(fs::file_size( p ) > 0 ){
                    compute[*thumbnail_name] = false;
                }
            }
        }
        abort = !(compute[*thumbnail_name]) && abort;
    }

    cv::VideoCapture* capture = new cv::VideoCapture(_filename.c_str());
    if ( !capture) {
        cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> Could not open video..." << endl;
        return thumbnails;
    }

    int width   = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
    int height  = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    //int fps     = (int) capture->get(CV_CAP_PROP_FPS);
    int total_frames = (int) capture->get(CV_CAP_PROP_FRAME_COUNT)-1; // XS -1 seems necessary in OpenCV 2.3

    //cv::Mat circular_slit_scan(2*height, 2*height, CV_8UC4);
    cv::Mat circular_slit_scan(4*height, 4*height, CV_8UC4);

    cv::Mat slit_scan(height, total_frames, CV_8UC4);
    cv::Mat current_frame;

    if(!abort){
        double angle = 0.5;/*360.0f/(float)(total_frames-1)*/;
        cv::Point2f src_center( 0.5f*circular_slit_scan.cols, 0.5f*circular_slit_scan.rows );

        double border = tan(angle/180.f*CV_PI)*height;
        int fills = floor(border);
        int sliced = ceil(border/2.0f);

        bool init = false;
        for (unsigned int f=0;f<total_frames;f++){
            int c = (int)(capture->get(CV_CAP_PROP_POS_FRAMES));
            *capture >> current_frame;

            if (current_frame.data) {

                //            if (!current_frame.data) {
                //                if(compute["Circular Slit-scan"]){
                //                    cv::Mat rot_mat = cv::getRotationMatrix2D( src_center, angle, 1.0 );
                //                    cv::warpAffine( circular_slit_scan/*src*/, circular_slit_scan/*out*/, rot_mat, circular_slit_scan.size(), /*flags*/ cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                //                }
                //                std::cerr << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan: frame " << f << " ("<< c <<") " << "/" << total_frames << " lost" << std::endl;
                //                break;
                //            }

                cv::Mat transparent_frame(current_frame.size(), CV_8UC4);

                cv::cvtColor(current_frame, transparent_frame, CV_RGB2RGBA, 4);

                cv::Mat slit(transparent_frame, cv::Rect((int)(0.5f*width), 0, 1, height));

                if(compute["Slit-scan"]){
                    cv::Mat slit_scan_part = slit_scan(cv::Rect(f, 0, 1, height));
                    slit.copyTo(slit_scan_part,slit);
                }

                //        From opencv2 2.4.3 imgproc.hpp:
                //
                //        BORDER_REPLICATE=IPL_BORDER_REPLICATE, BORDER_CONSTANT=IPL_BORDER_CONSTANT,
                //        BORDER_REFLECT=IPL_BORDER_REFLECT, BORDER_WRAP=IPL_BORDER_WRAP,
                //        BORDER_REFLECT_101=IPL_BORDER_REFLECT_101, BORDER_REFLECT101=BORDER_REFLECT_101,
                //        BORDER_TRANSPARENT=IPL_BORDER_TRANSPARENT,
                //        BORDER_DEFAULT=BORDER_REFLECT_101, BORDER_ISOLATED=16
                //
                //        INTER_NEAREST=CV_INTER_NN, //!< nearest neighbor interpolation
                //        INTER_LINEAR=CV_INTER_LINEAR, //!< bilinear interpolation
                //        INTER_CUBIC=CV_INTER_CUBIC, //!< bicubic interpolation
                //        INTER_AREA=CV_INTER_AREA, //!< area-based (or super) interpolation
                //        INTER_LANCZOS4=CV_INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
                //        INTER_MAX=7,
                //        WARP_INVERSE_MAP=CV_WARP_INVERSE_MAP

                if(compute["Circular slit-scan"] && f%((int) ((float)total_frames/360.0f*angle)) == 0){
                    // Circ slit scan, copying the slit to the thumbnail then rotating, very smeared


                    //                if( fills>1 ){
                    //                cv::Mat slice(transparent_frame, cv::Rect((int)(0.5f*width)-sliced, 0, 2*sliced, height));
                    //                cv::Mat rot_mat = cv::getRotationMatrix2D( src_center, -angle, 1.0 );
                    //                    cv::warpAffine(circular_slit_scan/*src*/, circular_slit_scan/*out*/, rot_mat, circular_slit_scan.size(),/*flags*/ cv::INTER_AREA, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                    //                    std::cout << "Fils 1" << std::endl;
                    //                    // cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_TRANSPARENT -> blurry disc, precise at end of the movie, border blurry, can be octogonal
                    //                    // cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_CONSTANT -> blurry, precise, border better but blurry, no octogonal
                    //                    // cv::INTER_NEAREST, /*borderMode*/ cv::BORDER_CONSTANT -> border gritty, egyptian snail effect
                    //                    // cv::INTER_AREA, /*borderMode*/ cv::BORDER_CONSTANT -> best results
                    //                    cv::Mat circ_slit_scan_part = circular_slit_scan(cv::Rect(height-sliced, 0, 2*sliced, height));
                    //                    slice.copyTo(circ_slit_scan_part,slice);


                    // Circ slit scan, rotating the slit before adding to the image, lots of white noise

                    //                    int slice_x = 0;
                    //                    int slice_w = 1;
                    //                    if(sliced>=1){
                    //                        slice_w = 2*sliced;
                    //                        slice_x = sliced;
                    //                    }

                    //                    cv::Mat slice(transparent_frame, cv::Rect((int)(0.5f*width)-slice_x, 0, slice_w, height));
                    //                    cv::Mat transparent_canvas(2*height, 2*height, CV_8UC4);
                    //                    cv::Mat transparent_slit(transparent_canvas, cv::Rect((int)(height)-slice_x, 0, slice_w, height));
                    //                    slice.copyTo(transparent_slit,slit);
                    //                    cv::Mat rotated_canvas(2*height, 2*height, CV_8UC4);

                    //                    cv::Mat rot_mat = cv::getRotationMatrix2D( src_center, -(float)f/(float)total_frames*360.0f, 1.0 );
                    //                    cv::warpAffine( transparent_canvas/*src*/, rotated_canvas/*out*/, rot_mat, transparent_canvas.size(), /*flags*/ cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                    //                    cv::add(rotated_canvas,circular_slit_scan,circular_slit_scan);

                    //                    transparent_canvas.release();
                    //                    rotated_canvas.release();

                    // Disc slit scan, set the sizes of circular_slit_scan to 4*height above

                    int slice_x = 0;
                    int slice_w = 1;
                    if(sliced>=1){
                        slice_w = 2*sliced;
                        slice_x = sliced;
                    }

                    cv::Mat slice(transparent_frame, cv::Rect((int)(0.5f*width)-slice_x-1, 0, slice_w+2, height));
                    cv::Mat transparent_canvas(4*height, 4*height, CV_8UC4);
                    cv::Mat transparent_slit(transparent_canvas, cv::Rect((int)(2*height)-slice_x-1, 0, slice_w+2, height));
                    slice.copyTo(transparent_slit,slit);
                    cv::Mat rotated_canvas(4*height, 4*height, CV_8UC4);
                    cv::Point2f center( 2*height, 2*height );
                    cv::Mat rot_mat = cv::getRotationMatrix2D( center, -(float)f/(float)total_frames*360.0f, 1.0 );
                    cv::warpAffine( transparent_canvas/*src*/, rotated_canvas/*out*/, rot_mat, transparent_canvas.size(), /*flags*/ cv::INTER_AREA, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                    cv::add(rotated_canvas,circular_slit_scan,circular_slit_scan);

                    transparent_canvas.release();
                    rotated_canvas.release();

                    //                }
                    //                else {
                    //                    cv::warpAffine( circular_slit_scan/*src*/, circular_slit_scan/*out*/, rot_mat, circular_slit_scan.size(), /*flags*/ cv::INTER_AREA, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                    //                    // cv::INTER_NEAREST, /*borderMode*/ cv::BORDER_CONSTANT -> just a few pixels scatter, plus 12 o'clock bar
                    //                    // cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_CONSTANT -> a bit more pixels
                    //                    cv::Mat circ_slit_scan_part = circular_slit_scan(cv::Rect(height, 0, 1, height));
                    //                    slit.copyTo(circ_slit_scan_part,slit);
                    //                }
                    slice.release();

                    int iteration = ceil((float)f/(float)total_frames*360.0f/angle);
                    std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeCircSlitScans: frame " << f << "("<< c <<")" << " / " << total_frames << " angle " <<   ceil((float)f/(float)total_frames*360.0f/angle) << " sliced " << sliced /*<< " border " << border << " fills " << fills*/ << std::endl;

                    //                vector<int> _compression_params;
                    //                _compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
                    //                _compression_params.push_back(9); // default: 3
                    //                std::stringstream _circular_name;
                    //                _circular_name << "Circular slit-scan " << iteration;
                    //                std::string _circular_thumbnail_filename = this->createFileName(_filename, _circular_name.str());
                    //                try {
                    //                    cv::imwrite(_circular_thumbnail_filename.c_str(), circular_slit_scan, _compression_params);
                    //                }
                    //                catch (runtime_error& ex) {
                    //                    std::cerr << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: couldn't save " << _circular_thumbnail_filename << ": " << ex.what() << std::endl;
                    //                }


                    // Circ slit scan, rotating the slit before adding to the image, lots of white noise

                    //                int slice_x = 0;
                    //                int slice_w = 1;
                    //                if(sliced>=1){
                    //                   slice_w = 2*sliced;
                    //                   slice_x = sliced;
                    //               }

                    //                cv::Mat slice(transparent_frame, cv::Rect((int)(0.5f*width)-slice_x, 0, slice_w, height));
                    //                cv::Mat transparent_canvas(2*height, 2*height, CV_8UC4);
                    //                cv::Mat transparent_slit(transparent_canvas, cv::Rect((int)(height)-slice_x, 0, slice_w, height));
                    //                slice.copyTo(transparent_slit,slit);
                    //                cv::Mat rotated_canvas(2*height, 2*height, CV_8UC4);
                    //                cv::Mat rot_mat = cv::getRotationMatrix2D( src_center, -(float)f*angle, 1.0 );
                    //                cv::warpAffine( transparent_canvas/*src*/, rotated_canvas/*out*/, rot_mat, transparent_canvas.size(), /*flags*/ cv::INTER_LINEAR, /*borderMode*/ cv::BORDER_TRANSPARENT, /*borderValue*/ 0);
                    //                cv::add(rotated_canvas,circular_slit_scan,circular_slit_scan);

                    //                transparent_canvas.release();
                    //                rotated_canvas.release();
                    //                slice.release();

                    // Disc slit scan, set the sizes of circular_slit_scan to 4*height above

                    //                int slice_x = 0;
                    //                int slice_w = 1;
                    //                if(sliced>=1){
                    //                    slice_w = 2*sliced;
                    //                    slice_x = sliced;
                    //                }

                    //                angle = 360.0f/(float)(total_frames);
                    //                cv::Mat slice(transparent_frame, cv::Rect((int)(0.5f*width)-slice_x, 0, slice_w, height));
                    //                cv::Mat transparent_canvas(4*height, 4*height, CV_8UC4);
                    //                cv::Mat transparent_slit(transparent_canvas, cv::Rect((int)(2*height)-slice_x, 0, slice_w, height));
                    //                slice.copyTo(transparent_slit,slit);
                    //                cv::Mat rotated_canvas(4*height, 4*height, CV_8UC4);
                    //                cv::Point2f center( 2*height, 2*height );
                    //                cv::Mat rot_mat = cv::getRotationMatrix2D( center, -(float)f*angle, 1.0 );
                    //                cv::warpAffine( transparent_canvas/*src*/, rotated_canvas/*out*/, rot_mat, transparent_canvas.size(), /*flags*/ cv::INTER_AREA, /*borderMode*/ cv::BORDER_CONSTANT, /*borderValue*/ 0);
                    //                cv::add(rotated_canvas,circular_slit_scan,circular_slit_scan);

                    //                transparent_canvas.release();
                    //                rotated_canvas.release();
                    //                slice.release();

                    // Better version to try, cut non-rectangular slices and skip frames
                }

                slit.release();
                current_frame.release();
                transparent_frame.release();
                //std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: frame " << f << "("<< c <<")" << " / " << total_frames << " angle " << (float)f*angle << " sliced " << sliced /*<< " border " << border << " fills " << fills*/ << std::endl;
                this->progress = (float)f/(float)total_frames;
            }
        }
    }

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9); // default: 3

    std::string _name("Slit-scan");
    std::string _thumbnail_filename = this->createFileName(_filename, _name, thumbnail_extensions[_name]);
    if(compute["Slit-scan"]){
        try {
            cv::imwrite(_thumbnail_filename.c_str(), slit_scan, compression_params);
        }
        catch (runtime_error& ex) {
            std::cerr << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: couldn't save " << _thumbnail_filename << ": " << ex.what() << std::endl;
        }
    }
    slit_scan.release();

    ACMediaThumbnail* _thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
    _thumbnail->setFileName(_thumbnail_filename);
    _thumbnail->setName(_name);
    _thumbnail->setWidth(total_frames);
    _thumbnail->setHeight(height);
    _thumbnail->setLength(total_frames);
    thumbnails.push_back(_thumbnail);
    std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: saved " << _thumbnail_filename << " with width " << total_frames << " height " << height << " length " << total_frames << std::endl;

    std::string circular_name("Circular slit-scan");
    std::string circular_thumbnail_filename = this->createFileName(_filename, circular_name, thumbnail_extensions[circular_name]);
    if(compute["Circular slit-scan"]){
        try {
            cv::imwrite(circular_thumbnail_filename.c_str(), circular_slit_scan, compression_params);
        }
        catch (runtime_error& ex) {
            std::cerr << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: couldn't save " << circular_thumbnail_filename << ": " << ex.what() << std::endl;
        }
    }
    circular_slit_scan.release();

    ACMediaThumbnail* circ_thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
    circ_thumbnail->setFileName(circular_thumbnail_filename);
    circ_thumbnail->setName(circular_name);
    circ_thumbnail->setWidth(2*height);
    circ_thumbnail->setHeight(2*height);
    circ_thumbnail->setLength(total_frames);
    circ_thumbnail->setCircular(true);
    thumbnails.push_back(circ_thumbnail);
    std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScans: saved " << circular_thumbnail_filename << " with width " << 2*height << " height " << 2*height << " length " << total_frames << std::endl;

    capture->release();

    this->progress = 1.0f;
    return thumbnails;
}
