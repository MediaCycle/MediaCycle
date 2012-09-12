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

#include "ACOsgBrowserRenderer.h"
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
#if defined (SUPPORT_MULTIMEDIA) 
#include "ACOsgMediaDocumentRenderer.h"
#endif //defined (SUPPORT_MULTIMEDIA) 
#include "ACOsgLabelRenderer.h"

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace osg;

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <iostream>

#include <osgViewer/GraphicsWindow>

#ifdef OSG_LIBRARY_STATIC
// include the plugins we need
//USE_OSGPLUGIN(ive) // for 3Dmodels?
//USE_OSGPLUGIN(osg) // for 3Dmodels?
//USE_OSGPLUGIN(osg2)
//USE_OSGPLUGIN(rgb)
USE_OSGPLUGIN(freetype) // for text

/*USE_DOTOSGWRAPPER_LIBRARY(osg)
USE_DOTOSGWRAPPER_LIBRARY(osgFX)
//USE_DOTOSGWRAPPER_LIBRARY(osgParticle)
USE_DOTOSGWRAPPER_LIBRARY(osgShadow)
//USE_DOTOSGWRAPPER_LIBRARY(osgSim)
//USE_DOTOSGWRAPPER_LIBRARY(osgTerrain)
USE_DOTOSGWRAPPER_LIBRARY(osgText)
USE_DOTOSGWRAPPER_LIBRARY(osgViewer)
USE_DOTOSGWRAPPER_LIBRARY(osgVolume)
USE_DOTOSGWRAPPER_LIBRARY(osgWidget)

USE_SERIALIZER_WRAPPER_LIBRARY(osg)
USE_SERIALIZER_WRAPPER_LIBRARY(osgAnimation)
USE_SERIALIZER_WRAPPER_LIBRARY(osgFX)
USE_SERIALIZER_WRAPPER_LIBRARY(osgManipulator)
USE_SERIALIZER_WRAPPER_LIBRARY(osgParticle)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgShadow)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgSim)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgTerrain)
USE_SERIALIZER_WRAPPER_LIBRARY(osgText)
USE_SERIALIZER_WRAPPER_LIBRARY(osgVolume)
*/
// include the platform specific GraphicsWindow implementation.
USE_GRAPHICSWINDOW()
//USE_GRAPICSWINDOW_IMPLEMENTATION(Win32)
//osgViewer::graphicswindowproxy_Win32(graphicswindow_Win32);

#endif

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif


ACOsgBrowserRenderer::ACOsgBrowserRenderer()
    : ACEventListener(),font(0)
{
    media_cycle = 0;
  //  pthread_mutexattr_init(&activity_update_mutex_attr);
  //  pthread_mutex_init(&activity_update_mutex, &activity_update_mutex_attr);
  //  pthread_mutexattr_destroy(&activity_update_mutex_attr);
    setting = AC_SETTING_NONE;
    group = new Group();
    media_group = new Group();
    link_group = new Group();
    label_group = new Group();
    group->addChild(label_group);
    group->addChild(media_group);
    group->addChild(link_group);
    this->clean();
}

ACOsgBrowserRenderer::~ACOsgBrowserRenderer(){
    media_cycle = 0;
    this->clean();
    
    //pthread_mutex_destroy(&activity_update_mutex);
}

void ACOsgBrowserRenderer::clean(){
    
   // pthread_mutex_lock(&activity_update_mutex);
    activity_update_mutex.lock();
    // SD - Results from centralized request to MediaCycle - GLOBAL
    media_cycle_time = 0.0;
    media_cycle_prevtime = 0.0;
    media_cycle_deltatime = 0.0;
    media_cycle_zoom = 0.0f;
    media_cycle_angle = 0.0f;
    media_cycle_mode = 0;
    media_cycle_global_navigation_level = 0;

    // SD - Results from centralized request to MediaCycle - NODE SPECIFIC
    media_cycle_node = 0;
    media_cycle_isdisplayed = false;
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
    node_index = -1;
    media_index = -1;
    prev_media_index = -1;
    media_cycle_filename = "";
    distance_mouse.clear();
    nodes_prepared = 0;
    this->removeNodes();
    this->removeLinks();
    this->removeLabels();
    media_group->removeChildren(0,media_group->getNumChildren());
    audio_waveform_type = AC_BROWSER_AUDIO_WAVEFORM_CLASSIC;
    //CF this is a temporary solution until we implement signals/slots in the core
    if(media_cycle){
        if(media_cycle->hasBrowser()){
            media_cycle->getBrowser()->setModeChanged(false);
        }
    }
    //pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
}

