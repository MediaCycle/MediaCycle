/*
 *  ACOsgVideoTrackRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/12/10
 *
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

// ----------- uncomment this to compute and visualize a slit scan
#define USE_SLIT_SCAN

// ----------- uncomment this to use selection videos to be sync'd by thread (as opposed as sync'd in updateTracks)
//#define SYNC_SELECTION_VIDEOS_BY_THREAD

// ----------- uncomment this to use one thread per selection video sync (as opposed to one thread for all), per side (left+right), requires SYNC_SELECTION_VIDEOS_BY_THREAD
//#define SYNC_THREAD_PER_SELECTION_VIDEO

// ----------- uncomment this to test the selection video sync (without time skip based on the selection width)
//#define TEST_SYNC_WITHOUT_TIME_SKIP

#include "ACOsgVideoTrackRenderer.h"
#include "ACOpenCVInclude.h"
#include "ACVideo.h"
#include <cmath>
#include <osg/ImageUtils>
#include <osgDB/WriteFile>
#include <osg/Version>

using namespace std;
using namespace osg;

#if defined (SUPPORT_VIDEO)

osg::ref_ptr<osg::Image> Convert_OpenCV_2_osg_Image(cv::Mat cvImg)
{
	// XS uncomment the following 4 lines for visual debug (e.g., thumbnail)
	//	cvNamedWindow("T", CV_WINDOW_AUTOSIZE);
	//	cvShowImage("T", cvImg);
	//	cvWaitKey(0);
	//	cvDestroyWindow("T");

	if(cvImg.channels() == 3)
	{
		// Flip image from top-left to bottom-left origin
//		if(cvImg->origin == 0) {
//			cvConvertImage(cvImg , cvImg, CV_CVTIMG_FLIP);
//			cvImg->origin = 1;
//		}

		// Convert from BGR to RGB color format
		//printf("Color format %s\n",cvImg->colorModel);
//		if ( !strcmp(cvImg->channelSeq,"BGR") ) {
//			cvCvtColor( cvImg, cvImg, CV_BGR2RGB );
//		}

		osg::ref_ptr<osg::Image> osgImg = new osg::Image();

		/*temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
		 for (i=0;i<cvImg->height;i++) {
		 memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
		 }*/
		
		//cvtColor( cvImg, cvImg, CV_BGR2RGB );
		osgImg->setWriteHint(osg::Image::NO_PREFERENCE);

		osgImg->setImage(
						 cvImg.cols, //s (witdh)
						 cvImg.rows, //t (height)
						 3, //r //CF needs to be 1 otherwise scaleImage can't be used
						 GL_LINE_STRIP, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
						 GL_RGB, // GLenum pixelFormat, (GL_RGB, 0x1907)
						 GL_UNSIGNED_BYTE, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
						 cvImg.data,//(unsigned char*)(cvImg.data), // unsigned char* data
						 //temp_data,
						 osg::Image::NO_DELETE // AllocationMode mode (shallow copy)
						 );//int packing=1); (???)

		//printf("Conversion completed\n");
		return osgImg;
	}
	// XS TODO : what happens with BW images ?
	else {
		cerr << "Unrecognized image type" << endl;
		//printf("Unrecognized image type");
		return 0;
	}
}

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};

    gettimeofday(&tv, &tz);

    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

#ifdef USE_SLIT_SCAN
#ifdef __APPLE__
#ifdef USE_FFMPEG
int ACOsgVideoSlitScanThread::convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,
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
#endif//def USE_FFMPEG
#endif//def __APPLE__

void ACOsgVideoSlitScanThread::run(void)
{
	std::cout << "Slit-scanning " << filename<< "..." << std::endl;
	double slit_scan_in = getTime();
	notify_level = osg::getNotifyLevel();
	osg::setNotifyLevel(osg::WARN);//to remove the copyImage NOTICEs
	_done = false;
	if (filename!=""){
		this->computeSlitScan();
		_done = true;
		std::cout << "Done slit-scanning " << filename << " in " << getTime()-slit_scan_in << " sec." << std::endl;
	}
	osg::setNotifyLevel(notify_level);
}

#ifdef __APPLE__
#ifdef USE_FFMPEG	
void ACOsgVideoSlitScanThread::yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height)
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
#endif//def USE_FFMPEG
#endif//def __APPLE__


//#ifndef USE_FFMPEG
// Using OpenCV, frame jitter
// XS TODO try with cv::VideoCapture
int ACOsgVideoSlitScanThread::computeSlitScan(){
	//CvCapture* video = cvCreateFileCapture(filename.c_str());
	cv::VideoCapture* capture = new cv::VideoCapture(filename.c_str());	
	if ( !capture) {
		cerr << "<ACVideoTrackRenderer::computeSlitScan> Could not open video..." << endl;
		return 0;
	}
	
	int width   = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH);
	int height  = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT);
	//int fps     = (int) capture->get(CV_CAP_PROP_FPS);
	int total_frames = (int) capture->get(CV_CAP_PROP_FRAME_COUNT)-1; // XS -1 seems necessary in OpenCV 2.3
	
	//cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)summary_frame_in);
	slit_scan = 0;
	
	//IplImage* slit_scan;
	for (unsigned int f=0;f<total_frames;f++){
		//cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)f);
		/*if(!cvGrabFrame(video)){
			cerr << "<ACVideoTrackRenderer::computeSlitScan> Could not find frame..." << endl;
		}
		else{*/
			cv::Mat openCVframe;
			capture->set(CV_CAP_PROP_POS_FRAMES, f); 	
			*capture >> openCVframe;
		if (!openCVframe.data) {
			cerr << "<ACVideoTrackRenderer::computeSlitScan> unexpected missing data frame" << endl;
			break;
		}
			//if(f==summary_frame_in)
			//	slit_scan = cvCreateImage( cvSize(summary_frame_out-summary_frame_in, height), frame->depth, frame->nChannels );
			//int ff = (int) capture->get(CV_CAP_PROP_POS_FRAMES);
			#ifdef USE_DEBUG
			//if (ff!=f) cout << "Mismatch at frame " << ff << " instead of " << f << " (offset:" << ff-f << ")" << endl;
			#endif
			osg::ref_ptr<osg::Image> frame = Convert_OpenCV_2_osg_Image(openCVframe);
			if (!frame){
				cerr << "<ACVideoTrackRenderer::computeSlitScan> problem converting from OpenCV to OSG" << endl;
				return 0;
			}
			else{
				if(!slit_scan){
					slit_scan = new osg::Image; 
					slit_scan->allocateImage(total_frames, height, 1, GL_RGB, frame->getDataType());
				}
				//frame->setOrigin(osg::Image::TOP_LEFT);
				//frame->flipVertical();

				osg::copyImage(frame,
							   (float)width/2.0f,//int  	src_s,
							   0,//int  	src_t,
							   0,//int  	src_r,
							   1,//int  	width,
							   height,//int  	height,
							   1,//int  	depth,
							   slit_scan,
							   f,//int  	dest_s,
							   0,//int  	dest_t,
							   0,//int  	dest_r,
							   false//bool  	doRescale = false
							   );
			}
		//}
	}
	return 1;
}
//#endif//ndef USE_FFMPEG
//#endif //not (defined(__APPLE__) && not defined(USE_FFMPEG))

