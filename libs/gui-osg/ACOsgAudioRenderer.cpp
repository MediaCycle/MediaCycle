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

#if defined (SUPPORT_AUDIO)

#include "ACOsgAudioRenderer.h"
#include <ACAudio.h>

#include <osg/Version>

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
}

ACOsgAudioRenderer::~ACOsgAudioRenderer() {
    // media_node->removeChild(0,1);
    waveform_geode=0;
    curser_geode=0;
    curser_transform=0;
    entry_geode=0;
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

void ACOsgAudioRenderer::waveformGeode() {

    int i;
    float zpos = 0.04; //CF sphere hack instead of 0.02 for boxes
    double xstep = 0.0005, ylim = 0.025;

    xstep*=afac;
    ylim*=afac;
    zpos*=afac;

    int width;
    float *thumbnail;

    StateSet *state;

    Vec3Array* vertices;
    //	Vec3Array* normals;

    osg::ref_ptr<Geometry> samples_geometry;
    osg::ref_ptr<Geometry> frame_geometry;
    osg::ref_ptr<Geometry> border_geometry;
    osg::ref_ptr<Geometry> axis_geometry;

    waveform_geode = new Geode();
    samples_geometry = new Geometry();
    frame_geometry = new Geometry();
    border_geometry = new Geometry();
    axis_geometry = new Geometry();

    if(media){
        //width = media_cycle->getThumbnailWidth(media_index);
        width = media->getThumbnailWidth();
        width = width / 2;

        //thumbnail = (float*)media_cycle->getThumbnailPtr(media_index);
        thumbnail = (float*)media->getThumbnailPtr();

        //////////////////////////
        // samples vertices
        vertices = new Vec3Array(2*width+2);
        if (thumbnail)
        {
            for(i=0; i<width; i++) {
                (*vertices)[2*i] = Vec3(i * xstep, ylim * thumbnail[2*i], zpos);
                (*vertices)[2*i+1] = Vec3(i * xstep, ylim * thumbnail[2*i+1], zpos);
            }
            (*vertices)[2*i] = Vec3(0.0, 0.0, zpos);
            (*vertices)[2*i+1] = Vec3((i-1) * xstep, 0.0, zpos);
        }
        samples_geometry->setVertexArray(vertices);
        /*
  line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2*width+2);
  for(i=0; i<width+1; i++) {
   (*line_p)[2*i] = 2*i;
   (*line_p)[2*i+1] = 2*i+1;
  }
  samples_geometry->addPrimitiveSet(line_p);
  */
        DrawElementsUShort* line_p = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP, 0);
        for(i=0; i<width; i++) {
            line_p->push_back(2*i);
            line_p->push_back(2*i+1);
        }
        samples_geometry->addPrimitiveSet(line_p);

        /////////////////////////
        //frame vertices
        vertices = new Vec3Array;
        vertices->push_back(Vec3(0.0, -ylim, zpos));
        vertices->push_back(Vec3(width * xstep, -ylim, zpos));
        vertices->push_back(Vec3(width * xstep, ylim, zpos));
        vertices->push_back(Vec3(0.0, ylim, zpos));
        frame_geometry->setVertexArray(vertices);
        /*
  osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 0);
  poly->push_back(0);
  poly->push_back(1);
  poly->push_back(2);
  poly->push_back(3);
  frame_geometry->addPrimitiveSet(poly);
  */
        DrawElementsUShort *poly = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP, 0);
        poly->push_back(0);
        poly->push_back(3);
        poly->push_back(1);
        poly->push_back(2);
        frame_geometry->addPrimitiveSet(poly);

        /////////////////////////
        //border vertices
        /*
  vertices = new Vec3Array(5);
  (*vertices)[0] = Vec3(0, -ylim, zpos);
  (*vertices)[1] = Vec3(width * xstep, -ylim+xstep, zpos);
  (*vertices)[2] = Vec3(width * xstep, ylim-xstep, zpos);
  (*vertices)[3] = Vec3(0, ylim, zpos);
  (*vertices)[4] = Vec3(0, -ylim, zpos);
  border_geometry->setVertexArray(vertices);
  */
        //border vertices for triangle strip
        vertices = new Vec3Array(8);
        (*vertices)[0] = Vec3(0, -ylim, zpos);
        (*vertices)[1] = Vec3(width * xstep, -ylim, zpos);
        (*vertices)[2] = Vec3(width * xstep, ylim, zpos);
        (*vertices)[3] = Vec3(0, ylim, zpos);
        (*vertices)[4] = Vec3(0-xstep*afac, -ylim-xstep*afac, zpos);
        (*vertices)[5] = Vec3(width * xstep+xstep*afac, -ylim-xstep*afac, zpos);
        (*vertices)[6] = Vec3(width * xstep+xstep*afac, ylim+xstep*afac, zpos);
        (*vertices)[7] = Vec3(0-xstep*afac, ylim+xstep*afac, zpos);
        border_geometry->setVertexArray(vertices);
        /*
  line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);
  for(i=0; i<4; i++) {
   (*line_p)[2*i] = i;
   (*line_p)[2*i+1] = i+1;
  }
  border_geometry->addPrimitiveSet(line_p); // CF, bounding box temporarily disabled
  */
        line_p = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP, 0);
        line_p->push_back(0);
        line_p->push_back(4);
        line_p->push_back(1);
        line_p->push_back(5);
        line_p->push_back(2);
        line_p->push_back(6);
        line_p->push_back(3);
        line_p->push_back(7);
        line_p->push_back(0);
        line_p->push_back(4);
        border_geometry->addPrimitiveSet(line_p); // CF, bounding box temporarily disabled

        /////////////////////////
        //axis vertices
        vertices = new Vec3Array(4);
        (*vertices)[0] = Vec3(0, 0-xstep*afac/2.0, zpos);
        (*vertices)[1] = Vec3(width * xstep, 0-xstep*afac/2.0, zpos);
        (*vertices)[2] = Vec3(width * xstep, 0+xstep*afac/2.0, zpos);
        (*vertices)[3] = Vec3(0, 0+xstep*afac/2.0, zpos);
        axis_geometry->setVertexArray(vertices);
        line_p = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP, 0);
        line_p->push_back(0);
        line_p->push_back(3);
        line_p->push_back(1);
        line_p->push_back(2);
        axis_geometry->addPrimitiveSet(line_p); // CF, bounding box temporarily disabled

        /////////////////////////

        Vec4 color(0.9f, 0.9f, 0.9f, 0.9f);
        osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
        colors->push_back(color);

        samples_geometry->setColorArray(colors);
        samples_geometry->setColorBinding(Geometry::BIND_OVERALL);
        border_geometry->setColorArray(colors);
        border_geometry->setColorBinding(Geometry::BIND_OVERALL);
        axis_geometry->setColorArray(colors);
        axis_geometry->setColorBinding(Geometry::BIND_OVERALL);
        // osg::Geometry::BIND_PER_VERTEX

        colors = new Vec4Array(1);
        (*colors)[0] = Vec4(0.0, 0.0, 0.0, 0.3); //0.3);
        frame_geometry->setColorArray(colors);
        frame_geometry->setColorBinding(Geometry::BIND_OVERALL);

        state = waveform_geode->getOrCreateStateSet();
        state->setMode(GL_BLEND, StateAttribute::ON);
        state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
