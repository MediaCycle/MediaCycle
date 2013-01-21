/**
 * @brief The media track renderer base class, implemented with OSG
 * @author Christian Frisson
 * @date 28/04/2010
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

#include "ACOsgTrackRenderer.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

using namespace osg;

ACOsgTrackRenderer::ACOsgTrackRenderer()
    : ACOsgBaseRenderer(),track_index(-1),media_index(-1),media_from_lib(true),
      screen_width(0.0f),width(0.0f),height(0.0f),screen_width_changed(false),width_changed(false),height_changed(false)
{
    media_type = MEDIA_TYPE_NONE;
    track_node = new MatrixTransform();
    displayed_media_index = -1;
    manual_selection = false;
    this->initSelection();
}

void ACOsgTrackRenderer::initSelection()
{
    selection_min_width = 0.10f;
    selection_center_pos_x = 0;
    selection_begin_pos_x = selection_center_pos_x - selection_min_width;
    selection_end_pos_x = selection_center_pos_x + selection_min_width;
    selection_center_pos_y = 0.5f;
    selection_begin_pos_y = selection_center_pos_y;
    selection_end_pos_y = selection_center_pos_y;
    selection_begin_pos_changed = true;
    selection_end_pos_changed = true;
    selection_center_pos_changed = true;
}

void ACOsgTrackRenderer::updateMedia(ACMedia* _media)
{
    media = _media;
    media_index = -1;
    media_from_lib = false;
    media_changed = true;
    this->initSelection();
}

/*void ACOsgTrackRenderer::updateMedia(int _media_index)
{
 media = media_cycle->getLibrary()->getMedia(_media_index);
 media_index = _media_index;
 media_from_lib = true;
 media_changed = true;
 this->initSelection();
}*/

void ACOsgTrackRenderer::clearMedia()
{
    media = 0;
    media_index = -1;
    media_from_lib = true;
    media_changed = true;
    this->initSelection();
}

void ACOsgTrackRenderer::updateScreenWidth(int _screen_width)
{
    if ( screen_width != _screen_width){
        this->screen_width = _screen_width;
        screen_width_changed = true;
        this->updateTracks();
        screen_width_changed = false;
    }
}

void ACOsgTrackRenderer::updateSize(int _width,float _height)
{
    if (( height != _height) || ( width != _width)){
        this->height = _height;
        this->width = _width;
        height_changed = true;
        width_changed = true;
        this->updateTracks();
        height_changed = false;
        width_changed = false;
    }
}

void ACOsgTrackRenderer::resizeSelectionFromBegin(float _begin_x,float _begin_y)
{
    if(_begin_x > this->selection_center_pos_x - selection_min_width/2.0f)
        return;

    float _extent_x = _begin_x - selection_begin_pos_x;
    this->selection_begin_pos_x = _begin_x;
    this->selection_end_pos_x -= _extent_x;
    float _extent_y = _begin_y - selection_begin_pos_y;
    this->selection_begin_pos_y = _begin_y;
    this->selection_end_pos_y -= _extent_y;
    selection_begin_pos_changed=true;
    selection_end_pos_changed=true;
}

void ACOsgTrackRenderer::resizeSelectionFromEnd(float _end_x,float _end_y)
{
    if(_end_x < this->selection_center_pos_x + selection_min_width/2.0f)
        return;

    float _extent_x = _end_x - selection_end_pos_x;
    this->selection_end_pos_x = _end_x;
    this->selection_begin_pos_x -= _extent_x;
    float _extent_y = _end_y - selection_end_pos_y;
    this->selection_end_pos_y = _end_y;
    this->selection_begin_pos_y -= _extent_y;
    selection_begin_pos_changed=true;
    selection_end_pos_changed=true;
}

void ACOsgTrackRenderer::resizeSelectionWidth(float _width){
    if(_width >= 1.0f)
        _width = 1.0f;
    if(_width <= selection_min_width)
        _width = selection_min_width;

    float current_width = this->selection_end_pos_x - this->selection_begin_pos_x;
    float _extent_x = (_width - current_width)/2.0f;
    this->selection_end_pos_x += _extent_x;
    this->selection_begin_pos_x -= _extent_x;
    selection_begin_pos_changed=true;
    selection_end_pos_changed=true;
}

void ACOsgTrackRenderer::moveSelection(float _center_x,float _center_y)
{
    float _extent_x = _center_x - selection_center_pos_x;
    this->selection_begin_pos_x += _extent_x;
    this->selection_center_pos_x += _extent_x;
    this->selection_end_pos_x += _extent_x;
    float _extent_y = _center_y - selection_center_pos_y;
    this->selection_begin_pos_y += _extent_y;
    this->selection_center_pos_y += _extent_y;
    this->selection_end_pos_y += _extent_y;
    selection_begin_pos_changed=true;
    selection_end_pos_changed=true;
    selection_center_pos_changed=true;
}