// Using FFmpeg
#ifdef __APPLE__
#ifdef USE_FFMPEG
int ACOsgVideoSlitScanThread::computeSlitScan(){
	
	double slit_in = getTime();
	
	AVFormatContext *pFormatCtx;
    unsigned int             i, videoStreams,videoStream;
	AVPacket        packet;
    int             frameFinished;
	
    // Register all formats and codecs
    av_register_all();
#if LIBAVFORMAT_BUILD < (53<<16 | 2<<8 | 0) 
    // Open video file
	if(av_open_input_file(&pFormatCtx, filename.c_str(), 0, 0, 0)!=0){
		std::cerr << "av_open_input_file : Couldn't open file" << std::endl;
		return -1;
	}
#else
	if(avformat_open_input(&pFormatCtx, filename.c_str(), 0, 0)!=0){
		std::cerr << "avformat_open_input : Couldn't open file" << std::endl;
		return -1;
	}
#endif
	
	// Retrieve stream information
	if(av_find_stream_info(pFormatCtx)<0){
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
		m_context = m_stream->codec;
		
		// Trust the video size given at this point
		// (avcodec_open seems to sometimes return a 0x0 size)
		int width = m_context->width;
		int height = m_context->height;
		
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
		int nb_frames =  m_stream->nb_frames; //CF alternative: pFormatCtx->streams[videoStream]->nb_index_entries or duration/summary_frame_rate for corrupted files?
		
		double slit_mid = getTime();
		
		int summary_frames_processed = 0;
		slit_scan = new osg::Image; 
		slit_scan->allocateImage(nb_frames, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
		
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
					//std::cout << "Processing frame " << m_context->frame_number << " / " << nb_frames << std::endl;
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
								   slit_scan,
								   m_context->frame_number,//int  	dest_s,
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
		
		// Close the codec
		avcodec_close(m_context);
		
		double slit_end = getTime();
		std::cout << "Slit-scanning took " << slit_end-slit_mid << " after " << slit_mid-slit_in << " of init " << std::endl;
		std::cout << "Missed " << nb_frames - summary_frames_processed << " frames over " << nb_frames << std::endl;
		//osgDB::writeImageFile(*slit_scan,std::string("/Users/christianfrisson/test.jpg"));
	}
	// Close the video file
	av_close_input_file(pFormatCtx);
	return 1;
}
#endif//def USE_FFMPEG
#endif//def __APPLE__

#endif//def USE_SLIT_SCAN

#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
void ACOsgVideoTrackPlayersSync::run(void)
{
	running = true;
	active = true;
	float max_latency = 0.001;
	if(slave_stream){
		float delay = slave_stream->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
		if( fabs(delay) > max_latency ){
			//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
			slave_stream->seek(master_stream->getReferenceTime()+ time_skip);
		}
		osg::ImageStream::StreamStatus streamStatus = slave_stream->getStatus();
		if (streamStatus != osg::ImageStream::PLAYING){
			slave_stream->play();
		}
		
	}
	//std::cout << "ACOsgVideoTrackPlayersSync::init " << getTime() << std::endl;
	while(running){// && master_stream){
		if(active){
			//std::cout << "ACOsgVideoTrackPlayersSync::running " << getTime() << std::endl;
			//osg::ImageStream::StreamStatus streamStatus = master_stream->getStatus();
			//if (streamStatus == osg::ImageStream::PLAYING){
			if(scrubbing){
				//std::cout << "Scrubbing" << std::endl;
				if(slave_stream){
					//float delay = slave_stream->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
					//if( fabs(delay) > max_latency ){
						//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
						slave_stream->seek(master_stream->getReferenceTime()+ time_skip);
					//}					
				}	
			}	
			//else {
				
				float delay = slave_stream->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
				if( fabs(delay) > max_latency ){
					//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
					slave_stream->seek(master_stream->getReferenceTime()+ time_skip);
				}
				
				osg::ImageStream::StreamStatus streamStatus = master_stream->getStatus();
				if (streamStatus == osg::ImageStream::PAUSED){
						if(slave_stream)
							if (slave_stream->getStatus() != osg::ImageStream::PAUSED)
								slave_stream->pause();
				}
				else if (streamStatus == osg::ImageStream::PLAYING){
						if(slave_stream){
							if(previousStreamStatus != osg::ImageStream::PLAYING){
								slave_stream->seek(master_stream->getReferenceTime()+ time_skip);
							}	
							if (slave_stream->getStatus() != osg::ImageStream::PLAYING)
								slave_stream->play();
						}	
				}
				previousStreamStatus = streamStatus;
			//}
		}
		microSleep(1000);//hack to update the thread once per frame
	}	
}
#else
void ACOsgVideoTrackPlayersSync::run(void)
{
	running = true;
	active = true;
	float max_latency = 0.005;
	int index=0;
	for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = slave_streams.begin(); _stream != slave_streams.end();++_stream){
		if(*_stream){
			index++;
			float delay = (*_stream)->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
			if( fabs(delay) > max_latency ){
				//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
				(*_stream)->seek(master_stream->getReferenceTime()+ time_skip*index);
			}
			osg::ImageStream::StreamStatus streamStatus = (*_stream)->getStatus();
			if (streamStatus != osg::ImageStream::PLAYING){
				(*_stream)->play();
			}
			
		}	
	}
	//std::cout << "ACOsgVideoTrackPlayersSync::init " << getTime() << std::endl;
	while(running){// && master_stream){
		if(active){
			//std::cout << "ACOsgVideoTrackPlayersSync::running " << getTime() << std::endl;
			//osg::ImageStream::StreamStatus streamStatus = master_stream->getStatus();
			//if (streamStatus == osg::ImageStream::PLAYING){
			if(scrubbing){
				//std::cout << "Scrubbing" << std::endl;
				int index=0;
				for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = slave_streams.begin(); _stream != slave_streams.end();++_stream){
					if(*_stream){
						index++;
						float delay = (*_stream)->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
						if( fabs(delay) > max_latency ){
							//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
							(*_stream)->seek(master_stream->getReferenceTime()+ time_skip*index);
						}					
					}	
				}
			}	
			else {
				
				int index=0;
				for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = slave_streams.begin(); _stream != slave_streams.end();++_stream){
					if(*_stream){
						index++;
						float delay = (*_stream)->getReferenceTime() - master_stream->getReferenceTime();//+ time_skip*index
						if( fabs(delay) > max_latency ){
							//std::cout << "Stream "<< index << " not in sync " << delay << " "<< fabs(delay) << std::endl;
							(*_stream)->seek(master_stream->getReferenceTime()+ time_skip*index);
						}					
					}	
				}
				
				osg::ImageStream::StreamStatus streamStatus = master_stream->getStatus();
				if (streamStatus == osg::ImageStream::PAUSED){
					for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = slave_streams.begin(); _stream != slave_streams.end();++_stream)
						if(*_stream)
							if ((*_stream)->getStatus() != osg::ImageStream::PAUSED)
								(*_stream)->pause();
				}
				else if (streamStatus == osg::ImageStream::PLAYING){
					for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = slave_streams.begin(); _stream != slave_streams.end();++_stream)
						if(*_stream)
							if ((*_stream)->getStatus() != osg::ImageStream::PLAYING)
								(*_stream)->play();
				}
			}	
		}
		microSleep(1000);
	}	
}
#endif//def SYNC_THREAD_PER_SELECTION_VIDEO

ACOsgVideoTrackRenderer::ACOsgVideoTrackRenderer() : ACOsgTrackRenderer() {		

	track_summary_type = AC_VIDEO_SUMMARY_KEYFRAMES;
	track_selection_type = AC_VIDEO_SELECTION_KEYFRAMES;
	track_playback_visibility = true;
	
	segments_height = yspan/16.0f;//[0;yspan/2.0f]
	summary_height = yspan/8.0f;//[0;yspan/2.0f]
	selection_height = yspan/4.0f;//[0;yspan/2.0f]
	playback_height = yspan/2.0f - summary_height - segments_height - selection_height; //yspan/2.0f;//[0;yspan/2.0f]
	
	segments_center_y = -yspan/2.0f + segments_height; 
	summary_center_y = -yspan/2.0f + summary_height + segments_height;//-yspan/2.0f+yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	selection_center_y = -yspan/2.0f + summary_height + segments_height + selection_height;
	playback_center_y = -yspan/2.0f + summary_height + segments_height + selection_height + playback_height; //yspan/4.0f; //[-yspan/2.0f;yspan/2.0f]

	playback_scale = 0.5f;
	selection_center_frame_width = 0.0f;
	summary_frame_min_width = 64;
	
	this->initTrack();
}

ACOsgVideoTrackRenderer::~ACOsgVideoTrackRenderer() {
	this->emptyTrack();
	this->initTrack();
}

