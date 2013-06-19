/**
 * @brief The media browser node renderer base class, implemented with OSG
 * @author Stéphane Dupont
 * @date 24/08/09
 * @copyright (c) 2009 – UMONS - Numediart
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

#include "ACOsgMediaRenderer.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
//#include "boost/filesystem/operations.hpp"
//#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include <sstream>

using namespace osg;

ACOsgMediaRenderer::ACOsgMediaRenderer() : ACOsgBaseRenderer() {
    label = "";
    local_group = new Group();
#ifdef AUTO_TRANSFORM
    media_node = new AutoTransform();
#else
    media_node = new MatrixTransform();
#endif
    node_thumbnail = "";
    node_color = Vec4(1,1,0.5,1); //CF seminal yellow
    cluster_colors.push_back(osg::Vec4(1,1,0.5,1));//yello<
    cluster_colors.push_back(osg::Vec4(1,0.5,1,1));//
    //cluster_colors.push_back(osg::Vec4(0.5,1,0.5,1));//green
    cluster_colors.push_back(osg::Vec4(0.1,0.8,0.4,1));//green
    cluster_colors.push_back(osg::Vec4(0.5,1,1,1));
    cluster_colors.push_back(osg::Vec4(1,0.5,0.5,1));//red
    cluster_colors.push_back(osg::Vec4(0.8,0.5,0.1,1));
    cluster_colors.push_back(osg::Vec4(0.5,0.1,0.8,1));
    cluster_colors.push_back(osg::Vec4(0.7,0.7,0.7,1));
    cluster_colors.push_back(osg::Vec4(0.1,0.1,0.1,1));
    neighbor_color = Vec4(1,1,1,1);
    user_defined_color = false;

    metadata_geode = 0;
    metadata = 0;

    // SD - Results from centralized request to MediaCycle - GLOBAL
    media_cycle_deltatime = 0.0;
    media_cycle_zoom = 0.0f;
    media_cycle_angle = 0.0f;
    media_cycle_mode = 0;
    media_cycle_global_navigation_level = 0;

    // SD - Results from centralized request to MediaCycle - NODE SPECIFIC
    media_cycle_isdisplayed = false;
    media_cycle_isTagged=false;
    media_cycle_current_pos.x = 0;
    media_cycle_current_pos.y = 0;
    media_cycle_current_pos.z = 0;
    media_cycle_view_pos.x = 0;
    media_cycle_view_pos.y = 0;
    media_cycle_view_pos.z = 0;
    media_cycle_next_pos.x = 0;
    media_cycle_next_pos.y = 0;
    media_cycle_next_pos.z = 0;
    media_cycle_navigation_level = 0;
    media_cycle_activity = 0;
    media_cycle_filename = "";
    media_index = -1;
    cluster_index = -1;
    prev_media_index = -1;

#if defined(APPLE_IOS)
    afac = 0.5;
#else
    afac = 1.0;
#endif
    initialized = 0;
    frac = 0.0f;
    local_group->addChild(media_node);

    this->media_node->setDataVariance( osg::Object::DYNAMIC );
    this->media_node->setUpdateCallback( new ACOsgMediaNodeCallback(this) );
}

ACOsgMediaRenderer::~ACOsgMediaRenderer() {
    //this->media_node->removeUpdateCallback( this );
    //this->media_node->removeNestedCallback( this );
    //this->local_group->removeChild(this->media_node);
    //this->media_node->setUpdateCallback(new osg::NodeCallback());
    media_node = 0;
    // label = "";
    cluster_colors.clear();
    metadata_geode = 0;
    metadata = 0;
    //local_group=0;
    cluster_colors.clear();
}

void ACOsgMediaNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv ){

    // CF normally we would have had to do the following if media_node was accessible from the callback class (shared with this class)
    //#ifdef AUTO_TRANSFORM
    //    osg::AutoTransform* media_node = static_cast<osg::AutoTransform*>( node );
    //#else
    //    osg::MatrixTransform* media_node = static_cast<osg::MatrixTransform*>( node );
    //#endif

    if(renderer)
        renderer->updateNodes();

    /// osg::NodeCallback::traverse() needs be executed to ensure that the update traversal visitor can continue traversing the scene graph.
    traverse( node, nv );
}

void ACOsgMediaRenderer::metadataGeode() {

//    osg::Vec4 textColor(0.9f,0.9f,0.9f,1.0f);
    osg::Vec4 textColor(1.0f,1.0f,1.0f,1.0f);

    float textCharacterSize = 20.0f;
    if(this->setting == AC_SETTING_INSTALLATION)
        textCharacterSize = 46.0f;

    metadata_geode = new Geode();

    metadata = new osgText::Text;
    if(font)
        metadata->setFont(font);
    
    metadata->setColor(textColor);
    metadata->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    metadata->setCharacterSize(textCharacterSize);
    metadata->setPosition(osg::Vec3(0,0.025,0.04));
    
    //	text->setPosition(osg::Vec3(pos.x,pos.y,pos.z));
    metadata->setLayout(osgText::Text::LEFT_TO_RIGHT);
    metadata->setFontResolution(textCharacterSize,textCharacterSize);
    //metadata->setAlignment( osgText::Text::CENTER_CENTER );
    //metadata->setAxisAlignment( osgText::Text::SCREEN );

    metadata->setDrawMode(osgText::Text::TEXT);// osgText::Text::BOUNDINGBOX, osgText::Text::ALIGNMENT

    std::stringstream content;
    content << fs::basename(media->getFileName());
    //CPL
    if (media->getParentId()>-1)// in case of segments
        content << "(segment with media ID" << media->getId() << ")";
    if(media->getTaggedClassId()>-1){
        content << " - Tag: " << media->getTaggedClassId();//->getLabel();
    }
    //std::cout<<content.str()<<std::endl;
    metadata->setText( content.str() );

    //state = text_geode->getOrCreateStateSet();
    //state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    //state->setMode(GL_BLEND, StateAttribute::ON);
    //state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);

    metadata_geode->addDrawable(metadata);
}

void ACOsgMediaRenderer::setDeltaTime(double media_cycle_deltatime) {
    this->media_cycle_deltatime = media_cycle_deltatime;
}

void ACOsgMediaRenderer::setZoomAngle(float media_cycle_zoom, float media_cycle_angle) {
    this->media_cycle_zoom = media_cycle_zoom;
    this->media_cycle_angle = media_cycle_angle;
}

void ACOsgMediaRenderer::setMode(int media_cycle_mode) {
    this->media_cycle_mode = media_cycle_mode;
}

void ACOsgMediaRenderer::setGlobalNavigation(int media_cycle_global_navigation_level) {
    this->media_cycle_global_navigation_level = media_cycle_global_navigation_level;
}

void ACOsgMediaRenderer::setIsDisplayed(int media_cycle_isdisplayed) {
    this->media_cycle_isdisplayed = media_cycle_isdisplayed;
}
void ACOsgMediaRenderer::setIsTagged(int media_cycle_isTagged) {
    this->media_cycle_isTagged = media_cycle_isTagged;
}

/*
void ACOsgMediaRenderer::setPos(ACPoint &media_cycle_current_pos, ACPoint &media_cycle_next_pos) {
 this->media_cycle_current_pos = media_cycle_current_pos;
 this->media_cycle_next_pos = media_cycle_next_pos;
}
*/

