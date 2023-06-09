/*
 *  ACOsgBrowserRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACOsgImageRenderer.h"
#include <ACImage.h>

#include <osg/Version>
using namespace osg;

#define IMAGE_BORDER

#include "ACOsgMediaThumbnail.h"

ACOsgImageRenderer::ACOsgImageRenderer()
    :ACOsgMediaRenderer()
{
    media_type = MEDIA_TYPE_IMAGE;
    node_color = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
    image_geode = 0;
    border_geode = 0;
    image_transform = 0;
    aura_geode=0;
    node_size = 1.0f;
}

ACOsgImageRenderer::~ACOsgImageRenderer() {
    image_geode=0;
    border_geode=0;
    image_transform=0;
    node_size = 1.0f;
}

void ACOsgImageRenderer::imageGeode(bool flip, float sizemul, float zoomin) {
    image_geode = 0;
    border_geode = 0;
    image_transform = 0;
    aura_geode=0;

    if(this->shared_thumbnail==""){
        std::cerr << "ACOsgImageRenderer::imageGeode: no shared thumbnail set, can't create image geode" << std::endl;
        return;
    }

    if(media){
        ACMediaType media_type = media->getType();
        int i;
        
        double xstep = 0.0005;
        
        float zpos = 0.02;
        double ylim = 0.025, xlim = 0.025;
        double imagex, imagey;
        
        double imagesurf;
        
        ylim *=sizemul;
        xlim *=sizemul;
        
        int width, height;
        std::string thumbnail_filename;
        
        StateSet *state;
        
        Vec3Array* vertices;
        osg::ref_ptr<DrawElementsUInt> line_p;
        Vec2Array* texcoord;
        
        osg::ref_ptr<Geometry> image_geometry;
        osg::ref_ptr<Geometry> border_geometry;
        Texture2D *image_texture;

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
        width = osg_thumbnail->getWidth();
        height = osg_thumbnail->getHeight();
        image_texture = osg_thumbnail->getTexture();

        //std::cout << "Geode with (thumbnail) width " <<  width << " and height " << height << std::endl;
        
        image_transform = new MatrixTransform();
        
        image_geode = new Geode();
        image_geometry = new Geometry();
        
        //zpos = zpos - 0.00001 * node_index;
        
        // image vertices
        float scale;
        imagesurf = xlim * ylim;
        scale = sqrt ((float)imagesurf / (width*height));
        imagex = scale * width;
        imagey = scale * height;
        vertices = new Vec3Array(4);
        (*vertices)[0] = Vec3(-imagex, -imagey, zpos);
        (*vertices)[1] = Vec3(imagex, -imagey, zpos);
        (*vertices)[2] = Vec3(imagex, imagey, zpos);
        (*vertices)[3] = Vec3(-imagex, imagey, zpos);
        image_geometry->setVertexArray(vertices);
        
        // Primitive Set
        osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
        poly->push_back(0);
        poly->push_back(1);
        poly->push_back(2);
        poly->push_back(3);
        image_geometry->addPrimitiveSet(poly);
        
        // State Set
        state = image_geode->getOrCreateStateSet();
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        
        //state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
        
        // Texture Coordinates
        texcoord = new Vec2Array;
        
        // XS TODO!!
        //http://lists.openscenegraph.org/pipermail/osg-users-openscenegraph.org/2009-August/032147.html
        //	other ways of flipping:
        //	* use osg::Image's flipVertical()
        //	* just flip the texture coordinates
        
        
        float a = (1.0-(1.0/zoomin)) / 2.0;
        float b = 1.0-a;
        texcoord->push_back(osg::Vec2(a, flip ? b : a));
        texcoord->push_back(osg::Vec2(b, flip ? b : a));
        texcoord->push_back(osg::Vec2(b, flip ? a : b));
        texcoord->push_back(osg::Vec2(a, flip ? a : b));
        image_geometry->setTexCoordArray(0, texcoord);
        
        // Texture State (image)
        
        //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();
        image_texture->setResizeNonPowerOfTwoHint(false);
        
        // XS TODO add this line?
        // http://groups.google.com/group/osg-users/browse_thread/thread/f623b62f62e39473?pli=1
        //image_texture->setUnRefImageDataAfterApply(true);
        state = image_geometry->getOrCreateStateSet();
        state->setTextureAttribute(0, image_texture);
        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
        
        state->setMode(GL_BLEND, osg::StateAttribute::ON);
              
        image_geode->addDrawable(image_geometry);
        
        image_transform->addChild(image_geode);
        
#ifdef IMAGE_BORDER
        /*
         //CF alternate version
         state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
         state->setMode(GL_BLEND,osg::StateAttribute::ON);
         
         osg::Vec3Array* normals = new osg::Vec3Array(1);
         (*normals)[0].set(-1.0f,0.0f,0.0f);
         image_geometry->setNormalArray(normals);
         image_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
         
         image_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.
         //image_texture->setBorderColor(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
         image_texture->setBorderColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
         //image_texture->setBorderWidth(1.5);
         image_texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
         image_texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
         //CF alternate version end
         */
        
        border_geode = new Geode();
        
        // border vertices by drawing lines, slightly more cost effective
        //if (media_type == MEDIA_TYPE_IMAGE){
        /*border_geometry = new Geometry();
         vertices = new Vec3Array(5);
         (*vertices)[0] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
         (*vertices)[1] = Vec3(imagex+xstep, -imagey-xstep, zpos);
         (*vertices)[2] = Vec3(imagex+xstep, imagey+xstep, zpos);
         (*vertices)[3] = Vec3(-imagex-xstep, imagey+xstep, zpos);
         (*vertices)[4] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
         border_geometry->setVertexArray(vertices);
         
         line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);
         for(i=0; i<4; i++) {
         (*line_p)[2*i] = i;
         (*line_p)[2*i+1] = i+1;
         }
         border_geometry->addPrimitiveSet(line_p);
                 
         state = border_geometry->getOrCreateStateSet();
         state->setAttribute(new LineWidth(2.0));//
         //}+/
         
         //border by box, more smooth
         /*if (media_type == MEDIA_TYPE_VIDEO){
         TessellationHints *hints = new TessellationHints();
         hints->setDetailRatio(0.0);
         border_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.0001),2*(imagex+5*xstep),2*(imagey+5*xstep),0), hints));
         }*/
        
        TessellationHints *hints = new TessellationHints();
        hints->setDetailRatio(0.0);
        
        state = border_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        border_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.0001),2*(imagex+5*xstep),2*(imagey+5*xstep),0), hints));
        
        state = border_geode->getOrCreateStateSet();
        
        
        
