/**
 * @brief The audio timeline track renderer class, implemented with OSG
 * @author Christian Frisson
 * @date 28/04/10
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

#ifndef __ACOSG_AUDIO_TRACK_RENDERER_H__
#define __ACOSG_AUDIO_TRACK_RENDERER_H__

#include "ACOsgTrackRenderer.h"
#include "ACAudio.h"

class ACOsgAudioTrackRenderer : public ACOsgTrackRenderer {

public:
    ACOsgAudioTrackRenderer();
    ~ACOsgAudioTrackRenderer();
    void prepareTracks();
    void updateTracks(double ratio=0.0);

    /// This virtual function is called by ACOsgTimelineRenderer::changeTrackPlaybackThumbnail
    virtual void changePlaybackThumbnail(std::string _thumbnail);
    /// This virtual function is called by ACOsgTimelineRenderer::changeTrackSummaryThumbnail
    virtual void changeSummaryThumbnail(std::string _thumbnail);

protected:
    // playback
    osg::ref_ptr<osg::Geode> playback_waveform_geode;
    osg::ref_ptr<osg::Geometry> playback_waveform_geometry;
    osg::ref_ptr<osg::MatrixTransform> playback_waveform_transform;
    std::string playback_waveform_thumbnail;

    // summary cursor
    osg::ref_ptr<osg::MatrixTransform> summary_cursor_transform;

    // summary selection
    osg::ref_ptr<osg::MatrixTransform> selection_begin_transform;
    osg::ref_ptr<osg::MatrixTransform> selection_zone_transform;
    osg::ref_ptr<osg::MatrixTransform> selection_end_transform;

    // summary waveform
    osg::ref_ptr<osg::Geode> summary_waveform_geode;
    osg::ref_ptr<osg::Geometry> summary_waveform_geometry;
    osg::ref_ptr<osg::MatrixTransform> summary_waveform_transform;

    std::string summary_waveform_thumbnail;

    // segments
    osg::ref_ptr<osg::MatrixTransform> segments_transform;
    osg::ref_ptr<osg::Group> segments_group;
    std::vector< osg::ref_ptr<osg::Geode> > segments_geodes;

    // track background
    osg::ref_ptr<osg::Geode> track_geode;

    void selectionWaveformGeode();
    void selectionCursorGeode();
    void selectionBeginGeode();
    void selectionZoneGeode();
    void selectionEndGeode();
    void playbackWaveformGeode();
    void segmentsGeode();
    void trackGeode();

    float zoom_x, zoom_y, track_left_x;
    float summary_center_y,summary_height;
    float playback_center_y,playback_height;
    //float* samples;
    int samples_hop_threshold; // above: envelope with quads, below: envelope with line loop
    int samples_n_threshold; // above: envelope with quads, below: envelope with line loop
    float segments_center_y,segments_height;
    int segments_number;
};

#endif
