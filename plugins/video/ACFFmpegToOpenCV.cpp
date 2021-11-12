/*
 *  ACFFmpegToOpenCV.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 21/03/2011
 *  Based on http://goppodu.wordpress.com/2010/02/21/ffmpeg-with-opencv/
 *
 */
#ifndef APPLE_LEOPARD
#if defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
#include "ACFFmpegToOpenCV.h"
#include<iostream>

//TO TEST use PIX_FMT_BGR24 instead of PIX_FMT_RGB24?

//TODO transform this into a FFmpeg to OpenCV/OSG utility class

int ACFFmpegToOpenCV::init(const char * file)
{
    fskip = 1;
    m = 0;
    n = 0;
    p = 0;
    nframes = 0;
    nb_frames = 0;
    movie_end = 0;

	// Register all formats and codecs
    av_register_all();

	// Open video file
#if LIBAVFORMAT_BUILD < (54<<16 | 29<<8 | 0)
    if(av_open_input_file(&pFormatCtx, file, NULL, 0, NULL)!=0){
#else
    if(avformat_open_input(&pFormatCtx, file, 0, 0) < 0){
#endif
    	std::cerr << "ACFFmpegToOpenCV Couldn't open file " << file << std::endl;
    	return -1;
    }

	// Retrieve stream information
    if(av_find_stream_info(pFormatCtx)<0){
    	std::cerr << "ACFFmpegToOpenCV Couldn't find stream information from file " << file << std::endl;
    	return -1;
    }

	// Dump information about file onto standard error
#if LIBAVFORMAT_BUILD < (54<<16 | 29<<8 | 0)
    dump_format(pFormatCtx, 0, file, 0);
#else
    av_dump_format(pFormatCtx, 0, file, 0);
#endif

	// Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
#if LIBAVUTIL_BUILD < (50<<16 | 14<<8 | 0) 
		if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
#else
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
#endif
        {
            videoStream=i;
            fr = pFormatCtx->streams[videoStream]->r_frame_rate;
            break;
        }
    if(videoStream==-1){
    	std::cout << "ACFFmpegToOpenCV Didn't find a video stream in file " << file << std::endl;
    	return -1;
    }

	// Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
    {
        std::cout << "ACFFmpegToOpenCV Codec not found for file " << file << std::endl;
        return -1;
    }

	// Open codec
    if(avcodec_open(pCodecCtx, pCodec)<0){
    	std::cout << "ACFFmpegToOpenCV Could not open codec for file " << file << std::endl;
    	return -1;
    }


	// Allocate video frame
    pFrame=avcodec_alloc_frame();

	// Allocate an AVFrame structure
	//  pFrameRGB=avcodec_alloc_frame();
	// if(pFrameRGB==NULL)
	//  return -1;

	// Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	//avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
	//         pCodecCtx->width, pCodecCtx->height);

    avpicture_fill(&pict, buffer, PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24,
                                     SWS_BICUBIC,
                                     NULL, NULL, NULL);

    if(img_convert_ctx == NULL)
    {
        fprintf(stderr, "ACFFmpegToOpenCV Cannot initialize the conversion context!\n");
        exit(1);
    }

    dum = cvCreateImage(cvSize(pCodecCtx->width,pCodecCtx->height),
                        IPL_DEPTH_32F,3);

//compute the frame rate
    fps = (fr.num + 0.0) / (fr.den + 0.0);

    nCols = pCodecCtx->width;
    nRows = pCodecCtx->height;
    nBands = 3;

    //CF
    nb_frames =  pFormatCtx->streams[videoStream]->nb_frames; //CF alternative: pFormatCtx->streams[videoStream]->nb_index_entries or duration/frame_rate for corrupted files?
}

void ACFFmpegToOpenCV::getframe(IplImage ** frame)
{
	#ifdef USE_DEBUG
    std::cout << "ACFFmpegToOpenCV Processing frame " << pCodecCtx->frame_number << " (" << nframes << "th) " << " / " << nb_frames << "\r" << std::flush;
    //CF the 1st number seems jittery, the 2nd should be then used as frame number
	#endif
	
	//Read frames and store to IplImages
    i=0;
    while(fskip <= FSKIP)//CF seems to be neverending with dancers videos...
    {
        if(av_read_frame(pFormatCtx, &packet)>=0)
        {
			// Is this a packet from the video stream?
            if(packet.stream_index==videoStream)
            {
				// Decode video frame
#if (AV_VERSION_INT(52,25,0) < LIBAVCODEC_VERSION_INT)
                avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
#else
                avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
#endif

				// Did we get a video frame?
                if(frameFinished)
                {

					// Convert the image from its native format to RGB
                    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize,
                              0, pCodecCtx->height, pict.data,
                              pict.linesize);

                    /*
                    img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24,
                    (AVPicture*)pFrame, pCodecCtx->pix_fmt,
                    pCodecCtx->width, pCodecCtx->height);
                    */

					//convert frame to 32F grayscale
                    if(fskip == FSKIP)
                    {
                        for(m = 0; m < pCodecCtx->height; m++)
                        {
                            ptr1 = pict.data[0]+ m*pict.linesize[0];

                            ptr2 = (float*)(dum->imageData + m*dum->widthStep);

                            for(n = 0; n < 3 * pCodecCtx->width; n++)

                                ptr2[n] =  (float)(ptr1[n]);

                        }

                        cvConvertScale(dum, *frame, 1.0, 0.0);

                        fskip=0;

                        break;
                    }

                    fskip++;
                }
            }

			// Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
        else
        {
            movie_end = 1;
            break;
        }
    }
    nframes++;
}

void ACFFmpegToOpenCV::closeit()
{
    // Free the RGB image
    av_free(buffer);
    //CF   av_free(&pict);  // to debug, with cvCopy? due to previousFrame in ACVideoAnalysis?

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    av_close_input_file(pFormatCtx);
}
#endif //defined(SUPPORT_VIDEO) and defined(USE_FFMPEG)
#endif