#if defined(APPLE_IOS)
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
#else
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
#endif
        
        image_transform->addChild(border_geode);
        

        
        
        ground_geode = new Geode();
        hints->setDetailRatio(0.0);
        
        state = ground_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        ground_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00005),2*(imagex),2*(imagey),0), hints));
        
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)ground_geode->getDrawable(0))->setColor(osg::Vec4(1,1,1,1));
        image_transform->addChild(ground_geode);

#endif
        
        image_transform->setUserData(new ACRefId(node_index));
        image_geode->setUserData(new ACRefId(node_index));
        
    }
}

void ACOsgImageRenderer::auraImageGeode(bool flip, float sizemul, float zoomin) {
    image_geode = 0;
    border_geode = 0;
    image_transform = 0;
    aura_geode=0;
    //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();
    if(media){
        ACMediaType media_type = media->getType();
        if (media_type == MEDIA_TYPE_VIDEO)
            flip=true;
        int i;
        
        double xstep = 0.0005;
        
        float zpos = 0.02;
        double ylim = 0.025, xlim = 0.025;
        double imagex, imagey;
        
        double imagesurf;
        
        ylim *=sizemul;
        xlim *=sizemul;
        
        int width, height;
        std::string thumbnail_filename;
        
        StateSet *state;
        
        Vec3Array* vertices;
        osg::ref_ptr<DrawElementsUInt> line_p;
        Vec2Array* texcoord;
        
        osg::ref_ptr<Geometry> image_geometry;
        osg::ref_ptr<Geometry> border_geometry;
        Texture2D *image_texture;
        
        ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
        if(!media_thumbnail){
            std::cerr << "ACOsgImageRenderer::auraImageGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
            return;
        }
        ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
        if(!osg_thumbnail){
            std::cerr << "ACOsgImageRenderer::auraImageGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
            return;
        }
        width = osg_thumbnail->getWidth();
        height = osg_thumbnail->getHeight();
        image_texture = osg_thumbnail->getTexture();

        //std::cout << "Geode with (thumbnail) width " <<  width << " and height " << height << std::endl;
        
        image_transform = new MatrixTransform();
        
        image_geode = new Geode();
        image_geometry = new Geometry();
        
        //zpos = zpos - 0.00001 * node_index;
        
        // image vertices
        float scale;
        imagesurf = xlim * ylim;
        scale = sqrt ((float)imagesurf / (width*height));
        imagex = scale * width;
        imagey = scale * height;
        vertices = new Vec3Array(4);
        (*vertices)[0] = Vec3(-imagex, -imagey, zpos);
        (*vertices)[1] = Vec3(imagex, -imagey, zpos);
        (*vertices)[2] = Vec3(imagex, imagey, zpos);
        (*vertices)[3] = Vec3(-imagex, imagey, zpos);
        image_geometry->setVertexArray(vertices);
        
        // Primitive Set
        osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
        poly->push_back(0);
        poly->push_back(1);
        poly->push_back(2);
        poly->push_back(3);
        image_geometry->addPrimitiveSet(poly);
        
        // State Set
        state = image_geode->getOrCreateStateSet();
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        
        //state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
        
        // Texture Coordinates
        texcoord = new Vec2Array;
        
        // XS TODO!!
        //http://lists.openscenegraph.org/pipermail/osg-users-openscenegraph.org/2009-August/032147.html
        //	other ways of flipping:
        //	* use osg::Image's flipVertical()
        //	* just flip the texture coordinates
        
        
        float a = (1.0-(1.0/zoomin)) / 2.0;
        float b = 1.0-a;
        texcoord->push_back(osg::Vec2(a, flip ? b : a));
        texcoord->push_back(osg::Vec2(b, flip ? b : a));
        texcoord->push_back(osg::Vec2(b, flip ? a : b));
        texcoord->push_back(osg::Vec2(a, flip ? a : b));
        image_geometry->setTexCoordArray(0, texcoord);
        
        // Texture State (image)
        image_texture->setResizeNonPowerOfTwoHint(false);
        
        // XS TODO add this line?
        // http://groups.google.com/group/osg-users/browse_thread/thread/f623b62f62e39473?pli=1
        //image_texture->setUnRefImageDataAfterApply(true);
        state = image_geometry->getOrCreateStateSet();
        state->setTextureAttribute(0, image_texture);
        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
        
        state->setMode(GL_BLEND, osg::StateAttribute::ON);

        image_geode->addDrawable(image_geometry);
        
        image_transform->addChild(image_geode);
        
#ifdef IMAGE_BORDER
        /*
         //CF alternate version
         state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); //CF from OSG's examples/osgmovie.cpp, doesn't solve the transparent first frame for video geodes
         state->setMode(GL_BLEND,osg::StateAttribute::ON);
         
         osg::Vec3Array* normals = new osg::Vec3Array(1);
         (*normals)[0].set(-1.0f,0.0f,0.0f);
         image_geometry->setNormalArray(normals);
         image_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
         
         image_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.
         //image_texture->setBorderColor(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
         image_texture->setBorderColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
         //image_texture->setBorderWidth(1.5);
         image_texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
         image_texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
         //CF alternate version end
         */
        
        border_geode = new Geode();
        
        // border vertices by drawing lines, slightly more cost effective
        //if (media_type == MEDIA_TYPE_IMAGE){
        /*border_geometry = new Geometry();
         vertices = new Vec3Array(5);
         (*vertices)[0] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
         (*vertices)[1] = Vec3(imagex+xstep, -imagey-xstep, zpos);
         (*vertices)[2] = Vec3(imagex+xstep, imagey+xstep, zpos);
         (*vertices)[3] = Vec3(-imagex-xstep, imagey+xstep, zpos);
         (*vertices)[4] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
         border_geometry->setVertexArray(vertices);
         
         line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);
         for(i=0; i<4; i++) {
         (*line_p)[2*i] = i;
         (*line_p)[2*i+1] = i+1;
         }
         border_geometry->addPrimitiveSet(line_p);
         
#if OSG_MIN_VERSION_REQUIRED(3,2,0)
         border_geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
#else
         border_geometry->setColorArray(colors);
         border_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
#endif
         border_geode->addDrawable(border_geometry);
         
         state = border_geometry->getOrCreateStateSet();
         state->setAttribute(new LineWidth(2.0));//
         //}+/
         
         //border by box, more smooth
         /*if (media_type == MEDIA_TYPE_VIDEO){
         TessellationHints *hints = new TessellationHints();
         hints->setDetailRatio(0.0);
         border_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.0001),2*(imagex+5*xstep),2*(imagey+5*xstep),0), hints));
         }*/
        
        TessellationHints *hints = new TessellationHints();
        hints->setDetailRatio(0.0);
        
        state = border_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        border_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.0001),2*(imagex+5*xstep),2*(imagey+5*xstep),0), hints));
        
        state = border_geode->getOrCreateStateSet();
        
        
        
