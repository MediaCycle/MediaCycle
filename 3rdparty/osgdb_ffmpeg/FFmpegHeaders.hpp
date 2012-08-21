
#ifndef HEADER_GUARD_FFMPEG_HEADERS_H
#define HEADER_GUARD_FFMPEG_HEADERS_H

//CF 2.8.x compatibility
#include <osg/Referenced>
#include <osg/Version>
#include <osg/Notify>
#if OSG_MIN_VERSION_REQUIRED(2,9,10)
#else
#undef OSG_NOTIFY
#define OSG_NOTIFY(level) if (osg::isNotifyEnabled(level)) osg::notify(level)
#define OSG_ALWAYS OSG_NOTIFY(osg::ALWAYS)
#define OSG_FATAL OSG_NOTIFY(osg::FATAL)
#define OSG_WARN OSG_NOTIFY(osg::WARN)
#define OSG_NOTICE OSG_NOTIFY(osg::NOTICE)
#define OSG_INFO OSG_NOTIFY(osg::INFO)
#define OSG_DEBUG OSG_NOTIFY(osg::DEBUG_INFO)
#define OSG_DEBUG_FP OSG_NOTIFY(osg::DEBUG_FP)
#endif
//CF end

//AM FFMPEG > 0.7 compatibility
//when AVFormatParameter was deprecated for AVDictionary
#define LIBAVFORMAT_BUILD_PARAM2DICT (53<<16 | 2<<8 | 0)
//when AVERROR_... was deprecated for AVERROR(e)
#define LIBAVUTIL_BUILD_AVERRORFCT (50<<16 | 12<<8 | 0) 
//when CODEC_TYPE_... was deprecated for AVMEDIA_TYPE_...
#define LIBAVUTIL_BUILD_CODEC2AVMEDIA (50<<16 | 14<<8 | 0) 

extern "C"
{
#define __STDC_CONSTANT_MACROS
#define FF_API_OLD_SAMPLE_FMT 0
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <avcodec.h>
#include <avformat.h>
#include <avdevice.h>
#include <mathematics.h>

#ifdef USE_SWSCALE    
    #include <swscale.h>
#endif

#if LIBAVUTIL_VERSION_INT <  AV_VERSION_INT(50,38,0)
#define AV_SAMPLE_FMT_NONE SAMPLE_FMT_NONE
#define AV_SAMPLE_FMT_U8   SAMPLE_FMT_U8
#define AV_SAMPLE_FMT_S16  SAMPLE_FMT_S16
#define AV_SAMPLE_FMT_S32  SAMPLE_FMT_S32
#define AV_SAMPLE_FMT_FLT  SAMPLE_FMT_FLT
#define AV_SAMPLE_FMT_DBL  SAMPLE_FMT_DBL
#define AV_SAMPLE_FMT_NB   SAMPLE_FMT_NB
#endif

}

#endif // HEADER_GUARD_FFMPEG_HEADERS_H
