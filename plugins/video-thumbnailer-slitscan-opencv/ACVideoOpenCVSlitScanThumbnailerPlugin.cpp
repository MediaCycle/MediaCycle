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
#include <osg/ImageUtils>
#include <osg/Version>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace osg;

ACVideoOpenCVSlitScanThumbnailerPlugin::ACVideoOpenCVSlitScanThumbnailerPlugin()
:ACThumbnailerPlugin(),filename(""),notify_level(osg::WARN),slit_scan(0)
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


osg::ref_ptr<osg::Image> Convert_OpenCV_2_osg_Image(cv::Mat cvImg)
{
    // XS uncomment the following 4 lines for visual debug (e.g., thumbnail)
    //	cvNamedWindow("T", CV_WINDOW_AUTOSIZE);
    //	cvShowImage("T", cvImg);
    //	cvWaitKey(0);
    //	cvDestroyWindow("T");

    if(cvImg.channels() == 3)
    {
        // Flip image from top-left to bottom-left origin
        //		if(cvImg->origin == 0) {
        //			cvConvertImage(cvImg , cvImg, CV_CVTIMG_FLIP);
        //			cvImg->origin = 1;
        //		}

        // Convert from BGR to RGB color format
        //printf("Color format %s\n",cvImg->colorModel);
        //		if ( !strcmp(cvImg->channelSeq,"BGR") ) {
        //			cvCvtColor( cvImg, cvImg, CV_BGR2RGB );
        //		}

        osg::ref_ptr<osg::Image> osgImg = new osg::Image();

        /*temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
   for (i=0;i<cvImg->height;i++) {
   memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
   }*/

        //cvtColor( cvImg, cvImg, CV_BGR2RGB );
        osgImg->setWriteHint(osg::Image::NO_PREFERENCE);

        osgImg->setImage(
                    cvImg.cols, //s (witdh)
                    cvImg.rows, //t (height)
                    3, //r //CF needs to be 1 otherwise scaleImage can't be used
                    GL_LINE_STRIP, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
                    GL_RGB, // GLenum pixelFormat, (GL_RGB, 0x1907)
                    GL_UNSIGNED_BYTE, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
                    cvImg.data,//(unsigned char*)(cvImg.data), // unsigned char* data
                    //temp_data,
                    osg::Image::NO_DELETE // AllocationMode mode (shallow copy)
                    );//int packing=1); (???)

        //printf("Conversion completed\n");
        return osgImg;
    }
    // XS TODO : what happens with BW images ?
    else {
        cerr << "Unrecognized image type" << endl;
        //printf("Unrecognized image type");
        return 0;
    }
}

std::vector<ACMediaThumbnail*> ACVideoOpenCVSlitScanThumbnailerPlugin::summarize(ACMedia* media){
    std::vector<ACMediaThumbnail*> thumbnails;
    filename = media->getFileName();

    std::string _name = "Slit-scan (OpenCV)";

    std::stringstream thumbnail_path;
    boost::filesystem::path media_path(filename.c_str());
    std::string thumbnail_suffix(_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    std::cout << _name << " converted to " << thumbnail_suffix << std::endl;
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
    double slit_scan_in = getTime();
    notify_level = osg::getNotifyLevel();
    osg::setNotifyLevel(osg::WARN);//to remove the copyImage NOTICEs
    if (this->computeSlitScan(thumbnail_filename)){
        ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
        thumbnail->setFileName(thumbnail_filename);
        thumbnail->setName(_name);
        //thumbnail->setWidth(thumbnail_specs->second.width);
        //thumbnail->setHeight(thumbnail_specs->second.height);
        //thumbnail->setLength(thumbnail_specs->second.length);
        thumbnails.push_back(thumbnail);
        std::cout << "Done slit-scanning " << filename << " in " << getTime()-slit_scan_in << " sec." << std::endl;
    }
    osg::setNotifyLevel(notify_level);
    return thumbnails;
}

std::vector<std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> thumbnail_names;
    thumbnail_names.push_back("Video slit-scan");
    return thumbnail_names;
}

std::map<std::string,ACMediaType> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["Video slit-scan"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["Video slit-scan"] = "Video slit-scan";
    return thumbnail_descriptions;
}

std::map<std::string,std::string> ACVideoOpenCVSlitScanThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["Video slit-scan"] = ".jpg";
    return extensions;
}

// Using OpenCV, frame jitter
// XS TODO try with cv::VideoCapture
int ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan(std::string _thumbnail_filename){
    //CvCapture* video = cvCreateFileCapture(filename.c_str());
    cv::VideoCapture* capture = new cv::VideoCapture(filename.c_str());
    if ( !capture) {
        cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> Could not open video..." << endl;
        return 0;
    }

    int width   = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
    int height  = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    //int fps     = (int) capture->get(CV_CAP_PROP_FPS);
    int total_frames = (int) capture->get(CV_CAP_PROP_FRAME_COUNT)-1; // XS -1 seems necessary in OpenCV 2.3

    //cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)summary_frame_in);
    slit_scan = 0;

    //IplImage* slit_scan;
    for (unsigned int f=0;f<total_frames;f++){
        //cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)f);
        /*if(!cvGrabFrame(video)){
   cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> Could not find frame..." << endl;
  }
  else{*/
        cv::Mat openCVframe;
        capture->set(CV_CAP_PROP_POS_FRAMES, f);
        *capture >> openCVframe;
        if (!openCVframe.data) {
            cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> unexpected missing data frame" << endl;
            break;
        }
        //if(f==summary_frame_in)
        //	slit_scan = cvCreateImage( cvSize(summary_frame_out-summary_frame_in, height), frame->depth, frame->nChannels );
        //int ff = (int) capture->get(CV_CAP_PROP_POS_FRAMES);
#ifdef USE_DEBUG
        //if (ff!=f) cout << "Mismatch at frame " << ff << " instead of " << f << " (offset:" << ff-f << ")" << endl;
#endif
        osg::ref_ptr<osg::Image> frame = Convert_OpenCV_2_osg_Image(openCVframe);
        if (!frame){
            cerr << "<ACVideoOpenCVSlitScanThumbnailerPlugin::computeSlitScan> problem converting from OpenCV to OSG" << endl;
            return 0;
        }
        else{
            if(!slit_scan){
                slit_scan = new osg::Image;
                slit_scan->allocateImage(total_frames, height, 1, GL_RGB, frame->getDataType());
            }
            //frame->setOrigin(osg::Image::TOP_LEFT);
            //frame->flipVertical();

            std::cout << "ACVideoOpenCVSlitScanThumbnailerPlugin: processing frame " << f << " / " << total_frames << std::endl;

            osg::copyImage(frame,
                           (float)width/2.0f,//int  	src_s,
                           0,//int  	src_t,
                           0,//int  	src_r,
                           1,//int  	width,
                           height,//int  	height,
                           1,//int  	depth,
                           slit_scan,
                           f,//int  	dest_s,
                           0,//int  	dest_t,
                           0,//int  	dest_r,
                           false//bool  	doRescale = false
                           );
        }
        //}
    }
    osgDB::writeImageFile(*slit_scan,_thumbnail_filename);
    return 1;
}
