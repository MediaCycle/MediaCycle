/**
 * @brief The audio browser node renderer class, implemented with OSG
 * @authors St&eacute;phane Dupont, Christian Frisson, Thierry Ravet
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

#include "ACOsgAudioRenderer.h"
#include <ACAudio.h>

#include <osg/Version>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

using namespace osg;

ACOsgAudioRenderer::ACOsgAudioRenderer()
    :ACOsgMediaRenderer()
{
    media_type = MEDIA_TYPE_AUDIO;
    waveform_geode = 0;
    curser_geode = 0;
    curser_transform = 0;
    entry_geode = 0;
    aura_geode=0;
    waveform_type = AC_BROWSER_AUDIO_WAVEFORM_CLASSIC;
    waveform_thumbnail = "Classic browser waveform";//"Classic timeline waveform";//
    node_thumbnail = "";//Circular browser waveform";
    node_geometry = 0;
    waveform_geometry = 0;
    node_shape_drawable = 0;
    waveform_transform = 0;
    entry_transform = 0;
    node_size = 1.0f;
}

ACOsgAudioRenderer::~ACOsgAudioRenderer() {
    waveform_geode=0;
    curser_geode=0;
    curser_transform=0;
    entry_geode=0;
    node_geometry = 0;
    waveform_geometry = 0;
    node_shape_drawable = 0;
    waveform_transform = 0;
    entry_transform = 0;
}

void ACOsgAudioRenderer::changeSetting(ACSettingType _setting)
{
    // Change setting
    if(this->setting == _setting)
        return;
    this->setting = _setting;

    // Force regeneration of the text:
    if(metadata_geode){
        media_node->removeChild(metadata_geode);
        metadata_geode = 0;
    }
}


void ACOsgAudioRenderer::curserGeode() {

    float zpos = 0.04; //CF sphere hack instead of 0.02
    double xstep = 0.0005, ylim = 0.025;

    xstep*=afac;
    ylim*=afac;

    StateSet *state;

    Vec3Array* vertices;

    DrawElementsUShort* line_p;

    osg::ref_ptr<Geometry> curser_geometry;

#ifdef AUTO_TRANSFORM
    curser_transform = new AutoTransform();
#else
    curser_transform = new MatrixTransform();
#endif

    curser_geode = new Geode();
    curser_geometry = new Geometry();

    /////////////////////////
    //curser vertices

#if defined(APPLE_IOS)
    //curser vertices for triangle strip
    // SD TODO - This works on iPad, but disturbing aliasing effects appear
    vertices = new Vec3Array(4);
    (*vertices)[0] = Vec3(0-xstep*afac/2.0, -ylim, zpos+0.00005);
    (*vertices)[1] = Vec3(0+xstep*afac/2.0, -ylim, zpos+0.00005);
    (*vertices)[2] = Vec3(0+xstep*afac/2.0, ylim, zpos+0.00005);
    (*vertices)[3] = Vec3(0-xstep*afac/2.0, ylim, zpos+0.00005);
    curser_geometry->setVertexArray(vertices);
    line_p = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP, 0);
    line_p->push_back(0);
    line_p->push_back(3);
    line_p->push_back(1);
    line_p->push_back(2);
    curser_geometry->addPrimitiveSet(line_p);
#else
    // SD TODO - This does not work on iPad, altough it should.
    vertices = new Vec3Array(2);
    (*vertices)[0] = Vec3(0, -ylim+xstep, zpos+0.00005);
    (*vertices)[1] = Vec3(0, ylim-xstep, zpos+0.00005);
    curser_geometry->setVertexArray(vertices);
    line_p = new DrawElementsUShort(PrimitiveSet::LINES, 3);
    (*line_p)[0] = 0;
    (*line_p)[1] = 1;
    curser_geometry->addPrimitiveSet(line_p);
#endif

    Vec4 curser_color(0.2f, 0.9f, 0.2f, 0.9f);
    osg::ref_ptr<osg::Vec4Array> curser_colors = new Vec4Array;
    curser_colors->push_back(curser_color);
    curser_geometry->setColorArray(curser_colors);
    curser_geometry->setColorBinding(Geometry::BIND_OVERALL);

    state = curser_geode->getOrCreateStateSet();
    state->setMode(GL_BLEND, StateAttribute::ON);

#if defined(APPLE_IOS)
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
#else
    state = curser_geometry->getOrCreateStateSet();
    state->setAttribute(new LineWidth(2.0));
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );//CF
    state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif

    curser_geode->addDrawable(curser_geometry);
    curser_transform->addChild(curser_geode);
    curser_transform->setUserData(new ACRefId(node_index));
    curser_geode->setUserData(new ACRefId(node_index));
}

void ACOsgAudioRenderer::entryGeode() {

    StateSet *state;

    float localsize = 0.01;
    localsize *= afac;

    TessellationHints *hints = new TessellationHints();
    hints->setDetailRatio(0.0);

    state = entry_geode->getOrCreateStateSet();
    state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    node_shape_drawable = 0;

#if defined(APPLE_IOS)
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    node_shape_drawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints); //draws a square // Vintage AudioCycle
#else
    //CF state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    //CF state->setMode(GL_BLEND, StateAttribute::ON);
    //node_shape_drawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints); //draws a square // Vintage AudioCycle
    node_shape_drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),localsize), hints); // draws a sphere // MultiMediaCycle
    //node_shape_drawable = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),localsize, 0.0f), hints); // draws a disc
    //node_shape_drawable = new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints); // draws a sphere
#endif

    entry_geode->addDrawable(node_shape_drawable);


    /*node_geometry = 0;
    node_geometry = this->imageGeometry("/Volumes/data/Dev/numediart/diskhover/data/textures/dot64.png");
    if(node_geometry){
        entry_geode->addDrawable(node_geometry);
    }*/

    entry_geode->setUserData(new ACRefId(node_index));
    entry_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
}

