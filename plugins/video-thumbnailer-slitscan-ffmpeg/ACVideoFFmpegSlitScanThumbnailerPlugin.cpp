/**
 * @brief A plugin that provides a Slit-scan thumbnail with JPG as output using FFMpeg.
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

#include "ACVideoFFmpegSlitScanThumbnailerPlugin.h"
#include "ACVideo.h"
#include <cmath>
#include <osg/ImageUtils>
#include <osg/Version>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;
using namespace osg;

/// Maximum image size described in the JPEG format standard
/// Should be 65535, but the the OSG imageio plugin limits it to 65500
int max_jpg_pixels = 65500;

ACVideoFFmpegSlitScanThumbnailerPlugin::ACVideoFFmpegSlitScanThumbnailerPlugin()
    : ACThumbnailerPlugin(),filename(""),notify_level(osg::WARN),slit_scan_image(0)
{
    this->mName = "Slit-scan Thumbnailer (FFmpeg)";
    this->mDescription ="Plugin for summarizing video files in slit-scans (with FFmpeg)";
    this->mMediaType = MEDIA_TYPE_VIDEO;
    m_context = 0;
}

ACVideoFFmpegSlitScanThumbnailerPlugin::~ACVideoFFmpegSlitScanThumbnailerPlugin()
{
    //if (m_context)
    //   avcodec_close(m_context);
    m_context = 0;
}

std::string ACVideoFFmpegSlitScanThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "";
}

std::vector<std::string> ACVideoFFmpegSlitScanThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    // No features plugin is required
    return features_plugins;
}

std::vector<std::string> ACVideoFFmpegSlitScanThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}


int ACVideoFFmpegSlitScanThumbnailerPlugin::convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,
                                                    int src_pix_fmt, int src_width, int src_height)
{
    osg::Timer_t startTick = osg::Timer::instance()->tick();
    //#ifdef USE_SWSCALE
    //if (m_swscale_ctx==0)
    //{
    struct SwsContext * m_swscale_ctx = sws_getContext(src_width, src_height, (PixelFormat) src_pix_fmt,
                                                       src_width, src_height, (PixelFormat) dst_pix_fmt,
                                                       /*SWS_BILINEAR*/ SWS_BICUBIC, 0, 0, 0);
    //}
#if OSG_MIN_VERSION_REQUIRED(2,9,11)
    OSG_INFO<<"Using sws_scale ";
#endif
    int result =  sws_scale(m_swscale_ctx,
                            (src->data), (src->linesize), 0, src_height,
                            (dst->data), (dst->linesize));
    /*#else
  OSG_INFO<<"Using img_convert ";
  int result = img_convert(dst, dst_pix_fmt, src,
  src_pix_fmt, src_width, src_height);
  #endif*/
    osg::Timer_t endTick = osg::Timer::instance()->tick();
#if OSG_MIN_VERSION_REQUIRED(2,9,11)
    OSG_INFO<<" time = "<<osg::Timer::instance()->delta_m(startTick,endTick)<<"ms"<<std::endl;
#endif
    return result;
}

std::string ACVideoFFmpegSlitScanThumbnailerPlugin::createFileName(std::string _media_filename, std::string _thumbnail_name, std::string _thumbnail_extension, int segment_id){
    std::stringstream thumbnail_path;
    boost::filesystem::path media_path(_media_filename.c_str());
    std::string thumbnail_suffix(_thumbnail_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    boost::replace_all(thumbnail_suffix,"-","_");
    std::cout << _thumbnail_name << " converted to " << thumbnail_suffix << std::endl;
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
    thumbnail_path << media_path.parent_path().string() << slash << media_path.stem().string() << "_" << thumbnail_suffix;
    if(segment_id>0)
        thumbnail_path << "_" << segment_id;
    thumbnail_path << _thumbnail_extension;
    std::string thumbnail_filename = thumbnail_path.str();
    return thumbnail_filename;
}

std::vector<ACMediaThumbnail*> ACVideoFFmpegSlitScanThumbnailerPlugin::summarize(ACMedia* media){
    std::vector<ACMediaThumbnail*> thumbnails;
    filename = media->getFileName();

    std::cout << "Slit-scanning " << filename<< "..." << std::endl;
    double slit_scan_image_in = getTime();
    notify_level = osg::getNotifyLevel();
    osg::setNotifyLevel(osg::WARN);//to remove the copyImage NOTICEs
    thumbnails.push_back( this->computeSlitScan(filename) );
    osg::setNotifyLevel(notify_level);

    return thumbnails;
}

void ACVideoFFmpegSlitScanThumbnailerPlugin::yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height)
{
    convert(dst, PIX_FMT_RGB32, src, m_context->pix_fmt, width, height);
    const size_t bpp = 4;
    uint8_t * a_dst = dst->data[0] + 3;
    for (unsigned int h = 0; h < height; ++h) {
        const uint8_t * a_src = src->data[3] + h * src->linesize[3];
        for (unsigned int w = 0; w < width; ++w) {
            *a_dst = *a_src;
            a_dst += bpp;
            a_src += 1;
        }
    }
}

