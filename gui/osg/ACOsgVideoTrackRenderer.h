/*
 *  ACOsgVideoTrackRenderer.h
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

#ifndef __ACOSG_VIDEO_TRACK_RENDERER_H__
#define __ACOSG_VIDEO_TRACK_RENDERER_H__

#if defined (SUPPORT_VIDEO)

#include "ACOsgBrowserRenderer.h"
//#include "ACOsgImageRenderer.h"
#include "ACOsgTrackRenderer.h"

// Slit-scan

//FFmpeg
extern "C" {
#define __STDC_CONSTANT_MACROS
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
}
#include <cassert>
#include <algorithm>
#include <vector>

//Threads
#include <iostream>
#include <OpenThreads/Thread>

#include <osg/Notify>


class ACOsgVideoSlitScanThread : public OpenThreads::Thread {
public:
	
    ACOsgVideoSlitScanThread()
	:m_context(0),_done(false),filename(""),notify_level(osg::WARN){}
	
    ~ACOsgVideoSlitScanThread()
    {
        //_done = true;
        while(isRunning())
        {
            OpenThreads::Thread::YieldCurrentThread();
        }
		if (m_context) {avcodec_close(m_context); m_context = 0;}
    }
	
    void run(void)
    {
        std::cout << "Slit-scanning " << filename<< "..." << std::endl;
		notify_level = osg::getNotifyLevel();
		osg::setNotifyLevel(osg::WARN);//to remove the copyImage NOTICEs
		_done = false;
		if (filename!=""){
			this->computeSlitScan();
			_done = true;
			std::cout << "Done slit-scanning " << filename << "..." <<std::endl;
		}
		osg::setNotifyLevel(notify_level);
    }
	
	private:
		osg::ref_ptr<osg::Image> slit_scan;
		AVCodecContext* m_context;
		bool _done;
		std::string filename;
		osg::NotifySeverity notify_level;
	protected:
		int convert(AVPicture *dst, int dst_pix_fmt, AVPicture *src,int src_pix_fmt, int src_width, int src_height);
		void yuva420pToRgba(AVPicture * const dst, AVPicture * const src, int width, int height);
		int computeSlitScan();
	public:
		osg::ref_ptr<osg::Image> getImage(){if (_done) return slit_scan; else return 0;}
		void setFileName(std::string _filename){filename = _filename;}
		std::string getFileName(){return filename;}
		void reset(){_done=false;}
		bool computed(){return _done;}
};

class ACOsgVideoTrackRenderer : public ACOsgTrackRenderer {
protected:
	osg::ref_ptr<osg::ImageStream>video_stream;
	CvCapture* summary_data;

	osg::ref_ptr<osg::MatrixTransform> playback_transform;
	osg::ref_ptr<osg::MatrixTransform> frames_transform;
	osg::ref_ptr<osg::MatrixTransform> segments_transform;
	osg::ref_ptr<osg::MatrixTransform> slit_scan_transform;
	osg::ref_ptr<osg::MatrixTransform> cursor_transform;
	
	osg::ref_ptr<osg::Geode> playback_geode;
	osg::ref_ptr<osg::Group> frames_group;
	osg::ref_ptr<osg::Geode> frame_geode;
	osg::ref_ptr<osg::Group> segments_group;
	std::vector< osg::ref_ptr<osg::Geode> > segments_geodes;
	//osg::ref_ptr<osg::Geode> segments_geodes;
	osg::ref_ptr<osg::Geode> slit_scan_geode;
	osg::ref_ptr<osg::Geode> cursor_geode;

	void playbackGeode();
	void framesGeode();
	void segmentsGeode();
	void slitScanGeode();
	void cursorGeode();
	
	float zoom_x, zoom_y, track_left_x;
	float summary_center_y,summary_height;
	float playback_center_y,playback_height,playback_scale;
	float segments_center_y,segments_height;
	float frame_min_width, frame_n;
	bool scrubbing;
	bool slit_scan_changed;
	
	ACOsgVideoSlitScanThread* slit_scanner;

	static const int NCOLORS ;
	osg::ref_ptr<osg::Vec4Array> colors;
	osg::ref_ptr<osg::Vec4Array> colors2;
	osg::ref_ptr<osg::Vec4Array> colors3;
	
	ACVideoSummaryType track_summary_type;
	
	int segments_number;
	
public:
	ACOsgVideoTrackRenderer();
	~ACOsgVideoTrackRenderer();
	void prepareTracks();
	void updateTracks(double ratio=0.0);
	
	void setSummaryType(ACVideoSummaryType type){this->track_summary_type = type;}
	ACVideoSummaryType getSummaryType(){return this->track_summary_type;}
};

#endif //defined (SUPPORT_VIDEO)
#endif
