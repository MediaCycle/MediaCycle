/*
*  ACOsgCompositeViewQt.h
*  MediaCycle
*
*  @author Christian Frisson
*  @date 29/04/10
*  @copyright (c) 2021 – UMONS - Numediart
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
*  Qt QGLWidget and OSG CompositeViewer that wraps
*  a MediaCycle browser and multitrack timeline viewer
*
*/

#ifndef HEADER_AC_COMPOSITE_VIEW_OSG_QT
#define HEADER_AC_COMPOSITE_VIEW_OSG_QT

#include "ACOsgQOpenGLWidget.h"

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
//#include <osgViewer/ViewerEventHandlers>

#include <QtGui>
#include <QtOpenGL/QGLWidget>
using Qt::WindowFlags;

#include <QApplication>
#include <QGestureEvent>

#include <iostream>
#include <MediaCycle.h>
#include <ACOsgBrowserRenderer.h>
#include <ACOsgBrowserEventHandler.h>
#include <ACOsgTimelineRenderer.h>
#include <ACOsgTimelineEventHandler.h>
#include <ACOsgHUDRenderer.h>

#include "ACEventListener.h"

//#include "ui_ACOsgCompositeViewQt.h"

#include <ACInputActionQt.h>
#include <ACAbstractViewQt.h>

class ACOsgCompositeViewQt : public ACOsgQOpenGLWidget, 
// public osgViewer::CompositeViewer, 
public ACEventListener, public ACAbstractViewQt
{
    Q_OBJECT

public slots:
    void initialize();

    // Library
    void openMediaExternally(); // open file using the default application for the media type (to configure)
    void browseMediaExternally(); // view file with the default file browser
    //void examineMediaExternally(); // display properties using the default file browser

    // Browser
    void forwardNextLevel(); // recluster the selected cluster (cluster mode) or unwrap node (network mode)
    void changeReferenceNode();
    //void stopPlayback(); // stop the playback of audio/video files
    void toggleMediaHover(bool toggle); // audio hover, image/video ... -> need a panel to configure the behaviour of hover
    void triggerMediaHover(bool trigger); // audio hover, image/video ... -> need a panel to configure the behaviour of hover
    void resetBrowser(); // reset the browser view (center, rotation, zoom)
    void rotateBrowser(float x, float y);
    void zoomBrowser(float x, float y);
    void translateBrowser(float x, float y);
    void discardMedia();

    // Timeline
    void addMediaOnTimelineTrack();
    void toggleTimelineVisibility();
    void toggleTimelinePlayback(bool toggle); // "transport"
    /// adjust the timeline height ratio, between 0 and 1
    void adjustTimelineHeight(float _ratio_y);

public:
    ACOsgCompositeViewQt( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );
    ~ACOsgCompositeViewQt();
    virtual void clean(/*bool updategl=true*/) override;
    osgViewer::GraphicsWindow* getGraphicsWindow();
    // const osgViewer::GraphicsWindow* getGraphicsWindow();
    void paintGL() override;
    double width();
    double height();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    virtual void initFont();
    // virtual void initializeGL();
    void resizeGL(int w, int h) override;
    virtual void updateGL();
    //virtual bool event( QEvent* event );
    void propagateEventToActions( QEvent* event );
    virtual void keyPressEvent( QKeyEvent* event ) override;
    virtual void keyReleaseEvent( QKeyEvent* event ) override;
    virtual void mousePressEvent( QMouseEvent* event ) override;
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent( QMouseEvent* event ) override;
    osg::ref_ptr<osgViewer::GraphicsWindow> browser_viewer;
    QTimer _timer;

protected:
    // bool event(QEvent *event);
    //void paintEvent(QPaintEvent *event);
    //void resizeEvent(QResizeEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);
    void panTriggered(QPanGesture*);
    void pinchTriggered(QPinchGesture*);
    void swipeTriggered(QSwipeGesture*);

private:
    MediaCycle *media_cycle;
    ACOsgBrowserRenderer *browser_renderer;
    ACOsgBrowserEventHandler *browser_event_handler;
    ACOsgTimelineRenderer *timeline_renderer;
    ACOsgTimelineEventHandler *timeline_event_handler;
    ACOsgHUDRenderer *hud_renderer;
    osg::ref_ptr<osgViewer::View> browser_view;
    osg::ref_ptr<osgViewer::View> hud_view;
    osg::ref_ptr<osgViewer::View> timeline_view;
    osg::ref_ptr<osgText::Font> font;

    ACInputActionQt *openMediaExternallyAction, *browseMediaExternallyAction,
    *examineMediaExternallyAction, *forwardNextLevelAction,*changeReferenceNodeAction,
    /**stopPlaybackAction,*/ *toggleMediaHoverAction, *triggerMediaHoverAction, *resetBrowserAction,
    *rotateBrowserAction, *zoomBrowserAction, *translateBrowserAction,
    *addMediaOnTimelineTrackAction, *toggleTimelinePlaybackAction, *adjustTimelineHeightAction,
    *discardMediaAction;
    //*neighborsOfReferentAction, *clusterAroundReferentAction, *removeMediaAction


    void initInputActions();
public:
    virtual void addInputAction(ACInputActionQt* _action) override;

    // MediaCycle listener callback
    virtual void pluginLoaded(std::string plugin_name) override;
    void mediaImported(int n,int nTot,int mId) override;

private:
    void updateBrowserView(double width, double height);
    void updateHUDCamera(double width, double height);
    void updateTimelineView(double width, double height);

public:
    // needs to be called when medias are added or removed
    virtual void prepareBrowser() override;
    // needs to be called when node positions are changed
    void updateTransformsFromBrowser( double frac);
    void setMediaCycle(MediaCycle* _media_cycle) override;
    // needs to be called when tracks are added or removed
    virtual void prepareTimeline() override;
    // needs to be called when tracks positions are changed
    void updateTransformsFromTimeline( double frac);
    ACOsgBrowserRenderer* getBrowserRenderer(){return browser_renderer;}
    ACOsgHUDRenderer* getHUDRenderer(){return hud_renderer;}
    ACOsgTimelineRenderer* getTimelineRenderer() {return timeline_renderer;}
    virtual ACAbstractBrowserRenderer* getBrowser() override {return browser_renderer;}
    virtual ACAbstractTimelineRenderer* getTimeline() override {return timeline_renderer;}
    virtual bool isLibraryLoaded() override {return library_loaded;}
    virtual void setLibraryLoaded(bool load_status) override {library_loaded = load_status;}

private:
    int mousedown, borderdown;
    float refx, refy;
    float refcamx, refcamy;
    float refzoom, refrotation;
    int septhick; // CF half of the thickness of the border that separates the browser and timeline viewers
    float sepy; //CF location (in OSG coordinates) of the border that separates the browser and timeline viewers
    float refsepy;
    int screen_width;
    bool library_loaded;
    bool mouseover;
    QPoint dragStartPosition;
    bool dragFlag;
    bool mouse_disabled;
    double scaleX, scaleY;

    //MediaBlender specific members:
private:
    int mediaOnTrack;
    bool track_playing;

protected:
    ACSettingType setting;
public:
    virtual void changeSetting(ACSettingType _setting) override;
//CF signals:
//    void importDirectoriesThreaded(std::vector<std::string> directories,bool flag);
};
#endif
