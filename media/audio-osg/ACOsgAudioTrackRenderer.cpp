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

#include "ACOsgAudioTrackRenderer.h"
#include <osgUtil/Tessellator> // to tessellate multiple contours

#include <osg/Version>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

using namespace osg;

ACOsgAudioTrackRenderer::ACOsgAudioTrackRenderer() : ACOsgTrackRenderer()
{
    media_type = MEDIA_TYPE_AUDIO;
    summary_waveform_geode = 0;
    summary_waveform_transform = 0;
    summary_waveform_geometry = 0;
    summary_cursor_transform = 0;
    track_geode = 0;
    playback_waveform_geode = 0;
    playback_waveform_geometry = 0;
    playback_waveform_transform = 0;

    //zoom_x = 10.0; zoom_y = 6.0;
    zoom_x = 1.0; zoom_y = 1.0;
    track_left_x = 0.0;

    summary_height = 1.0f/8.0f;//[0;1]
    segments_height = 1.0f/8.0f;//[0;1]
    playback_height = 1.0f - summary_height; //[0;1]
    summary_center_y = summary_height/2; //[0;1]

    //segments_center_y = segments_height/2;
    //playback_center_y = summary_height + segments_height + playback_height/2.0f;

    playback_center_y = summary_height + playback_height/2.0f;

    samples_hop_threshold = 10;
    samples_n_threshold = screen_width*15;
    //samples = 0;

    segments_transform = new MatrixTransform();
    segments_group = new Group();
    segments_number = 0;

    summary_waveform_thumbnail = "Classic timeline waveform";
    playback_waveform_thumbnail = "Classic timeline waveform";
}

ACOsgAudioTrackRenderer::~ACOsgAudioTrackRenderer() {
    // track_node->removeChild(0,1);
    summary_waveform_geode = 0;
    summary_waveform_geometry = 0;
    summary_cursor_transform=0;
    track_geode=0;
    playback_waveform_geode = 0;
    playback_waveform_geometry = 0;
    playback_waveform_transform = 0;
    segments_transform=0;
    summary_waveform_transform = 0;
}

void ACOsgAudioTrackRenderer::selectionWaveformGeode() {
    if(summary_waveform_thumbnail == "")
        return;

    if(track_node->containsNode(summary_waveform_transform))
        track_node->removeChild(summary_waveform_transform);

    summary_waveform_transform = 0;
    summary_waveform_transform = new MatrixTransform;

    summary_waveform_geode = 0;
    summary_waveform_geode = new Geode();

    summary_waveform_geometry = 0;
    summary_waveform_geometry = this->thumbnailGeometry(summary_waveform_thumbnail);

    osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
    (*colors)[0] = Vec4(1.0,1.0,1.0,1.0);
    summary_waveform_geometry->setColorArray(colors);

    if(summary_waveform_geometry){
        summary_waveform_geode->addDrawable(summary_waveform_geometry);
        summary_waveform_geode->setUserData(new ACRefId(track_index,"audio track summary waveform"));
        summary_waveform_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
        summary_waveform_transform->addChild(summary_waveform_geode);
        //track_node->addChild(summary_waveform_geode);
    }
}

void ACOsgAudioTrackRenderer::selectionCursorGeode() {
    this->boxTransform(summary_cursor_transform, 2, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "audio track cursor");
}

void ACOsgAudioTrackRenderer::selectionZoneGeode() {
    this->boxTransform(selection_zone_transform, 1, osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f), "track selection zone");
}