void ACOsgBrowserRenderer::mediaImported(int n, int nTot,int mId){
    
    
    if(n==0){
        std::cout << "ACOsgBrowserRenderer::mediaImported: creating renderers for " << nTot << " medias " << std::endl;
        return;
    }
    else if(mId==-1){
        std::cerr << "ACOsgBrowserRenderer::mediaImported " << n << "/" << nTot << " doesn't have a proper media id" << std::endl;
        return;
    }
    if(!media_cycle){
        std::cerr << "ACOsgBrowserRenderer::mediaImported: no media cycle set" << std::endl;
        return;
    }
    if(media_cycle->getLibrary()->getMedia(mId) == 0){
        std::cerr << "ACOsgBrowserRenderer::mediaImported: media id " << mId << " ("<< n << "/" << nTot << ") not accessible" << std::endl;
        return;
    }
    //pthread_mutex_lock(&activity_update_mutex);
    activity_update_mutex.lock();
   /* if(media_cycle->getMediaType() == media_cycle->getLibrary()->getMedia(mId)->getType())*/{
       
       std::cout << "ACOsgBrowserRenderer::mediaImported adding to " << node_renderers.size() << " renderers the renderer for media id " << mId << " ("<< n << "/" << nTot << ") " << std::endl;
       for (int i=0;i<100;i++){
           if (media_cycle->getMediaNode(mId)!=0) 
               break;
           usleep(1);
       }
        if(this->addNode(mId)){
            this->addLink(mId);
            nodes_prepared = 1;
        }
    }
//    pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
}

void ACOsgBrowserRenderer::libraryCleaned(){
    std::cout << "ACOsgBrowserRenderer::libraryCleaned" << std::endl;
    this->clean();
}

// adds/removes nodes to the node_renderers
void ACOsgBrowserRenderer::prepareNodes(int _start) {

    /*int start;
    int n = media_cycle->getLibrarySize();
    if (_start) {
        start = _start;
    }
    else {
        start = node_renderers.size();
    }
    if (node_renderers.size()>n) {
        this->removeNodes(n, node_renderers.size());
    }
    else if (node_renderers.size()<n) {
        this->addNodes(start,n);

    }

    // Remove all nodelinks if present
    for (int i=0;i<link_renderers.size();i++) {
        if(link_renderers[i]){
            link_group->removeChild(link_renderers[i]->getLink());
            delete link_renderers[i];
            link_renderers[i] = 0;
        }
    }
    link_renderers.clear();

    // XS  TODO why this ?
    if ((n-start)>0)
        nodes_prepared = 1;*/
}

