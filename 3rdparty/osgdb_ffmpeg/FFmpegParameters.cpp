
#include "FFmpegParameters.hpp"

#include <string>
#include <iostream>
#include <sstream>



namespace osgFFmpeg {



FFmpegParameters::FFmpegParameters() :
    m_format(0)
{
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
    memset(&m_parameters, 0, sizeof(m_parameters));
#else
    m_dictionary = NULL;
#endif
}


FFmpegParameters::~FFmpegParameters()
{}


void FFmpegParameters::parse(const std::string& name, const std::string& value)
{
#if LIBAVFORMAT_BUILD >= LIBAVFORMAT_BUILD_PARAM2DICT
	char buf[1024];
#endif

    if (value.empty())
    {
        return;
    }
    else if (name == "format")
    {
        avdevice_register_all();
        m_format = av_find_input_format(value.c_str());
        if (!m_format)
            OSG_NOTICE<<"Failed to apply input video format: "<<value.c_str()<<std::endl;
    }
    else if (name == "pixel_format")
    {
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
        m_parameters.pix_fmt = av_get_pix_fmt(value.c_str());
#else
        //TODO there must be a simpler sol. than "av_get_pix_fmt_name(av_get_pix_fmt(value.c_str()))"
        av_dict_set(&m_dictionary, "pixel_format", av_get_pix_fmt_name(av_get_pix_fmt(value.c_str())), 0);
#endif
    }
    else if (name == "frame_size")
    {
        int frame_width = 0, frame_height = 0;
        if (av_parse_video_size(&frame_width, &frame_height, value.c_str()) < 0)
        {
            OSG_NOTICE<<"Failed to apply frame size: "<<value.c_str()<<std::endl;
            return;
        }
        if ((frame_width % 2) != 0 || (frame_height % 2) != 0)
        {
            OSG_NOTICE<<"Frame size must be a multiple of 2: "<<frame_width<<"x"<<frame_height<<std::endl;
            return;
        }
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
        m_parameters.width = frame_width;
        m_parameters.height = frame_height;
#else
        snprintf(buf, sizeof(buf), "%dx%d", frame_width, frame_height);
        av_dict_set(&m_dictionary, "video_size", buf, 0);
#endif
    }
    else if (name == "frame_rate")
    {
        AVRational frame_rate;
        if (av_parse_video_rate(&frame_rate, value.c_str()) < 0)
        {
            OSG_NOTICE<<"Failed to apply frame rate: "<<value.c_str()<<std::endl;
            return;
        }
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
        m_parameters.time_base.den = frame_rate.num;
        m_parameters.time_base.num = frame_rate.den;
#else
        snprintf(buf, sizeof(buf), "%d/%d", frame_rate.num, frame_rate.den);
        av_dict_set(&m_dictionary, "framerate", buf, 0);
#endif
    }
    else if (name == "audio_sample_rate")
    {
        int audio_sample_rate = 44100;
        std::stringstream ss(value); ss >> audio_sample_rate;
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
        m_parameters.sample_rate = audio_sample_rate;
#else
        snprintf(buf, sizeof(buf), "%d", audio_sample_rate);
        av_dict_set(&m_dictionary, "sample_rate", buf, 0);
#endif
    }
}



} // namespace osgFFmpeg