#if defined(APPLE_IOS)
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
#else
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
#endif
        
        image_transform->addChild(border_geode);
        
        aura_geode = new Geode();
        
        state = aura_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00015),2*(imagex)+30*xstep,2*(imagey)+30*xstep,0), hints));
        
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)aura_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
        image_transform->addChild(aura_geode);
        
        
        
        ground_geode = new Geode();
        hints->setDetailRatio(0.0);
        
        state = ground_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        ground_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00005),2*(imagex),2*(imagey),0), hints));
        
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)ground_geode->getDrawable(0))->setColor(osg::Vec4(1,1,1,1));
        image_transform->addChild(ground_geode);
        
#endif

        image_transform->setUserData(new ACRefId(node_index));
        image_geode->setUserData(new ACRefId(node_index));
        
    }
}

void ACOsgImageRenderer::prepareNodes() {

    if (media && media_cycle->getNodeFromMedia(media)->isDisplayed()) {
        if (!image_geode) {
            imageGeode();
            media_node->addChild(image_transform);
        }
    }

    //prev_media_index = media_index;
    media_changed = false;
}

void ACOsgImageRenderer::updateNodes(double ratio) {

    float x, y, z;
    float zpos = 0.001;

    const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
    if (!attribute->isDisplayed()){//TR mod to implement isDisplayed
        if(media_node->getNumChildren() == 1) {
            media_node->removeChild(0, 1);
        }
        if (image_transform)
            image_transform->setNodeMask(0);
        return;
    }

    const ACPoint &p = attribute->getCurrentPosition(), &p2 = attribute->getNextPosition();
    double omr = 1.0-ratio;

    //if (media_index!=prev_media_index) {
    if (media_changed) {
        if(media_node->getNumChildren() == 1) {
            media_node->removeChild(0, 1);
        }
        if (!image_geode)
            imageGeode();
        media_node->addChild(image_transform);
        //prev_media_index = media_index;
        media_changed = false;
    }

    unsigned int mask = (unsigned int)-1;
    if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()) {
        if (image_transform)
            image_transform->setNodeMask(mask);
    }
    else {
        if (image_transform)
            image_transform->setNodeMask(0);
    }

    z = 0;
    if (this->getIsTagged()){
        
        if (media_node->containsNode(image_transform)&&(image_transform->containsNode(aura_geode)==false)){
            
            media_node->removeChild(image_transform);
            image_transform=0;
            this->auraImageGeode();
            media_node->addChild(image_transform);
            if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                cluster_index = attribute->getClusterId();
                osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
                (*colors)[0] = node_color;
                if(cluster_colors.size()>0)
                    (*colors)[0] = cluster_colors[attribute->getClusterId()%cluster_colors.size()];
                if ((ShapeDrawable*)border_geode->getDrawable(0))
                    ((ShapeDrawable*)border_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);

            }
            //unsigned int mask = (unsigned int)-1;
            //aura_geode->setNodeMask(mask);
        }
    }
    else
        if (this->getIsTagged()==0&&aura_geode){
            if (media_node->containsNode(image_transform)&&image_transform->containsNode(aura_geode)){
                media_node->removeChild(image_transform);
                image_transform=0;
                this->imageGeode();
                media_node->addChild(image_transform);
                if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                    cluster_index = attribute->getClusterId();
                    osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
                    (*colors)[0] = node_color;
                    if(cluster_colors.size()>0)
                        (*colors)[0] = cluster_colors[attribute->getClusterId()%cluster_colors.size()];
                    if ((ShapeDrawable*)border_geode->getDrawable(0))
                        ((ShapeDrawable*)border_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
                }
                //aura_geode->setNodeMask(0);
            }
            
            
        }