void ACOsgVideoTrackRenderer::initTrack(){	
	selection_transform = 0;
	summary_transform = 0;
	playback_transform = 0;
	
	playback_video_transform = 0;
	video_stream = 0;
	
	selection_center_frame_transform = 0;
	right_selection_video_group=0;
	left_selection_video_group=0;
	#ifdef SYNC_SELECTION_VIDEOS_BY_THREAD
	#ifndef SYNC_THREAD_PER_SELECTION_VIDEO
	right_selection_video_sync = 0;
	left_selection_video_sync = 0;
	#endif
	#endif
	
	summary_cursor_transform=0;
	
	selection_begin_transform=0;
	selection_zone_transform=0;
	selection_end_transform=0;
	
	summary_frames_transform=0;
	
	#ifdef USE_SLIT_SCAN
	slit_scan_transform=0;
	slit_scan_changed = false;
	slit_scanner = 0;
	#endif//def USE_SLIT_SCAN
	
	segments_transform=0;
	segments_group=0;
	segments_number=0;
	
	summary_frame_n=0;
	selection_frame_n=0;

	scrubbing = false;
	selection_needs_resync = false;
	
	track_summary_type_changed = true;
	track_selection_type_changed = true;
	track_playback_visibility_changed = true;
}	

void ACOsgVideoTrackRenderer::emptyTrack(){
	
	track_node->removeChildren(0,track_node->getNumChildren());
	
	// playback
	if (video_stream) {
		video_stream->quit();
		video_stream=0;
	}
	for (std::vector< osg::ref_ptr<osg::Image> >::iterator _image = right_selection_video_images.begin(); _image != right_selection_video_images.end();++_image)
		(*_image) = 0;
	for (std::vector< osg::ref_ptr<osg::Texture2D> >::iterator _texture = right_selection_video_textures.begin(); _texture != right_selection_video_textures.end();++_texture)
		(*_texture) = 0;
	for (std::vector< osg::ref_ptr<osg::MatrixTransform> >::iterator _transform = right_selection_video_transforms.begin(); _transform != right_selection_video_transforms.end();++_transform)
		(*_transform) = 0;
	for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
		if((*_stream).valid())
			(*_stream)->quit();
		(*_stream) = 0;
	}
	right_selection_video_images.resize(0);
	right_selection_video_streams.resize(0);
	right_selection_video_textures.resize(0);
	right_selection_video_transforms.resize(0);
	
	for (std::vector< osg::ref_ptr<osg::Image> >::iterator _image = left_selection_video_images.begin(); _image != left_selection_video_images.end();++_image)
		(*_image) = 0;
	for (std::vector< osg::ref_ptr<osg::Texture2D> >::iterator _texture = left_selection_video_textures.begin(); _texture != left_selection_video_textures.end();++_texture)
		(*_texture) = 0;
	for (std::vector< osg::ref_ptr<osg::MatrixTransform> >::iterator _transform = left_selection_video_transforms.begin(); _transform != left_selection_video_transforms.end();++_transform)
		(*_transform) = 0;
	for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
		if((*_stream).valid())
			(*_stream)->quit();
		(*_stream) = 0;
	}
	left_selection_video_images.resize(0);
	left_selection_video_streams.resize(0);
	left_selection_video_textures.resize(0);
	left_selection_video_transforms.resize(0);
	
	#ifdef SYNC_SELECTION_VIDEOS_BY_THREAD
	#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
	for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = right_selection_video_syncs.begin(); _sync != right_selection_video_syncs.end();_sync++){
		if(*_sync){
			(*_sync)->cancel();
			delete (*_sync);
			(*_sync)=0;
		}	
	}
	for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = left_selection_video_syncs.begin(); _sync != left_selection_video_syncs.end();_sync++){
		if(*_sync){
			(*_sync)->cancel();
			delete (*_sync);
			(*_sync)=0;
		}	
	}
	#else
	if (right_selection_video_sync){
		if (right_selection_video_sync->isRunning())
			right_selection_video_sync->cancel();
		delete right_selection_video_sync;
		right_selection_video_sync=0;
	}
	if (left_selection_video_sync){
		if (left_selection_video_sync->isRunning())
			left_selection_video_sync->cancel();
		delete left_selection_video_sync;
		left_selection_video_sync=0;
	}
	#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
	#endif//def SYNC_SELECTION_VIDEOS_BY_THREAD
	
	for (std::vector< osg::ref_ptr<osg::Image> >::iterator _image = summary_images.begin(); _image != summary_images.end();++_image)
		(*_image) = 0;
	for (std::vector< osg::ref_ptr<osg::Texture2D> >::iterator _texture = summary_textures.begin(); _texture != summary_textures.end();++_texture)
		(*_texture) = 0;
	for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = summary_streams.begin(); _stream != summary_streams.end();_stream++){
		if((*_stream).valid())
			(*_stream)->quit();
		(*_stream) = 0;
	}
	summary_images.resize(0);
	summary_streams.resize(0);
	summary_textures.resize(0);
	
	// slit-scan
	#ifdef USE_SLIT_SCAN
	if (slit_scanner){
		if (slit_scanner->isRunning())
			slit_scanner->cancel();
		delete slit_scanner;
		slit_scanner=0;
	}
	slit_scan_transform=0;
	#endif//def USE_SLIT_SCAN
	
	for (std::vector< osg::ref_ptr<osg::Geode> >::iterator _geode = segments_geodes.begin(); _geode != segments_geodes.end();++_geode)
		(*_geode) = 0;
	segments_geodes.clear();
}

void ACOsgVideoTrackRenderer::boxTransform(osg::ref_ptr<osg::MatrixTransform>& _transform, float _width, osg::Vec4 _color, std::string _name) {
	osg::StateSet *state;
	osg::Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;
	osg::ref_ptr<osg::Geometry> _geometry = new osg::Geometry();
	osg::ref_ptr<osg::Geode> _geode = new osg::Geode();
	
	_transform = 0;
	_transform = new osg::MatrixTransform();
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-_width/2.0f, -yspan/2.0f, zpos);
	(*vertices)[1] = Vec3(_width/2.0f, -yspan/2.0f, zpos);
	(*vertices)[2] = Vec3(_width/2.0f, yspan/2.0f, zpos);
	(*vertices)[3] = Vec3(-_width/2.0f, yspan/2.0f, zpos);
	_geometry->setVertexArray(vertices);
	
	//Vec4 _color(0.0f, 1.0f, 0.0f, 1.0f);
	//Vec4 _color(0.2f, 0.9f, 0.2f, 0.9f);//(1.0f, 0.0f, 0.0f, 0.4f);
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
	colors->push_back(_color);
	
	line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	line_p->push_back(0);
	line_p->push_back(1);
	line_p->push_back(2);
	line_p->push_back(3);
	_geometry->addPrimitiveSet(line_p);
	
	_geometry->setColorArray(colors);
	_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	_geometry->setNormalArray(normals);
	_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	state = _geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = _geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = _transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	_geode->addDrawable(_geometry);
	
	_transform->addChild(_geode);
	
	_geode->setUserData(new ACRefId(track_index,_name));
}


void ACOsgVideoTrackRenderer::selectionZoneTransform() {
	this->boxTransform(selection_zone_transform, yspan, osg::Vec4(0.0f, 0.0f, 0.0f, 0.2f), "track selection zone");
}

void ACOsgVideoTrackRenderer::selectionBeginTransform() {
	this->boxTransform(selection_begin_transform, selection_sensing_width, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection begin");	
}

void ACOsgVideoTrackRenderer::selectionEndTransform() {
	this->boxTransform(selection_end_transform, selection_sensing_width, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection end");
}

void ACOsgVideoTrackRenderer::selectionCursorTransform() {
	this->boxTransform(summary_cursor_transform, yspan/600, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "video track cursor");
}

void ACOsgVideoTrackRenderer::playbackVideoTransform() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> playback_geometry;
	Texture2D *playback_texture;
	
	playback_video_transform = new MatrixTransform();
	osg::ref_ptr<osg::Geode> playback_geode = new Geode();
	playback_geometry = new Geometry();	
	
	double imagey = yspan/2.0f;
	double imagex = xspan/2.0f;
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-imagex, -imagey, zpos);
	(*vertices)[1] = Vec3(imagex, -imagey, zpos);
	(*vertices)[2] = Vec3(imagex, imagey, zpos);
	(*vertices)[3] = Vec3(-imagex, imagey, zpos);
	playback_geometry->setVertexArray(vertices);
	
	// Primitive Set
	osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	playback_geometry->addPrimitiveSet(poly);
	
	// State Set
	state = playback_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	float a = 0.0;
	float b = 1.0-a;
	bool flip = true;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	playback_geometry->setTexCoordArray(0, texcoord);	
	
	//if (media_index > -1){
	if (media){	
		
		//playback_texture = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getTexture();
		playback_texture = ((ACVideo*)media)->getTexture();

		playback_texture->setResizeNonPowerOfTwoHint(false);
		//playback_texture->setUnRefImageDataAfterApply(true);
		state = playback_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, playback_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
		playback_geometry->setColorArray(colors);
		playback_geometry->setColorBinding(Geometry::BIND_OVERALL);
		playback_geode->addDrawable(playback_geometry);
		playback_video_transform->addChild(playback_geode);
		playback_geode->setUserData(new ACRefId(track_index,"video track playback"));
	}
}