#if defined(APPLE_IOS)
        state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
#else
        state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
        state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif
        //state->setAttribute(new LineWidth(1.0));

        waveform_geode->addDrawable(samples_geometry);
        //waveform_geode->addDrawable(border_geometry);
        waveform_geode->addDrawable(axis_geometry);
        waveform_geode->addDrawable(frame_geometry);
        waveform_geode->setUserData(new ACRefId(node_index));
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
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif
    /*
  state = curser_transform->getOrCreateStateSet();
 state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
  */

    curser_geode->addDrawable(curser_geometry);
    //TessellationHints *hints = new TessellationHints();
    //hints->setDetailRatio(0.0);
    //curser_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),0.05), hints)); // draws a sphere // MultiMediaCycle

    curser_transform->addChild(curser_geode);

    curser_transform->setUserData(new ACRefId(node_index));
    curser_geode->setUserData(new ACRefId(node_index));
}

void ACOsgAudioRenderer::entryGeode() {

    StateSet *state;

    float localsize = 0.01;
    localsize *= afac;

    entry_geode = new Geode();

    TessellationHints *hints = new TessellationHints();
    hints->setDetailRatio(0.0);

    state = entry_geode->getOrCreateStateSet();
    state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

#if defined(APPLE_IOS)
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
#else
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode(GL_BLEND, StateAttribute::ON);
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
    entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),localsize), hints)); // draws a sphere // MultiMediaCycle
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),localsize, 0.0f), hints)); // draws a disc
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints)); // draws a sphere
#endif
    entry_geode->setUserData(new ACRefId(node_index));
}
void ACOsgAudioRenderer::auraGeode(){StateSet *state;
    
    float localsize = 0.01;
    localsize *= afac;
    
    aura_geode = new Geode();
    
    TessellationHints *hints = new TessellationHints();
    hints->setDetailRatio(0.0);
    
    state = aura_geode->getOrCreateStateSet();
    state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    
#if defined(APPLE_IOS)
    state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
#else
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode(GL_BLEND, StateAttribute::ON);
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
    //aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,-localsize*1.5),localsize*1.5), hints)); // draws a sphere // MultiMediaCycle
    aura_geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),localsize*1.5, 0.0f), hints)); // draws a disc
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints)); // draws a sphere
#endif
    aura_geode->setUserData(new ACRefId(node_index));
}