void ACOsgBrowserRenderer::updateNodes(double ratio) {

    if (!nodes_prepared) {
        return;
    }
    
    //pthread_mutex_lock(&activity_update_mutex);
    activity_update_mutex.lock();

    int n = media_cycle->getLibrarySize();

    if(media_cycle->getBrowserMode() == AC_MODE_NEIGHBORS){
        n=n;
        /*// Create new nodelinks if the layout requires them
        if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
            //CF to check with future changes when number of nodelinks may decrease (folding nodes)
            link_renderers.resize(n);
            for (int i=0;i<n;i++) {
                link_renderers[i] = new ACOsgNodeLinkRenderer();
                if (link_renderers[i]) {
                    link_renderers[i]->setMediaCycle(media_cycle);
                    node_index = node_renderers[i]->getNodeIndex();
                    link_renderers[i]->prepareLinks();

                    link_renderers[i]->setNodeIn(node_renderers[i]);
                    int p = media_cycle->getBrowser()->getParentFromNeighborNode(node_index);
                    if ( p!= -1 )
                        link_renderers[i]->setNodeOut(node_renderers[p]);

                    link_group->addChild(link_renderers[i]->getLink());
                }
            }
        }*/
    }

    // SD 2010 OCT - This animation has moved from Browser to Renderer
    /*
 #define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
 #define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
 double t = getTime();
 double frac;
 double andur = 2.0;
 */

    media_cycle_time = getTime();
    media_cycle_deltatime = media_cycle_time - media_cycle_prevtime;
    media_cycle_prevtime = media_cycle_time;
    media_cycle_zoom = media_cycle->getCameraZoom();		// SD TODO - why still part of mediacycle?
    media_cycle_angle = media_cycle->getCameraRotation();
    media_cycle_mode = media_cycle->getBrowser()->getMode();
    media_cycle_global_navigation_level = media_cycle->getNavigationLevel();

    //for (unsigned int i=0;i<node_renderers.size();i++) {
    for(ACOsgMediaRenderers::iterator node_renderer = node_renderers.begin();node_renderer!=node_renderers.end();node_renderer++){
        //if(node_renderer->second){
        int i = node_renderer->first;
        media_cycle_node = media_cycle->getMediaNode(i);
        media_cycle_isdisplayed = media_cycle_node->isDisplayed();
        media_cycle_current_pos = media_cycle_node->getCurrentPosition();
        media_cycle_next_pos = media_cycle_node->getNextPosition();
        media_cycle_navigation_level = media_cycle_node->getNavigationLevel();
        media_cycle_activity = media_cycle_node->getActivity();
        node_index = i;//node_renderer->second->getNodeIndex();
        media_index = node_index;
        media_cycle_filename = media_cycle->getMediaFileName(media_index);

        /* if (media_cycle_isdisplayed) {*/
        //if(node_renderer->second != 0) { // CF temporary
        // GLOBAL
        node_renderer->second->setDeltaTime(media_cycle_deltatime);
        node_renderer->second->setZoomAngle(media_cycle_zoom, media_cycle_angle);
        node_renderer->second->setMode(media_cycle_mode);
        node_renderer->second->setGlobalNavigation(media_cycle_global_navigation_level);
        // NODE SPECIFIC
        node_renderer->second->setIsDisplayed(media_cycle_isdisplayed);
        // SD 2010 OCT
        //node_renderer->second->setPos(media_cycle_current_pos, media_cycle_next_pos);
        node_renderer->second->setNavigation(media_cycle_navigation_level);
        node_renderer->second->setActivity(media_cycle_activity);
        //node_renderer->second->setMediaIndex(media_index);
        node_renderer->second->setMedia(media_cycle->getLibrary()->getMedia(media_index));
        node_renderer->second->setFilename(media_cycle_filename);
        node_renderer->second->setWaveformType(audio_waveform_type);
        // UPDATE
        //std::cout << "Node renderer size " << node_renderers.size() << std::endl;
        node_renderer->second->updateNodes(ratio);
        // }
        //media_group->addChild(node_renderer->second->getNode());
        /*}
        else
                media_group->removeChild(node_renderer->second->getNode());
        }*/
    }

    // Update nodelinks if the layout requires
    if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
        //for (unsigned int i=0;i<link_renderers.size();i++) {
        for(ACOsgNodeLinkRenderers::iterator link_renderer = link_renderers.begin();link_renderer!=link_renderers.end();link_renderer++){
            link_renderer->second->updateLinks();
        }
    }
    //pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
}

void ACOsgBrowserRenderer::prepareLabels(int _start) {
    activity_update_mutex.lock();
//    pthread_mutex_lock(&activity_update_mutex);
    int start;
    int n = media_cycle->getLabelSize();
    if (_start) {
        start = _start;
    }
    else {
        start = label_renderer.size();
    }
    if (label_renderer.size()>n) {
        this->removeLabels(n, label_renderer.size());
    }
    else if (label_renderer.size()<n) {
        this->addLabels(start,n);
    }
    
   // pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
}

void ACOsgBrowserRenderer::updateLabels(double ratio) {
    
//    pthread_mutex_lock(&activity_update_mutex);
    activity_update_mutex.lock();  
    for (unsigned int i=0;i<label_renderer.size();i++) {
        label_renderer[i]->updateNodes(ratio);
    }
//    pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
}