void ACOsgVideoTrackRenderer::selectionCenterFrameTransform() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> selection_center_frame_geometry;
	Texture2D *selection_center_frame_texture;
	
	selection_center_frame_transform = new MatrixTransform();
	osg::ref_ptr<Geode> selection_center_frame_geode = new Geode();
	selection_center_frame_geometry = new Geometry();	
	
	double imagey = yspan/2.0f;
	double imagex = xspan/2.0f;
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-imagex, -imagey, zpos);
	(*vertices)[1] = Vec3(imagex, -imagey, zpos);
	(*vertices)[2] = Vec3(imagex, imagey, zpos);
	(*vertices)[3] = Vec3(-imagex, imagey, zpos);
	selection_center_frame_geometry->setVertexArray(vertices);
	
	// Primitive Set
	osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	selection_center_frame_geometry->addPrimitiveSet(poly);
	
	// State Set
	state = selection_center_frame_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	float a = 0.0;
	float b = 1.0-a;
	bool flip = true;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	selection_center_frame_geometry->setTexCoordArray(0, texcoord);	
	
	//if (media_index > -1){
	if (media){	
		
		selection_center_frame_texture = ((ACVideo*)media)->getTexture();
		
		selection_center_frame_texture->setResizeNonPowerOfTwoHint(false);
		state = selection_center_frame_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, selection_center_frame_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
		selection_center_frame_geometry->setColorArray(colors);
		selection_center_frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
		selection_center_frame_geode->addDrawable(selection_center_frame_geometry);
		selection_center_frame_transform->addChild(selection_center_frame_geode);
		selection_center_frame_geode->setUserData(new ACRefId(track_index,"video track selection center frame"));
	}
}


#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && defined(SYNC_THREAD_PER_SELECTION_VIDEO)
void ACOsgVideoTrackRenderer::updateSelectionVideos(
	std::vector< osg::ref_ptr<osg::Image> >& _selection_video_images,
	std::vector< osg::ref_ptr<osg::ImageStream> >& _selection_video_streams,
	std::vector< osg::ref_ptr<osg::Texture2D> >& _selection_video_textures,
	std::vector< osg::ref_ptr<osg::MatrixTransform> >& _selection_video_transforms,
	osg::ref_ptr<osg::Group>& _selection_video_group,
	std::vector< ACOsgVideoTrackPlayersSync* >& _selection_video_syncs, 
	int _number)
#else
void ACOsgVideoTrackRenderer::updateSelectionVideos(
	std::vector< osg::ref_ptr<osg::Image> >& _selection_video_images,
	std::vector< osg::ref_ptr<osg::ImageStream> >& _selection_video_streams,
	std::vector< osg::ref_ptr<osg::Texture2D> >& _selection_video_textures,
	std::vector< osg::ref_ptr<osg::MatrixTransform> >& _selection_video_transforms,
	osg::ref_ptr<osg::Group>& _selection_video_group,
	int _number)
#endif
{
	if(!_selection_video_group) _selection_video_group = new osg::Group;
	
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> video_geometry;
	
	int n(0),current_image_n(0),current_texture_n(0),current_transform_n(0),current_stream_n(0);
	current_image_n = _selection_video_images.size();
	current_texture_n = _selection_video_textures.size();
	current_transform_n = _selection_video_transforms.size();
	current_stream_n = _selection_video_streams.size();
	n = current_stream_n;
	
	if( current_image_n != n || current_texture_n != n || current_transform_n != n)
		std::cerr << "ACOsgVideoTrackRenderer::updateRightSelectionVideos: wrong sizes" << std::endl;
	
	//track_node->removeChild(_selection_video_transform);
	/*for (std::vector< osg::ref_ptr<osg::MatrixTransform> >::iterator _transform = _selection_video_transforms.begin(); _transform != _selection_video_transforms.end();++_transform)
	 track_node->removeChild((*_transform));*/
	
	if(n==0){
		//osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(media->getThumbnailFileName()).substr(1));
		osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");// necessary for multithreaded video access for now
		if (!readerWriter){
			cerr << "<ACOsgVideoTrackRenderer> problem loading file, no OSG plugin available" << endl;
		}
		else{
			cout <<"<ACOsgVideoTrackRenderer> using OSG plugin: "<< readerWriter->className() <<std::endl;
		}
	}
	
	_selection_video_group->removeChildren(0,_selection_video_group->getNumChildren());
	
	if(_number < n){
		for(int m = n-1; m >= _number;--m){
			_selection_video_transforms[m]=0;
			_selection_video_images[m]=0;
			if(_selection_video_streams[m].valid())
				_selection_video_streams[m]->quit();
			_selection_video_streams[m]=0;
			_selection_video_textures[m]=0;
			_selection_video_transforms.resize(m);
			_selection_video_images.resize(m);
			_selection_video_streams.resize(m);
			_selection_video_textures.resize(m);
			//std::cout << "Removed selection video number " << m << std::endl;
		}
	}
	
	for(int m = 0; m < n;++m){
		if(_selection_video_transforms[m])
			_selection_video_group->addChild(_selection_video_transforms[m]);
	}	
		
	
	if(_number > n){
		
		for(int m = n; m < _number;++m){
			//std::cout << "Added selection video number " << m << std::endl;
			osg::NotifySeverity notify_level = osg::getNotifyLevel();
			osg::setNotifyLevel(osg::ALWAYS);
			//_selection_video_image = osgDB::readImageFile(media->getThumbnailFileName());
			if(_selection_video_images.size()<m+1)
				_selection_video_images.push_back(osgDB::readImageFile(media->getThumbnailFileName()));
			osg::setNotifyLevel(notify_level);
			
			//if (!_selection_video_image){
			//	cerr << "<ACOsgVideoTrackRenderer> problem creating image" << endl;
			//}
			if (!_selection_video_images[m]){
				cerr << "<ACOsgVideoTrackRenderer> problem creating image" << endl;
			}
			
			//osg::ref_ptr<osg::Texture2D>_selection_video_texture = new osg::Texture2D;
			if(_selection_video_textures.size()<m+1)
				_selection_video_textures.push_back(new osg::Texture2D);
			//_selection_video_texture->setImage(_selection_video_images[m]);
			_selection_video_textures[m]->setImage(_selection_video_images[m]);
			
			// Converting the video as preloaded stream to transmit the same instance to multiple recipient with unified playback controls
			if(_selection_video_streams.size()<m+1)
				_selection_video_streams.push_back(dynamic_cast<osg::ImageStream*>(_selection_video_images[m].get()));
			//_selection_video_stream = dynamic_cast<osg::ImageStream*>(_selection_video_image.get());
			_selection_video_streams[m]->setLoopingMode(osg::ImageStream::LOOPING);
			_selection_video_streams[m]->setVolume(0);
			
			//_selection_video_transform = new MatrixTransform();
			if(_selection_video_transforms.size()<m+1)
				_selection_video_transforms.push_back(new MatrixTransform());
			//osg::ref_ptr<MatrixTransform> video_transform = new MatrixTransform();
			
			osg::ref_ptr<Geode> video_geode = new Geode();
			video_geometry = new Geometry();	
			
			double imagey = yspan/2.0f;
			double imagex = xspan/2.0f;
			
			vertices = new Vec3Array(4);
			(*vertices)[0] = Vec3(-imagex, -imagey, zpos);
			(*vertices)[1] = Vec3(imagex, -imagey, zpos);
			(*vertices)[2] = Vec3(imagex, imagey, zpos);
			(*vertices)[3] = Vec3(-imagex, imagey, zpos);
			video_geometry->setVertexArray(vertices);
			
			// Primitive Set
			osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
			poly->push_back(0);
			poly->push_back(1);
			poly->push_back(2);
			poly->push_back(3);
			video_geometry->addPrimitiveSet(poly);
			
			// State Set
			state = video_geode->getOrCreateStateSet();
			state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
			state->setMode(GL_BLEND, StateAttribute::ON);
			state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
			
			// Texture Coordinates
			texcoord = new Vec2Array;
			float a = 0.0;
			float b = 1.0-a;
			bool flip = true;
			texcoord->push_back(osg::Vec2(a, flip ? b : a));
			texcoord->push_back(osg::Vec2(b, flip ? b : a));
			texcoord->push_back(osg::Vec2(b, flip ? a : b));
			texcoord->push_back(osg::Vec2(a, flip ? a : b));
			video_geometry->setTexCoordArray(0, texcoord);	
			
			//_selection_video_texture->setResizeNonPowerOfTwoHint(false);
			_selection_video_textures[m]->setResizeNonPowerOfTwoHint(false);								  
			state = video_geometry->getOrCreateStateSet();
			state->setTextureAttribute(0, _selection_video_textures[m]);
			//state->setTextureAttribute(0, _selection_video_texture);
			state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
			video_geometry->setColorArray(colors);
			video_geometry->setColorBinding(Geometry::BIND_OVERALL);
			video_geode->addDrawable(video_geometry);
			//_selection_video_transform->addChild(video_geode);
			_selection_video_transforms[m]->addChild(video_geode);
			
			//track_node->addChild(_selection_video_transforms[m]);
			_selection_video_group->addChild(_selection_video_transforms[m]);
			
			#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && defined(SYNC_THREAD_PER_SELECTION_VIDEO)
			if(_selection_video_syncs.size()<m+1)
				_selection_video_syncs.push_back(new ACOsgVideoTrackPlayersSync(video_stream,_selection_video_streams[m],0));
			//_selection_video_syncs[m]->start();
			#endif
			_selection_video_streams[m]->play();
		}
		selection_needs_resync = true;
	}	
}

