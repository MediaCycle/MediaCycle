/*
 *  ACFFmpegToOpenCV.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 21/03/2011
 *  Based on http://goppodu.wordpress.com/2010/02/21/ffmpeg-with-opencv/
 *
 */
#ifndef APPLE_LEOPARD
#if defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
#ifndef ACFFmpegToOpenCV_H
#define ACFFmpegToOpenCV_H

#include <ACFFmpegInclude.h>
#include <ACOpenCVInclude.h>

#define FSKIP 1

//CF TODO make this a real/proper class

class ACFFmpegToOpenCV
{
public:
	ACFFmpegToOpenCV(){};
	~ACFFmpegToOpenCV(){};
    AVFormatContext *pFormatCtx;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPicture       pict;
    AVPacket        packet;
    AVRational      fr;
    int             frameFinished;
    int             numBytes;
    uint8_t         *buffer;
    int fskip, m, n, p;
    IplImage * dum;
    unsigned char *ptr1;
    float *ptr2;
    float fps;
    int nCols;
    int nRows;
    int nBands;
    int nframes;
    int nb_frames;

    int init(const char *);
    int movie_end;
    struct SwsContext * img_convert_ctx;

    void getframe(IplImage ** frame);

    void closeit();

};
#endif //defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
#endif
#endif