#ifdef IMAGE_BORDER
    if (border_geode){
        if (border_geode->getDrawable(0)) {
            //CF nodes colored along their relative cluster on in Clusters Mode
            if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                if(cluster_index != attribute->getClusterId()){
                    cluster_index = attribute->getClusterId();
                    osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
                    (*colors)[0] = node_color;
                    if(cluster_colors.size()>0)
                        (*colors)[0] = cluster_colors[attribute->getClusterId()%cluster_colors.size()];
                    if ((ShapeDrawable*)border_geode->getDrawable(0))
                        ((ShapeDrawable*)border_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
                }
            }
        }
    }
#endif
    float localscale;
    float maxdistance = 0.2;
    float maxscale = 3;//1.5;//CF
    float minscale = 0.6;
    x = media_cycle_view_pos.x;
    y = media_cycle_view_pos.y;
    localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
    localscale = max(localscale,minscale);
    if (localscale>minscale) {
        z += 2*zpos;
    }
    else if (attribute->getActivity()==1) {
        z += zpos;
    }

#ifdef AUTO_TRANSFORM
    media_node->setPosition(Vec3(x,y,z));
    media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    media_node->setScale(Vec3(node_size*localscale/media_cycle_zoom,node_size*localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else

    Matrix T;
    T.makeTranslate(Vec3(x, y, z)); // omr*p.z + ratio*p2.z));
    T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0))
            * Matrix::scale(node_size*localscale/media_cycle_zoom,node_size*localscale/media_cycle_zoom,localscale/media_cycle_zoom)
            * T;
    media_node->setMatrix(T);
#endif //AUTO_TRANSFORM
}

void ACOsgImageRenderer::changeNodeSize(double _size){
    this->node_size = _size;
}
