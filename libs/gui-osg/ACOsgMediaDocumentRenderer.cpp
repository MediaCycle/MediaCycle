/*
 *  ACOsgMediaDocumentRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 29/06/11
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

#if defined (SUPPORT_MULTIMEDIA) 

#include "ACOsgMediaDocumentRenderer.h"
#include "ACMediaDocument.h"

#if defined (SUPPORT_AUDIO)
#include "ACOsgAudioRenderer.h"
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_IMAGE)
#include "ACOsgImageRenderer.h"
#endif //defined (SUPPORT_IMAGE)
#if defined (SUPPORT_VIDEO)
#include "ACOsgVideoRenderer.h"
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_3DMODEL)
#include "ACOsg3DModelRenderer.h"
#endif //defined (SUPPORT_3DMODEL)
#if defined (SUPPORT_TEXT)
#include "ACOsgTextRenderer.h"
#endif //defined (SUPPORT_TEXT)
#if defined (SUPPORT_SENSOR)
#include "ACOsgSensorRenderer.h"
#endif //defined (SUPPORT_SENSOR)

#include <osg/Version>

using namespace osg;

ACOsgMediaDocumentRenderer::ACOsgMediaDocumentRenderer() {
    entry_geode = 0;
}

ACOsgMediaDocumentRenderer::~ACOsgMediaDocumentRenderer() {
    if 	(entry_geode) {
        entry_geode=0;
    }

    ACOsgMediaDocumentRenderers::iterator render_iter = media_renderers.end();
    for ( render_iter=media_renderers.begin() ; render_iter!=media_renderers.end(); ++render_iter ){
        delete (*render_iter);
    }
}

void ACOsgMediaDocumentRenderer::entryGeode() {

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
    //entry_geode->setName(name);
    //ref_ptr//entry_geode->ref();
}


void ACOsgMediaDocumentRenderer::prepareNodes() {

    entry_geode = 0;

    if  (media && media_cycle->getNodeFromMedia(media)->isDisplayed())
    {
        entryGeode();
        media_node->addChild(entry_geode);
    }

    //if (!metadata_geode)
    //    metadataGeode();

    //ACMediaContainer medias = (static_cast<ACMediaDocument*> (media_cycle->getLibrary()->getMedia(media_index)))->getMedias();
    ACMediaContainer medias = (static_cast<ACMediaDocument*> (media))->getContainer();
    ACMediaContainer::iterator iter;

    for ( iter=medias.begin() ; iter!=medias.end(); ++iter ){
        cout << (*iter).first << " => " << (*iter).second << endl;
	
        bool renderer_added = true;
        switch (iter->second->getType()) {
                    case MEDIA_TYPE_AUDIO:
    #if defined (SUPPORT_AUDIO)
    media_renderers.push_back(new ACOsgAudioRenderer());
    #endif //defined (SUPPORT_AUDIO)
    break;
        case MEDIA_TYPE_IMAGE:
#if defined (SUPPORT_IMAGE)
            media_renderers.push_back(new ACOsgImageRenderer());
            media_node->removeChild(entry_geode);
#endif //defined (SUPPORT_IMAGE)
            break;
            /*			case MEDIA_TYPE_VIDEO:
    #if defined (SUPPORT_VIDEO)
    media_renderers.push_back(new ACOsgVideoRenderer());
    #endif //defined (SUPPORT_VIDEO)
    break;
   case MEDIA_TYPE_3DMODEL:
    #if defined (SUPPORT_3DMODEL)
    media_renderers.push_back(new ACOsg3DModelRenderer());
    #endif //defined (SUPPORT_3DMODEL)
    break;
   case MEDIA_TYPE_TEXT:
    #if defined (SUPPORT_TEXT)
    media_renderers.push_back(new ACOsgTextRenderer());
    #endif //defined (SUPPORT_TEXT)
    break;*/
        default:
            renderer_added = false;
            break;
        }

        if (renderer_added){
            media_renderers.back()->setMediaCycle(media_cycle);

            //render_iter->second->setMediaIndex(media_index);//dangerous! before each media of media documents are part of the library
            //TODO TR problem with submedia node_index. We must fix it
            media_renderers.back()->setMedia(iter->second);
            media_renderers.back()->setNodeIndex(this->getNodeIndex()+media_renderers.size());

            media_renderers.back()->prepareNodes();
            //media_node->addChild(media_renderers.back()->getNode());
            media_node->addChild(media_renderers.back()->getMainGeode());
        }
    }
}

void ACOsgMediaDocumentRenderer::updateNodes(double ratio) {

    double xstep = 0.00025;

    xstep *= afac;

    const ACMediaNode* attribute = media_cycle->getMediaNode(node_index);
    if (!attribute->isDisplayed()){
        media_node->removeChild(metadata_geode);
        metadata_geode = 0;
        if (entry_geode)
            entry_geode->setNodeMask(0);
        return;
    }
    if (!entry_geode){
        entryGeode();
        media_node->addChild(entry_geode);

    }
    Matrix T;
    Matrix Trotate;

    if (entry_geode){

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
            if(media_node->getNumChildren() == 1) {// only entry_geode so far
                if (!metadata_geode)
                    metadataGeode();
                media_node->addChild(metadata_geode);
            }
        }
        else {
            media_node->removeChild(metadata_geode);
            metadata_geode = 0;
            //CF nodes colored along their relative cluster on in Clusters Mode
            if (media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                const vector<int> centerNodeIds=media_cycle->getBrowser()->getIdNodeClusterCenter();
                if(cluster_colors.size()>0){
                    ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
                    if(centerNodeIds.size() != 0 && attribute->getClusterId() < centerNodeIds.size())
                        if (centerNodeIds[attribute->getClusterId()]==attribute->getMediaId())
                            ((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(osg::Vec4(0,0,0,1));
                }
            }
            //			((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(cluster_colors[attribute->getClusterId()%cluster_colors.size()]);
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


        if (attribute->getActivity()>=2){//hover
            std::vector<ACMedia*> tmpSegments;
            tmpSegments = media->getAllSegments();
            for (int j=0; j<tmpSegments.size(); j++)
                media_cycle->getMediaNode(tmpSegments[j]->getId())->setDisplayed(true);
        }
        else {
            std::vector<ACMedia*> tmpSegments;
            tmpSegments = media->getAllSegments();
            for (int j=0; j<tmpSegments.size(); j++)
                media_cycle->getMediaNode(tmpSegments[j]->getId())->setDisplayed(true);// TR true CF false
        }

        unsigned int mask = (unsigned int)-1;
        if(attribute->getNavigationLevel() >= media_cycle->getNavigationLevel()&&attribute->isDisplayed()) {
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

        ACOsgMediaDocumentRenderers::iterator render_iter = media_renderers.end();
        for ( render_iter=media_renderers.begin() ; render_iter!=media_renderers.end(); ++render_iter ){
            (*render_iter)->updateNodes();
#ifdef AUTO_TRANSFORM
            (*render_iter)->getNode()->setPosition(Vec3(x,y,z));
            (*render_iter)->getNode()->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
            (*render_iter)->getNode()->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else
            //(*render_iter)->getNode()->setMatrix(T);
#endif
        }
    }
}

#endif //defined (SUPPORT_MULTIMEDIA) 
