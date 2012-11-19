
#ifndef HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H
#define HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H

#include "FFmpegHeaders.hpp"

#include <osg/Notify>


namespace osgFFmpeg {


#if LIBAVFORMAT_BUILD < (54<<16 | 29<<8 | 0)

class FFmpegParameters : public osg::Referenced
{
public:

    FFmpegParameters();
    ~FFmpegParameters();

    bool isFormatAvailable() const { return m_format!=NULL; }
    
    AVInputFormat* getFormat() { return m_format; }
    AVFormatParameters* getFormatParameter() { return &m_parameters; }
    
    void parse(const std::string& name, const std::string& value);

protected:

    AVInputFormat* m_format;
    AVFormatParameters m_parameters;
};
#endif


} // namespace osgFFmpeg



#endif // HEADER_GUARD_OSGFFMPEG_FFMPEG_PARAMETERS_H
