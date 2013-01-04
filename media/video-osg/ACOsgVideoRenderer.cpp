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

#include "ACOsgVideoRenderer.h"
#include "ACVideo.h"

using namespace osg;

#define video_BORDER

#include "ACOsgMediaThumbnail.h"

ACOsgVideoRenderer::ACOsgVideoRenderer()
    :ACOsgMediaRenderer()
{
    media_type = MEDIA_TYPE_VIDEO;
    node_color = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
    video_geode = 0;
    border_geode = 0;
    video_transform = 0;
    aura_geode=0;
    video_stream = 0;
}

ACOsgVideoRenderer::~ACOsgVideoRenderer() {
    if (video_stream)
        video_stream->quit();
    video_geode=0;
    border_geode=0;
    video_transform=0;
}

void ACOsgVideoRenderer::videoGeode(bool flip, float sizemul, float zoomin) {
    video_geode = 0;
    border_geode = 0;
    video_transform = 0;
    aura_geode=0;

    if(this->shared_thumbnail==""){
        std::cerr << "ACOsgImageRenderer::videoGeode: no shared thumbnail set, can't create image geode" << std::endl;
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

        osg::ref_ptr<Geometry> video_geometry;
        osg::ref_ptr<Geometry> border_geometry;
        Texture2D *video_texture;

        ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
        if(!media_thumbnail){
            std::cerr << "ACOsgImageRenderer::videoGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
            return;
        }
        ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
        if(!osg_thumbnail){
            std::cerr << "ACOsgImageRenderer::videoGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
            return;
        }
        width = osg_thumbnail->getWidth();
        height = osg_thumbnail->getHeight();
        video_texture = osg_thumbnail->getTexture();

        //std::cout << "Geode with (thumbnail) width " <<  width << " and height " << height << std::endl;

        video_transform = new MatrixTransform();

        video_geode = new Geode();
        video_geometry = new Geometry();

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
        /// CF do not set GL_BLEND to ON otherwise the video might be transparent!

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
        video_geometry->setTexCoordArray(0, texcoord);

        // Texture State (image)

        //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();
        video_texture->setResizeNonPowerOfTwoHint(false);

        // XS TODO add this line?
        // http://groups.google.com/group/osg-users/browse_thread/thread/f623b62f62e39473?pli=1
        //video_texture->setUnRefImageDataAfterApply(true);
        state = video_geometry->getOrCreateStateSet();
        state->setTextureAttribute(0, video_texture);
        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
        /// CF do not set GL_BLEND to ON otherwise the video might be transparent!

        osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
        (*colors)[0] = node_color;
        //video_geometry->setColorArray(colors);
        //video_geometry->setColorBinding(Geometry::BIND_OVERALL);

        video_geode->addDrawable(video_geometry);

        video_transform->addChild(video_geode);

#ifdef video_BORDER
        border_geode = new Geode();

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
        /// CF do not set GL_BLEND to ON otherwise the video might be transparent!
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
#endif
        video_transform->addChild(border_geode);

        ground_geode = new Geode();
        hints->setDetailRatio(0.0);

        state = ground_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        ground_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00005),2*(imagex),2*(imagey),0), hints));

        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)ground_geode->getDrawable(0))->setColor(osg::Vec4(1,1,1,1));
        video_transform->addChild(ground_geode);

#endif
        video_transform->setUserData(new ACRefId(node_index));
        video_geode->setUserData(new ACRefId(node_index));
    }
}

void ACOsgVideoRenderer::auraVideoGeode(bool flip, float sizemul, float zoomin) {
    video_geode = 0;
    border_geode = 0;
    video_transform = 0;
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

        osg::ref_ptr<Geometry> video_geometry;
        osg::ref_ptr<Geometry> border_geometry;
        Texture2D *video_texture;

        ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
        if(!media_thumbnail){
            std::cerr << "ACOsgImageRenderer::auraVideoGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
            return;
        }
        ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
        if(!osg_thumbnail){
            std::cerr << "ACOsgImageRenderer::auraVideoGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
            return;
        }
        width = osg_thumbnail->getWidth();
        height = osg_thumbnail->getHeight();
        video_texture = osg_thumbnail->getTexture();

        //std::cout << "Geode with (thumbnail) width " <<  width << " and height " << height << std::endl;

        video_transform = new MatrixTransform();

        video_geode = new Geode();
        video_geometry = new Geometry();

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
        video_geometry->setTexCoordArray(0, texcoord);

        // Texture State (image)
        video_texture->setResizeNonPowerOfTwoHint(false);

        // XS TODO add this line?
        // http://groups.google.com/group/osg-users/browse_thread/thread/f623b62f62e39473?pli=1
        //video_texture->setUnRefImageDataAfterApply(true);
        state = video_geometry->getOrCreateStateSet();
        state->setTextureAttribute(0, video_texture);
        state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

        state->setMode(GL_BLEND, osg::StateAttribute::ON);

        osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array(1);
        (*colors)[0] = node_color;
        //video_geometry->setColorArray(colors);
        //video_geometry->setColorBinding(Geometry::BIND_OVERALL);

        video_geode->addDrawable(video_geometry);

        video_transform->addChild(video_geode);

#ifdef video_BORDER
        border_geode = new Geode();

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
        video_transform->addChild(border_geode);

        aura_geode = new Geode();

        state = aura_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00015),2*(imagex)+30*xstep,2*(imagey)+30*xstep,0), hints));

        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)aura_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
        video_transform->addChild(aura_geode);

        ground_geode = new Geode();
        hints->setDetailRatio(0.0);

        state = ground_geode->getOrCreateStateSet();
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        ground_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,zpos-0.00005),2*(imagex),2*(imagey),0), hints));

        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
        ((ShapeDrawable*)ground_geode->getDrawable(0))->setColor(osg::Vec4(1,1,1,1));
        video_transform->addChild(ground_geode);

