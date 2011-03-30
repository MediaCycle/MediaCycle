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

#include "ACOsgVideoTrackRenderer.h"
#include "ACVideo.h"
//#include "ACImage.h"
#include <cmath>
#include <osg/ImageUtils>
#include <osgDB/WriteFile>

using namespace std;
using namespace osg;

// ----------- uncomment this to compute and visualize a slit scan
//#define USE_SLIT_SCAN

#if defined (SUPPORT_VIDEO)

osg::ref_ptr<osg::Image> Convert_OpenCV_2_osg_Image(IplImage* cvImg)
{
	// XS uncomment the following 4 lines for visual debug (e.g., thumbnail)	
	//	cvNamedWindow("T", CV_WINDOW_AUTOSIZE);
	//	cvShowImage("T", cvImg);
	//	cvWaitKey(0);
	//	cvDestroyWindow("T");	
	
	
	if(cvImg->nChannels == 3)
	{
		// Flip image from top-left to bottom-left origin
		if(cvImg->origin == 0) {
			cvConvertImage(cvImg , cvImg, CV_CVTIMG_FLIP);
			cvImg->origin = 1;
		}
		
		// Convert from BGR to RGB color format
		//printf("Color format %s\n",cvImg->colorModel);
		if ( !strcmp(cvImg->channelSeq,"BGR") ) {
			cvCvtColor( cvImg, cvImg, CV_BGR2RGB );
		}
		
		osg::ref_ptr<osg::Image> osgImg = new osg::Image();
		
		/*temp_data = new unsigned char[cvImg->width * cvImg->height * cvImg->nChannels];
		 for (i=0;i<cvImg->height;i++) {
		 memcpy( (char*)temp_data + i*cvImg->width*cvImg->nChannels, (char*)(cvImg->imageData) + i*(cvImg->widthStep), cvImg->width*cvImg->nChannels);
		 }*/
		
		osgImg->setImage(
						 cvImg->width, //s
						 cvImg->height, //t
						 1, //r //CF needs to be 1 otherwise scaleImage can't be used
						 3, //GLint internalTextureformat, (GL_LINE_STRIP, 0x0003)
						 6407, // GLenum pixelFormat, (GL_RGB, 0x1907)
						 5121, // GLenum type, (GL_UNSIGNED_BYTE, 0x1401)
						 (unsigned char*)(cvImg->imageData), // unsigned char* data
						 //temp_data,
						 osg::Image::NO_DELETE, // AllocationMode mode (shallow copy)
						 1);//int packing=1); (???)
		
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
    OSG_INFO<<"Using sws_scale ";
    int result =  sws_scale(m_swscale_ctx,
                            (src->data), (src->linesize), 0, src_height,
                            (dst->data), (dst->linesize));
	/*#else
	 OSG_INFO<<"Using img_convert ";
	 int result = img_convert(dst, dst_pix_fmt, src,
	 src_pix_fmt, src_width, src_height);
	 #endif*/
    osg::Timer_t endTick = osg::Timer::instance()->tick();
    OSG_INFO<<" time = "<<osg::Timer::instance()->delta_m(startTick,endTick)<<"ms"<<std::endl;
    return result;
}

void ACOsgVideoSlitScanThread::yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height)
{
    convert(dst, PIX_FMT_RGB32, src, m_context->pix_fmt, width, height);
    const size_t bpp = 4;
    uint8_t * a_dst = dst->data[0] + 3;
    for (int h = 0; h < height; ++h) {
        const uint8_t * a_src = src->data[3] + h * src->linesize[3];
        for (int w = 0; w < width; ++w) {
            *a_dst = *a_src;
            a_dst += bpp;
            a_src += 1;
        }
    }
}