void ACOsgAudioRenderer::auraGeode()
{
    float localsize = 0.01;
    localsize *= afac;
    aura_geode = new Geode();

    TessellationHints *hints = new TessellationHints();
    hints->setDetailRatio(0.0);

    StateSet *state = aura_geode->getOrCreateStateSet();
    state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

#if defined(APPLE_IOS)
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
#else
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );//CF
    state->setMode(GL_BLEND, StateAttribute::ON);
    //aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
    //aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,-localsize*1.5),localsize*1.5), hints)); // draws a sphere // MultiMediaCycle
    aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),localsize*1.5, 0.0f), hints)); // draws a disc
    //aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints)); // draws a sphere
#endif
    ((ShapeDrawable*)aura_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
    aura_geode->setUserData(new ACRefId(node_index));
}

void ACOsgAudioRenderer::prepareNodes() {

    if(media_node->containsNode(entry_transform))
        media_node->removeChild(entry_transform);

    waveform_geode = 0;
    curser_transform = 0;
    curser_geode = 0;
    entry_geode = 0;
    entry_transform = 0;
    metadata_geode = 0;
    node_geometry = 0;
    waveform_geometry = 0;
    node_shape_drawable = 0;

    // This waveform must be initialized before the renderer node is added to the browser renderer scenegraph, otherwise it gets visual noise
    /*waveform_geometry = thumbnailGeometry(waveform_thumbnail);
    waveform_geode = new Geode();
    waveform_geode->addDrawable(waveform_geometry);
    waveform_geode->setUserData(new ACRefId(node_index));
    waveform_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
#ifdef AUTO_TRANSFORM
    waveform_transform = new AutoTransform();
#else
    waveform_transform = new MatrixTransform();
#endif
    waveform_transform->addChild(waveform_geode);*/

    //if  (media_cycle->getMediaNode(node_index)->isDisplayed()){
    if (media && media_cycle->getNodeFromMedia(media)==0){
        cout<<"test Error"<<endl;
    }
    this->changeNodeThumbnail(node_thumbnail);
}

void ACOsgAudioRenderer::setEntryGeodeVisible(bool visibility){
    if(!entry_geode)
        return;
    if(visibility){
        entry_geode->setNodeMask((unsigned int)-1);
    }
    else{
        entry_geode->setNodeMask(0);
    }
}