int ACOsgBrowserRenderer::computeScreenCoordinates(osgViewer::View* view, double ratio) //CF: use osgViewer::Viewer* for the simple Viewer
{
   // pthread_mutex_lock(&activity_update_mutex);
    activity_update_mutex.lock();
    int closest_node = 1;//CF to deprecate
    {
        float mx(0.0f), my(0.0f);
        float x(0.0f), y(0.0f), z(0.0f);

        int n = node_renderers.size();

        //osg::Matrix modelModel = view->getModelMatrix();
        osg::Matrix viewMatrix = view->getCamera()->getViewMatrix();
        osg::Matrix projectionMatrix = view->getCamera()->getProjectionMatrix();
        //osg::Matrix window = view->getWindowMatrix();
        osg::Matrix VPM = viewMatrix * projectionMatrix;

        // convertpoints in model coordinates to view coordinates
        // Not necessary to go to screen coordinated because pick function can get normalized mouse coordinates
        osg::Vec3 modelPoint;
        osg::Vec3 screenPoint;

        // SD 2010 OCT - This animation has moved from Browser to Renderer
#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
#define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
        double t = getTime();
        double frac;
        double andur = 1.0;
        //	double alpha = 0.99;
        double omr;


        //printf ("POINTER %d: %f %f\n", p, mx, my);

        //distance_mouse.clear();//CF
        //distance_mouse.resize(n);//CF

        closest_node = -1;
        //for(int i=0; i<n; i++) {
        for(ACOsgMediaRenderers::iterator node_renderer = node_renderers.begin();node_renderer!=node_renderers.end();node_renderer++){
            int i = node_renderer->first;

            ACMediaNode* attribute = media_cycle->getMediaNode(i);

            if(node_renderer->second){
                if (attribute->getChanged()) {
                    if (node_renderer->second->getInitialized()) {
                        node_renderer->second->setCurrentPos(node_renderer->second->getViewPos());
                    }
                    else {
                        node_renderer->second->setCurrentPos(attribute->getCurrentPosition());
                        node_renderer->second->setViewPos(attribute->getCurrentPosition());
                    }
                    node_renderer->second->setNextPos(attribute->getNextPosition());
                    attribute->setChanged(0);
                }

                const ACPoint &p = node_renderer->second->getCurrentPos();
                const ACPoint &p2 = node_renderer->second->getNextPos();
                double refTime = attribute->getNextTime();

                frac = (t-refTime)/andur;
                if (frac<1) {
                    //frac = CUB_FRAC(frac);
                }
                frac = TI_CLAMP(frac, 0, 1);

                omr = 1.0-frac;
                x = omr*p.x + frac*p2.x;
                y = omr*p.y + frac*p2.y;
                z = 0;

                media_cycle_view_pos.x = x;
                media_cycle_view_pos.y = y;

                node_renderer->second->setFrac(frac);
                node_renderer->second->setViewPos(media_cycle_view_pos);

                //attribute->setViewPosition(media_cycle_view_pos);

                modelPoint = Vec3(x,y,z);
                screenPoint = modelPoint * VPM;
                node_renderer->second->setDistanceMouse(1.f);


            }
        }

    }
    for(int p_index=0; p_index<media_cycle->getNumberOfPointers();p_index++){
        float mx(0.0f), my(0.0f);

        float closest_distance = 1000000;
        closest_node = -1;

        float x(0.0f), y(0.0f), z(0.0f);

        int n = media_cycle->getLibrarySize();
        n = node_renderers.size();

        //osg::Matrix modelModel = view->getModelMatrix();
        osg::Matrix viewMatrix = view->getCamera()->getViewMatrix();
        osg::Matrix projectionMatrix = view->getCamera()->getProjectionMatrix();
        //osg::Matrix window = view->getWindowMatrix();
        osg::Matrix VPM = viewMatrix * projectionMatrix;

        // convertpoints in model coordinates to view coordinates
        // Not necessary to go to screen coordinated because pick function can get normalized mouse coordinates
        osg::Vec3 modelPoint;
        osg::Vec3 screenPoint;

        // SD 2010 OCT - This animation has moved from Browser to Renderer
#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
#define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
        double t = getTime();
        double frac;
        double andur = 1.0;
        //	double alpha = 0.99;
        double omr;

        ACPointer* pt = media_cycle->getPointerFromIndex(p_index);
        if (pt){
            mx = pt->getCurrentPosition().x;
            my = pt->getCurrentPosition().y;
        }
        else
            std::cerr << "ACOsgBrowserRenderer::computeScreenCoordinates: couldn't get pointer from index " << p_index << std::endl;

        //printf ("POINTER %d: %f %f\n", p, mx, my);

        //distance_mouse.clear();//CF
        //distance_mouse.resize(n);//CF

        int closest_node = -1;
        //for(int i=0; i<n; i++) {
        for (ACOsgMediaRenderers::iterator node_renderer =node_renderers.begin();node_renderer !=node_renderers.end();node_renderer++)
        {

            int i=node_renderer->first;
            ACMediaNode* attribute = media_cycle->getMediaNode(i );
            /*
   const ACPoint &p = attribute->getCurrentPosition();
   const ACPoint &p2 = attribute->getNextPosition();
   double refTime = attribute->getNextTime();

   frac = (t-refTime)/andur;
   if (frac<1) {
    frac = CUB_FRAC(frac);
   }
   frac = TI_CLAMP(frac, 0, 1);

   omr = 1.0-frac;
   x = omr*p.x + frac*p2.x;
   y = omr*p.y + frac*p2.y;
   z = 0;
   */

            /*
    if (i==1) {
    printf ("POS: %f, %f, %f\n",p.x,p2.x,frac);
    }
    */
            if (attribute==0)
                continue;
            //ACOsgMediaRenderers::iterator node_renderer = node_renderers.find(i);
            if(node_renderer != node_renderers.end()){
                if (attribute->getChanged()) {
                    if (node_renderer->second->getInitialized()) {
                        node_renderer->second->setCurrentPos(node_renderer->second->getViewPos());
                    }
                    else {
                        node_renderer->second->setCurrentPos(attribute->getCurrentPosition());
                        node_renderer->second->setViewPos(attribute->getCurrentPosition());
                    }
                    node_renderer->second->setNextPos(attribute->getNextPosition());
                    attribute->setChanged(0);
                }

                const ACPoint &p = node_renderer->second->getCurrentPos();
                const ACPoint &p2 = node_renderer->second->getNextPos();
                double refTime = attribute->getNextTime();

                frac = (t-refTime)/andur;
                if (frac<1) {
                    //frac = CUB_FRAC(frac);
                }
                frac = TI_CLAMP(frac, 0, 1);

                omr = 1.0-frac;
                x = omr*p.x + frac*p2.x;
                y = omr*p.y + frac*p2.y;
                z = 0;

                media_cycle_view_pos.x = x;
                media_cycle_view_pos.y = y;

                node_renderer->second->setFrac(frac);
                node_renderer->second->setViewPos(media_cycle_view_pos);
                //attribute->setViewPosition(media_cycle_view_pos);

                modelPoint = Vec3(x,y,z);
                screenPoint = modelPoint * VPM;

                // compute distance between mouse and media element in view
                distance_mouse[i] = sqrt((screenPoint[0]-mx)*(screenPoint[0]-mx)+(screenPoint[1]-my)*(screenPoint[1]-my));

                node_renderer->second->setDistanceMouse(MIN(distance_mouse[i],node_renderer->second->getDistanceMouse()));

                if (distance_mouse[i]<closest_distance) {
                    closest_distance = distance_mouse[i];
                    closest_node = node_renderer->second->getNodeIndex();
                }
            }
        }
        media_cycle->setClosestNode(closest_node,p_index);
    }
    
//    pthread_mutex_unlock(&activity_update_mutex);
    activity_update_mutex.unlock();
    return closest_node; //CF to deprecate
}

