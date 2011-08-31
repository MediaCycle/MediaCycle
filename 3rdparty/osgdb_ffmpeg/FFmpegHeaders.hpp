
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
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <avcodec.h>
#include <avformat.h>
#include <avdevice.h>
#include <imgutils.h>
#include <parseutils.h>

#ifdef USE_SWSCALE    
    #include <swscale.h>
#endif

}



#endif // HEADER_GUARD_FFMPEG_HEADERS_H
