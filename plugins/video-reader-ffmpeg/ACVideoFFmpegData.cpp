/**
 * @brief A class that provides a media data instance to parse and read videos using FFmpeg.
 * @author Christian Frisson
 * @date 14/12/2012
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

#include "ACVideoFFmpegData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

bool ACVideoFFmpegData::readData(std::string filename){
    if(filename==""){
        return false;
    }
    if(pFormatCtx)
        delete pFormatCtx;
    pFormatCtx = 0;
    unsigned int i, videoStreams,videoStream;

    // Register all formats and codecs
    av_register_all();
#if LIBAVFORMAT_BUILD < (53<<16 | 2<<8 | 0)
    // Open video file
    if(av_open_input_file(&pFormatCtx, filename.c_str(), 0, 0, 0)!=0){
        std::cerr << "av_open_input_file : Couldn't open file" << std::endl;
        return false;
    }
#else
    if(avformat_open_input(&pFormatCtx, filename.c_str(), 0, 0)<0){
        std::cerr << "avformat_open_input : Couldn't open file" << std::endl;
        return false;
    }
#endif

    // Retrieve stream information
    // av_find_stream_info deprecated, use avformat_find_stream_info
    if(avformat_find_stream_info(pFormatCtx, NULL)<0){
        std::cerr << "Couldn't find stream information" << std::endl;
        return -1;
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
    if(videoStreams == 0){
        std::cout << "Didn't find any video stream." << std::endl;
        return false;
    }
    return true;
}

bool ACVideoFFmpegData::closeFile(){
    if(pFormatCtx){
        av_close_input_file(pFormatCtx);
        return true;
    }
    else
        return false;
}

ACMediaDataContainer* ACVideoFFmpegData::getBuffer(int start_frame, int number_of_frames, int channel)
{
    if(start_frame < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid start frame " << start_frame << " (<0)" << std::endl;
        return 0;
    }

    if(number_of_frames < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid number of frames " << number_of_frames << " (<0)" << std::endl;
        return 0;
    }

    if(channel < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid channel " << channel << " (<0)" << std::endl;
        return 0;
    }

    if(!pFormatCtx)
        this->readData(this->file_name);
    if(!pFormatCtx){
        std::cerr << "ACVideoFFmpegData::getBuffer: couldn't load file '" << this->file_name << "'" << std::endl;
        return 0;
    }

    if(start_frame > this->getNumberOfFrames()){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid start frame " << start_frame << " beyond " << this->getNumberOfFrames() << std::endl;
        return 0;
    }

    if(start_frame+number_of_frames > this->getNumberOfFrames()){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid number of frames " << number_of_frames << " goes beyond " << this->getNumberOfFrames() << " with start frame " << start_frame << std::endl;
        return 0;
    }

    if(channel > this->getNumberOfChannels()){
        std::cerr << "ACVideoFFmpegData::getBuffer: invalid channel " << channel << " beyond " << this->getNumberOfChannels() << std::endl;
        return 0;
    }


#if LIBAVUTIL_BUILD < (50<<16 | 14<<8 | 0)
    if(pFormatCtx->streams[channel]->codec->codec_type!=CODEC_TYPE_VIDEO)
#else
    if(pFormatCtx->streams[channel]->codec->codec_type!=AVMEDIA_TYPE_VIDEO)
#endif
    {
        std::cerr  << "ACVideoFFmpegData::getBuffer: channel " << channel << " doesn't contain video" << std::endl;
        return 0;
    }

    AVStream* m_stream = pFormatCtx->streams[channel];
    m_context = m_stream->codec;

    // Trust the video size given at this point
    // (avcodec_open seems to sometimes return a 0x0 size)
    int width = m_context->width;
    int height = m_context->height;

    AVCodec* m_codec = avcodec_find_decoder(m_context->codec_id);

    if (m_codec == 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: couldn't find find a decoder" << std::endl;
        return 0;
    }
#if LIBAVCODEC_BUILD < (53<<16 | 8<<8 | 0)
    // Open codec
    if (avcodec_open(m_context, m_codec) < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: couldn't open the decoder" << std::endl;
        return 0;
    }
#else
    // Open codec
    if (avcodec_open2(m_context, m_codec, NULL) < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: couldn't open the decoder" << std::endl;
        return 0;
    }
#endif
    // Allocate video frame
    AVFrame* m_frame=avcodec_alloc_frame();

    // Allocate converted RGB frame
    AVFrame* m_summary_frame_rgba=avcodec_alloc_frame();

    std::vector<uint8_t> m_buffer_rgba;
    m_buffer_rgba.resize(avpicture_get_size(PIX_FMT_RGB32, width, height));

    // Assign appropriate parts of the buffer to image planes in m_summary_frame_rgba
    avpicture_fill((AVPicture *) m_summary_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);

    AVPacket        packet;
    bool done = false;
    AVPicture* src = 0;
    AVPicture* dst = 0;
    int frameFinished;


    const AVRational AvTimeBaseQ = { 1, AV_TIME_BASE }; // = AV_TIME_BASE_Q

    //const int64_t pos = int64_t(m_clocks.getStartTime()+time * double(AV_TIME_BASE));
    const int64_t pos = int64_t(start_frame);
    const int64_t seek_target = av_rescale_q(pos, AvTimeBaseQ, m_stream->time_base);


    if (av_seek_frame(pFormatCtx, channel, seek_target, 0/*AVSEEK_FLAG_BYTE |*/ /*AVSEEK_FLAG_BACKWARD*/) < 0){
        std::cerr << "ACVideoFFmpegData::getBuffer: couldn't seek to desired start frame" << std::endl;
        return 0;
    }

    while(av_read_frame(pFormatCtx, &packet)>=0 && !done){
        // Is this a packet from the video stream?
        if(packet.stream_index==channel)
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
                src = (AVPicture *) m_frame;
                dst = (AVPicture *) m_summary_frame_rgba;

                std::cout << "Processing frame " << m_context->frame_number << " / " << this->getNumberOfFrames() << std::endl;
                // Assign appropriate parts of the buffer to image planes in m_summary_frame_rgba
                avpicture_fill((AVPicture *) m_summary_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);

                // Convert YUVA420p (i.e. YUV420p plus alpha channel) using our own routine
                if (m_context->pix_fmt == PIX_FMT_YUVA420P)
                    yuva420pToRgba(dst, src, width, height);
                else
                    convert(dst, PIX_FMT_RGB32, src, m_context->pix_fmt, width, height);

            }
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }

    }
    // Free the RGB image
    m_buffer_rgba.empty();
    //av_free(m_summary_frame_rgba);


    // Free the YUV frame
    av_free(m_frame);

    // Close the codec
    avcodec_close(m_context);

    ACVideoFFmpegDataContainer* video_data = new ACVideoFFmpegDataContainer();
    video_data->setData((AVFrame*)dst);
    //video_data->setNumberOfFrames(1);
    return video_data;
}