// private methods

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeNodes(){//private method
    ACOsgMediaRenderers::iterator iterm;
    for (iterm = node_renderers.begin(); iterm != node_renderers.end(); iterm++) {
        if(iterm->second){
            long tempId=iterm->first;
            media_group->removeChild(iterm->second->getNode());
            ACOsgMediaRenderer* tempMedia=iterm->second;
            delete iterm->second;
            iterm->second=0;
        }
    }
    node_renderers.clear();
    return true;
}

//bool ACOsgBrowserRenderer::addNodes(int _first, int _last){
bool ACOsgBrowserRenderer::addNode(long int _id){//private method
    bool ok = false;

    /*if (_first < 0 || _last < _first){
        cerr << "<ACOsgBrowserRenderer::addNodes> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
        ok = false;
    }
    else {

        node_renderers.resize(_last);
        distance_mouse.resize(_last);*/

    ACMediaType media_type;

    if(media_cycle->getMediaNode(_id) == 0){
        std::cerr << "ACOsgBrowserRenderer::addNode: id "<< _id << " does not have a corresponding node in the media browser" << std::endl;
        return false;
    }
    if(media_cycle->getLibrarySize() == 0){
        std::cerr << "ACOsgBrowserRenderer::addNode: the media library is empty" << std::endl;
        return false;
    }
    if(media_cycle->getLibrary()->getMedia(_id) == 0){
        std::cerr << "ACOsgBrowserRenderer::addNode: id "<< _id << " does not have a corresponding media in the media library" << std::endl;
        return false;
    }
    if(node_renderers.find(_id) != node_renderers.end()){
        std::cerr << "ACOsgBrowserRenderer::addNode: node renderer for id "<< _id << " is already present, overriding." << std::endl;
    }
    media_type = media_cycle->getMediaType(_id);

    ACOsgMediaRenderer* renderer = 0;
    /*if(media_cycle->getLibrary()->getMediaType() != MEDIA_TYPE_MIXED || (media_type == MEDIA_TYPE_MIXED||media_type == MEDIA_TYPE_AUDIO) )*/{
        switch (media_type) {
        case MEDIA_TYPE_AUDIO:
#if defined (SUPPORT_AUDIO)
            renderer = new ACOsgAudioRenderer();
#endif //defined (SUPPORT_AUDIO)
            break;
        case MEDIA_TYPE_IMAGE:
#if defined (SUPPORT_IMAGE)
            renderer = new ACOsgImageRenderer();
#endif //defined (SUPPORT_IMAGE)
            break;
        case MEDIA_TYPE_VIDEO:
#if defined (SUPPORT_VIDEO)
            renderer = new ACOsgVideoRenderer();
#endif //defined (SUPPORT_VIDEO)
            break;
        case MEDIA_TYPE_3DMODEL:
#if defined (SUPPORT_3DMODEL)
            renderer = new ACOsg3DModelRenderer();
#endif //defined (SUPPORT_3DMODEL)
            break;
        case MEDIA_TYPE_TEXT:
#if defined (SUPPORT_TEXT)
            renderer = new ACOsgTextRenderer();
#endif //defined (SUPPORT_TEXT)
            break;
        case MEDIA_TYPE_SENSOR:
#if defined (SUPPORT_SENSOR)
            renderer = new ACOsgSensorRenderer();
#endif //defined (SUPPORT_SENSOR)
            break;
        case MEDIA_TYPE_MIXED:
#if defined (SUPPORT_MULTIMEDIA)
            renderer = new ACOsgMediaDocumentRenderer();
#endif //defined (SUPPORT_MULTIMEDIA)
            break;
        }
    }
    if (renderer != 0) {
        renderer->setMediaCycle(media_cycle);
        renderer->setNodeIndex(_id);
        renderer->setFont(font);
        renderer->changeSetting(this->setting);
        media_cycle_node = media_cycle->getMediaNode(_id);
        node_index =  _id;
        media_index = node_index;
        media_cycle_filename = media_cycle->getMediaFileName(_id);
        //renderer->setMediaIndex(media_index);
        renderer->setMedia(media_cycle->getLibrary()->getMedia(_id));
        renderer->setFilename(media_cycle_filename);
        renderer->setActivity(0);
        node_renderers[_id] = renderer;
        //distance_mouse.resize(node_renderer.size());
        renderer->prepareNodes();
        media_group->addChild(renderer->getNode());
        return true;
    }
    else
        std::cerr << "ACOsgBrowserRenderer::addNode: couldn't create a renderer for id "<< _id << " media type unsupported" << std::endl;
    return false;
}