// Using OpenCV, frame jitter
int ACOsgVideoSlitScanThread::computeSlitScan(){
	CvCapture* video = cvCreateFileCapture(filename.c_str());
	if ( !video) {
		cerr << "<ACVideoTrackRenderer::computeSlitScan> Could not open video..." << endl;
		return 0;
	}	
	
	int total_frames = (int) cvGetCaptureProperty(video,CV_CAP_PROP_FRAME_COUNT);
	int width = (int) cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int) cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_HEIGHT);
	
	//cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)frame_in);

	//IplImage* slit_scan;
	for (int f=0;f<total_frames;f++){
		//cvSetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES,(double)f);
		if(!cvGrabFrame(video)){
			cerr << "<ACVideoTrackRenderer::computeSlitScan> Could not find frame..." << endl;
		}
		else{
			IplImage* openCVframe = cvRetrieveFrame(video);
			//if(f==frame_in)
			//	slit_scan = cvCreateImage( cvSize(frame_out-frame_in, height), frame->depth, frame->nChannels );
			int ff = (int) cvGetCaptureProperty(video,CV_CAP_PROP_POS_FRAMES);
			if (ff!=f) cout << "Mismatch at frame " << ff << " instead of " << f << " (offset:" << ff-f << ")" << endl;
			
			osg::ref_ptr<osg::Image> frame = Convert_OpenCV_2_osg_Image(openCVframe);
			if (!frame){
				cerr << "<ACVideoTrackRenderer::computeSlitScan> problem converting from OpenCV to OSG" << endl;
				return 0;
			}
			else{
				frame->setOrigin(osg::Image::TOP_LEFT);
				frame->flipVertical();
				
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
		}	
	}
	return 1;
}

// Using FFmpeg
/*int ACOsgVideoSlitScanThread::computeSlitScan(){
	
	double slit_in = getTime();
	
	AVFormatContext *pFormatCtx;
    unsigned int             i, videoStreams,videoStream;
	AVPacket        packet;
    int             frameFinished;
	
    // Register all formats and codecs
    av_register_all();
	
    // Open video file
	if(av_open_input_file(&pFormatCtx, filename.c_str(), 0, 0, 0)!=0){
		std::cerr << "Couldn't open file" << std::endl;
		return -1;
	}
	
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
		if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
			videoStreams++;
	if(videoStreams == 0)
		std::cout << "Didn't find any video stream." << std::endl;
	else
		std::cout << "Found " << videoStreams << " video stream(s)." << std::endl;
	
	// Find the first video stream
	if (videoStreams>0){
		
		videoStream=-1;
		for(i=0; i<pFormatCtx->nb_streams; i++)
			if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
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
		
		// Open codec
		if (avcodec_open(m_context, m_codec) < 0)
			throw std::runtime_error("avcodec_open() failed");
		
		// Allocate video frame
		AVFrame* m_frame=avcodec_alloc_frame();
		
		// Allocate converted RGB frame
		AVFrame* m_frame_rgba=avcodec_alloc_frame();
		std::vector<uint8_t> m_buffer_rgba;
		m_buffer_rgba.resize(avpicture_get_size(PIX_FMT_RGB32, width, height));
		
		// Assign appropriate parts of the buffer to image planes in m_frame_rgba
		avpicture_fill((AVPicture *) m_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);
		
		// Back to FFmpeg tuto
		
		// Video stream properties
		//float frame_rate = av_q2d(m_stream->r_frame_rate);
		//float duration = (float)(pFormatCtx->duration)/AV_TIME_BASE;
		int nb_frames =  m_stream->nb_frames; //CF alternative: pFormatCtx->streams[videoStream]->nb_index_entries or duration/frame_rate for corrupted files?
		
		double slit_mid = getTime();
		
		int frames_processed = 0;
		slit_scan = new osg::Image; 
		slit_scan->allocateImage(nb_frames, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
		
		while(av_read_frame(pFormatCtx, &packet)>=0)
		{
			// Is this a packet from the video stream?
			if(packet.stream_index==videoStream)
			{
				// Decode video frame
				avcodec_decode_video(m_context, m_frame, &frameFinished,packet.data, packet.size);
				
				// Did we get a video frame?
				if(frameFinished)
				{
					//std::cout << "Processing frame " << m_context->frame_number << " / " << nb_frames << std::endl;
					AVPicture * const src = (AVPicture *) m_frame;
					AVPicture * const dst = (AVPicture *) m_frame_rgba;
					
					// Assign appropriate parts of the buffer to image planes in m_frame_rgba
					avpicture_fill((AVPicture *) m_frame_rgba, &(m_buffer_rgba)[0], PIX_FMT_RGB32, width, height);
					
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
					
					frames_processed++;
				}
			}
			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
		}
		
		// Free the RGB image
		m_buffer_rgba.empty();
		av_free(m_frame_rgba);
		
		// Free the YUV frame
		av_free(m_frame);
		
		// Close the codec
		avcodec_close(m_context);
		
		double slit_end = getTime();
		std::cout << "Slit-scanning took " << slit_end-slit_mid << " after " << slit_mid-slit_in << " of init " << std::endl;
		std::cout << "Missed " << nb_frames - frames_processed << " frames over " << nb_frames << std::endl;
		//sosgDB::writeImageFile(*slit_scan,std::string("/Users/christianfrisson/test.jpg"));
	}
	// Close the video file
	av_close_input_file(pFormatCtx);
	return 1;
}
*/ 

