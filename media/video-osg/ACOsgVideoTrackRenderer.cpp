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

// ----------- uncomment this to use selection videos to be sync'd by thread (as opposed as sync'd in updateTracks)
//#define SYNC_SELECTION_VIDEOS_BY_THREAD

// ----------- uncomment this to use one thread per selection video sync (as opposed to one thread for all), per side (left+right), requires SYNC_SELECTION_VIDEOS_BY_THREAD
//#define SYNC_THREAD_PER_SELECTION_VIDEO

// ----------- uncomment this to test the selection video sync (without time skip based on the selection width)
//#define TEST_SYNC_WITHOUT_TIME_SKIP

#include "ACOsgVideoTrackRenderer.h"
#include "ACOsgMediaThumbnail.h"
#include "ACVideo.h"
#include <cmath>
#include <osg/ImageUtils>
#include <osg/Version>
#include <osgDB/ReadFile>

using namespace std;
using namespace osg;

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

    media_type = MEDIA_TYPE_VIDEO;
    track_summary_type = "Keyframes";
    track_selection_type = "Keyframes";
    track_playback_visibility = true;

    segments_height = 1.0f/16.0f;//[0;1]
    summary_height = 1.0f/8.0f;//[0;1]
    slitscan_height = 1.0f/8.0f;//[0;1]
    selection_height = 1.0f/8.0f;//[0;1]
    playback_height = 1.0f - summary_height - segments_height - selection_height;//[0;1]

    segments_center_y = segments_height/2.0f;
    summary_center_y = segments_height + summary_height/2.0f;//[0;1]
    slitscan_center_y = segments_height + summary_height + slitscan_height/2.0f;//[0;1]
    selection_center_y = summary_height + segments_height + slitscan_height + selection_height/2.0f;
    playback_center_y = summary_height + segments_height + slitscan_height + selection_height + playback_height/2.0f;//[0;1]

    selection_center_frame_width = 0.0f;
    summary_frame_min_width = 32;
    selection_frame_min_width = 64;

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

    slit_scan_transform=0;
    slit_scan_changed = false;

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

    resized_thumbnail_filename = "";

    corresponding_segment_index = -1;
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
    slit_scan_transform=0;
    slitscan_transforms.resize(0);

    for (std::vector< osg::ref_ptr<osg::Geode> >::iterator _geode = segments_geodes.begin(); _geode != segments_geodes.end();++_geode)
        (*_geode) = 0;
    segments_geodes.clear();
}

void ACOsgVideoTrackRenderer::changePlaybackThumbnail(std::string _thumbnail){
    bool _visibility = (_thumbnail != "None");
    if(this->track_playback_visibility != _visibility){
        this->track_playback_visibility = _visibility;
        this->track_playback_visibility_changed = true;
    }
}

void ACOsgVideoTrackRenderer::changeSelectionThumbnail(std::string _thumbnail){
    if(this->track_selection_type != _thumbnail){
        this->track_selection_type = _thumbnail;
        this->track_selection_type_changed = true;
    }
}

void ACOsgVideoTrackRenderer::changeSummaryThumbnail(std::string _thumbnail){
    if(this->track_summary_type != _thumbnail){
        this->track_summary_type = _thumbnail;
        this->track_summary_type_changed = true;
    }
}

void ACOsgVideoTrackRenderer::selectionZoneTransform() {
    this->boxTransform(selection_zone_transform, 1, osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f), "track selection zone");
}