#ifdef USE_SLIT_SCAN
void ACOsgVideoTrackRenderer::slitScanTransform() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> slit_scan_geometry;
	Texture2D *slit_scan_texture;
	
	slit_scan_geometry = 0;
	slit_scan_transform = 0;
	slit_scan_transform = new MatrixTransform();
	osg::ref_ptr<osg::Geode> slit_scan_geode = new Geode();
	slit_scan_geometry = new Geometry();	
	
	double imagey = yspan/2.0f;
	double imagex = xspan/2.0f;
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-imagex, -imagey, 0.0f);
	(*vertices)[1] = Vec3(imagex, -imagey, 0.0f);
	(*vertices)[2] = Vec3(imagex, imagey, 0.0f);
	(*vertices)[3] = Vec3(-imagex, imagey, 0.0f);
	slit_scan_geometry->setVertexArray(vertices);
	
	// Primitive Set
	osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	slit_scan_geometry->addPrimitiveSet(poly);
	
	// State Set
	state = slit_scan_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	float a = 0.0;
	float b = 1.0-a;
	bool flip = true;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	slit_scan_geometry->setTexCoordArray(0, texcoord);	
	
	if (slit_scanner->computed()){
		slit_scan_texture = new osg::Texture2D;
		slit_scan_texture->setResizeNonPowerOfTwoHint(false);
		slit_scan_texture->setImage(slit_scanner->getImage());
		state = slit_scan_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, slit_scan_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
		slit_scan_geometry->setColorArray(colors);
		slit_scan_geometry->setColorBinding(Geometry::BIND_OVERALL);
		slit_scan_geode->addDrawable(slit_scan_geometry);
		slit_scan_transform->addChild(slit_scan_geode);
		slit_scan_geode->setUserData(new ACRefId(track_index,"video track summary slit-scan"));
	}
	
}	

#endif//def USE_SLIT_SCAN

void ACOsgVideoTrackRenderer::framesTransform() {
	double summary_start = getTime();
	std::cout << "Generating frames... ";
	summary_frames_transform = 0;
	summary_frames_transform = new MatrixTransform;
	osg::ref_ptr<osg::Group> summary_frames_group = new Group;
	//summary_frames_group->removeChildren(0,	summary_frames_group->getNumChildren ());
	//summary_frame_n = floor(width/summary_frame_min_width);
	StateSet *state;
	
	for (std::vector< osg::ref_ptr<osg::Image> >::iterator _image = summary_images.begin(); _image != summary_images.end();++_image)
		(*_image) = 0;
	for (std::vector< osg::ref_ptr<osg::Texture2D> >::iterator _texture = summary_textures.begin(); _texture != summary_textures.end();++_texture)
		(*_texture) = 0;
	for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = summary_streams.begin(); _stream != summary_streams.end();_stream++){
		if((*_stream).valid())
			(*_stream)->quit();
		(*_stream) = 0;
	}
	summary_images.resize(0);
	summary_streams.resize(0);
	summary_textures.resize(0);
	
	//for (int f=summary_frame_n-1;f>=0;f--){ // hysteresis test
	for (unsigned int f=0;f<summary_frame_n;f++){
		osg::ref_ptr<osg::Geode> summary_frame_geode = new Geode;
		TessellationHints *hints = new TessellationHints();
		hints->setDetailRatio(0.0);
		state = summary_frame_geode->getOrCreateStateSet();
		state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
		state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		
		if(summary_images.size()<f+1)
			summary_images.push_back(osgDB::readImageFile(media->getThumbnailFileName()));
		if(summary_textures.size()<f+1)
			summary_textures.push_back(new osg::Texture2D);
		summary_textures[f]->setImage(summary_images[f]);
		if(summary_streams.size()<f+1)
			summary_streams.push_back(dynamic_cast<osg::ImageStream*>(summary_images[f].get()));
		//summary_streams[f]->setLoopingMode(osg::ImageStream::LOOPING);
		summary_streams[f]->setVolume(0);
		
		summary_streams[f]->play();
		summary_streams[f]->seek( (f+0.5f)*summary_streams[f]->getLength()/(float)summary_frame_n );
		//summary_streams[f]->pause();

		
		StateSet *state;
		Vec3Array* vertices;
		Vec2Array* texcoord;
		osg::ref_ptr<Geometry> summary_frame_geometry;
		//Texture2D *summary_frame_texture = new osg::Texture2D;
		//summary_frame_texture->setImage(thumbnail);
		//summary_frame_texture->setImage(tmposg);
		summary_textures[f]->setResizeNonPowerOfTwoHint(false);
		summary_frame_geometry = new Geometry();	
		
		//CF this is a hack, shouldn't be, maybe there's a delay while accessing frames from the video
		//int g = f-1;
		//if (f == 0) g = summary_frame_n-1;
		int g = f;
		
		vertices = new Vec3Array(4);
		(*vertices)[0] = Vec3(-xspan/2.0f+2*g*xspan/summary_frame_n/2.0f, -yspan/2.0f, 0.0);
		(*vertices)[1] = Vec3(-xspan/2.0f+(2*g+2)*xspan/summary_frame_n/2.0f, -yspan/2.0f, 0.0);
		(*vertices)[2] = Vec3(-xspan/2.0f+(2*g+2)*xspan/summary_frame_n/2.0f, yspan/2.0f, 0.0);
		(*vertices)[3] = Vec3(-xspan/2.0f+2*g*xspan/summary_frame_n/2.0f, yspan/2.0f, 0.0);
		summary_frame_geometry->setVertexArray(vertices);
		
		// Primitive Set
		osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
		poly->push_back(0);
		poly->push_back(1);
		poly->push_back(2);
		poly->push_back(3);
		summary_frame_geometry->addPrimitiveSet(poly);
		
		// State Set
		state = summary_frame_geode->getOrCreateStateSet();
		state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
		state->setMode(GL_BLEND, StateAttribute::ON);
		state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
		
		// Texture Coordinates
		texcoord = new Vec2Array;
		float a = 0.0;
		float b = 1.0-a;
		bool flip = true;
		texcoord->push_back(osg::Vec2(a, flip ? b : a));
		texcoord->push_back(osg::Vec2(b, flip ? b : a));
		texcoord->push_back(osg::Vec2(b, flip ? a : b));
		texcoord->push_back(osg::Vec2(a, flip ? a : b));
		summary_frame_geometry->setTexCoordArray(0, texcoord);	
		
		state = summary_frame_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, summary_textures[f]);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
		summary_frame_geometry->setColorArray(colors);
		summary_frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
		summary_frame_geode->addDrawable(summary_frame_geometry);
		summary_frame_geode->setUserData(new ACRefId(track_index,"video track summary frames"));
		
		summary_frames_group->addChild(summary_frame_geode);
	}
	summary_frames_transform->addChild(summary_frames_group);
	
	std::cout << "Frames generated in " << getTime()-summary_start << " sec." << std::endl;
}