#endif
        video_transform->setUserData(new ACRefId(node_index));
        video_geode->setUserData(new ACRefId(node_index));
    }
}

void ACOsgVideoRenderer::prepareNodes() {

    if (!video_geode) {
        videoGeode(true, 2.0, 1.0); // true is for flip -- necessary for video
        media_node->addChild(video_transform);
    }

    ACMediaThumbnail* media_thumbnail = media->getThumbnail(this->shared_thumbnail);
    if(!media_thumbnail){
        std::cerr << "ACOsgImageRenderer::videoGeode: couldn't get shared thumbnail '" << this->shared_thumbnail << "'" << std::endl;
        return;
    }
    ACOsgMediaThumbnail* osg_thumbnail = dynamic_cast<ACOsgMediaThumbnail*>(media_thumbnail);
    if(!osg_thumbnail){
        std::cerr << "ACOsgImageRenderer::videoGeode: shared thumbnail '" << this->shared_thumbnail << "' isn't of OSG type" << std::endl;
        return;
    }
    video_stream = osg_thumbnail->getStream();
    //std::cout << "Movie length " << video_stream->getLength() << " and framerate "<< media->getFrameRate() << std::endl;//video_stream->getFrameRate() << std::endl;

    // Hack to display a first valid frame,
    video_stream->play();

    video_stream->seek(this->media->getStart()); // to start with the correct frame, especially for segments
}

void ACOsgVideoRenderer::updateNodes(double ratio) {

    float x, y, z;
    float zpos = 0.001;

    const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
    if (!attribute->isDisplayed()){//TR mod to implement isDisplayed
        if(media_node->getNumChildren() == 1) {
            media_node->removeChild(0, 1);
        }
        if (video_transform)
            video_transform->setNodeMask(0);
        return;
    }

    const ACPoint &p = attribute->getCurrentPosition(), &p2 = attribute->getNextPosition();
    double omr = 1.0-ratio;

    //if (media_index!=prev_media_index) {
    if (media_changed) {
        if(media_node->getNumChildren() == 1) {
            media_node->removeChild(0, 1);
        }
        if (!video_geode)
            videoGeode();
        media_node->addChild(video_transform);
        //prev_media_index = media_index;
        media_changed = false;
    }

    unsigned int mask = (unsigned int)-1;
    if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()) {
        if (video_transform)
            video_transform->setNodeMask(mask);
    }
    else {
        if (video_transform)
            video_transform->setNodeMask(0);
    }

    z = 0;
    if (this->getIsTagged()){

        if (media_node->containsNode(video_transform)&&(video_transform->containsNode(aura_geode)==false)){

            media_node->removeChild(video_transform);
            video_transform=0;
            this->auraVideoGeode();
            media_node->addChild(video_transform);
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
            if (media_node->containsNode(video_transform)&&video_transform->containsNode(aura_geode)){
                media_node->removeChild(video_transform);
                video_transform=0;
                this->videoGeode();
                media_node->addChild(video_transform);
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
#ifdef video_BORDER
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
    media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else

    Matrix T;
    T.makeTranslate(Vec3(x, y, z)); // omr*p.z + ratio*p2.z));
    T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0))
            * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom)
            * T;
    media_node->setMatrix(T);
#endif //AUTO_TRANSFORM

    if (!attribute->isDisplayed()){
        if (video_stream)
            video_stream->pause();
        if (video_geode)
            video_geode->setNodeMask(0);
        return;
    }
    
    if (video_geode)
        video_geode->setNodeMask(mask);

    if(!video_stream)
        return;

    osg::ImageStream::StreamStatus streamStatus = video_stream->getStatus();

    switch (streamStatus) {
    case osg::ImageStream::INVALID:
        std::cout << "Image stream invalid status" << std::endl;
        break;
    case osg::ImageStream::PLAYING:
        if( video_stream->getCurrentTime() > media->getEnd() ){
            std::cout << "Video stream for media id " << media->getId() << " of current time " << video_stream->getCurrentTime() << " goes beyond the media end time " << media->getEnd() << ", rewinding to start time " << media->getStart() << std::endl;
            video_stream->seek(media->getStart() );
        }
        if (attribute->getActivity()==0)
            video_stream->pause();
        break;
    case osg::ImageStream::PAUSED:
        if (attribute->getActivity()==1)
            video_stream->play();
        break;
    case osg::ImageStream::REWINDING:
        std::cout << "Image stream rewinding" << std::endl;
        break;
    default:
        break;
    }
}