void ACOsgAudioTrackRenderer::selectionBeginGeode() {
    this->boxTransform(selection_begin_transform, 4, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection begin");
}

void ACOsgAudioTrackRenderer::selectionEndGeode() {
    this->boxTransform(selection_end_transform, 4, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection end");
}

void ACOsgAudioTrackRenderer::playbackWaveformGeode() {
    if(playback_waveform_thumbnail == "")
        return;

    if(track_node->containsNode(playback_waveform_transform))
        track_node->removeChild(playback_waveform_transform);

    playback_waveform_transform = 0;
    playback_waveform_transform = new MatrixTransform;

    playback_waveform_geode = 0;
    playback_waveform_geode = new Geode();

    playback_waveform_geometry = 0;
    playback_waveform_geometry = this->thumbnailGeometry(playback_waveform_thumbnail);

    osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
    (*colors)[0] = Vec4(1.0,1.0,1.0,1.0);
    playback_waveform_geometry->setColorArray(colors);

    if(playback_waveform_geometry){ 
        playback_waveform_geode->addDrawable(playback_waveform_geometry);
        playback_waveform_geode->setUserData(new ACRefId(track_index,"audio track summary waveform"));
        playback_waveform_transform->addChild(playback_waveform_geode);
        playback_waveform_transform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
        //track_node->addChild(playback_waveform_transform);
    }

}

void ACOsgAudioTrackRenderer::trackGeode() {
    StateSet *state;

    track_geode = new Geode();

    TessellationHints *hints = new TessellationHints();
    hints->setDetailRatio(0.0);

    state = track_geode->getOrCreateStateSet();
    state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    state->setMode(GL_BLEND, StateAttribute::ON); // CF new


    track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),1.0f,1.0f,0.0), hints)); //draws a square // Vintage AudioCycle

    track_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );//CF new
}

void ACOsgAudioTrackRenderer::segmentsGeode() {
    segments_group->removeChildren(0,	segments_group->getNumChildren());

    /*std::vector< osg::ref_ptr<osg::Geode> >::iterator segments_geodes_iter;
 for (segments_geodes_iter=segments_geodes.begin();segments_geodes_iter!=segments_geodes.begin();segments_geodes_iter++)
  (*segments_geodes_iter)=0;*/

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
            segment_color = Vec4(1,1,0.5,0.5f);//Vec4(0.0f, 0.0f, 1.0f, 1.0f);
        else // even segment index
            segment_color = Vec4(1,0.5,1,0.5f);//Vec4(1.0f, 0.0f, 0.0f, 1.0f);
        osg::ref_ptr<osg::Vec4Array> segment_colors = new Vec4Array;
        segment_colors->push_back(segment_color);

        segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-0.5f+ (media->getSegment(s)->getStart()+ media->getSegment(s)->getEnd())/2.0f/media_length,segments_center_y,0.0f) , (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length , segments_height*2 , 0.0f), hints));
        //std::cout << "Segment " << s << " start " << media->getSegment(s)->getStart()/media_length << " end " << media->getSegment(s)->getEnd()/media_length << " width " << (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length << std::endl;
        ((ShapeDrawable*)(segments_geodes[s])->getDrawable(0))->setColor(segment_color);
        segments_geodes[s]->setUserData(new ACRefId(track_index,"audio track segments"));
        segments_group->addChild(segments_geodes[s]);
    }
    if(segments_n>0){
        if (segments_transform)
            segments_transform->addChild(segments_group);
    }
}

void ACOsgAudioTrackRenderer::prepareTracks() {  

    summary_waveform_geode = 0;
    summary_waveform_geometry = 0;
    //summary_cursor_transform=0;
    track_geode=0;
    playback_waveform_geode = 0;
    playback_waveform_geometry = 0;
    playback_waveform_transform = 0;
    segments_transform=0;
    summary_waveform_transform = 0;

    if(!summary_cursor_transform){
        selectionCursorGeode();
        track_node->addChild(summary_cursor_transform);
    }
}