#endif//def USE_SLIT_SCAN

ACOsgVideoTrackRenderer::ACOsgVideoTrackRenderer() : ACOsgTrackRenderer() {
	//ACOsgTrackRenderer
	selection_center_pos = -xspan/2.0f;
	selection_begin_pos = selection_center_pos;
	selection_end_pos = selection_center_pos;
	
	video_stream = 0;
	summary_data = 0;
	
	zoom_x = 1.0; zoom_y = 1.0;
	track_left_x = 0.0;
	summary_height = yspan/8.0f;//[0;yspan/2.0f]
	segments_height = yspan/16.0f;//[0;yspan/2.0f]
	playback_height = yspan/2.0f - summary_height - segments_height; //yspan/2.0f;//[0;yspan/2.0f]
	summary_center_y = -yspan/2.0f + summary_height + segments_height;//-yspan/2.0f+yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	segments_center_y = -yspan/2.0f + segments_height; 
	playback_center_y = -yspan/2.0f + summary_height + segments_height + playback_height; //yspan/4.0f; //[-yspan/2.0f;yspan/2.0f]
	playback_scale = 0.5f;
	
	playback_geode = 0; 
	playback_transform = 0;
	scrubbing = false;
	
	frame_min_width = 64;
	frame_n = 1;
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.9f);	
	Vec4 color2(0.2f, 0.8f, 0.2f, 1.0f);	
	Vec4 color3(0.4f, 0.4f, 0.4f, 1.0f);	
	colors = new Vec4Array;
	colors2 = new Vec4Array;
	colors3 = new Vec4Array;
	colors->push_back(color);		
	colors2->push_back(color2);	
	colors3->push_back(color3);	
	
	frames_transform = new MatrixTransform();
	frames_group = new Group();
	
	segments_transform = new MatrixTransform();
	segments_group = new Group();
	
	#ifdef USE_SLIT_SCAN
	slit_scan_transform = new MatrixTransform();
	slit_scanner = new ACOsgVideoSlitScanThread();
	slit_scan_changed = false;
	#endif//def USE_SLIT_SCAN
	
	track_summary_type = AC_VIDEO_SUMMARY_KEYFRAMES;
	
	segments_number = 0;
}

ACOsgVideoTrackRenderer::~ACOsgVideoTrackRenderer() {
	#ifdef USE_SLIT_SCAN
	if (slit_scanner){
		if (slit_scanner->isRunning()) slit_scanner->cancel();
		delete slit_scanner; slit_scanner=0;}
	if (slit_scan_geode) { //ref_ptr//slit_scan_geode->unref();
		slit_scan_geode=0; }
	if (slit_scan_transform) { //ref_ptr//slit_scan_transform->unref();
		slit_scan_transform=0;}
	#endif//def USE_SLIT_SCAN
	if (video_stream) {video_stream->quit(); video_stream=0;}
	if (playback_geode) { //ref_ptr//playback_geode->unref();
		playback_geode=0; }
	if (playback_transform) { //ref_ptr//playback_transform->unref();
		playback_transform=0;}
	if (cursor_geode) {	//ref_ptr//cursor_geode->unref();
		cursor_geode=0; }
	if (segments_transform) { //ref_ptr//segments_transform->unref();
		segments_transform=0;}	
	if(summary_data) {summary_data=0;} //{cvReleaseCapture(&summary_data);} // no! ACVideo does it!
}

