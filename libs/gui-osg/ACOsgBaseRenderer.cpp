/**
 * @brief Base media renderer, for browser and timeline
 * @author Christian Frisson
 * @date 20/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACOsgBaseRenderer.h"

#include <iostream>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Tessellator>
#include <osgUtil/Optimizer>
#include <osgUtil/Simplifier>
#include <osgUtil/TriStripVisitor>

#include <osg/Image>
#include <osg/Texture2D>

//USE_OSGPLUGIN(SVG)

using namespace osg;

ACOsgBaseRenderer::ACOsgBaseRenderer() {
    media_cycle = 0;
    media_type = MEDIA_TYPE_NONE;
    setting = AC_SETTING_NONE;
    media = 0;
    media_changed = false;
    media_cycle_filename = "";
    shared_thumbnail = "";
    font = 0;
}

ACOsgBaseRenderer::~ACOsgBaseRenderer() {
    media_cycle = 0;
    media = 0;// the core will free media files
}

void ACOsgBaseRenderer::setMedia(ACMedia* _media) {
    this->media = _media;
    media_changed = true;
}

void ACOsgBaseRenderer::setFilename(std::string media_cycle_filename) {
    this->media_cycle_filename = media_cycle_filename;
}

osg::ref_ptr<osg::Geometry> ACOsgBaseRenderer::thumbnailGeometry(std::string _thumbnail_name) {
    //osg::ref_ptr<osg::Geode> geode = 0;
    if(_thumbnail_name=="" || _thumbnail_name=="None"){
        //std::cout << "ACOsgBaseRenderer::thumbnailGeometry: no thumbnails name specified" << std::endl;
        return 0;
    }
    if(!this->media){
        //std::cout << "ACOsgBaseRenderer::thumbnailGeometry: can't access thumbnails, media not set" << std::endl;
        return 0;
    }
    std::string thumbnail_filename = media->getThumbnailFileName(_thumbnail_name);
    if(thumbnail_filename==""){
        //std::cout << "ACOsgBaseRenderer::thumbnailGeometry: can't access thumbnail" << std::endl;
        return 0;
    }
    ACMediaThumbnail* media_thumbnail = media->getThumbnail(_thumbnail_name);
    if(!media_thumbnail){
        //std::cout << "ACOsgBaseRenderer::thumbnailGeometry: can't access media thumbnail" << std::endl;
        return 0;
    }
    std::string extension = boost::filesystem::extension(thumbnail_filename).substr(1);
    bool transparent = false;
    if(extension == "png" || extension == "svg")
        transparent = true;

    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(extension);
    if (!readerWriter){
       // cerr << "ACOsgBaseRenderer::thumbnailGeometry: can't find an OSG plugin to read file '" << thumbnail_filename << "'" << endl;
        return 0;
    }
    else{
       // cout <<"ACOsgBaseRenderer::thumbnailGeometry: using OSG plugin: "<< readerWriter->className() <<std::endl;
    }

    osg::ref_ptr<osg::Image> thumbnail_image = osgDB::readImageFile(thumbnail_filename);
    if (!thumbnail_image){
        //cerr << "<ACOsgBaseRenderer::thumbnailGeometry: problem loading thumbnail" << endl;
        return 0;
    }
    osg::ref_ptr<osg::Texture2D> thumbnail_texture = new osg::Texture2D;

    thumbnail_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.

    thumbnail_texture->setImage(thumbnail_image);
    //thumbnail_texture->setResizeNonPowerOfTwoHint(false);

    int width = media->getThumbnail(_thumbnail_name)->getWidth();
    int height = media->getThumbnail(_thumbnail_name)->getHeight();
    //std::cout << "ACOsgBaseRenderer::thumbnailGeometry: geode with (thumbnail) width " <<  width << " and height " << height << std::endl;

    // image vertices
    float zpos = 0;// 0.02;
    double imagex, imagey;
    imagex = 0.5*width;
    imagey = 0.5*height;
    Vec3Array* vertices = new Vec3Array(4);
    if(media_thumbnail->isCircular()){
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
    if(transparent){
        state->setMode(GL_BLEND, StateAttribute::ON);
    }
    else{
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
    }

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    thumbnail_geometry->setNormalArray(normals);
    thumbnail_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

    //    osgUtil::Optimizer optimizer;
    //    optimizer.optimize(thumbnail_geometry);

    //    osgUtil::Tessellator tessellator;
    //        tessellator.retessellatePolygons( *thumbnail_geometry );

    if(transparent)
        osgUtil::SmoothingVisitor::smooth( * thumbnail_geometry );

    //    osgUtil::Simplifier simplifier;
    //    simplifier.setSampleRatio( 0.1 );
    //    simplifier.simplify(* thumbnail_geometry);

    //            osgUtil::TriStripVisitor tsv;
    //               tsv. stripify( *thumbnail_geometry );

    if(!transparent){
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
    }
    thumbnail_geometry->setColorBinding(Geometry::BIND_OVERALL);


    //state->setRenderingHint(osg::StateSet::OPAQUE_BIN );

    return thumbnail_geometry;
}

osg::ref_ptr<osg::Geometry> ACOsgBaseRenderer::imageGeometry(std::string thumbnail_filename) {
    //osg::ref_ptr<osg::Geode> geode = 0;
    if(thumbnail_filename==""){
        std::cout << "ACOsgBaseRenderer::thumbnailGeometry: no thumbnails name specified" << std::endl;
        return 0;
    }
    std::string extension = boost::filesystem::extension(thumbnail_filename).substr(1);
    bool transparent = false;
    if(extension == "png" || extension == "svg")
        transparent = true;

    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(extension);
    if (!readerWriter){
        cerr << "ACOsgBaseRenderer::thumbnailGeometry: can't find an OSG plugin to read file '" << thumbnail_filename << "'" << endl;
        return 0;
    }
    else{
        cout <<"ACOsgBaseRenderer::thumbnailGeometry: using OSG plugin: "<< readerWriter->className() <<std::endl;
    }

    osg::ref_ptr<osg::Image> thumbnail_image = osgDB::readImageFile(thumbnail_filename);
    if (!thumbnail_image){
        cerr << "<ACOsgBaseRenderer::thumbnailGeometry: problem loading thumbnail" << endl;
        return 0;
    }
    osg::ref_ptr<osg::Texture2D> thumbnail_texture = new osg::Texture2D;

    thumbnail_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.

    thumbnail_texture->setImage(thumbnail_image);
    //thumbnail_texture->setResizeNonPowerOfTwoHint(false);

    int width = thumbnail_image->s();
    int height = thumbnail_image->t();
    std::cout << "ACOsgBaseRenderer::thumbnailGeometry: geode with (thumbnail) width " <<  width << " and height " << height << std::endl;

    // image vertices
    float zpos = 0;// 0.02;
    double imagex, imagey;
    imagex = 0.5*width;
    imagey = 0.5*height;
    Vec3Array* vertices = new Vec3Array(4);
    /*if(media_thumbnail->isCircular()){*/
        (*vertices)[0] = Vec3(-imagex, -imagey, zpos);
        (*vertices)[1] = Vec3(imagex, -imagey, zpos);
        (*vertices)[2] = Vec3(imagex, imagey, zpos);
        (*vertices)[3] = Vec3(-imagex, imagey, zpos);
    /*}
    else
    {
        (*vertices)[0] = Vec3(0, -imagey, zpos);
        (*vertices)[1] = Vec3(2*imagex, -imagey, zpos);
        (*vertices)[2] = Vec3(2*imagex, imagey, zpos);
        (*vertices)[3] = Vec3(0, imagey, zpos);
    }*/
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
    if(transparent){
        state->setMode(GL_BLEND, StateAttribute::ON);
    }
    else{
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
    }

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    thumbnail_geometry->setNormalArray(normals);
    thumbnail_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

    //    osgUtil::Optimizer optimizer;
    //    optimizer.optimize(thumbnail_geometry);

    //    osgUtil::Tessellator tessellator;
    //        tessellator.retessellatePolygons( *thumbnail_geometry );

    if(transparent)
        osgUtil::SmoothingVisitor::smooth( * thumbnail_geometry );

    //    osgUtil::Simplifier simplifier;
    //    simplifier.setSampleRatio( 0.1 );
    //    simplifier.simplify(* thumbnail_geometry);

    //            osgUtil::TriStripVisitor tsv;
    //               tsv. stripify( *thumbnail_geometry );

    if(!transparent){
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 0.9f));
    }
    thumbnail_geometry->setColorBinding(Geometry::BIND_OVERALL);


    //state->setRenderingHint(osg::StateSet::OPAQUE_BIN );

    return thumbnail_geometry;
}

