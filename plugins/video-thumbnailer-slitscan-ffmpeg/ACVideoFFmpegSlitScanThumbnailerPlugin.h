/**
 * @brief A plugin that provides a video slit-scan thumbnail with JPG as output using FFMpeg.
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

#ifndef ACVideoFFmpegSlitScanThumbnailerPlugin_H
#define	ACVideoFFmpegSlitScanThumbnailerPlugin_H

#include <ACPlugin.h>

#include <ACFFmpegInclude.h>

#include <cassert>
#include <algorithm>
#include <vector>

#include <iostream>

#include <osg/Notify>
#include <osg/Image>

class ACVideoFFmpegSlitScanThumbnailerPlugin : public ACThumbnailerPlugin
{
public:

    ACVideoFFmpegSlitScanThumbnailerPlugin();
    virtual ~ACVideoFFmpegSlitScanThumbnailerPlugin();

    virtual std::vector<std::string> getThumbnailNames();
    virtual std::map<std::string,ACMediaType> getThumbnailTypes();
    virtual std::map<std::string,std::string> getThumbnailDescriptions();
    virtual std::map<std::string,std::string> getThumbnailExtensions();
    virtual std::string requiresMediaReaderPlugin();
    virtual std::vector<std::string> requiresFeaturesPlugins(); // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins(); // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia);

    std::string createFileName(std::string _media_filename, std::string _thumbnail_name, std::string _thumbnail_extension, int segment_id=0);

private:
    osg::ref_ptr<osg::Image> slit_scan_image;
    AVCodecContext* m_context;
    bool _done;
    std::string filename;
    osg::NotifySeverity notify_level;
protected:
    int convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,int src_pix_fmt, int src_width, int src_height);
    void yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height);
    ACMediaThumbnail* computeSlitScan(std::string _media_filename);
public:
    osg::ref_ptr<osg::Image> getImage(){if (_done) return slit_scan_image; else return 0;}
    void setFileName(std::string _filename){filename = _filename;}
    std::string getFileName(){return filename;}
};

#endif