void ACOsgVideoTrackRenderer::playbackGeode() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> playback_geometry;
	Texture2D *playback_texture;
	
	playback_transform = new MatrixTransform();
	playback_geode = new Geode();
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
	
	if (media_index > -1){
		playback_texture = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getTexture();
		playback_texture->setResizeNonPowerOfTwoHint(false);
		//playback_texture->setUnRefImageDataAfterApply(true);
		state = playback_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, playback_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		playback_geometry->setColorArray(colors);
		playback_geometry->setColorBinding(Geometry::BIND_OVERALL);
		playback_geode->addDrawable(playback_geometry);
		playback_transform->addChild(playback_geode);
		//ref_ptr//playback_transform->ref();
		playback_geode->setUserData(new ACRefId(track_index,"video track"));
	}
}

#ifdef USE_SLIT_SCAN
void ACOsgVideoTrackRenderer::slitScanGeode() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	osg::ref_ptr<Geometry> slit_scan_geometry;
	Texture2D *slit_scan_texture;
	
	//slit_scan_transform = new MatrixTransform();
	slit_scan_geode = new Geode();
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
		//playback_texture->setUnRefImageDataAfterApply(true);
		state = slit_scan_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, slit_scan_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		slit_scan_geometry->setColorArray(colors);
		slit_scan_geometry->setColorBinding(Geometry::BIND_OVERALL);
		slit_scan_geode->addDrawable(slit_scan_geometry);
		slit_scan_transform->addChild(slit_scan_geode);
		//ref_ptr//slit_scan_transform->ref();
		slit_scan_geode->setUserData(new ACRefId(track_index,"video track summary slit-scan"));
	}
	
}	
#endif//def USE_SLIT_SCAN

void ACOsgVideoTrackRenderer::cursorGeode() {
	StateSet *state;	
	Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;
	osg::ref_ptr<Geometry> cursor_geometry;
	
	cursor_transform = new MatrixTransform();
	cursor_geode = new Geode();
	cursor_geometry = new Geometry();
	
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0.0f, -yspan/2.0f, 0.0f);
	(*vertices)[1] = Vec3(0.0f, yspan/2.0f, 0.0f);	
	cursor_geometry->setVertexArray(vertices);
	
	Vec4 cursor_color(0.2f, 0.9f, 0.2f, 0.9f);	
	osg::ref_ptr<osg::Vec4Array> cursor_colors = new Vec4Array;
	cursor_colors->push_back(cursor_color);		
	cursor_geometry->setColorArray(cursor_colors);
	cursor_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;	
	cursor_geometry->addPrimitiveSet(line_p);
	
	state = cursor_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	state = cursor_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif		
	state = cursor_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	
	cursor_geode->addDrawable(cursor_geometry);
	cursor_transform->addChild(cursor_geode);
	//ref_ptr//cursor_transform->ref();
	cursor_geode->setUserData(new ACRefId(track_index,"video track cursor"));
	//ref_ptr//cursor_geode->ref();
}