void ACOsgAudioTrackRenderer::updateTracks(double ratio) {
    float selection_pos_x = 0;
    if (!manual_selection && media)// && media_from_lib)
    {
        //if (media_index > -1)
        if (media)
        {
            //const ACMediaNode* attribute = media_cycle->getMediaNode(media_index);
            const ACMediaNode* attribute = media_cycle->getNodeFromMedia(media);
            if ( attribute->getActivity()==1)
            {
                selection_pos_x = (float) attribute->getCurrentFrame() / (float)(((ACAudio*) media)->getNFrames());
                this->moveSelection(selection_pos_x,this->getSelectionPosY());
            }
        }
    }
    //manual_selection = false;

    if (media_changed)// || screen_width_changed)
    {
        //track_node->removeChild(summary_cursor_transform);
        track_node->removeChild(summary_waveform_transform);
        track_node->removeChild(playback_waveform_transform);
    }

    if (media_changed)// || selection_begin_pos_changed || selection_end_pos_changed)
    {
        track_node->removeChild(selection_begin_transform);
        track_node->removeChild(selection_zone_transform);
        track_node->removeChild(selection_end_transform);
        /*track_node->removeChild(selection_begin_geode);
   track_node->removeChild(selection_zone_geode);
   track_node->removeChild(selection_end_geode);*/
//        track_node->removeChild(summary_cursor_transform);
    }
    if (media){
        if (media_changed)
        {
            /*if (samples) delete samples;
            samples = ((ACAudio*) media)->getMonoSamples();*/
            selection_min_width = screen_width/(((ACAudio*) media)->getNFrames()); // one frame per pixel

            //CF debug
            /*std::cout << "Updating audio track with ACAudio of";
   std::cout << " samplerate: " << ((ACAudio*) media)->getSampleRate() << "," ;
   std::cout << " "<<((ACAudio*) media)->getNFrames() <<" frames,";
   std::cout << " start " << ((ACAudio*) media)->getStart() <<",";
   std::cout << " end " << ((ACAudio*) media)->getEnd() << ",";
   std::cout << " from lib " << media_from_lib  << ",";
   std::cout << " " << media->getNumberOfSegments()  << " segments";
   std::cout << std::endl;
   std::cout << "Audio track with ";
   std::cout << " selection_begin_pos_x " << selection_begin_pos_x  << ",";
   std::cout << " selection_center_pos_x " << selection_center_pos_x  << ",";
   std::cout << " selection_end_pos_x " << selection_end_pos_x  << ",";
   std::cout << std::endl;*/

            //CF: dummy segments for testing
            /*if(media_cycle->getLibrary()->getMediaType() != MEDIA_TYPE_MIXED){
    this->createDummySegments();
    track_node->removeChild(segments_transform);
                        }*/
            if (media->getNumberOfSegments()>0){//////CF dangerous if a new media has the same number of segments than the previous one:  && segments_number != media->getNumberOfSegments()){
                /////CF track_node->removeChild(segments_transform);
                //if (frame_n != floor(width/frame_min_width)){
                //double segments_start = getTime();
                std::cout << "Generating segments... ";
                if (segments_transform)
                    segments_transform->removeChild(segments_group);
                segmentsGeode();
                //std::cout << getTime()-segments_start << " sec." << std::endl;
                segments_number = media->getNumberOfSegments();
            }
            if (media->getNumberOfSegments()>0)
                track_node->addChild(segments_transform);
        }

        if (media_changed)
        {
            selectionWaveformGeode();
            track_node->addChild(summary_waveform_transform);
        }
        if (media_changed)
        {
            playbackWaveformGeode();
            track_node->addChild(playback_waveform_transform);
        }
        if (media_changed )//|| selection_begin_pos_changed || selection_end_pos_changed)
        {
            selectionZoneGeode();
            track_node->addChild(selection_zone_transform);
            //if (media_changed || selection_begin_pos_changed)
                selectionBeginGeode();
            track_node->addChild(selection_begin_transform);
            //if (media_changed || selection_end_pos_changed)
                selectionEndGeode();
            track_node->addChild(selection_end_transform);
        }
        selection_begin_pos_changed = false;
        selection_end_pos_changed = false;

        //displayed_media_index = media_index;
        media_changed = false;
    }

// CF solve this for media outside lib
//    Matrix curserT;
//    if (media_from_lib)
//    {
//        //if (media_index > -1)
//        if(media)
//        {
//            //const ACMediaNode* attribute = media_cycle->getMediaNode(media_index);
//            const ACMediaNode* attribute = media_cycle->getNodeFromMedia(media);
//            if ( attribute->getActivity()==1)
//            {
//                curserT.makeTranslate(Vec3(attribute->getCursor() * screen_width/media->getThumbnailWidth(), 0.0f, 0.0f));			// curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
//                summary_cursor_transform->setMatrix(curserT);
//            }
//        }
//    }

    Matrix summary_cursor_matrix,selection_begin_matrix,selection_end_matrix,selection_zone_matrix;

    if(summary_cursor_transform){
        const ACMediaNode* attribute = media_cycle->getNodeFromMedia(media);
        //float selection_pos_x = (float) attribute->getCurrentFrame() / (float)(((ACAudio*) media)->getNFrames());
        summary_cursor_matrix.makeTranslate(Vec3(selection_center_pos_x*this->width, summary_center_y*this->height, 0));
        //float _height = media->getThumbnailHeight(summary_waveform_thumbnail);
        summary_cursor_matrix = Matrix::scale(1.0f,summary_height*this->height,1.0f)*summary_cursor_matrix;
        summary_cursor_transform->setMatrix(summary_cursor_matrix);
    }

    selection_begin_matrix.makeTranslate(selection_begin_pos_x*this->width,summary_center_y*this->height,0.0f);
    selection_begin_matrix = Matrix::scale(1.0f,summary_height*this->height,1.0f)*selection_begin_matrix;
    if(selection_begin_transform)
        selection_begin_transform->setMatrix(selection_begin_matrix);
    selection_end_matrix.makeTranslate(selection_end_pos_x*this->width,summary_center_y*this->height,0.0f);
    selection_end_matrix = Matrix::scale(1.0f,summary_height*this->height,1.0f)*selection_end_matrix;
    if(selection_end_transform)
        selection_end_transform->setMatrix(selection_end_matrix);
    selection_zone_matrix.makeTranslate((selection_begin_pos_x+selection_end_pos_x)/2.0f*this->width,summary_center_y*this->height,0.0f);
    selection_zone_matrix = Matrix::scale((selection_end_pos_x-selection_begin_pos_x)*this->width,summary_height*this->height,1.0f)*selection_zone_matrix;
    if(selection_zone_transform)
        selection_zone_transform->setMatrix(selection_zone_matrix);

    if(summary_waveform_transform){
        Matrix T;
        T.makeTranslate(Vec3(0, summary_center_y*this->height, 0));
        float _height = media->getThumbnailHeight(summary_waveform_thumbnail);
        float _width = media->getThumbnailWidth(summary_waveform_thumbnail);
        T = Matrix::scale(1.0f,summary_height*this->height/_height,1.0f)*T;
        summary_waveform_transform->setMatrix(T);
    }
    if(playback_waveform_transform){
        const ACMediaNode* attribute = media_cycle->getNodeFromMedia(media);
        //float selection_pos_x = (float) attribute->getCurrentFrame() / (float)(((ACAudio*) media)->getNFrames());
        Matrix T;
        T.makeTranslate(Vec3((-selection_center_pos_x)*this->width, playback_center_y*this->height, 0));
        float _height = media->getThumbnailHeight(summary_waveform_thumbnail);
        T = Matrix::scale(1.0f/(selection_end_pos_x-selection_begin_pos_x),playback_height*this->height/_height,1.0f)*T;
        playback_waveform_transform->setMatrix(T);
    }
}

void ACOsgAudioTrackRenderer::changePlaybackThumbnail(std::string _thumbnail)
{
    playback_waveform_thumbnail = _thumbnail;
    track_node->removeChild(playback_waveform_transform);
    playbackWaveformGeode();
    track_node->addChild(playback_waveform_transform);
}

void ACOsgAudioTrackRenderer::changeSummaryThumbnail(std::string _thumbnail)
{
    summary_waveform_thumbnail = _thumbnail;
    track_node->removeChild(summary_waveform_transform);
    selectionWaveformGeode();
    track_node->addChild(summary_waveform_transform);
}