void ACOsgVideoTrackRenderer::selectionBeginTransform() {
    this->boxTransform(selection_begin_transform, 4, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection begin");
}

void ACOsgVideoTrackRenderer::selectionEndTransform() {
    this->boxTransform(selection_end_transform, 4, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection end");
}

void ACOsgVideoTrackRenderer::selectionCursorTransform() {
    this->boxTransform(summary_cursor_transform, 2, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "video track cursor");
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

    double imagey = (float)(media->getHeight())/2.0f;
    double imagex = (float)(media->getWidth())/2.0f;

    if(imagex == 0.0f)
        std::cerr << "ACOsgVideoTrackRenderer::playbackVideoTransform: null video width" << std::endl;
    if(imagey == 0.0f)
        std::cerr << "ACOsgVideoTrackRenderer::playbackVideoTransform: null video height" << std::endl;

    vertices = new Vec3Array(4);
    (*vertices)[0] = Vec3(-imagex, -imagey, 0.0f);
    (*vertices)[1] = Vec3(imagex, -imagey, 0.0f);
    (*vertices)[2] = Vec3(imagex, imagey, 0.0f);
    (*vertices)[3] = Vec3(-imagex, imagey, 0.0f);
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

        ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
        if(!media_thumbnail){
            std::cerr << "ACOsgImageRenderer::imageGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
            return;
        }
        ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
        if(!osg_thumbnail){
            std::cerr << "ACOsgImageRenderer::imageGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
            return;
        }

        playback_texture = osg_thumbnail->getTexture();

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

    double imagey = 0.5f;
    double imagex = 0.5f;

    vertices = new Vec3Array(4);
    (*vertices)[0] = Vec3(-imagex, -imagey, 0.0f);
    (*vertices)[1] = Vec3(imagex, -imagey, 0.0f);
    (*vertices)[2] = Vec3(imagex, imagey, 0.0f);
    (*vertices)[3] = Vec3(-imagex, imagey, 0.0f);
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

        ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
        if(!media_thumbnail){
            std::cerr << "ACOsgImageRenderer::imageGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
            return;
        }
        ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
        if(!osg_thumbnail){
            std::cerr << "ACOsgImageRenderer::imageGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
            return;
        }

        selection_center_frame_texture = osg_thumbnail->getTexture();

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
        //selection_center_frame_geode->setUserData(new ACRefId(track_index,"video track selection frames"));
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
            if(_selection_video_images.size()<m+1){
                osg::ref_ptr<osg::Image> thumbnail = osgDB::readImageFile(resized_thumbnail_filename);
                if(thumbnail == 0){
                    std::cerr << "ACOsgVideoTrackRenderer::updateSelectionVideos: couldn't load " << resized_thumbnail_filename << ", aborting " << std::endl;
                    return;
                }
                _selection_video_images.push_back(thumbnail);
            }
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

            double imagey = 0.5f;
            double imagex = 0.5f;

            vertices = new Vec3Array(4);
            (*vertices)[0] = Vec3(-imagex, -imagey, 0.0f);
            (*vertices)[1] = Vec3(imagex, -imagey, 0.0f);
            (*vertices)[2] = Vec3(imagex, imagey, 0.0f);
            (*vertices)[3] = Vec3(-imagex, imagey, 0.0f);
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
            video_geode->setUserData(new ACRefId(track_index,"video track selection frames"));
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

void ACOsgVideoTrackRenderer::slitScanTransform() {
    /*StateSet *state;
    Vec3Array* vertices;
    Vec2Array* texcoord;
    osg::ref_ptr<Geometry> slit_scan_geometry;
    Texture2D *slit_scan_texture;

    slit_scan_geometry = 0;
    slit_scan_transform = 0;
    slit_scan_transform = new MatrixTransform();
    osg::ref_ptr<osg::Geode> slit_scan_geode = new Geode();
    slit_scan_geometry = new Geometry();

    double imagey = 0.5f;
    double imagex = 0.5f;

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
    slit_scan_geometry->setTexCoordArray(0, texcoord);*/

    //    if (slit_scanner->computed()){
    //        slit_scan_texture = new osg::Texture2D;
    //        slit_scan_texture->setResizeNonPowerOfTwoHint(false);
    //        slit_scan_texture->setImage(slit_scanner->getImage());
    //        state = slit_scan_geometry->getOrCreateStateSet();
    //        state->setTextureAttribute(0, slit_scan_texture);
    //        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
    //        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    //        colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
    //        slit_scan_geometry->setColorArray(colors);
    //        slit_scan_geometry->setColorBinding(Geometry::BIND_OVERALL);
    //        slit_scan_geode->addDrawable(slit_scan_geometry);
    //        slit_scan_transform->addChild(slit_scan_geode);
    //        slit_scan_geode->setUserData(new ACRefId(track_index,"video track summary slit-scan"));
    //    }

    slitscan_transforms.clear();
    slitscan_transforms.resize(0);

    if(!media)
        return;
    ACMediaThumbnail* slitscan_thumbnail = media->getThumbnail("Slit-scan");
    if(slitscan_thumbnail==0){
        std::cerr << "ACOsgVideoTrackRenderer::slitScanTransform: no slit-scan available" << std::endl;
        return;
    }
    if(slitscan_thumbnail->getNumberOfSegments()>1){
        std::vector<ACMediaThumbnail*> thumbnail_segments = slitscan_thumbnail->getAllSegments();
        for(std::vector<ACMediaThumbnail*>::iterator thumbnail_segment = thumbnail_segments.begin();thumbnail_segment != thumbnail_segments.end(); thumbnail_segment++){
            std::string thumbnail_filename = (*thumbnail_segment)->getFileName();
            //slitscan_thumbnail->getFileName();
            osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(thumbnail_filename).substr(1));
            if (!readerWriter){
                cerr << "ACOsgBaseRenderer::thumbnailGeometry: can't find an OSG plugin to read file '" << thumbnail_filename << "'" << endl;
                return;
            }
            else{
                cout <<"ACOsgBaseRenderer::thumbnailGeometry: using OSG plugin: "<< readerWriter->className() <<std::endl;
            }

            osg::ref_ptr<osg::Image> thumbnail_image = 0;
            try{
                thumbnail_image = osgDB::readImageFile(thumbnail_filename);
            }
            catch(const std::exception e){
                std::cerr << e.what() << std::endl;
            }

            if (!thumbnail_image){
                cerr << "<ACOsgBaseRenderer::thumbnailGeometry: problem loading thumbnail" << endl;
                return;
            }
            osg::ref_ptr<osg::Texture2D> thumbnail_texture = new osg::Texture2D;

            thumbnail_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.

            thumbnail_texture->setImage(thumbnail_image);
            //thumbnail_texture->setResizeNonPowerOfTwoHint(false);

            int width = (*thumbnail_segment)->getWidth();
            int height = (*thumbnail_segment)->getHeight();
            std::cout << "ACOsgBaseRenderer::thumbnailGeometry: geode with (thumbnail) width " <<  width << " and height " << height << std::endl;

            // image vertices
            float zpos = 0;
            double imagex, imagey;
            imagex = 0.5*width;
            imagey = 0.5*height;
            Vec3Array* vertices = new Vec3Array(4);
            if((*thumbnail_segment)->isCircular()){
                (*vertices)[0] = Vec3(-imagex, -imagey, zpos);
                (*vertices)[1] = Vec3(imagex, -imagey, zpos);
                (*vertices)[2] = Vec3(imagex, imagey, zpos);
                (*vertices)[3] = Vec3(-imagex, imagey, zpos);
            }
            else
            {
                (*vertices)[0] = Vec3(0, -imagey, zpos);
                (*vertices)[1] = Vec3(2*imagex, -imagey, zpos);
                (*vertices)[2] = Vec3(2*imagex, imagey, zpos);
                (*vertices)[3] = Vec3(0, imagey, zpos);
            }
            osg::ref_ptr<Geometry> thumbnail_geometry = new Geometry();
            thumbnail_geometry->setVertexArray(vertices);

            // Primitive Set
            osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
            poly->push_back(0);
            poly->push_back(1);
            poly->push_back(2);
            poly->push_back(3);
            thumbnail_geometry->addPrimitiveSet(poly);

            // Texture Coordinates
            Vec2Array* texcoord = new Vec2Array;
            float zoomin=1.0;
            float a = (1.0-(1.0/zoomin)) / 2.0;
            float b = 1.0-a;
            texcoord->push_back(osg::Vec2(a, a));
            texcoord->push_back(osg::Vec2(b, a));
            texcoord->push_back(osg::Vec2(b, b));
            texcoord->push_back(osg::Vec2(a, b));
            thumbnail_geometry->setTexCoordArray(0, texcoord);

            StateSet *state = thumbnail_geometry->getOrCreateStateSet();
            state->setTextureAttribute(0, thumbnail_texture);
            state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
            state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
            state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
            state->setMode(GL_BLEND, StateAttribute::ON);


            osg::Vec3Array* normals = new osg::Vec3Array;
            normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
            thumbnail_geometry->setNormalArray(normals);
            thumbnail_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
            osg::ref_ptr<osg::MatrixTransform> slitscan_transform = new osg::MatrixTransform;
            osg::ref_ptr<osg::Geode> slitscan_geode = new osg::Geode;
            slitscan_geode->addDrawable(thumbnail_geometry);
            slitscan_transform->addChild(slitscan_geode);
            slitscan_transforms.push_back(slitscan_transform);
        }
    }
    else{
        osg::ref_ptr<osg::Geometry> slitscan_geometry = this->thumbnailGeometry("Slit-scan");
        osg::ref_ptr<osg::MatrixTransform> slitscan_transform = new osg::MatrixTransform;
        osg::ref_ptr<osg::Geode> slitscan_geode = new osg::Geode;
        slitscan_geode->addDrawable(slitscan_geometry);
        slitscan_transform->addChild(slitscan_geode);
        slitscan_transforms.push_back(slitscan_transform);
    }
    slit_scan_changed = false;
}	

void ACOsgVideoTrackRenderer::framesTransform() {
    double summary_start = getTime();
    std::cout << "Generating frames... ";
    summary_frames_transform = 0;
    summary_frames_transform = new MatrixTransform;
    osg::ref_ptr<osg::Group> summary_frames_group = new Group;
    //summary_frames_group->removeChildren(0,	summary_frames_group->getNumChildren ());
    //summary_frame_n = floor(width/summary_frame_min_width);

    float thumbnail_height = media->getThumbnailHeight("Timeline Resized");
    float thumbnail_width = media->getThumbnailWidth("Timeline Resized");

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

        if(summary_images.size()<f+1){
            osg::ref_ptr<osg::Image> thumbnail = osgDB::readImageFile(resized_thumbnail_filename);
            if(thumbnail == 0){
                std::cerr << "ACOsgVideoTrackRenderer::framesTransform: couldn't load " << resized_thumbnail_filename << std::endl;
                return;
            }
            summary_images.push_back(thumbnail);
        }
        if(summary_textures.size()<f+1)
            summary_textures.push_back(new osg::Texture2D);
        summary_textures[f]->setImage(summary_images[f]);
        if(summary_streams.size()<f+1)
            summary_streams.push_back(dynamic_cast<osg::ImageStream*>(summary_images[f].get()));
        //summary_streams[f]->setLoopingMode(osg::ImageStream::LOOPING);
        summary_streams[f]->setVolume(0);

        summary_streams[f]->play();
        summary_streams[f]->seek( (f/*+0.5f*/)*summary_streams[f]->getLength()/(float)summary_frame_n );
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
        (*vertices)[0] = Vec3(2.0f*g/summary_frame_n/2.0f*this->width, -0.5f, 0.0);
        (*vertices)[1] = Vec3((2.0f*g+2.0f)/summary_frame_n/2.0f*this->width, -0.5f, 0.0);
        (*vertices)[2] = Vec3((2.0f*g+2.0f)/summary_frame_n/2.0f*this->width, 0.5f, 0.0);
        (*vertices)[3] = Vec3(2.0f*g/summary_frame_n/2.0f*this->width, 0.5f, 0.0);
        summary_frame_geometry->setVertexArray(vertices);
        std::cout << "Frame " << f << " begin " << 2.0f*g/summary_frame_n/2.0f*this->width << " end " << (2.0f*g+2.0f)/summary_frame_n/2.0f*this->width << std::endl;

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
            segment_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        else // even segment index
            segment_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        osg::ref_ptr<osg::Vec4Array> segment_colors = new Vec4Array;
        segment_colors->push_back(segment_color);

        std::cout << "Segment start " << media->getSegment(s)->getStart() << " end " << media->getSegment(s)->getEnd() << std::endl;

        segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(/*this->width**/(media->getSegment(s)->getStart()+media->getSegment(s)->getEnd())/2.0f/media_length,0.0f,0.0f),/*this->width**/(media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length,1.0f,0.0f), hints));

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

            resized_thumbnail_filename = media->getThumbnailFileName("Timeline Resized");
            if(resized_thumbnail_filename == "")
                resized_thumbnail_filename = media->getFileName();

            std::cout << "Thumbnail (used)" << resized_thumbnail_filename << std::endl;

            // Access the video stream (of the browser node)
            std::cout << "Getting video stream... ";
            double video_stream_in = getTime();

            ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
            if(!media_thumbnail){
                std::cerr << "ACOsgImageRenderer::imageGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
                return;
            }
            ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
            if(!osg_thumbnail){
                std::cerr << "ACOsgImageRenderer::imageGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
                return;
            }

            video_stream = osg_thumbnail->getStream();
            std::cout << getTime()-video_stream_in << " sec." << std::endl;

            // Optional, for testing the segment visualization without segmentation
            //this->createDummySegments();

            this->slit_scan_changed = true;
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

        if(slit_scan_changed){
            this->slitScanTransform();
            if(slitscan_transforms.size()>0){
                for(std::vector< osg::ref_ptr<osg::MatrixTransform> >::iterator slitscan_transform = slitscan_transforms.begin();slitscan_transform != slitscan_transforms.end();slitscan_transform++)
                    track_node->addChild(*slitscan_transform);
                this->slit_scan_changed = false;
            }
        }

        if (video_stream)
            this->syncVideoStreams();

        this->updateSliderContents();

        this->updateTransformsAspects();
    }

    // Hovering the segment in the browser corresponding to the current playback if there is:
    if(this->media && this->media->getNumberOfSegments() > 0){
        //corresponding_segment_index
        float current_time = this->video_stream->getCurrentTime();
        std::vector<ACMedia*> segments = media->getAllSegments();
        std::vector<ACMedia*>::iterator segment = segments.end();
        for(segment = segments.begin();segment!=segments.end();segment++){
            if( (*segment)->getStart() >= current_time && (*segment)->getEnd() <= current_time )
                break;
        }
        int current_segment_index = -1;
        if(segment!=segments.end()){
            current_segment_index = std::distance(segments.begin(),segment);
        }
        if(corresponding_segment_index !=-1 && current_segment_index != corresponding_segment_index){
            ACMedia* corresponding_segment = media->getSegment(corresponding_segment_index);
            if(corresponding_segment){
                ACMediaNode* node = media_cycle->getNodeFromMedia(corresponding_segment);
                if(node){
                    node->setHover(0);
                    std::cout << "ACOsgVideoTrackRenderer::updateTracks: not hovering segment " << corresponding_segment << " anymore" << std::endl;
                }
            }
        }
        if(current_segment_index !=-1){
            ACMediaNode* node = media_cycle->getNodeFromMedia(*segment);
            if(node){
                node->setHover(1);
                std::cout << "ACOsgVideoTrackRenderer::updateTracks: hovering segment " << current_segment_index << std::endl;
            }
        }
        corresponding_segment_index = current_segment_index;
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

void ACOsgVideoTrackRenderer::pushOnElement(std::string _refname,float _pos_x,float _pos_y){
    if(_refname == "video track selection frames"){
        int frame_clicked = floor((float)(2*selection_frame_n+1)*_pos_x);
        int center_frame = selection_frame_n;
        std::cout << "Frame clicked " << frame_clicked << "/" << 2*selection_frame_n+1 << " center " << center_frame << std::endl;
        if(frame_clicked > center_frame && frame_clicked < 2*selection_frame_n+1 && right_selection_video_streams.size() > frame_clicked-center_frame){
            float frame_pos = right_selection_video_streams[frame_clicked-center_frame-1]->getCurrentTime()/right_selection_video_streams[frame_clicked-center_frame-1]->getLength();
            this->moveSelection(frame_pos,0.0f);
        }
        else if(frame_clicked < center_frame && frame_clicked > 0 && left_selection_video_streams.size() > center_frame-frame_clicked){
            float frame_pos = left_selection_video_streams[center_frame-frame_clicked-1]->getCurrentTime()/left_selection_video_streams[center_frame-frame_clicked-1]->getLength();
            this->moveSelection(frame_pos,0.0f);
        }
        //this->moveSelection(this->selection_center_pos_x + this->getSelectionWidth()* (float)(frame_clicked-selection_frame_n-0.5f)/(float)(2*selection_frame_n+1), 0.0f);
    }
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
    if (track_summary_type == "Keyframes"){
        if (summary_frame_n != floor(width/summary_frame_min_width) || media_changed){
            track_summary_type_changed = true;
            summary_frame_n = floor(width/summary_frame_min_width);
            framesTransform();
            std::cout << "summary_frame_n " << summary_frame_n << std::endl;
        }
        else{
            for (int f=0;f<summary_frame_n;f++){
                summary_streams[f]->seek( (f/*+0.5f*/)*summary_streams[f]->getLength()/(float)summary_frame_n );
                summary_streams[f]->pause();
                summary_streams[f]->quit();
            }
        }
    }

//    if (track_summary_type == "Slit-scan" && slit_scan_changed)
//        track_summary_type_changed = true;

    if (track_summary_type_changed){
        //std::cout << "summary_type " << track_summary_type << std::endl;
        summary_transform->removeChildren(0,summary_transform->getNumChildren());
        if (track_summary_type == "Keyframes"){
            summary_transform->addChild(summary_frames_transform);
        }
        //        else if (track_summary_type == "Slit-scan"){
        //            if (slit_scanner->computed()){
        //                if(slit_scan_changed){
        //                    slitScanTransform();
        //                    slit_scan_changed = false;
        //                }
        //                summary_transform->addChild(slit_scan_transform);
        //            }
        //        }
    }

    float w = (float)(media->getWidth());
    float h = (float)(media->getHeight());
    if (track_summary_type == "Keyframes")
        summary_height = h/height * width/w/summary_frame_n;// summary_frame_n = width/summary_frame_min_width
    else
        summary_height = 1.0f/8.0f;
}	

void ACOsgVideoTrackRenderer::updateSelectionContents()
{
    float w = (float)(media->getWidth());
    float h = (float)(media->getHeight());
    //selection_height = 1.0f/8.0f; //selection_height = summary_height * 2.0f;//makes a fixed-heigh proportion independent of the timeline height variation
    selection_center_frame_width = selection_height * (w/width)/(h/height);

    // Update the selection contents
    if(track_selection_type == "Keyframes"){

        //int _selection_frame_n = floor(1.0f/selection_center_frame_width/2.0f;
        int _selection_frame_n = floor((width/selection_frame_min_width)/2.0f);
        if ((selection_frame_n != _selection_frame_n) || media_changed){
            track_selection_type_changed = true;
            selection_frame_n = _selection_frame_n;
            //std::cout << "selection_frame_n " << selection_frame_n << std::endl;

            selection_min_width = 1.0f * selection_frame_n/(media->getFrameRate()*media->getDuration());
            std::cout << "ACOsgVideoTrackRenderer::updateSelectionContents: selection min width " << selection_min_width << std::endl;

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
        selection_height = h/height * width/w/(2*selection_frame_n+1);
    }

    if (track_selection_type_changed){
        //std::cout << "selection_type " << track_selection_type << std::endl;
        selection_transform->removeChildren(0,selection_transform->getNumChildren());
        if(track_selection_type == "Keyframes"){
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

        float cursor_pos_normd = selection_center_pos_x;//(selection_center_pos_x/0.5f+1)/2.0f;
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
                float position_norm = (selection_center_pos_x + ++right_skip/(float)(right_selection_video_streams.size())*(selection_end_pos_x-selection_begin_pos_x)/2.0f );
                if(position_norm > 1){
                    position_norm -= 1;//looping
                }
                (*_stream)->seek( position_norm *video_stream->getLength());
#else
                // Sync without time skip
                if((*_stream).valid()){
                    (*_stream)->seek( selection_center_pos_x*video_stream->getLength());
                }
#endif
            }
        }
        int left_skip(0);
        for (std::vector< osg::ref_ptr<osg::ImageStream> >::iterator _stream = left_selection_video_streams.begin(); _stream != left_selection_video_streams.end();_stream++){
            if((*_stream).valid()){
#ifndef TEST_SYNC_WITHOUT_TIME_SKIP
                // Time skip so that the leftest frame corresponds to the left selection cursor (or looping from the end if before the first frame)
                float position_norm = (selection_center_pos_x +  --left_skip/(float)(left_selection_video_streams.size())*(selection_end_pos_x-selection_begin_pos_x)/2.0f );
                if(position_norm < 0){
                    position_norm += 1;//looping
                }
                (*_stream)->seek( position_norm *video_stream->getLength());
#else
                // Sync without time skip
                if((*_stream).valid()){
                    (*_stream)->seek( selection_center_pos_x*video_stream->getLength());
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
        if (streamStatus == osg::ImageStream::PLAYING && !this->manual_selection){
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
            this->moveSelection(/*-0.5f+*/video_stream->getReferenceTime()/video_stream->getLength(),this->getSelectionPosY()); // update the visual cursor from the stream itself
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
    if(track_selection_type != "None"){
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
    Matrix summary_cursor_matrix,selection_begin_matrix,selection_end_matrix,selection_zone_matrix;
    std::vector< Matrix > left_selection_videos_matrices,right_selection_videos_matrices;

    float _segments_height = 0.0f;
    float _segments_center_y = 0.0f;
    if (media->getNumberOfSegments()>0){
        _segments_height = this->segments_height;
        _segments_center_y = _segments_height/2.0f;
//        segments_matrix.makeTranslate(this->width/2.0f,0.0f/*_segments_center_y*this->height*/,0.0f);
//        segments_matrix = Matrix::scale(this->width,1.0f/*_segments_height*this->height*/,1.0f)*segments_matrix;
//        segments_transform->setMatrix(segments_matrix);
        segments_matrix.makeTranslate(0.0f,_segments_center_y*this->height,0.0f);
        segments_matrix = Matrix::scale(this->width,_segments_height*this->height,1.0f)*segments_matrix;
        segments_transform->setMatrix(segments_matrix);
    }

    float _slitscan_height = 0.0f;
    float _slitscan_center_y = 0.0f;
    if(slitscan_transforms.size()>0){
        _slitscan_height = this->slitscan_height;
        _slitscan_center_y = _segments_height + _slitscan_height/2.0f;

        ACMediaThumbnail* slitscan_thumbnail = media->getThumbnail("Slit-scan");
        if(slitscan_thumbnail!=0){
            if(slitscan_thumbnail->getNumberOfSegments()>1){
                std::vector<ACMediaThumbnail*> thumbnail_segments = slitscan_thumbnail->getAllSegments();
//                    std::vector< Matrix > slit_scan_matrices;
////                for(std::vector< osg::ref_ptr<osg::MatrixTransform> >::iterator slitscan_transform = slitscan_transforms.begin();slitscan_transform != slitscan_transforms.end();slitscan_transform++){
////                    slit_scan_matrices.push_back(Matrix::translate());
////                    (*slitscan_transform)->setMatrix(slit_scan_matrices.back());
////                }
            }
            else if(slitscan_thumbnail->getNumberOfSegments()==0){
                Matrix slit_scan_matrix;
                float slit_scan_height = slitscan_thumbnail->getHeight();
                float slit_scan_width = slitscan_thumbnail->getWidth();
                slit_scan_matrix.makeTranslate(0.0f,_slitscan_center_y*this->height,0.0f);
                slit_scan_matrix = Matrix::scale(this->width/slit_scan_width,_slitscan_height*this->height/slit_scan_height,1.0f)*slit_scan_matrix;
                slitscan_transforms.back()->setMatrix(slit_scan_matrix);
            }
        }
    }

    float _summary_height = 0.0f;
    float _summary_center_y = 0.0f;
    if (track_summary_type != "None"){
        _summary_height = this->summary_height;
        _summary_center_y = _segments_height + _slitscan_height + _summary_height/2.0f;
        //summary_matrix.makeTranslate(0.0f,(_segments_height+_summary_height/2.0f)*this->height,0.0f);
        summary_matrix.makeTranslate(0.0f,(_summary_center_y)*this->height,0.0f);
        summary_matrix = Matrix::scale(1.0f,_summary_height*this->height,1.0f)*summary_matrix;
        summary_transform->setMatrix(summary_matrix);
    }

    float _selection_center_y = (_segments_height + _slitscan_height + _summary_height)/2.0f;
    float _selection_height = _segments_height + _slitscan_height + _summary_height;
    summary_cursor_matrix.makeTranslate((selection_center_pos_x/*+0.5*/)*this->width,_selection_center_y*this->height,0.0f);
    summary_cursor_matrix = Matrix::scale(1.0f,_selection_height*this->height,1.0f)*summary_cursor_matrix;
    if(summary_cursor_transform)
        summary_cursor_transform->setMatrix(summary_cursor_matrix);
    selection_begin_matrix.makeTranslate((selection_begin_pos_x/*+0.5*/)*this->width,_selection_center_y*this->height,0.0f);
    selection_begin_matrix = Matrix::scale(1.0f,_selection_height*this->height,1.0f)*selection_begin_matrix;
    if(selection_begin_transform)
        selection_begin_transform->setMatrix(selection_begin_matrix);
    selection_end_matrix.makeTranslate((selection_end_pos_x/*+0.5*/)*this->width,_selection_center_y*this->height,0.0f);
    selection_end_matrix = Matrix::scale(1.0f,_selection_height*this->height,1.0f)*selection_end_matrix;
    if(selection_end_transform)
        selection_end_transform->setMatrix(selection_end_matrix);
    selection_zone_matrix.makeTranslate((selection_begin_pos_x+selection_end_pos_x/*+1*/)/2.0f*this->width,_selection_center_y*this->height,0.0f);
    selection_zone_matrix = Matrix::scale((selection_end_pos_x-selection_begin_pos_x)*this->width,_selection_height*this->height,1.0f)*selection_zone_matrix;
    if(selection_zone_transform)
        selection_zone_transform->setMatrix(selection_zone_matrix);

    float _selection_frames_height = 0.0f;
    float _selection_frames_center_y = 0.0f;
    if (track_selection_type != "None"){
        _selection_frames_height = this->selection_height;
        _selection_frames_center_y = _segments_height + _slitscan_height + _summary_height + _selection_frames_height/2.0f;

        selection_center_frame_matrix.makeTranslate(0.5f*this->width,_selection_frames_center_y*this->height,0.0f);
        selection_center_frame_matrix = Matrix::scale(selection_center_frame_width*this->width,_selection_frames_height*this->height,1.0f)*selection_center_frame_matrix;
        for (unsigned int m=0;m<right_selection_video_transforms.size();++m){
            right_selection_videos_matrices.resize(m+1);
            right_selection_videos_matrices[m].makeTranslate((0.5f+selection_center_frame_width*(m+1))*this->width,_selection_frames_center_y*this->height,0.0f);
            right_selection_videos_matrices[m] = Matrix::scale(selection_center_frame_width*this->width,_selection_frames_height*this->height,1.0f)*right_selection_videos_matrices[m];
        }
        for (unsigned int m=0;m<left_selection_video_transforms.size();++m){
            left_selection_videos_matrices.resize(m+1);
            left_selection_videos_matrices[m].makeTranslate((0.5f-selection_center_frame_width*(m+1))*this->width,_selection_frames_center_y*this->height,0.0f);
            left_selection_videos_matrices[m] = Matrix::scale(selection_center_frame_width*this->width,_selection_frames_height*this->height,1.0f)*left_selection_videos_matrices[m];
        }
        if(track_selection_type == "Keyframes"){
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
    }

    if(track_playback_visibility){
        playback_height = 1.0f-_segments_height-_summary_height-_slitscan_height-_selection_frames_height;
        if (w/h*playback_height*height/width<1.0f){ // video fits view height
            playback_center_y = _segments_height + _summary_height + _slitscan_height + _selection_frames_height + 0.5f* playback_height;
            playback_matrix.makeTranslate(0.5f*this->width,playback_center_y*this->height,0.0f);
            playback_matrix = Matrix::scale(playback_height*this->height/h,playback_height*this->height/h,1.0f)*playback_matrix;
            //std::cout << "video fits view height" << std::endl;
        }
        else{ // video fits view width
            playback_height = width/height/w*h; // computed from the previous if condition blocked to 1.0f
            playback_center_y = _segments_height + _summary_height + _slitscan_height + _selection_frames_height + 0.5f* playback_height;
            playback_matrix.makeTranslate(0.5f*this->width,playback_center_y*this->height,0.0f);
            playback_matrix = Matrix::scale(this->width/w,this->width/w,1.0f)*playback_matrix;
            //std::cout << "video fits view width" << std::endl;
        }
        playback_transform->setMatrix(playback_matrix);
    }
}