void ACOsgAudioRenderer::prepareNodes() {

    waveform_geode = 0;
    curser_transform = 0;
    curser_geode = 0;
    entry_geode = 0;
    metadata_geode = 0;

    //waveformGeode();
    //curserGeode();
    //if  (media_cycle->getMediaNode(node_index)->isDisplayed()){
    if (media && media_cycle->getNodeFromMedia(media)==0){
        cout<<"test Error"<<endl;
    }
    if (media && media_cycle->getNodeFromMedia(media)->isDisplayed()){
        entryGeode();
        media_node->addChild(entry_geode);
    }
}

void ACOsgAudioRenderer::updateNodes(double ratio) {

    double xstep = 0.00025;
    xstep *= afac;

    const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
    if (!attribute->isDisplayed()){
        media_node->removeChild(waveform_geode);
        media_node->removeChild(metadata_geode);
        metadata_geode = 0;
        media_node->removeChild(curser_transform);
        media_node->removeChild(aura_geode);
        (aura_geode)=0;
        if (entry_geode)
            entry_geode->setNodeMask(0);
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

    // SD 2010 OCT - This animation has moved from Browser to Renderer
    /*
  const ACPoint &p = attribute->getCurrentPosition(), &p2 = attribute->getNextPosition();
  double omr = 1.0-ratio;
  x = omr*p.x + ratio*p2.x;
  y = omr*p.y + ratio*p2.y;
  z = 0;
  */

    x = media_cycle_view_pos.x;
    y = media_cycle_view_pos.y;
    z = 0;

    T.makeTranslate(Vec3(x, y, z));
    localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
    localscale = max(localscale,minscale);
    // localscale = 0.5;

    if (media && attribute->getActivity()>=1) {	// with waveform
        //if (0) {	// without waveform
        localscale = 0.5;

        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE){
            if(waveform_geode == 0 ) {
                waveformGeode();
            }
            if (curser_transform == 0) {
                curserGeode();
            }
            if (metadata_geode == 0) {
                metadataGeode();
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

        //if(media_node->getNumChildren() > 0 && media_node->getChild(0) == entry_geode) {
        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE) {// waveform + curser + metadata
            //waveform_geode->setNodeMask(-1);
            if (media_node->containsNode(entry_geode))
                media_node->removeChild(entry_geode);
            if (aura_geode&&media_node->containsNode(metadata_geode)){
                media_node->removeChild(aura_geode);
            }
            if (media_node->containsNode(waveform_geode)==false)
                media_node->addChild(waveform_geode);
            //media_node->setChild(0, waveform_geode);
            if (media_node->containsNode(metadata_geode)==false)
                media_node->addChild(metadata_geode);
            if (media_node->containsNode(curser_transform)==false)
                media_node->addChild(curser_transform);
        }
        else if( waveform_type == AC_BROWSER_AUDIO_WAVEFORM_NONE){// when switching to none mode while waveforms are already displayed
            if (media_node->containsNode(waveform_geode))
                media_node->removeChild(waveform_geode);
            if (media_node->containsNode(metadata_geode))
            media_node->removeChild(metadata_geode);
            metadata_geode = 0;
            if (media_node->containsNode(curser_transform))
                media_node->removeChild(curser_transform);
            if (media_node->containsNode(entry_geode)==false)
                media_node->addChild(entry_geode);
            if (aura_geode){
                if (media_node->containsNode(aura_geode)==false)
                    media_node->addChild(aura_geode);
            }
        }

        if(waveform_type != AC_BROWSER_AUDIO_WAVEFORM_NONE){
            // curserT.makeTranslate(Vec3(omr*p.x + ratio*p2.x + attribute->curser * xstep * 0.5 / zoom, omr*p.y + ratio*p2.y, 0.0)); // omr*p.z + ratio*p2.z));
            // curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
#ifdef AUTO_TRANSFORM
            //curser_transform->setPosition(Vec3(attribute->getCursor() * xstep, 0.0, 0.0));

            //curser_transform->setPosition(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media_cycle->getLibrary()->getMedia(media_index) )->getNFrames()) * media_cycle->getThumbnailWidth(media_index) * xstep, 0.0, 0.0));
            curser_transform->setPosition(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media )->getNFrames()) * media->getThumbnailWidth() * xstep, 0.0, 0.0));
#else
            //curserT.makeTranslate(Vec3(attribute->getCursor() * xstep, 0.0, 0.0));

            //curserT.makeTranslate(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media_cycle->getLibrary()->getMedia(media_index) )->getNFrames()) * media_cycle->getThumbnailWidth(media_index) * xstep, 0.0, 0.0));
            curserT.makeTranslate(Vec3((float) attribute->getCurrentFrame() / (float)(((ACAudio*) media )->getNFrames()) * media->getThumbnailWidth() * xstep, 0.0, 0.0));

            curser_transform->setMatrix(curserT);
#endif

            T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) * T;
        }
    }
    else {
        //if(media_node->getNumChildren() == 3) {
        if (!entry_geode){
            entryGeode();
            //std::cout << "ACOsgAudioRenderer::updateNodes created missing node geode" << std::endl;
        }
        { // entry_geode
            if (media_node->containsNode(waveform_geode))
                media_node->removeChild(waveform_geode);
            if (media_node->containsNode(metadata_geode))
                media_node->removeChild(metadata_geode);
            metadata_geode = 0;
            if (media_node->containsNode(curser_transform))
                media_node->removeChild(curser_transform);
            if (media_node->containsNode(entry_geode)==false)
                media_node->addChild(entry_geode);
            if (aura_geode){
                if (media_node->containsNode(aura_geode)==false)
                    media_node->addChild(aura_geode);
            }
            //media_node->setChild(0, entry_geode);
            //media_node->removeChild(1, 1);
        }


        //CF nodes colored along their relative cluster on in Clusters Mode
        if (entry_geode &&entry_geode->getDrawable(0)){
            if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                const vector<int> centerNodeIds=media_cycle->getBrowser()->getIdNodeClusterCenter();
                if(cluster_colors.size()>0){
                    ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
                /*if(setting == AC_SETTING_DESKTOP){
                    if(centerNodeIds.size() != 0 && attribute->getClusterId() < centerNodeIds.size())
                        if (centerNodeIds[attribute->getClusterId()]==attribute->getMediaId())
                            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
                }*/
                }
                else
                    ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(node_color);
                if (this->getIsTagged()&&(aura_geode==0)){
                    auraGeode();
                    ((ShapeDrawable*)aura_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
                }
                else
                    if (this->getIsTagged()==0&&aura_geode){
                        media_node->removeChild(aura_geode);
                        aura_geode=0;
                    }
                
                
            }
            else
                ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(neighbor_color);
        

        //CF color (multiple) selected nodes in black (for AudioGarden)
            if (attribute->isSelected())
                ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));

        //CF color discarded nodes in black (for LoopJam composition)
            if(media)
                if(media->isDiscarded())
                    ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));

            if (user_defined_color)
                ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(node_color);
        }

        T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) * T;
    }

    unsigned int mask = (unsigned int)-1;
    if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()) {
        entry_geode->setNodeMask(mask);
        if (aura_geode)
            aura_geode->setNodeMask(mask);
    }
    else {
        entry_geode->setNodeMask(0);
        if (aura_geode)
            aura_geode->setNodeMask(0);
    }

#ifdef AUTO_TRANSFORM
    media_node->setPosition(Vec3(x,y,z));
    media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else
    media_node->setMatrix(T);
#endif

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
#endif //defined (SUPPORT_AUDIO)