void ACOsgVideoTrackRenderer::segmentsTransform() {
	if(segments_group == 0) segments_group = new Group();
	segments_group->removeChildren(0,	segments_group->getNumChildren());
	int segments_n = media->getNumberOfSegments();
	StateSet *state;
	
	float media_length = media->getEnd() - media->getStart();
	//std::cout << "Media duration " << media_length << std::endl;
	//std::cout << "Media frame rate " << media->getFrameRate() << std::endl;
	
	for (int s=0;s<segments_n;s++){
		segments_geodes.resize(segments_geodes.size()+1);
		segments_geodes[s] = new Geode;
		TessellationHints *hints = new TessellationHints();
		hints->setDetailRatio(0.0);
		
		state = segments_geodes[s]->getOrCreateStateSet();
		state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
		state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
		state->setMode(GL_BLEND, StateAttribute::ON);
		//state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
		
		Vec4 segment_color;
		if ( (float)s/2.0f != s/2) // odd segment index
			segment_color = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
		else // even segment index
			segment_color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		osg::ref_ptr<osg::Vec4Array> segment_colors = new Vec4Array;
		segment_colors->push_back(segment_color);

		
		//std::cout << "Segment start " << media->getSegment(s)->getStart() << " end " << media->getSegment(s)->getEnd() << std::endl;
		
		
		/*std::vector<std::string> segment_plugins = media_cycle->getPluginManager()->getSegmentPlugins()->getName(MEDIA_TYPE_VIDEO);
		for (std::vector<std::string>::iterator segment_plugin = segment_plugins.begin(); segment_plugin != segment_plugins.end();++segment_plugin)
			std::cout << "Segment plugin '" << (*segment_plugin) << "'" << std::endl;
		*/
		//segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-xspan/2.0f+ (media->getSegment(s)->getStart()+media->getSegment(s)->getEnd())/2.0f*xspan/media_length,0.0f,0.0f),(media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())*xspan/media_length,yspan,0.0f), hints));
		//CF workaround since BIC segmentation sets frames not seconds as start/end
		segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-xspan/2.0f+ (media->getSegment(s)->getStart()+media->getSegment(s)->getEnd())/2.0f*xspan/media->getFrameRate()/media_length,0.0f,0.0f),(media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())*xspan/media->getFrameRate()/media_length,yspan,0.0f), hints));
		
		//std::cout << "Segment " << s << " start " << media->getSegment(s)->getStart()/media_length << " end " << media->getSegment(s)->getEnd()/media_length << " width " << (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length << std::endl;
		((ShapeDrawable*)(segments_geodes[s])->getDrawable(0))->setColor(segment_color);
		segments_geodes[s]->setUserData(new ACRefId(track_index,"video track segment",s));// s should be replaced by the segment media id
		segments_group->addChild(segments_geodes[s]);
	}	
	if(segments_n>0){
		//segments_group->setUserData(new ACRefId(track_index,"video track segments"));
		segments_transform->addChild(segments_group);
	}	
}

void ACOsgVideoTrackRenderer::prepareTracks() {
	this->emptyTrack();
	this->initTrack();
}

void ACOsgVideoTrackRenderer::updateTracks(double ratio)
{
	if (media_changed)
	{
		this->emptyTrack();
		this->initTrack();

		selection_transform = new MatrixTransform;
		summary_transform = new MatrixTransform;
		playback_transform = new MatrixTransform;
		track_node->addChild(selection_transform);
		track_node->addChild(summary_transform);
		track_node->addChild(playback_transform);

		if (media){
			
			// Try to locate a thumbnail (more compressed video file)
			std::stringstream thumbnail_filename;
			boost::filesystem::path filename_path(media->getFileName().c_str());
			std::string slash = "/";
			#ifdef WIN32
			slash = "\\";
			#endif
			#ifdef __APPLE__
				thumbnail_filename << filename_path.parent_path().string() << slash << filename_path.stem().string() << "_thumbnail" << filename_path.extension().string();
			#else // this seems required on ubuntu to compile...
				thumbnail_filename << filename_path.parent_path() << slash << filename_path.stem() << "_thumbnail" << filename_path.extension();
			#endif
			std::cout << "Thumbnail " << thumbnail_filename.str() << std::endl;
			if(boost::filesystem::exists(thumbnail_filename.str()))
				media->setThumbnailFileName( thumbnail_filename.str() );
			else
				media->setThumbnailFileName( media->getFileName() );
			std::cout << "Thumbnail (used)" << media->getThumbnailFileName() << std::endl;
			
			// Access the video stream (of the browser node)
			std::cout << "Getting video stream... ";
			double video_stream_in = getTime();
			video_stream = static_cast<ACVideo*>(media)->getStream();			
			std::cout << getTime()-video_stream_in << " sec." << std::endl;
		
			// Launch slit-scan computation
			#ifdef USE_SLIT_SCAN
			slit_scanner = new ACOsgVideoSlitScanThread();
			slit_scanner->setFileName(media->getThumbnailFileName());
			slit_scanner->startThread();
			slit_scan_changed = true;
			#endif//def USE_SLIT_SCAN
			
			// Optional, for testing the segment visualization without segmentation
			//this->createDummySegments();
		}	
	}	

	if (media && height > 0.0f && width > 0.0f){
		float w = (float)(media->getWidth());
		float h = (float)(media->getHeight());
		
		// Update segments viz if media changed or not previously created
		if (media->getNumberOfSegments()>0 && segments_number != media->getNumberOfSegments()){
			if(segments_transform)	
				track_node->removeChild(segments_transform);
			segments_transform=0;
			segments_transform=new MatrixTransform;
			//if (summary_frame_n != floor(width/summary_frame_min_width)){
			double segments_start = getTime();
			std::cout << "Generating segments... ";
			segmentsTransform();
			std::cout << getTime()-segments_start << " sec." << std::endl;
			segments_number = media->getNumberOfSegments();
			track_node->addChild(segments_transform);
		}
		
		this->updatePlaybackContents();
		this->updateSummaryContents();
		this->updateSelectionContents();

		if (video_stream)
			this->syncVideoStreams();
		
		this->updateSliderContents();
	
		this->updateTransformsAspects();

	}

	// Reset "signals"
	media_changed = false;
	selection_begin_pos_changed = false;
	selection_end_pos_changed = false;
	selection_center_pos_changed = false;
	track_selection_type_changed = false;
	track_playback_visibility_changed = false;
	selection_needs_resync = false;
}

void ACOsgVideoTrackRenderer::updatePlaybackContents()
{
	if (track_playback_visibility_changed){
		playback_transform->removeChildren(0,playback_transform->getNumChildren());
		if(track_playback_visibility){
			if(!playback_video_transform)
				playbackVideoTransform();
			playback_transform->addChild(playback_video_transform);
		}
	}
}

void ACOsgVideoTrackRenderer::updateSummaryContents()
{
	if (track_summary_type == AC_VIDEO_SUMMARY_KEYFRAMES){
		if (summary_frame_n != floor(width/summary_frame_min_width) || media_changed){
			track_summary_type_changed = true;
			summary_frame_n = floor(width/summary_frame_min_width);
			framesTransform();
		}
		else{
			for (int f=0;f<summary_frame_n;f++){
				summary_streams[f]->seek( (f+0.5f)*summary_streams[f]->getLength()/(float)summary_frame_n );
				summary_streams[f]->pause();
				summary_streams[f]->quit();
			}	
		}
	}

	#ifdef USE_SLIT_SCAN
	if (track_summary_type == AC_VIDEO_SUMMARY_SLIT_SCAN && slit_scan_changed)
		track_summary_type_changed = true;
	#endif//def USE_SLIT_SCAN

	if (track_summary_type_changed){
		//std::cout << "summary_type " << track_summary_type << std::endl;
		summary_transform->removeChildren(0,summary_transform->getNumChildren());
		if (track_summary_type == AC_VIDEO_SUMMARY_KEYFRAMES){
			summary_transform->addChild(summary_frames_transform);
		}
	#ifdef USE_SLIT_SCAN
		else if (track_summary_type == AC_VIDEO_SUMMARY_SLIT_SCAN){
			if (slit_scanner->computed()){
				if(slit_scan_changed){
					slitScanTransform();
					slit_scan_changed = false;						
				}	
				summary_transform->addChild(slit_scan_transform);
			}
		}
	#endif//def USE_SLIT_SCAN
	}

	float w = (float)(media->getWidth());
	float h = (float)(media->getHeight());
	if (track_summary_type == AC_VIDEO_SUMMARY_KEYFRAMES)
		summary_height = yspan*(h/height * width/w/summary_frame_n);// summary_frame_n = width/summary_frame_min_width
	else
		summary_height = yspan/8.0f;
}	

