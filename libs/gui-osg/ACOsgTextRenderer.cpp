/*
 *  ACOsgTextRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson and T. Ravet
 *  @date 26/05/2011
 *
 *  @copyright (c) 2011 – UMONS - Numediart
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

#if defined (SUPPORT_TEXT)

#include "ACOsgTextRenderer.h"
#include <ACText.h>
#include <textFile.h>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations

#include <sstream>

#include <osg/Version>

namespace fs = boost::filesystem;

using namespace osg;

ACOsgTextRenderer::ACOsgTextRenderer()
    :ACOsgMediaRenderer()
{
    media_type = MEDIA_TYPE_TEXT;
    metadata_geode = 0;
    metadata = 0;
    entry_geode = 0;
}

ACOsgTextRenderer::~ACOsgTextRenderer() {
    entry_geode=0;
    metadata_geode=0;
    metadata=0;
}

void ACOsgTextRenderer::metadataGeode() {

    osg::Vec4 textColor(0.9f,0.9f,0.9f,1.0f);
    float textCharacterSize = 20.0f;
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

    //string textLabel=media_cycle->getLibrary()->getMedia(media_index)->getLabel();
    string textLabel=media->getLabel();

    metadata->setText( textLabel );

    //state = text_geode->getOrCreateStateSet();
    //state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    //state->setMode(GL_BLEND, StateAttribute::ON);
    //state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);

    metadata_geode->addDrawable(metadata);
}

void ACOsgTextRenderer::entryGeode() {

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
    entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.1), hints)); //draws a square // Vintage TextCycle
#else
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode(GL_BLEND, StateAttribute::ON);
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.1), hints)); //draws a square // Vintage TextCycle
    entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),localsize), hints)); // draws a sphere // MultiMediaCycle
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.0f), hints)); // draws a disc
    //entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints)); // draws a sphere
    //sprintf(name, "some audio element");
#endif
    entry_geode->setUserData(new ACRefId(node_index));
}

void ACOsgTextRenderer::prepareNodes() {
    entry_geode = 0;
    metadata_geode = 0;
    metadata=0;

    //if  (media_cycle->getMediaNode(node_index)->isDisplayed()){
    if  (media && media_cycle->getNodeFromMedia(media)->isDisplayed()){
        entryGeode();
        media_node->addChild(entry_geode);
    }
    if (!metadata_geode)
        metadataGeode();
}

void ACOsgTextRenderer::updateNodes(double ratio) {

    double xstep = 0.00025;

    xstep *= afac;

	const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
	if (!attribute->isDisplayed()){
		media_node->removeChild(metadata_geode);
		if (entry_geode)
			entry_geode->setNodeMask(0);
		return;			
	}
	Matrix T;
	Matrix Trotate;

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
    localscale = max(localscale,minscale);

    if (attribute->getActivity()>=1) { // 0 inactive, 1 clicked, 2 hover
        localscale = 0.5;
        if(media_node->getNumChildren() == 1) // only entry_geode so far
            media_node->addChild(metadata_geode);
    }
    else {
        media_node->removeChild(metadata_geode);

        //CF nodes colored along their relative cluster on in Clusters Mode
        if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS)
            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
        else
            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(neighbor_color);
        if (attribute->isSelected()) {
            //CF color (multiple) selected nodes in black
            Vec4 selected_color(0,0,0,1);
            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(selected_color);
        }

        if (user_defined_color)
            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(node_color);

        T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom) * T;
    }

    unsigned int mask = (unsigned int)-1;
    if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()) {
        entry_geode->setNodeMask(mask);
    }
    else {
        entry_geode->setNodeMask(0);
    }

#ifdef AUTO_TRANSFORM
    media_node->setPosition(Vec3(x,y,z));
    media_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    media_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else
    media_node->setMatrix(T);
#endif

}
#endif //defined (SUPPORT_TEXT)