void ACOsgMediaRenderer::setViewPos(ACPoint media_cycle_view_pos) {
    this->media_cycle_view_pos = media_cycle_view_pos;
}

void ACOsgMediaRenderer::setCurrentPos(ACPoint media_cycle_current_pos) {
    this->media_cycle_current_pos = media_cycle_current_pos;
    initialized = 1;
}

void ACOsgMediaRenderer::setNextPos(ACPoint media_cycle_next_pos) {
    this->media_cycle_next_pos = media_cycle_next_pos;
}

ACPoint ACOsgMediaRenderer::getViewPos() {
    return this->media_cycle_view_pos;
}

ACPoint ACOsgMediaRenderer::getCurrentPos() {
    return this->media_cycle_current_pos;
}

ACPoint ACOsgMediaRenderer::getNextPos() {
    return this->media_cycle_next_pos;
}

void ACOsgMediaRenderer::setNavigation(int media_cycle_navigation_level) {
    this->media_cycle_navigation_level = media_cycle_navigation_level;
}

void ACOsgMediaRenderer::setActivity(int media_cycle_activity) {
    this->media_cycle_activity = media_cycle_activity;
}

/*void ACOsgMediaRenderer::setMediaIndex(int media_index) {
 this->media_index = media_index;
}*/

ACOsgMediaThumbnail* ACOsgMediaRenderer::getNodeThumbnail()
{
    if(this->media == 0){
        std::cerr << "ACOsgBaseRenderer::getNodeThumbnail: no media set" << std::endl;
        return 0;
    }
    if(this->node_thumbnail == ""){
        std::cerr << "ACOsgBaseRenderer::getNodeThumbnail: no node thumbnail name set" << std::endl;
        return 0;
    }
    if(this->media->getThumbnail(this->node_thumbnail)==0){
        std::cerr << "ACOsgBaseRenderer::getNodeThumbnail: no node thumbnail available" << std::endl;
        return 0;
    }
    ACOsgMediaThumbnail* thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(this->media->getThumbnail(this->node_thumbnail));
    if(thumbnail == 0){
        std::cerr << "ACOsgBaseRenderer::getNodeThumbnail: node thumbnail isn't an osg media thumbnail" << std::endl;
        return 0;
    }
    return thumbnail;
}

osg::ref_ptr<osg::Image> ACOsgMediaRenderer::getNodeThumbnailImage(){
    ACOsgMediaThumbnail* thumbnail = this->getNodeThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getImage();
}

osg::ref_ptr<osg::Texture2D> ACOsgMediaRenderer::getNodeThumbnailTexture()
{
    ACOsgMediaThumbnail* thumbnail = this->getNodeThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getTexture();
}

osg::ref_ptr<osg::ImageStream> ACOsgMediaRenderer::getNodeThumbnailStream()
{
    ACOsgMediaThumbnail* thumbnail = this->getNodeThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getStream();
}