std::vector<std::string> ACVideoFFmpegSlitScanThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> thumbnail_names;
    thumbnail_names.push_back("Slit-scan");
    return thumbnail_names;
}

std::map<std::string,ACMediaType> ACVideoFFmpegSlitScanThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["Slit-scan"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACVideoFFmpegSlitScanThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["Slit-scan"] = "Slit-scan";
    return thumbnail_descriptions;
}

std::map<std::string,std::string> ACVideoFFmpegSlitScanThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["Slit-scan"] = ".jpg";
    return extensions;
}

ACMediaThumbnail* ACVideoFFmpegSlitScanThumbnailerPlugin::computeSlitScan(std::string _media_filename){
    ACMediaThumbnail* slit_scan_thumbnail = 0;

    std::string _name = "Slit-scan";

    double slit_in = getTime();

    AVFormatContext *pFormatCtx = 0;
    //pFormatCtx = new AVFormatContext();
    unsigned int             i, videoStreams,videoStream;
    AVPacket        packet;
    int             frameFinished;

    // Register all formats and codecs
    av_register_all();
#if LIBAVFORMAT_BUILD < (53<<16 | 2<<8 | 0)
    // Open video file
    if(av_open_input_file(&pFormatCtx, filename.c_str(), 0, 0, 0)!=0){
        std::cerr << "av_open_input_file : Couldn't open file" << std::endl;
        return 0;
    }
#else
    if(avformat_open_input(&pFormatCtx, filename.c_str(), 0, 0)<0){
        std::cerr << "avformat_open_input : Couldn't open file" << std::endl;
        return 0;
    }
#endif

    // Retrieve stream information
    // av_find_stream_info deprecated, use avformat_find_stream_info
    if(avformat_find_stream_info(pFormatCtx, NULL)<0){
        std::cerr << "Couldn't find stream information" << std::endl;
        return 0;
    }

    // Dump information about file onto standard error
    //dump_format(pFormatCtx, 0, filename.c_str(), false);

    // Count video streams
    videoStreams=0;
    for(i=0; i<pFormatCtx->nb_streams; i++)
#if LIBAVUTIL_BUILD < (50<<16 | 14<<8 | 0)
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
#else
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
#endif
            videoStreams++;
    if(videoStreams == 0)
        std::cout << "Didn't find any video stream." << std::endl;
    else
        std::cout << "Found " << videoStreams << " video stream(s)." << std::endl;

    // Find the first video stream
    if (videoStreams>0){

        videoStream=-1;
        for(i=0; i<pFormatCtx->nb_streams; i++)
#if LIBAVUTIL_BUILD < (50<<16 | 14<<8 | 0)
            if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
#else
            if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
#endif
            {
                videoStream=i;
                break;
            }

        // osgFFMpeg FFmpegDecoderVideo::open

        AVStream* m_stream = pFormatCtx->streams[videoStream];
        int nb_frames =  m_stream->nb_frames; //CF alternative: pFormatCtx->streams[videoStream]->nb_index_entries or duration/summary_frame_rate for corrupted files?

        // Computing the number of slit scan segments depending on max_jpg_pixels
        int segment_width = 0;
        int number_of_segments = 0;
        number_of_segments = (int) ceil( (float)(nb_frames)/(float)(max_jpg_pixels) );
        segment_width =  (int) floor((float)(nb_frames) / (float)number_of_segments );
        int last_segment_width = nb_frames - (number_of_segments-1)*segment_width;

        m_context = m_stream->codec;

        // Trust the video size given at this point
        // (avcodec_open seems to sometimes return a 0x0 size)
        int width = m_context->width;
        int height = m_context->height;

        // Checking if the slit-scan segments are already on the disk
        if(number_of_segments == 0)
            return 0;
        else if(number_of_segments == 1){
            std::string current_filename = this->createFileName(_media_filename,_name,".jpg");
            fs::path p( current_filename.c_str());// , fs::native );
            if ( fs::exists( p ) )
            {
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << current_filename << std::endl;
                if ( fs::is_regular( p ) )
                {
                    std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: file is regular: " << current_filename << std::endl;
                    if(fs::file_size( p ) > 0 ){
                        std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: size of " << current_filename << " is non-zero, not recomputing "<< std::endl;
                        slit_scan_thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                        slit_scan_thumbnail->setName(_name);
                        slit_scan_thumbnail->setFileName(current_filename);
                        slit_scan_thumbnail->setWidth(nb_frames);
                        slit_scan_thumbnail->setHeight(height);
                        slit_scan_thumbnail->setLength(nb_frames);
                    }
                }
            }
        }
        else{
            bool fail = false;
            ACMediaThumbnail* tentative_thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
            tentative_thumbnail->setName(_name);
            tentative_thumbnail->setWidth(nb_frames);
            tentative_thumbnail->setHeight(height);
            tentative_thumbnail->setLength(nb_frames);
            for(int current_segment = 1; current_segment<=number_of_segments;current_segment++){
                std::string current_filename = this->createFileName(_media_filename,_name,".jpg", current_segment);
                int current_segment_width = segment_width;
                if(current_segment == number_of_segments)
                    current_segment_width = last_segment_width;

                fs::path p( current_filename.c_str());// , fs::native );
                if ( !fs::exists( p ) ){
                    fail = true;
                    break;
                }
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: expected thumbnail " << current_segment << "/" << number_of_segments << " already exists as file: " << current_filename << std::endl;
                if ( !fs::is_regular( p ) ){
                    fail = true;
                    break;
                }
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: file is regular: " << current_filename << std::endl;
                if(fs::file_size( p ) <= 0 ){
                    fail = true;
                    break;
                }
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: size of " << current_filename << " is non-zero, not recomputing "<< std::endl;
                ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                thumbnail->setFileName(current_filename);
                thumbnail->setName(_name);
                thumbnail->setWidth(current_segment_width);
                thumbnail->setHeight(height);
                thumbnail->setLength(current_segment_width);
                tentative_thumbnail->addSegment(thumbnail);
            }
            if(!fail){
                slit_scan_thumbnail = tentative_thumbnail;
            }
            else{
                if(tentative_thumbnail)
                    delete tentative_thumbnail;
                std::cout << "ACFFmpegVideoResizeThumbnailerPlugin::summarize: couldn't load all segments from the slit-scan, recomputing "<< std::endl;
            }
        }

        if(!slit_scan_thumbnail){

            // Find the decoder for the video stream
            AVCodec* m_codec = avcodec_find_decoder(m_context->codec_id);

            if (m_codec == 0)
                throw std::runtime_error("avcodec_find_decoder() failed");
#if LIBAVCODEC_BUILD < (53<<16 | 8<<8 | 0)
            // Open codec
            if (avcodec_open(m_context, m_codec) < 0)
                throw std::runtime_error("avcodec_open() failed");
#else
            // Open codec
            if (avcodec_open2(m_context, m_codec, NULL) < 0)
                throw std::runtime_error("avcodec_open2() failed");
#endif
            // Allocate video frame
            AVFrame* m_frame=avcodec_alloc_frame();

            // Allocate converted RGB frame
            AVFrame* m_summary_frame_rgba=avcodec_alloc_frame();
            std::vector<uint8_t> m_buffer_rgba;
            m_buffer_rgba.resize(avpicture_get_size(PIX_FMT_RGB32, width, height));

            // Assign appropriate parts of the buffer to image planes in m_summary_frame_rgba
            avpicture_fill((AVPicture *) m_summary_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);

            // Back to FFmpeg tuto

            // Video stream properties
            //float summary_frame_rate = av_q2d(m_stream->r_summary_frame_rate);
            //float duration = (float)(pFormatCtx->duration)/AV_TIME_BASE;


            // Computing the number of slit scan segments depending on max_jpg_pixels
//            std::cout << "width " << width << std::endl;
//            std::cout << "height " << height << std::endl;
//            std::cout << "nb_frames " << nb_frames << std::endl;
//            std::cout << "max_jpg_pixels " << max_jpg_pixels << std::endl;
//            std::cout << "number_of_segments " << number_of_segments << std::endl;
//            std::cout << "segment_width " << segment_width << std::endl;
//            std::cout << "last_segment_width " << last_segment_width << std::endl;
            int current_segment = 1;
            int current_segment_width = segment_width;
            std::string current_filename("");

            if(number_of_segments>0){
                slit_scan_thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                slit_scan_thumbnail->setName(_name);
                slit_scan_thumbnail->setWidth(nb_frames);
                slit_scan_thumbnail->setHeight(height);
                slit_scan_thumbnail->setLength(nb_frames);
            }
            else
                return 0;

            if(number_of_segments == 1){
                current_filename = this->createFileName(_media_filename,_name,".jpg");
                slit_scan_thumbnail->setFileName( current_filename );
                slit_scan_thumbnail->setName(_name);
            }
            else{
                current_filename = this->createFileName(_media_filename,_name,".jpg", current_segment);
            }

            slit_scan_image = 0;
            double slit_mid = getTime();
            int summary_frames_processed = 0;

            while(av_read_frame(pFormatCtx, &packet)>=0)
            {
                // Is this a packet from the video stream?
                if(packet.stream_index==videoStream)
                {
                    // Decode video frame
#if LIBAVCODEC_BUILD < (52<<16 | 23<<8 | 0)
                    avcodec_decode_video(m_context, m_frame, &frameFinished,packet.data, packet.size); // deprecated since 2009-04-07 - r18351 - lavc 52.23.0, totally removed in ffmpeg 0.8
#else
                    AVPacket avpacket;
                    av_init_packet(&avpacket);
                    avpacket.data = packet.data;
                    avpacket.size = packet.size;
                    avpacket.flags = AV_PKT_FLAG_KEY; //TODO : check that this is needed
                    avcodec_decode_video2(m_context, m_frame, &frameFinished, &avpacket);
#endif
                    // Did we get a video frame?
                    if(frameFinished)
                    {
                        //std::cout << "ACVideoFFmpegSlitScanThumbnailerPlugin: processing frame " << m_context->frame_number << " / " << nb_frames << std::endl;

                        if(m_context->frame_number % segment_width == 1){
                            if(slit_scan_image && current_segment >=1){
                                osgDB::writeImageFile(*slit_scan_image,current_filename);
                                if(number_of_segments > 1){
                                    ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                                    thumbnail->setFileName(current_filename);
                                    thumbnail->setName(_name);
                                    thumbnail->setWidth(current_segment_width);
                                    thumbnail->setHeight(height);
                                    thumbnail->setLength(current_segment_width);
                                    slit_scan_thumbnail->addSegment(thumbnail);
                                }
                                current_segment++;
                                current_filename = this->createFileName(_media_filename,_name,".jpg", current_segment);
                                if(current_segment == number_of_segments)
                                    current_segment_width = last_segment_width;
                            }
                            slit_scan_image = 0;
                            slit_scan_image = new osg::Image;
                            slit_scan_image->allocateImage(current_segment_width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
                        }


                        AVPicture * const src = (AVPicture *) m_frame;
                        AVPicture * const dst = (AVPicture *) m_summary_frame_rgba;

                        // Assign appropriate parts of the buffer to image planes in m_summary_frame_rgba
                        avpicture_fill((AVPicture *) m_summary_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);

                        // Convert YUVA420p (i.e. YUV420p plus alpha channel) using our own routine
                        if (m_context->pix_fmt == PIX_FMT_YUVA420P)
                            yuva420pToRgba(dst, src, width, height);
                        else
                            convert(dst, PIX_FMT_RGB32, src, m_context->pix_fmt, width, height);

                        osg::ref_ptr<osg::Image> frame = new osg::Image();
                        frame->setOrigin(osg::Image::TOP_LEFT);
                        frame->setImage(
                                    width,height, 1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
                                    const_cast<unsigned char *>( &((m_buffer_rgba)[0])), osg::Image::NO_DELETE
                                );
                        frame->flipVertical();

                        osg::copyImage(frame,
                                       (float)width/2.0f,//int  	src_s,
                                       0,//int  	src_t,
                                       0,//int  	src_r,
                                       1,//int  	width,
                                       height,//int  	height,
                                       1,//int  	depth,
                                       slit_scan_image,
                                       m_context->frame_number - (current_segment-1)*segment_width,//int  	dest_s,
                                       0,//int  	dest_t,
                                       0,//int  	dest_r,
                                       false//bool  	doRescale = false
                                       );

                        summary_frames_processed++;
                    }
                }

                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);
            }
            // Free the RGB image
            m_buffer_rgba.empty();
            av_free(m_summary_frame_rgba);

            // Free the YUV frame
            av_free(m_frame);


            if(slit_scan_image && current_segment >=1){
                osgDB::writeImageFile(*slit_scan_image,current_filename);
                if(number_of_segments > 1){
                    ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                    thumbnail->setFileName(current_filename);
                    thumbnail->setName(_name);
                    thumbnail->setWidth(current_segment_width);
                    thumbnail->setHeight(height);
                    thumbnail->setLength(current_segment_width);
                    slit_scan_thumbnail->addSegment(thumbnail);
                }
            }
            slit_scan_image = 0;


            double slit_end = getTime();
            std::cout << "Slit-scanning took " << slit_end-slit_mid << " after " << slit_mid-slit_in << " of init " << std::endl;
            std::cout << "Missed " << nb_frames - summary_frames_processed << " frames over " << nb_frames << std::endl;
        }

        // Close the codec
        avcodec_close(m_context);

    }
    // Close the video file
    av_close_input_file(pFormatCtx);
    return slit_scan_thumbnail;
}