bool ACOsgBrowserRenderer::addLink(long int _id){
    bool ok = false;
    if(media_cycle->getBrowserMode() == AC_MODE_NEIGHBORS){
        // Create new nodelinks if the layout requires them
        if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {

            if(link_renderers.find(_id) != link_renderers.end()){
                std::cerr << "ACOsgBrowserRenderer::addLink: link renderer for id "<< _id << " is already present, overriding." << std::endl;
            }
            else
                link_renderers[_id] = new ACOsgNodeLinkRenderer();

            if (link_renderers[_id]) {
                link_renderers[_id]->setMediaCycle(media_cycle);
                if (node_renderers[_id] == 0){
                    std::cerr << "ACOsgBrowserRenderer::addLink: associated node for link renderer of id "<< _id << " doesn't exist" << std::endl;
                    return false;
                }

                node_index = node_renderers[_id]->getNodeIndex();
                link_renderers[_id]->prepareLinks();

                link_renderers[_id]->setNodeIn(node_renderers[_id]);
                int p = media_cycle->getBrowser()->getParentFromNeighborNode(node_index);
                if ( p!= -1 )
                    link_renderers[_id]->setNodeOut(node_renderers[p]);
                else{
                    std::cerr << "ACOsgBrowserRenderer::addLink: associated parent node for link renderer of id "<< _id << " doesn't exist" << std::endl;
                    return false;
                }

                link_group->addChild(link_renderers[_id]->getLink());
                ok = true;
            }

        }
    }
    return ok;
}

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeLinks(int _first, int _last){
    bool ok = false;
    if (_first < 0 || _last > link_renderers.size() || _last < _first){
        cerr << "<ACOsgBrowserRenderer::removeLinks> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
        ok = false;
    }
    // (default) remove ALL nodes
    else if (_first == 0 && _last==0) {
        ACOsgNodeLinkRenderers::iterator itern;
        for (itern = link_renderers.begin(); itern != link_renderers.end(); itern++) {
            link_group->removeChild(itern->second->getLink());
            delete itern->second;
        }
        link_renderers.clear();
        ok = true;
    }
    else {
        for (int i=_first;i<_last;i++) {
            link_group->removeChild(link_renderers[i]->getLink());
            delete link_renderers[i];
            link_renderers.erase(i);
        }
        //link_renderers.resize(_first);
        ok = true;
    }
    return ok;
}

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeLabels(int _first, int _last){
    bool ok = false;
    if (_first < 0 || _last > label_renderer.size() || _last < _first){
        cerr << "<ACOsgBrowserRenderer::removeLabels> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
        ok = false;
    }
    // (default) remove ALL nodes
    else if (_first == 0 && _last==0) {
        std::vector<ACOsgLabelRenderer*>::iterator iterm;
        for (iterm = label_renderer.begin(); iterm != label_renderer.end(); iterm++) {
            label_group->removeChild((*iterm)->getNode());
            delete *iterm;
        }
        label_renderer.clear();
        ok = true;
    }
    else {
        for (int i=_first;i<_last;i++) {
            label_group->removeChild(label_renderer[i]->getNode());
            delete label_renderer[i];
        }
        label_renderer.resize(_first);
        ok = true;
    }
    return ok;
}