void ACOsgVideoTrackRenderer::framesGeode() {
	frames_group->removeChildren(0,	frames_group->getNumChildren ());
	frame_n = floor(width/frame_min_width);
	StateSet *state;
	
	//for (int f=frame_n-1;f>=0;f--){ // hysteresis test
	for (int f=0;f<frame_n;f++){
		frame_geode = new Geode;
		TessellationHints *hints = new TessellationHints();
		hints->setDetailRatio(0.0);
		state = frame_geode->getOrCreateStateSet();
		state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
		state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		int total_frames = (int) cvGetCaptureProperty( summary_data, CV_CAP_PROP_FRAME_COUNT);
		
		cvSetCaptureProperty( summary_data, CV_CAP_PROP_POS_FRAMES, (int)((f+0.5f)*total_frames/(float)frame_n) );// +0.5f means we're taking the center frame as a representative
		if(!cvGrabFrame(summary_data)){
			cerr << "<ACVideoTrackRenderer::updateTrack> Could not find frame " << f << endl;
		}
		else {
			IplImage* tmp = cvRetrieveFrame(summary_data);
			
			osg::ref_ptr<osg::Image> tmposg = Convert_OpenCV_2_osg_Image(tmp);
			osg::ref_ptr<osg::Image> thumbnail = new osg::Image;
			thumbnail->allocateImage(tmposg->s(), tmposg->t(), tmposg->r(), GL_RGB, tmposg->getDataType());
			osg::copyImage(tmposg, 0, 0, 0, tmposg->s(), tmposg->t(), tmposg->r(),thumbnail, 0, 0, 0, false);
			
			StateSet *state;
			Vec3Array* vertices;
			Vec2Array* texcoord;
			osg::ref_ptr<Geometry> frame_geometry;
			Texture2D *frame_texture = new osg::Texture2D;
			frame_texture->setImage(thumbnail);
			frame_texture->setResizeNonPowerOfTwoHint(false);
			frame_geometry = new Geometry();	
			
			//CF this is a hack, shouldn't be, maybe there's a delay while accessing frames from the video
			//int g = f-1;
			//if (f == 0) g = frame_n-1;
			int g = f;
			
			vertices = new Vec3Array(4);
			(*vertices)[0] = Vec3(-xspan/2.0f+2*g*xspan/frame_n/2.0f, -yspan/2.0f, 0.0);
			(*vertices)[1] = Vec3(-xspan/2.0f+(2*g+2)*xspan/frame_n/2.0f, -yspan/2.0f, 0.0);
			(*vertices)[2] = Vec3(-xspan/2.0f+(2*g+2)*xspan/frame_n/2.0f, yspan/2.0f, 0.0);
			(*vertices)[3] = Vec3(-xspan/2.0f+2*g*xspan/frame_n/2.0f, yspan/2.0f, 0.0);
			frame_geometry->setVertexArray(vertices);
			
			// Primitive Set
			osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
			poly->push_back(0);
			poly->push_back(1);
			poly->push_back(2);
			poly->push_back(3);
			frame_geometry->addPrimitiveSet(poly);
			
			// State Set
			state = frame_geode->getOrCreateStateSet();
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
			frame_geometry->setTexCoordArray(0, texcoord);	
			
			state = frame_geometry->getOrCreateStateSet();
			state->setTextureAttribute(0, frame_texture);
			state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
			//summary_geometry->setColorArray(colors[0]);
			frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
			frame_geode->addDrawable(frame_geometry);
			frame_geode->setUserData(new ACRefId(track_index,"video track summary frames"));
		}
		//ref_ptr//frame_geode->ref();
		frames_group->addChild(frame_geode);
	}
	//ref_ptr//frames_group->ref();
	frames_transform->addChild(frames_group);
	//ref_ptr//frames_transform->ref();
}

void ACOsgVideoTrackRenderer::segmentsGeode() {
	segments_group->removeChildren(0,	segments_group->getNumChildren());
	int segments_n = media->getNumberOfSegments();
	StateSet *state;
	
	float media_length = media->getEnd() - media->getStart();
	
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

		segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-xspan/2.0f+ (media->getSegment(s)->getStart()+media->getSegment(s)->getEnd())/2.0f*xspan/media_length,0.0f,0.0f),(media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())*xspan/media_length,yspan,0.0f), hints));
		//std::cout << "Segment " << s << " start " << media->getSegment(s)->getStart()/media_length << " end " << media->getSegment(s)->getEnd()/media_length << " width " << (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length << std::endl;
		((ShapeDrawable*)(segments_geodes[s])->getDrawable(0))->setColor(segment_color);
		segments_geodes[s]->setUserData(new ACRefId(track_index,"video track segments"));
		//ref_ptr//segments_geodes[s]->ref();
		segments_group->addChild(segments_geodes[s]);
	}	
	if(segments_n>0){
		//ref_ptr//segments_group->ref();
		segments_transform->addChild(segments_group);
		//ref_ptr//segments_transform->ref();
	}	
}

void ACOsgVideoTrackRenderer::prepareTracks() {
	playback_geode = 0;
	cursor_transform = 0;
	cursor_geode = 0;
	//segments_transform = 0;//CF EXEC_BAD_ACCESS
	//segments_geodes = 0;//CF EXEC_BAD_ACCESS
}

