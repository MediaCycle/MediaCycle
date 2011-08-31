
#ifndef HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H
#define HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H

#include "FFmpegHeaders.hpp"

#include <osg/Notify>


namespace osgFFmpeg {



class FFmpegParameters : public osg::Referenced
{
public:

    FFmpegParameters();
    ~FFmpegParameters();

    bool isFormatAvailable() const { return m_format!=NULL; }
    
    AVInputFormat* getFormat() { return m_format; }
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
    AVFormatParameters* getFormatParameter() { return &m_parameters; }
#else 
    AVDictionary* getDictionary() { return m_dictionary; }
#endif
    void parse(const std::string& name, const std::string& value);

protected:

    AVInputFormat* m_format;
#if LIBAVFORMAT_BUILD < LIBAVFORMAT_BUILD_PARAM2DICT
    AVFormatParameters m_parameters;
#else
    AVDictionary *m_dictionary;
#endif
};



} // namespace osgFFmpeg



#endif // HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H