ACOsgMediaThumbnail* ACOsgBaseRenderer::getSharedThumbnail()
{
    if(this->media == 0){
        std::cerr << "ACOsgBaseRenderer::getSharedThumbnailImage: no media set" << std::endl;
        return 0;
    }
    if(this->shared_thumbnail == ""){
        std::cerr << "ACOsgBaseRenderer::getSharedThumbnailImage: no shared thumbnail name set" << std::endl;
        return 0;
    }
    if(this->media->getThumbnail(this->shared_thumbnail)==0){
        std::cerr << "ACOsgBaseRenderer::getSharedThumbnailImage: no shared thumbnail available" << std::endl;
        return 0;
    }
    ACOsgMediaThumbnail* thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(this->media->getThumbnail(this->shared_thumbnail));
    if(thumbnail == 0){
        std::cerr << "ACOsgBaseRenderer::getSharedThumbnailImage: shared thumbnail isn't an osg media thumbnail" << std::endl;
        return 0;
    }
    return thumbnail;
}

osg::ref_ptr<osg::Image> ACOsgBaseRenderer::getSharedThumbnailImage(){
    ACOsgMediaThumbnail* thumbnail = this->getSharedThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getImage();
}

osg::ref_ptr<osg::Texture2D> ACOsgBaseRenderer::getSharedThumbnailTexture()
{
    ACOsgMediaThumbnail* thumbnail = this->getSharedThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getTexture();
}

osg::ref_ptr<osg::ImageStream> ACOsgBaseRenderer::getSharedThumbnailStream()
{
    ACOsgMediaThumbnail* thumbnail = this->getSharedThumbnail();
    if(!thumbnail)
        return 0;
    return thumbnail->getStream();
}