void ACOsgVideoTrackRenderer::updateTracks(double ratio) {
	if (media_changed)
	{
		track_node->removeChild(playback_transform);
		if (media){
			#ifdef USE_SLIT_SCAN
			if (slit_scanner){
				if (slit_scanner->isRunning()) slit_scanner->cancel();
				delete slit_scanner; slit_scanner=0;
				slit_scanner = new ACOsgVideoSlitScanThread();
			}
			slit_scan_geode=0;			
			#endif//def USE_SLIT_SCAN
			
			playback_geode=0;
			playback_transform=0;
			cursor_geode=0;
			if(summary_data) {summary_data=0;} //{cvReleaseCapture(&summary_data);} // no! ACVideo does it!
			if (video_stream) {video_stream->quit();}
			video_stream=0;	
		
			#ifdef USE_SLIT_SCAN
			slit_scanner->reset();
			slit_scanner->setFileName(media->getFileName());
			slit_scanner->startThread();
			slit_scan_changed = true;
			#endif//def USE_SLIT_SCAN
			
			playbackGeode();
			track_node->addChild(playback_transform);
			
			//if (video_stream) delete video_stream;
			std::cout << "Getting video stream... ";
			double video_stream_in = getTime();
			video_stream = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getStream();
			std::cout << getTime()-video_stream_in << " sec." << std::endl;
			
			//if (summary_data) delete summary_data;
			std::cout << "Getting summary data... ";
			double summary_data_in = getTime();
			summary_data = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getData();
			std::cout << getTime()-summary_data_in << " sec." << std::endl;
			
			//CF: dummy segments
			if (media->getNumberOfSegments()==0){
                std::cout << "Dummy segments" << std::endl;
				for (int s=0;s<4;s++){
					ACMedia* seg = ACMediaFactory::getInstance().create(media);
					seg->setParentId(media->getId());
					media->addSegment(seg);//dummy
				}	
				float media_start = media->getStart();
				float media_end = media->getEnd();
				media->getSegment(0)->setStart(media_start);
				media->getSegment(0)->setEnd((media_end-media_start)/4.0f);
				media->getSegment(1)->setStart((media_end-media_start)/4.0f);
				media->getSegment(1)->setEnd(3*(media_end-media_start)/8.0f);
				media->getSegment(2)->setStart(3*(media_end-media_start)/8.0f);
				media->getSegment(2)->setEnd((media_end-media_start)/2.0f);
				media->getSegment(3)->setStart((media_end-media_start)/2.0f);
				media->getSegment(3)->setEnd(media_end);
			}
            else
                std::cout << media->getNumberOfSegments() << " segments" << std::endl;
		}	
	}

	static Vec4 colors[2];
	static bool colors_ready = false;
	
	if(!colors_ready)
	{
		colors[0] = Vec4(1,0,0,1);
		colors[1] = Vec4(0,1,0,1);
		colors_ready = true;
	}

	if (height != 0.0f && width != 0.0f){
		float w = (float)(media_cycle->getWidth(media_index));
		float h = (float)(media_cycle->getHeight(media_index));
		
		track_node->removeChild(frames_transform);
		track_node->removeChild(segments_transform);
		#ifdef USE_SLIT_SCAN
		track_node->removeChild(slit_scan_transform);
		if (track_summary_type == AC_VIDEO_SUMMARY_SLIT_SCAN && slit_scanner->computed()){
			if (slit_scan_changed){
				std::cout << "Using slit-scan" << std::endl;
				slit_scan_transform->removeChild(slit_scan_geode);
				slitScanGeode();
				slit_scan_changed = false;
			}	
			track_node->addChild(slit_scan_transform);
		}
		else {
		#endif//def USE_SLIT_SCAN	
			if (frame_n != floor(width/frame_min_width) || media_changed){
				double summary_start = getTime();
				std::cout << "Generating frames... ";
				frames_transform->removeChild(frames_group);
				framesGeode();
				std::cout << getTime()-summary_start << " sec." << std::endl;
			}
			track_node->addChild(frames_transform);
		#ifdef USE_SLIT_SCAN	
		}
		#endif//def USE_SLIT_SCAN
		
		if (media && media_changed){
			if (media->getNumberOfSegments()>0 && segments_number != media->getNumberOfSegments()){
				//if (frame_n != floor(width/frame_min_width)){
				double segments_start = getTime();
				std::cout << "Generating segments... ";
				segments_transform->removeChild(segments_group);
				segmentsGeode();
				std::cout << getTime()-segments_start << " sec." << std::endl;
				segments_number = media->getNumberOfSegments();
			}	
		}
		if (media->getNumberOfSegments()>0) track_node->addChild(segments_transform);
				
		Matrix playback_matrix,summary_matrix,segments_matrix;
		
		if (media->getNumberOfSegments()>0){
			// Stacked playback + summary + segments 
			summary_height = yspan*(h/height * width/w/frame_n);
			playback_scale = (yspan-summary_height-segments_height)/yspan;
			playback_height = height * playback_scale;
			playback_center_y = yspan/2.0f-(yspan-summary_height-segments_height)/2.0f;
			summary_matrix.makeTranslate(0.0f,-yspan/2.0f+summary_height/2.0f+segments_height,0.0f);
			summary_matrix = Matrix::scale(1.0f,summary_height/yspan,1.0f)*summary_matrix;
			segments_matrix.makeTranslate(0.0f,-yspan/2.0f+segments_height/2.0f,0.0f);
			segments_matrix = Matrix::scale(1.0f,segments_height/yspan,1.0f)*segments_matrix;
		}
		else {
			// Overlapped summary + segments, stacked with playback
			summary_height = yspan*(h/height * width/w/frame_n);
			playback_scale = (yspan-summary_height)/yspan;
			playback_height = height * playback_scale;
			playback_center_y = yspan/2.0f-(yspan-summary_height)/2.0f;
			summary_matrix.makeTranslate(0.0f,-yspan/2.0f+summary_height/2.0f,0.0f);
			summary_matrix = Matrix::scale(1.0f,summary_height/yspan,1.0f)*summary_matrix;
		}
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
		#ifdef USE_SLIT_SCAN
		if (track_summary_type == AC_VIDEO_SUMMARY_SLIT_SCAN && slit_scanner->computed())
			slit_scan_transform->setMatrix(summary_matrix);
		else {
		#endif//def USE_SLIT_SCAN	
			frames_transform->setMatrix(summary_matrix);
		#ifdef USE_SLIT_SCAN	
		}
		#endif//def USE_SLIT_SCAN
		if (media->getNumberOfSegments()>0) segments_transform->setMatrix(segments_matrix);
	}
	
	track_node->removeChild(cursor_transform.get());
	if (media_changed)
	{
		selection_center_pos = -xspan/2.0f;
		cursorGeode();
		media_changed = false;
	}
	track_node->addChild(cursor_transform.get());
	
	if (media){
		if (video_stream){
			if (height != 0.0f && width != 0.0f){
				Matrix cursor_matrix;
				osg::ImageStream::StreamStatus streamStatus = video_stream->getStatus();
				if (manual_selection){
					video_stream->seek((selection_center_pos/(xspan/2.0f)+1)/2.0f*video_stream->getLength());
					if (streamStatus == osg::ImageStream::PAUSED){
						scrubbing = true;
						video_stream->play();
					}
				}	
				else {
					if (scrubbing){
						scrubbing = false;
						video_stream->pause();
					}	
					if (streamStatus == osg::ImageStream::PLAYING){
						selection_center_pos = -xspan/2.0f+video_stream->getReferenceTime()/video_stream->getLength()*xspan;
						selection_begin_pos = selection_center_pos;
						selection_end_pos = selection_center_pos;
					}
				}
				if (media->getNumberOfSegments()>0){
					// Stacked playback + summary + segments 
					cursor_matrix.makeTranslate(selection_center_pos,-yspan/2.0f+summary_height/2.0f+segments_height/2.0f,0.0f);	
					cursor_matrix = Matrix::scale(1.0f,(summary_height+segments_height)/yspan,1.0f)*cursor_matrix;
				}
				else {
					// Overlapped summary + segments, stacked with playback
					cursor_matrix.makeTranslate(selection_center_pos,-yspan/2.0f+summary_height/2.0f,0.0f);	
					cursor_matrix = Matrix::scale(1.0f,summary_height/yspan,1.0f)*cursor_matrix;
				}	
				cursor_transform->setMatrix(cursor_matrix);
			}	
		}
			
	}		
}
#endif //defined (SUPPORT_VIDEO)