void ACOsgVideoTrackRenderer::updateSelectionContents()
{
	float w = (float)(media->getWidth());
	float h = (float)(media->getHeight());
	//selection_height = yspan/8.0f; //selection_height = summary_height * 2.0f;//makes a fixed-heigh proportion independent of the timeline height variation
	selection_center_frame_width = selection_height * (w/width)/(h/height);

	// Update the selection contents
	if(track_selection_type == AC_VIDEO_SELECTION_KEYFRAMES){
		
		//int _selection_frame_n = floor(xspan/selection_center_frame_width/2.0f;
		int _selection_frame_n = floor((width/summary_frame_min_width)/2.0f);						   
		if ((selection_frame_n != _selection_frame_n) || media_changed){
			track_selection_type_changed = true;
			selection_frame_n = _selection_frame_n;
			//std::cout << "selection_frame_n " << selection_frame_n << std::endl;
			
			//playback_min_width = xspan * selection_frame_n/(media->getFrameRate()*media->getDuration());
			
			updateSelectionVideos(right_selection_video_images,
				right_selection_video_streams,
				right_selection_video_textures,
				right_selection_video_transforms,
				right_selection_video_group,
				#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && defined(SYNC_THREAD_PER_SELECTION_VIDEO)
				right_selection_video_syncs, 
				#endif
				selection_frame_n);

			updateSelectionVideos(left_selection_video_images,
				left_selection_video_streams,
				left_selection_video_textures,
				left_selection_video_transforms,
				left_selection_video_group,
				#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && defined(SYNC_THREAD_PER_SELECTION_VIDEO)
				left_selection_video_syncs, 
				#endif	
				selection_frame_n);
			
			#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && not defined(SYNC_THREAD_PER_SELECTION_VIDEO)
			if(!left_selection_video_sync){
				left_selection_video_sync = new ACOsgVideoTrackPlayersSync(video_stream,left_selection_video_streams,0);
				//left_selection_video_sync->startSync();
				left_selection_video_sync->start();
			}
			else
				left_selection_video_sync->updateSlaves(left_selection_video_streams);
				
				if(!right_selection_video_sync){
					right_selection_video_sync = new ACOsgVideoTrackPlayersSync(video_stream,right_selection_video_streams,0);
					//right_selection_video_sync->startSync();
					right_selection_video_sync->start();
				}
				else
					right_selection_video_sync->updateSlaves(right_selection_video_streams);
			#endif
		}
		selection_height = yspan*(h/height * width/w/(2*selection_frame_n+1));
	}

	if (track_selection_type_changed){
		//std::cout << "selection_type " << track_selection_type << std::endl;
		selection_transform->removeChildren(0,selection_transform->getNumChildren());
		if(track_selection_type == AC_VIDEO_SELECTION_KEYFRAMES){
			if(!selection_center_frame_transform)
				selectionCenterFrameTransform();
			selection_transform->addChild(selection_center_frame_transform);
			selection_transform->addChild(right_selection_video_group);
			selection_transform->addChild(left_selection_video_group);
		}
	}
}

void ACOsgVideoTrackRenderer::syncVideoStreams()
{	
	osg::ImageStream::StreamStatus streamStatus = video_stream->getStatus();
	
	if ((selection_begin_pos_changed || selection_end_pos_changed) && manual_selection){
		
		#ifdef SYNC_SELECTION_VIDEO_BY_THREAD
		#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = right_selection_video_syncs.begin(); _sync != right_selection_video_syncs.end();_sync++){
			if(*_sync){
				(*_sync)->setScrubbing(true);
				(*_sync)->setActive(true);
				//(*_sync)->updateTimeSkip();
			}
		}
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = left_selection_video_syncs.begin(); _sync != left_selection_video_syncs.end();_sync++){
			if(*_sync){
				(*_sync)->setScrubbing(true);
				(*_sync)->setActive(true);
				//(*_sync)->updateTimeSkip();
			}
		}	
		#else
		if (right_selection_video_sync){
			right_selection_video_sync->setScrubbing(true);
			right_selection_video_sync->setActive(true);
		}	
		if (left_selection_video_sync){
			left_selection_video_sync->setScrubbing(true);
			left_selection_video_sync->setActive(true);
		}	
		#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
		#endif//def SYNC_SELECTION_VIDEO_BY_THREAD
	
		float cursor_pos_normd = (selection_center_pos/(xspan/2.0f)+1)/2.0f;
		if (cursor_pos_normd > 1)
			cursor_pos_normd -= 1;
		else if (cursor_pos_normd < 0)
			cursor_pos_normd += 1;
		video_stream->seek(cursor_pos_normd*video_stream->getLength());
	}
	
	if (selection_begin_pos_changed || selection_end_pos_changed || selection_needs_resync){
		
		//inefficient if (SYNC_SELECTION_VIDEOS_BY_THREAD and not SYNC_THREAD_PER_SELECTION_VIDEO)
		int right_skip(0);
		for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
			if((*_stream).valid()){
				#ifndef TEST_SYNC_WITHOUT_TIME_SKIP
				// Time skip so that the rightest frame corresponds to the right selection cursor (or looping from the beginning if after the last frame)
				float position_norm = ((selection_center_pos/(xspan/2.0f)+1)/2.0f +  ++right_skip/(float)(right_selection_video_streams.size())*(selection_end_pos-selection_begin_pos)/xspan/2.0f );
				if(position_norm > 1){
					position_norm -= 1;//looping
				}
				(*_stream)->seek( position_norm *video_stream->getLength());
				#else
				// Sync without time skip
				if((*_stream).valid()){
					(*_stream)->seek(  ((selection_center_pos/(xspan/2.0f)+1)/2.0f  ) *video_stream->getLength());
				}
				#endif
			}	
		}
		int left_skip(0);
		for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
			if((*_stream).valid()){
				#ifndef TEST_SYNC_WITHOUT_TIME_SKIP
				// Time skip so that the leftest frame corresponds to the left selection cursor (or looping from the end if before the first frame)
				float position_norm = ((selection_center_pos/(xspan/2.0f)+1)/2.0f +  --left_skip/(float)(left_selection_video_streams.size())*(selection_end_pos-selection_begin_pos)/xspan/2.0f );
				if(position_norm < 0){
					position_norm += 1;//looping
				}
				(*_stream)->seek( position_norm *video_stream->getLength());
				#else
				// Sync without time skip
				if((*_stream).valid()){
					(*_stream)->seek(  ((selection_center_pos/(xspan/2.0f)+1)/2.0f  ) *video_stream->getLength());
				}
				#endif
			}
		}
	}
	
	if ((selection_begin_pos_changed || selection_end_pos_changed) && manual_selection){
		if (streamStatus == osg::ImageStream::PAUSED){
			
			//std::cout << "Video stream reference time " << video_stream->getReferenceTime() << " time multiplier " << video_stream->getTimeMultiplier() << std::endl;
			
			scrubbing = true;
			video_stream->play();

			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->play();
			}
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->play();
			}
		}
	}	
	else {
		if (scrubbing){
			scrubbing = false;
			video_stream->pause();																  
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->pause();
			}
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->pause();
			}
		}
		#ifdef SYNC_SELECTION_VIDEO_BY_THREAD
		#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = right_selection_video_syncs.begin(); _sync != right_selection_video_syncs.end();_sync++)
			if(*_sync)
				(*_sync)->setScrubbing(false);
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = left_selection_video_syncs.begin(); _sync != left_selection_video_syncs.end();_sync++)
			if(*_sync)
				(*_sync)->setScrubbing(false);
		#else
		if (right_selection_video_sync)
			right_selection_video_sync->setScrubbing(false);
		if (left_selection_video_sync)
			left_selection_video_sync->setScrubbing(false);
		#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
		#endif//def SYNC_SELECTION_VIDEO_BY_THREAD
		
		bool _active = false;
		if (streamStatus == osg::ImageStream::PLAYING){
			_active = true;
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->play();
				//time skip
			}
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->play();
			}
			this->moveSelection(-xspan/2.0f+video_stream->getReferenceTime()/video_stream->getLength()*xspan); // update the visual cursor from the stream itself
		}
		else if (streamStatus == osg::ImageStream::PAUSED){
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = right_selection_video_streams.begin(); _stream != right_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->pause();
			}
			for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
				if((*_stream).valid())
					(*_stream)->pause();
			}
		}
		#ifdef SYNC_SELECTION_VIDEO_BY_THREAD
		#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = right_selection_video_syncs.begin(); _sync != right_selection_video_syncs.end();_sync++)
			if(*_sync)
				(*_sync)->setActive(_active);
		for (std::vector< ACOsgVideoTrackPlayersSync* >::iterator _sync = left_selection_video_syncs.begin(); _sync != left_selection_video_syncs.end();_sync++)
			if(*_sync)
				(*_sync)->setActive(_active);
		#else
		if (right_selection_video_sync)
			right_selection_video_sync->setActive(_active);
		if (left_selection_video_sync)
			left_selection_video_sync->setActive(_active);
		#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
		#endif//def SYNC_SELECTION_VIDEO_BY_THREAD		
	}
}