int ACVideoFFmpegData::convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,
                                      int src_pix_fmt, int src_width, int src_height)
{
    //#ifdef USE_SWSCALE
    //if (m_swscale_ctx==0)
    //{
    struct SwsContext * m_swscale_ctx = sws_getContext(src_width, src_height, (PixelFormat) src_pix_fmt,
            src_width, src_height, (PixelFormat) dst_pix_fmt,
            /*SWS_BILINEAR*/ SWS_BICUBIC, 0, 0, 0);
    //}
    int result =  sws_scale(m_swscale_ctx,
                            (src->data), (src->linesize), 0, src_height,
                            (dst->data), (dst->linesize));
    return result;
}


void ACVideoFFmpegData::yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height)
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

float ACVideoFFmpegData::getSampleRate()
{
    if(pFormatCtx)
        return pFormatCtx->streams[0]->codec->sample_rate;
    else
        return 0.0f;
}

int ACVideoFFmpegData::getNumberOfChannels()
{
    if(pFormatCtx){
        unsigned int videoStreams = 0;
        for(int i=0; i<pFormatCtx->nb_streams; i++)
#if LIBAVUTIL_BUILD < (50<<16 | 14<<8 | 0)
            if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
#else
            if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
#endif
                videoStreams++;
        return videoStreams;
    }
    else
        return 0;
}

int ACVideoFFmpegData::getNumberOfFrames()
{
    if(pFormatCtx)
        return pFormatCtx->streams[0]->nb_frames;
    else
        return 0;
}

int ACVideoFFmpegData::getWidth()
{
    if(pFormatCtx)
        return pFormatCtx->streams[0]->codec->width;
    else
        return 0;
}

int ACVideoFFmpegData::getHeight()
{
    if(pFormatCtx)
        return pFormatCtx->streams[0]->codec->height;
    else
        return 0;
}