bool ACOsgBrowserRenderer::addLabels(int _first, int _last){
    bool ok = false;
    if (_first < 0 || _last < _first){
        cerr << "<ACOsgBrowserRenderer::addLabels> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
        ok = false;
    }
    else {
        label_renderer.resize(_last);

        for (unsigned int i=_first;i<_last;i++) {
            label_renderer[i] = new ACOsgLabelRenderer();
            if (label_renderer[i]) {
                label_renderer[i]->setText(media_cycle->getLabelText(i));
                label_renderer[i]->setPos(media_cycle->getLabelPos(i));
                label_renderer[i]->setMediaCycle(media_cycle);
                label_renderer[i]->setNodeIndex(i);
                label_renderer[i]->setFont(font);
                label_renderer[i]->prepareNodes();
                label_group->addChild(label_renderer[i]->getNode());
            }
        }
    }
}

void ACOsgBrowserRenderer::setAudioWaveformType(ACBrowserAudioWaveformType _type){
    if (audio_waveform_type != _type){
        this->audio_waveform_type = _type;
        for(ACOsgMediaRenderers::iterator node_renderer = node_renderers.begin();node_renderer!=node_renderers.end();node_renderer++){
            if (node_renderer->second->getMediaType() == MEDIA_TYPE_AUDIO)
                node_renderer->second->updateWaveformType(_type);
        }
        //media_cycle->setNeedsDisplay(true); // done by each waveform
    }
}

void ACOsgBrowserRenderer::changeSetting(ACSettingType _setting)
{
    if(this->setting == _setting)
        return;

    this->setting = _setting;

    for(ACOsgMediaRenderers::iterator node_renderer = node_renderers.begin();node_renderer!=node_renderers.end();node_renderer++){
        node_renderer->second->changeSetting(this->setting);
    }
}