void ACOsgAudioRenderer::updateNodes(double ratio) {

    double xstep = 0.00025;
    xstep *= afac;
    unsigned int mask = (unsigned int)-1;

    const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
    if (attribute==0)
        return;
    if (!attribute->isDisplayed()){
        media_node->removeChild(waveform_transform);
        media_node->removeChild(metadata_geode);
        metadata_geode = 0;
        media_node->removeChild(curser_transform);
        media_node->removeChild(aura_geode);
        (aura_geode)=0;
        this->setEntryGeodeVisible(false);
        return;
    }
    Matrix T;
    Matrix Trotate;
    Matrix curserT;

    float x, y, z;
    float localscale;
    float maxdistance = 0.2;
    float maxscale = 1.5;
    float minscale = 0.33;

    x = media_cycle_view_pos.x;
    y = media_cycle_view_pos.y;
    z = 0;

    T.makeTranslate(Vec3(x, y, z));

    localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
    if(waveform_type == AC_BROWSER_AUDIO_WAVEFORM_NONE) localscale = minscale;//CF for KIS tests
    localscale = max(localscale,minscale);

    osg::Vec4f current_color;

    if (media && attribute->getActivity()>=1) {	// with waveform
        localscale = 0.5;
        //std::cout << "ACOsgAudioRenderer::updateNodes: current frame " << attribute->getCurrentFrame() << std::endl;

        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE){
            if (curser_transform == 0) {
                curserGeode();
            }
            if (metadata_geode == 0) {
                metadataGeode();
            }

            if (waveform_geometry == 0) {
                waveform_geometry = thumbnailGeometry(waveform_thumbnail);
                waveform_geode = new Geode();
                waveform_geode->addDrawable(waveform_geometry);
                waveform_geode->setUserData(new ACRefId(node_index));
                waveform_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
#ifdef AUTO_TRANSFORM
                waveform_transform = new AutoTransform();
#else
                waveform_transform = new MatrixTransform();
#endif
                waveform_transform->addChild(waveform_geode);
            }

        }
        
        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE){
            if (label != media->getLabel() ) {
                if (media_node->containsNode(metadata_geode))
                    media_node->removeChild(metadata_geode);
                metadata_geode = 0;
                metadataGeode();
                media_node->addChild(metadata_geode);
                label = media->getLabel();
            }
        }

        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE) {// waveform + curser + metadata
            this->setEntryGeodeVisible(false);
            if (aura_geode&&media_node->containsNode(metadata_geode)){
                media_node->removeChild(aura_geode);
            }
            if (media_node->containsNode(waveform_transform)==false)
                media_node->addChild(waveform_transform);
            if (media_node->containsNode(metadata_geode)==false)
                media_node->addChild(metadata_geode);
            if (media_node->containsNode(curser_transform)==false)
                media_node->addChild(curser_transform);
        }
        else if( waveform_type == AC_BROWSER_AUDIO_WAVEFORM_NONE){// when switching to none mode while waveforms are already displayed
            if (media_node->containsNode(waveform_transform))
                media_node->removeChild(waveform_transform);
            if (media_node->containsNode(metadata_geode))
                media_node->removeChild(metadata_geode);
            metadata_geode = 0;
            if (media_node->containsNode(curser_transform))
                media_node->removeChild(curser_transform);
            this->setEntryGeodeVisible(true);
            if (aura_geode){
                if (media_node->containsNode(aura_geode)==false)
                    media_node->addChild(aura_geode);
            }
        }

        //CF for KIS tests
        if(waveform_type == AC_BROWSER_AUDIO_WAVEFORM_NONE){
            current_color = osg::Vec4(0,0,0,1);
            if(node_shape_drawable){
                node_shape_drawable->setColor(current_color);
            }
        }

        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE){
#ifdef AUTO_TRANSFORM
            curser_transform->setPosition(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media )->getNFrames()) * media->getThumbnailWidth(waveform_thumbnail) * xstep, 0.0, 0.0));
#else
            curserT.makeTranslate(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media )->getNFrames()) * media->getThumbnailWidth(waveform_thumbnail) * xstep, 0.0, 0.0));
            curser_transform->setMatrix(curserT);