void ACOsgVideoTrackRenderer::updateSliderContents()
{
	//Update selection slider in summmary view
	track_node->removeChild(selection_begin_transform);
	track_node->removeChild(selection_end_transform);
	track_node->removeChild(selection_zone_transform);
	track_node->removeChild(summary_cursor_transform);
	if (media_changed)
	{
		selectionCursorTransform();
		selectionBeginTransform();
		selectionEndTransform();
		selectionZoneTransform();
	}
	if(track_selection_type != AC_VIDEO_SELECTION_NONE){
		track_node->addChild(selection_zone_transform);
		track_node->addChild(selection_begin_transform);
		track_node->addChild(selection_end_transform);
	}	
	track_node->addChild(summary_cursor_transform);
}

void ACOsgVideoTrackRenderer::updateTransformsAspects()
{
	float w = (float)(media->getWidth());
	float h = (float)(media->getHeight());
	
	// Update the playback/selection/summary/segments view aspects
	Matrix playback_matrix,summary_matrix,segments_matrix,selection_center_frame_matrix;
	Matrix cursor_matrix,selection_begin_matrix,selection_end_matrix,selection_zone_matrix;
	std::vector< Matrix > left_selection_videos_matrices,right_selection_videos_matrices;
	
	float _segments_height = 0.0f;
	if (media->getNumberOfSegments()>0){
		_segments_height = this->segments_height;
		segments_matrix.makeTranslate(0.0f,-yspan/2.0f+_segments_height/2.0f,0.0f);
		segments_matrix = Matrix::scale(1.0f,_segments_height/yspan,1.0f)*segments_matrix;
		segments_transform->setMatrix(segments_matrix);
	}
	
	float _summary_height = this->summary_height;//= 0.0f;
	if (track_summary_type != AC_VIDEO_SUMMARY_NONE){
		//_summary_height = this->summary_height;
		summary_matrix.makeTranslate(0.0f,-yspan/2.0f+_segments_height+_summary_height/2.0f,0.0f);
		summary_matrix = Matrix::scale(1.0f,_summary_height/yspan,1.0f)*summary_matrix;
		summary_transform->setMatrix(summary_matrix);
	}
	
	float _selection_height = 0.0f;
	if (track_selection_type != AC_VIDEO_SELECTION_NONE){
		_selection_height = this->selection_height;	
		selection_center_frame_matrix.makeTranslate(0.0f,-yspan/2.0f+_segments_height+_summary_height+_selection_height/2.0f,0.0f);
		selection_center_frame_matrix = Matrix::scale(selection_center_frame_width/xspan,_selection_height/yspan,1.0f)*selection_center_frame_matrix;
		for (unsigned int m=0;m<right_selection_video_transforms.size();++m){
			right_selection_videos_matrices.resize(m+1);
			right_selection_videos_matrices[m].makeTranslate(selection_center_frame_width*(m+1),-yspan/2.0f+_segments_height+_summary_height+_selection_height/2.0f,0.0f);
			right_selection_videos_matrices[m] = Matrix::scale(selection_center_frame_width/xspan,_selection_height/yspan,1.0f)*right_selection_videos_matrices[m];
		}
		for (unsigned int m=0;m<left_selection_video_transforms.size();++m){
			left_selection_videos_matrices.resize(m+1);
			left_selection_videos_matrices[m].makeTranslate(-selection_center_frame_width*(m+1),-yspan/2.0f+_segments_height+_summary_height+_selection_height/2.0f,0.0f);
			left_selection_videos_matrices[m] = Matrix::scale(selection_center_frame_width/xspan,_selection_height/yspan,1.0f)*left_selection_videos_matrices[m];
		}
		if(track_selection_type == AC_VIDEO_SELECTION_KEYFRAMES){	
			selection_center_frame_transform->setMatrix(selection_center_frame_matrix);
			for (unsigned int m=0;m<right_selection_videos_matrices.size();++m){
				if(right_selection_video_transforms[m]){
					right_selection_video_transforms[m]->setMatrix(right_selection_videos_matrices[m]);
				}
			}
			for (unsigned int m=0;m<left_selection_videos_matrices.size();++m){
				if(left_selection_video_transforms[m]){
					left_selection_video_transforms[m]->setMatrix(left_selection_videos_matrices[m]);
				}
			}
		}
		selection_begin_matrix.makeTranslate(selection_begin_pos,-yspan/2.0f+_segments_height+_summary_height/2.0f,0.0f);	
		selection_begin_matrix = Matrix::scale(1.0f,_summary_height/yspan,1.0f)*selection_begin_matrix;
		selection_begin_transform->setMatrix(selection_begin_matrix);
		selection_end_matrix.makeTranslate(selection_end_pos,-yspan/2.0f+_segments_height+_summary_height/2.0f,0.0f);	
		selection_end_matrix = Matrix::scale(1.0f,_summary_height/yspan,1.0f)*selection_end_matrix;
		selection_end_transform->setMatrix(selection_end_matrix);
		selection_zone_matrix.makeTranslate((selection_begin_pos+selection_end_pos)/2.0f,-yspan/2.0f+_segments_height+_summary_height/2.0f,0.0f);	
		selection_zone_matrix = Matrix::scale((selection_end_pos-selection_begin_pos)/xspan,_summary_height/yspan,1.0f)*selection_zone_matrix;
		selection_zone_transform->setMatrix(selection_zone_matrix);
	}
	
	if(track_playback_visibility){
		playback_scale = (yspan-_summary_height-_segments_height-_selection_height)/yspan;
		playback_height = height * playback_scale;
		playback_center_y = yspan/2.0f-(yspan-_summary_height-_segments_height-_selection_height)/2.0f;
		playback_matrix.makeTranslate(0.0f,playback_center_y,0.0f);
		if (w/h*playback_height/width<1.0f){ // video fits view height
			playback_matrix = Matrix::scale(w/h*playback_height/width,playback_scale,1.0f)*playback_matrix;
			//std::cout << "video fits view height" << std::endl;
		}
		else{ //if (h/w*width/playback_height<1.0f) // video fits view width 
			playback_matrix = Matrix::scale(1.0f,h/w*width/height,1.0f)*playback_matrix;
			//std::cout << "video fits view width" << std::endl;
		}
		playback_transform->setMatrix(playback_matrix);
	}
	
	cursor_matrix.makeTranslate(selection_center_pos,-yspan/2.0f+_segments_height+_summary_height/2.0f,0.0f);	
	cursor_matrix = Matrix::scale(1.0f,_summary_height/yspan,1.0f)*cursor_matrix;
	summary_cursor_transform->setMatrix(cursor_matrix);	
}
#endif //defined (SUPPORT_VIDEO)