bool ACOsgTrackRenderer::skipToNextSegment(){
    if(this->media == 0){
        std::cerr << "ACOsgTrackRenderer::skipToNextSegment: no media set" << std::endl;
        return false;
    }
    if(this->media->getNumberOfSegments() == 0){
        std::cerr << "ACOsgTrackRenderer::skipToNextSegment: media has no segments" << std::endl;
        return false;
    }
    float media_duration = media->getDuration();
    float current_time = this->selection_center_pos_x * media_duration;
    std::vector<ACMedia*> segments = this->media->getAllSegments();
    std::vector<ACMedia*>::iterator segment = segments.end();
    for(segment = segments.begin();segment!=segments.end();segment++){
        if( (*segment)->getStart() <= current_time && (*segment)->getEnd() >= current_time )
            break;
    }
    if(segment == segments.end()){
        std::cerr << "ACOsgTrackRenderer::skipToNextSegment: couldn't find the segment corresponding to the playback" << std::endl;
        return false;
    }
    int current_segment_number = std::distance(segments.begin(),segment);
    std::cout << "ACOsgTrackRenderer::skipToNextSegment: current segment corresponding to the playback time " << current_time << " is number " << current_segment_number << "/" << segments.size() << std::endl;

    segment++;
    if(segment == segments.end())
        segment = segments.begin();

    float next_time = (*segment)->getStart();
    float next_selection = next_time / media_duration;

    std::cout << "ACOsgTrackRenderer::skipToNextSegment: moving selection from " << current_time << " (" << this->selection_center_pos_x << "%) to " << next_time << " (" << next_selection << "%)" << std::endl;
    //this->manual_selection = true;
    this->moveSelection(next_selection,0.0f);
    return true;
}

bool ACOsgTrackRenderer::skipToPreviousSegment(){
    if(this->media == 0){
        std::cerr << "ACOsgTrackRenderer::skipToPreviousSegment: no media set" << std::endl;
        return false;
    }
    if(this->media->getNumberOfSegments() == 0){
        std::cerr << "ACOsgTrackRenderer::skipToPreviousSegment: media has no segments" << std::endl;
        return false;
    }
    float media_duration = media->getDuration();
    float current_time = this->selection_center_pos_x * media_duration;
    std::vector<ACMedia*> segments = this->media->getAllSegments();
    std::vector<ACMedia*>::iterator segment = segments.end();
    for(segment = segments.begin();segment!=segments.end();segment++){
        if( (*segment)->getStart() <= current_time && (*segment)->getEnd() >= current_time )
            break;
    }
    if(segment == segments.end()){
        std::cerr << "ACOsgTrackRenderer::skipToPreviousSegment: couldn't find the segment corresponding to the playback" << std::endl;
        return false;
    }
    int current_segment_number = std::distance(segments.begin(),segment);
    std::cout << "ACOsgTrackRenderer::skipToPreviousSegment: current segment corresponding to the playback time " << current_time << " is number " << current_segment_number << "/" << segments.size() << std::endl;

    if(segment == segments.begin())
        segment = segments.end();
    segment--;

    float next_time = (*segment)->getStart();
    float next_selection = next_time / media_duration;

    std::cout << "ACOsgTrackRenderer::skipToPreviousSegment: moving selection from " << current_time << " (" << this->selection_center_pos_x << "%) to " << next_time << " (" << next_selection << "%)" << std::endl;
    //this->manual_selection = true;
    this->moveSelection(next_selection,0.0f);
    return true;
}

void ACOsgTrackRenderer::createDummySegments()
{
    if(media){
        if (media->getNumberOfSegments()==0){
            //std::cout << "Dummy segments" << std::endl;
            for (unsigned int s=0;s<4;s++){
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
        //else
        //	std::cout << media->getNumberOfSegments() << " segments" << std::endl;
    }
}

void ACOsgTrackRenderer::boxTransform(osg::ref_ptr<osg::MatrixTransform>& _transform, float _width, osg::Vec4 _color, std::string _name) {
    osg::StateSet *state;
    osg::Vec3Array* vertices;
    osg::ref_ptr<DrawElementsUInt> line_p;
    osg::ref_ptr<osg::Geometry> _geometry = new osg::Geometry();
    osg::ref_ptr<osg::Geode> _geode = new osg::Geode();

    _transform = 0;
    _transform = new osg::MatrixTransform();

    vertices = new Vec3Array(4);
    (*vertices)[0] = Vec3(-_width/2.0f, -0.5f, 0.0f);
    (*vertices)[1] = Vec3(_width/2.0f, -0.5f, 0.0f);
    (*vertices)[2] = Vec3(_width/2.0f, 0.5f, 0.0f);
    (*vertices)[3] = Vec3(-_width/2.0f, 0.5f, 0.0f);
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