#endif
            //std::cout << "ACOsgAudioRenderer::updateNodes: current frame " << attribute->getCurrentFrame() << std::endl;
            T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) * T;
        }
    }
    else {
        { // entry_geode
            if (media_node->containsNode(waveform_transform))
                media_node->removeChild(waveform_transform);
            if (media_node->containsNode(metadata_geode))
                media_node->removeChild(metadata_geode);
            metadata_geode = 0;
            if (media_node->containsNode(curser_transform))
                media_node->removeChild(curser_transform);
            this->setEntryGeodeVisible(true);
            if (aura_geode){
                if (media_node->containsNode(aura_geode)==false)
                    media_node->addChild(aura_geode);
            }
        }

        //CF nodes colored along their relative cluster on in Clusters Mode
        if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
            const vector<int> centerNodeIds=media_cycle->getBrowser()->getIdNodeClusterCenter();
            if(cluster_colors.size()>0){
                current_color = cluster_colors[attribute->getClusterId()%cluster_colors.size()];
                //                if(setting == AC_SETTING_DESKTOP){
                //                    if(centerNodeIds.size() != 0 && attribute->getClusterId() < centerNodeIds.size())
                //                        if (centerNodeIds[attribute->getClusterId()]==attribute->getMediaId())
                //                            current_color(osg::Vec4(0,0,0,1));
                //                }
            }
            else
                current_color = node_color;
            if (this->getIsTagged()&&(aura_geode==0)){
                auraGeode();
                current_color = osg::Vec4(0,0,0,1);
            }
            else
                if (this->getIsTagged()==0&&aura_geode){
                    media_node->removeChild(aura_geode);
                    aura_geode=0;
                }
        }
        else
            current_color = neighbor_color;

        //CF color (multiple) selected nodes in black (for AudioGarden)
        if (attribute->isSelected())
            current_color = osg::Vec4(0,0,0,1);

        //CF color discarded nodes in black (for LoopJam composition)
        if(media)
            if(media->isDiscarded())
                current_color = osg::Vec4(0,0,0,1);

        //CF KIS tests
        if(waveform_type == AC_BROWSER_AUDIO_WAVEFORM_NONE){
            current_color = osg::Vec4(1,1,1,1);
        }

        if (user_defined_color)
            current_color = node_color;

        osg::ref_ptr<osg::Vec4Array> current_color_array = new Vec4Array(1);
        (*current_color_array)[0] = current_color;
        if (node_geometry){
            node_geometry->setColorArray(current_color_array);
        }
        if( waveform_geometry){
            waveform_geometry->setColorArray(current_color_array);
        }
        if(node_shape_drawable){
            node_shape_drawable->setColor(current_color);
        }

        T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) * T;
        //}

        if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()) {
            if (aura_geode)
                aura_geode->setNodeMask(mask);
        }
        else {
            this->setEntryGeodeVisible(false);
            if (aura_geode)
                aura_geode->setNodeMask(0);
        }

        float magic_number = 0.0005; // since we're not using an ortho2D projection
#ifdef AUTO_TRANSFORM
        if(waveform_transform)
            waveform_transform->setScale(magic_number*node_size,magic_number*node_size,1.0);
        if(entry_transform){
            if(node_geometry) // if the node is a thumbnail and not a shape
                entry_transform->setScale(magic_number*node_size,magic_number*node_size,1.0);
            else
                entry_transform->setScale(node_size,node_size,1.0);
        }
#else
        Matrix magic_numbers;
        magic_numbers.makeScale(magic_number*node_size,magic_number*node_size,1.0);
        if(waveform_transform)
            waveform_transform->setMatrix(magic_numbers);
        if(entry_transform){
            if(node_geometry) // if the node is a thumbnail and not a shape
                entry_transform->setMatrix(magic_numbers);
            else{
                Matrix node_scale;
                node_scale.makeScale(node_size,node_size,1.0);
                entry_transform->setMatrix(node_scale);
            }
        }
#endif

#ifdef AUTO_TRANSFORM
        media_node->setPosition(Vec3(x,y,z));
        media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
        media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else
        media_node->setMatrix(T);
#endif
    }
}

void ACOsgAudioRenderer::setWaveformType(ACBrowserAudioWaveformType _type)
{
    waveform_type = _type;
}

void ACOsgAudioRenderer::updateWaveformType(ACBrowserAudioWaveformType _type)
{
    if (waveform_type!=_type){
        waveform_type = _type;
        this->updateNodes();
    }
}

void ACOsgAudioRenderer::changeNodeThumbnail(std::string thumbnail){
    this->node_thumbnail = thumbnail;

    if(!media)
        return;

    if(media_cycle->getNodeFromMedia(media)==0)
        return;

    if(media_node->containsNode(entry_transform))
        media_node->removeChild(entry_transform);

    entry_transform=0;
#ifdef AUTO_TRANSFORM
    entry_transform = new AutoTransform();
#else
    entry_transform = new MatrixTransform();
#endif

    entry_geode = 0;
    entry_geode = new Geode();

    node_geometry = 0;
    node_geometry = this->thumbnailGeometry(node_thumbnail);
    if(node_geometry){
        entry_geode->addDrawable(node_geometry);
        entry_geode->setUserData(new ACRefId(node_index));
        entry_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );
    }
    else
        entryGeode();
    entry_transform->addChild(entry_geode);
    media_node->addChild(entry_transform);
}

void ACOsgAudioRenderer::changePlaybackThumbnail(std::string thumbnail){
    if(thumbnail == "Waveform"){
        waveform_type = AC_BROWSER_AUDIO_WAVEFORM_CLASSIC;
    }
    else{
        waveform_type = AC_BROWSER_AUDIO_WAVEFORM_NONE;
    }
    this->updateNodes();
}

void ACOsgAudioRenderer::changeNodeSize(double _size){
    this->node_size = _size;
}
