/**
 * @brief The video timeline track renderer class, implemented with OSG
 * @author Christian Frisson
 * @date 13/12/10
 * @copyright (c) 2010 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef __ACOSG_VIDEO_TRACK_RENDERER_H__
#define __ACOSG_VIDEO_TRACK_RENDERER_H__

#include "ACOsgTrackRenderer.h"
#include <osg/ImageStream>

class ACOsgVideoTrackPlayersSync : public OpenThreads::Thread {
public:
#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
    ACOsgVideoTrackPlayersSync(osg::ref_ptr<osg::ImageStream> _master_stream,osg::ref_ptr<osg::ImageStream> _slave_stream,float _time_skip = 0)
    :master_stream(_master_stream),slave_stream(_slave_stream),time_skip(_time_skip),running(false),scrubbing(false),active(false),previousStreamStatus(osg::ImageStream::PAUSED){}
#else
    ACOsgVideoTrackPlayersSync(osg::ref_ptr<osg::ImageStream> _master_stream, std::vector< osg::ref_ptr<osg::ImageStream> > _slave_streams, float _time_skip = 0)
    :master_stream(_master_stream),slave_streams(_slave_streams),time_skip(_time_skip),running(false),scrubbing(false),active(false){}
#endif

    ~ACOsgVideoTrackPlayersSync()
    {
        //_done = true;
        while(isRunning())
        {
            OpenThreads::Thread::YieldCurrentThread();
        }
    }
    void run(void);
    void stopSync(){running = false;}

private:
    osg::ref_ptr<osg::ImageStream> master_stream;
#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
    osg::ref_ptr<osg::ImageStream> slave_stream;
#else
    std::vector< osg::ref_ptr<osg::ImageStream> > slave_streams;
#endif
    float time_skip;
    bool running;
    bool scrubbing;
    bool active;
#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
    osg::ImageStream::StreamStatus previousStreamStatus;
#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
public:
    void updateTimeSkip(float _time){time_skip = _time;}
#ifndef SYNC_THREAD_PER_SELECTION_VIDEO
    void updateSlaves(std::vector< osg::ref_ptr<osg::ImageStream> > _slave_streams){slave_streams = _slave_streams;}
#endif//def SYNC_THREAD_PER_SELECTION_VIDEO
    void setScrubbing(bool _scrubbing){scrubbing=_scrubbing;}
    void setActive(bool _active){active=_active;}
};

class ACOsgVideoTrackRenderer : public ACOsgTrackRenderer {

public:
    ACOsgVideoTrackRenderer();
    virtual ~ACOsgVideoTrackRenderer();
    void initTrack();
    void emptyTrack();
    void prepareTracks();
    void updateTracks(double ratio=0.0);

    /// This virtual function is called by ACOsgTimelineRenderer::changeTrackPlaybackThumbnail
    virtual void changePlaybackThumbnail(std::string _thumbnail);
    /// This virtual function is called by ACOsgTimelineRenderer::changeTrackSelectionThumbnail
    virtual void changeSelectionThumbnail(std::string _thumbnail);
    /// This virtual function is called by ACOsgTimelineRenderer::changeTrackSummaryThumbnail
    virtual void changeSummaryThumbnail(std::string _thumbnail);

protected:

    osg::ref_ptr<osg::MatrixTransform> selection_transform, summary_transform, playback_transform;

    // main video stream
    osg::ref_ptr<osg::ImageStream> video_stream;

    // playback video
    osg::ref_ptr<osg::MatrixTransform> playback_video_transform;

    // selection frames
    osg::ref_ptr<osg::MatrixTransform> selection_center_frame_transform;
    std::vector< osg::ref_ptr<osg::Image> > right_selection_video_images,left_selection_video_images;
    std::vector< osg::ref_ptr<osg::ImageStream> > right_selection_video_streams,left_selection_video_streams;
    std::vector< osg::ref_ptr<osg::Texture2D> > right_selection_video_textures,left_selection_video_textures;
    std::vector< osg::ref_ptr<osg::MatrixTransform> > right_selection_video_transforms,left_selection_video_transforms;
    osg::ref_ptr<osg::Group> right_selection_video_group,left_selection_video_group;
#ifdef SYNC_SELECTION_VIDEOS_BY_THREAD
#ifdef SYNC_THREAD_PER_SELECTION_VIDEO
    std::vector< ACOsgVideoTrackPlayersSync* > right_selection_video_syncs,left_selection_video_syncs;
#else
    ACOsgVideoTrackPlayersSync *right_selection_video_sync,*left_selection_video_sync;
#endif
#endif//def SYNC_SELECTION_VIDEOS_BY_THREAD

    // summary cursor
    osg::ref_ptr<osg::MatrixTransform> summary_cursor_transform;

    // summary selection
    osg::ref_ptr<osg::MatrixTransform> selection_begin_transform;
    osg::ref_ptr<osg::MatrixTransform> selection_zone_transform;
    osg::ref_ptr<osg::MatrixTransform> selection_end_transform;

    // summary frames
    std::vector< osg::ref_ptr<osg::Image> > summary_images;
    std::vector< osg::ref_ptr<osg::ImageStream> > summary_streams;
    std::vector< osg::ref_ptr<osg::Texture2D> > summary_textures;
    osg::ref_ptr<osg::MatrixTransform> summary_frames_transform;

    // summary slit-scan
    osg::ref_ptr<osg::MatrixTransform> slit_scan_transform;
    void slitScanTransform();
    bool slit_scan_changed;

    // summary segments
    osg::ref_ptr<osg::MatrixTransform> segments_transform;
    osg::ref_ptr<osg::Group> segments_group;
    std::vector< osg::ref_ptr<osg::Geode> > segments_geodes;

    void playbackVideoTransform();
    void framesTransform();
    void segmentsTransform();
    void selectionCenterFrameTransform();
#if defined(SYNC_SELECTION_VIDEOS_BY_THREAD) && defined(SYNC_THREAD_PER_SELECTION_VIDEO)
    void updateSelectionVideos(std::vector< osg::ref_ptr<osg::Image> >& _selection_video_images,
                               std::vector< osg::ref_ptr<osg::ImageStream> >& _selection_video_streams,
                               std::vector< osg::ref_ptr<osg::Texture2D> >& _selection_video_textures,
                               std::vector< osg::ref_ptr<osg::MatrixTransform> >& _selection_video_transforms,
                               osg::ref_ptr<osg::Group>& _selection_video_group,
                               std::vector< ACOsgVideoTrackPlayersSync* >& _selection_video_syncs,
                               int _number);
#else
    void updateSelectionVideos(std::vector< osg::ref_ptr<osg::Image> >& _selection_video_images,
                               std::vector< osg::ref_ptr<osg::ImageStream> >& _selection_video_streams,
                               std::vector< osg::ref_ptr<osg::Texture2D> >& _selection_video_textures,
                               std::vector< osg::ref_ptr<osg::MatrixTransform> >& _selection_video_transforms,
                               osg::ref_ptr<osg::Group>& _selection_video_group,
                               int _number);
#endif
    void selectionCursorTransform();
    void selectionBeginTransform();
    void selectionZoneTransform();
    void selectionEndTransform();

    float summary_center_y,summary_height;
    float segments_center_y,segments_height;
    float selection_height,selection_center_y,selection_center_frame_width;
    float playback_center_y,playback_height;
    float summary_frame_min_width,selection_frame_min_width;
    int summary_frame_n, selection_frame_n, segments_number;
    bool scrubbing;
    bool selection_needs_resync;

    std::string track_summary_type;
    std::string track_selection_type;
    bool track_playback_visibility;

    bool track_summary_type_changed;
    bool track_selection_type_changed;
    bool track_playback_visibility_changed;

    std::string resized_thumbnail_filename;

public:
    // Using std::string instead of typedefs for faster serialization, but less error-proof
    void setSummaryType(std::string type){this->track_summary_type = type;}
    void updateSummaryType(std::string type){this->track_summary_type = type; track_summary_type_changed = true;}
    std::string getSummaryType(){return this->track_summary_type;}
    void setSelectionType(std::string type){this->track_selection_type = type;}
    void updateSelectionType(std::string type){this->track_selection_type = type; track_selection_type_changed = true;}
    std::string getSelectionType(){return this->track_selection_type;}
    void setPlaybackVisibility(bool _visibility){this->track_playback_visibility = _visibility;}
    void updatePlaybackVisibility(bool _visibility){this->track_playback_visibility = _visibility; track_playback_visibility_changed = true;}
    bool getPlaybackVisibility(){return this->track_playback_visibility;}

private:
    // Reconstructs transforms, geometries and shapes
    void updatePlaybackContents();
    void updateSummaryContents();
    void updateSelectionContents();
    void updateSliderContents();

    // Syncs selection/video streams with time skip
    void syncVideoStreams();

    // Updates positions and sizes
    void updateTransformsAspects();
};

#endif
