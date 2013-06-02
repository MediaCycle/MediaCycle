/*
*  ACOsgCompositeViewQt.cpp
*  MediaCycle
*
*  @author Christian Frisson
*  @date 29/04/10
*  @copyright (c) 2010 – UMONS - Numediart
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

#define PI 3.1415926535897932384626433832795f

#include "ACOsgCompositeViewQt.h"
#include <cmath>
#include <QDesktopWidget>
#include <ACOsgRendererFactory.h>
#include <ACOsgReadAndShareImageCallback.h>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <ACPluginQt.h>

#include <sstream>

namespace fs = boost::filesystem;

using namespace osg;

ACOsgCompositeViewQt::ACOsgCompositeViewQt( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f)
    : QGLWidget(parent, shareWidget, f),ACEventListener(), media_cycle(0),font(0),
      browser_renderer(0), browser_event_handler(0), timeline_renderer(0), timeline_event_handler(0), hud_renderer(0), hud_view(0),
      mousedown(0), borderdown(0),
      refx(0.0f), refy(0.0f),
      refcamx(0.0f), refcamy(0.0f),
      refzoom(0.0f),refrotation(0.0f),
      septhick(15),sepy(0.0f),refsepy(0.0f),screen_width(0),
      library_loaded(false),mouseover(false),
      mediaOnTrack(-1),track_playing(false),
      openMediaExternallyAction(0), browseMediaExternallyAction(0), examineMediaExternallyAction(0), forwardNextLevelAction(0),changeReferenceNodeAction(0),
      /*stopPlaybackAction(0), */ toggleMediaHoverAction(0), triggerMediaHoverAction(0),
      resetBrowserAction(0), rotateBrowserAction(0), zoomBrowserAction(0),
      translateBrowserAction(0), addMediaOnTimelineTrackAction(0), toggleTimelinePlaybackAction(0), adjustTimelineHeightAction(0),
      discardMediaAction(0),
      setting(AC_SETTING_NONE)
{
#ifdef UNIX
    osg::setNotifyLevel(osg::DEBUG_INFO);
#endif

    osg_view = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
    setFocusPolicy(Qt::StrongFocus);// CF instead of ClickFocus

    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    /*
  CF: other threading models to test are:
   SingleThreaded
   CullDrawThreadPerContext
   ThreadPerContext
   DrawThreadPerContext
   CullThreadPerCameraDrawThreadPerContext
   ThreadPerCamera
   AutomaticSelection
  */
    this->setRunFrameScheme( osgViewer::Viewer::ON_DEMAND );

    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    _timer.start(10);

    // Share identical images between renderers
    //osgDB::Registry::instance()->setReadFileCallback( new ACOsgReadAndShareImageCallback);
    //osgDB::Registry::instance()->getOrCreateSharedStateManager();

    // Renderers
    browser_renderer = new ACOsgBrowserRenderer();
    timeline_renderer = new ACOsgTimelineRenderer();
    ACOsgRendererFactory::getInstance().setBrowserRenderer(browser_renderer);
    ACOsgRendererFactory::getInstance().setTimelineRenderer(timeline_renderer);
    hud_view = new osgViewer::View;
    hud_renderer = new ACOsgHUDRenderer();

    // Fonts:
    this->initFont();
    if(font){
        hud_renderer->setFont(font);
        timeline_renderer->setFont(font);
        browser_renderer->setFont(font);
    }

    //this->setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true); //CF necessary for the hover callback

    // Views
    sepy = 0;//height()/4;// CF browser/timeline proportions at startup

    browser_view = new osgViewer::View;
    browser_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
    this->updateBrowserView(width(),height());
    this->addView(browser_view);

    timeline_view = new osgViewer::View;
    timeline_view->getCamera()->setGraphicsContext(this->getGraphicsWindow());
    this->updateTimelineView(width(),height());
    this->addView(timeline_view);

    // full screen antialiasing (if supported)
    //osg::DisplaySettings::instance()->setNumMultiSamples( 4 );

    // Event handlers
    browser_event_handler = new ACOsgBrowserEventHandler;
    browser_view->addEventHandler(browser_event_handler);

    timeline_event_handler = new ACOsgTimelineEventHandler;
    timeline_view->addEventHandler(timeline_event_handler); // CF ((osgViewer::Viewer*) (this))->addEventHandler for the simple Viewer
    timeline_event_handler->setRenderer(timeline_renderer);

    // HUD renderer
    hud_renderer->getCamera()->setGraphicsContext(this->getGraphicsWindow());
    hud_view->setCamera(hud_renderer->getCamera());
    this->updateHUDCamera(width(),height());
    this->addView(hud_view);

    // Audio waveforms
    screen_width = QApplication::desktop()->screenGeometry().width();
    timeline_renderer->setScreenWidth(screen_width);
    timeline_renderer->updateSize(width(),sepy);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);

    osg::setNotifyLevel(osg::WARN);//remove the NaN CullVisitor messages
    this->initInputActions();
    
    setAcceptDrops(true);
    //setRunFrameScheme( osgViewer::Viewer::ON_DEMAND );
    dragFlag=false;
}

void ACOsgCompositeViewQt::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}
void ACOsgCompositeViewQt::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()&&dragFlag==false)
    {
        qDebug()<<event->mimeData()->urls().size();
        qDebug()<<event->mimeData()->urls()[0].toString();
        
        std::vector<string> directories;
        for (unsigned int i=0;i<event->mimeData()->urls().size();i++){
            QFileInfo filename(event->mimeData()->urls()[i].toLocalFile());
            qDebug()<<filename.fileName();
            if (filename.exists()&&filename.suffix().size()>0){
                std::vector<std::string> mediaExt = media_cycle->getExtensionsFromMediaType( media_cycle->getLibrary()->getMediaType() );
                
                if(mediaExt.size()==0){
                    qDebug()<<"ACOsgCompositeViewQt::dropEvent No file extensions supported for this media type. Please check the media factory. Can't import media files.";
                    continue;
                }
                
                QString mediaExts = "Supported Extensions (";
                std::vector<std::string>::iterator mediaIter = mediaExt.begin();
                for(;mediaIter!=mediaExt.end();++mediaIter){
                    if (mediaIter != mediaExt.begin())
                        mediaExts.append(" ");
                    mediaExts.append("*");
                    mediaExts.append(QString((*mediaIter).c_str()));
                }
                mediaExts.append(")");
                
                qDebug()<<mediaExts;
                if (mediaExts.contains(filename.suffix()))
                {
                    qDebug()<<filename.path();
                    qDebug()<<filename.baseName();
                    qDebug()<<filename.suffix();
                    directories.push_back(filename.absoluteFilePath().toStdString());
                }
                
            }
        }
        if (!(directories.empty())){
            emit (importDirectoriesThreaded(directories,false));
            for (std::vector<string>::reverse_iterator iter=directories.rbegin(); iter!=directories.rend();iter++){
                int locId;
                
                locId=media_cycle->getLibrary()->getMediaIndex(*iter);
                
                if (locId>=0){
                        media_cycle->setReferenceNode(locId);
                    break;
                
                }
            }
            directories.empty();
        }
        
    }
    else{
        dragFlag=false;
        int button = 1;
        QEvent *tempEvent=new QEvent(QEvent::MouseButtonRelease);
        this->propagateEventToActions(tempEvent);
        delete tempEvent;
        osg_view->getEventQueue()->mouseButtonRelease(event->pos().x(), event->pos().y(), button);
        std::cout << "ACOsgCompositeViewQt::mouseReleaseEvent clicked node " << media_cycle->getClickedNode() << std::endl;
        if (media_cycle == 0) return;
        if (media_cycle->getClickedNode()>-1)
            media_cycle->setClickedNode(-1);
        mousedown = 0;
        borderdown = 0;
        media_cycle->setNeedsDisplay(true);
    }
}


void ACOsgCompositeViewQt::updateBrowserView(int _width, int _height){
    if (browser_view){
        browser_view->getCamera()->setViewport(new osg::Viewport(0,sepy,_width,_height-sepy)); // CF: for OSG y=0 is on the bottom, for Qt on the top
        browser_view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width())/static_cast<double>(_height-sepy), 0.001f, 10.0f);
        browser_view->getCamera()->getViewMatrix().makeIdentity();
        browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(0,0,0.8), Vec3(0,0,0), Vec3(0,1,0));
        //browser_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
    }
}

void ACOsgCompositeViewQt::updateHUDCamera(int _width, int _height){
    if(hud_view){
        hud_view->getCamera()->setViewport(new osg::Viewport(0,sepy,_width,_height)); // CF: for OSG y=0 is on the bottom, for Qt on the top
        hud_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,_width, 0, _height));
    }
}

void ACOsgCompositeViewQt::updateTimelineView(int _width, int _height){
    if (timeline_view){
        //timeline_view->getCamera()->setClearColor(Vec4f(0.0,0.0,0.0,0.0));
        timeline_view->getCamera()->setClearColor(Vec4f(0.14,0.14,0.28,1.0));
        timeline_view->getCamera()->setViewport(new osg::Viewport(0,0,_width,sepy));
        //orth2D
        timeline_view->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,_width,0,sepy));
        timeline_view->getCamera()->setViewMatrix(osg::Matrix::identity());
    }
}

ACOsgCompositeViewQt::~ACOsgCompositeViewQt(){
    //browser_view->removeEventHandler(browser_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
    //timeline_view->removeEventHandler(timeline_event_handler); // reqs OSG >= 2.9.x and shouldn't be necessary
    this->clean(false);
    delete browser_renderer; browser_renderer = 0;
    //if (browser_event_handler) delete browser_event_handler;
    browser_event_handler = 0;
    delete timeline_renderer; timeline_renderer = 0;
    //if (timeline_event_handler) delete timeline_event_handler;
    timeline_event_handler = 0;
    delete hud_renderer; hud_renderer = 0;
    media_cycle = 0;
    if(openMediaExternallyAction) delete openMediaExternallyAction; openMediaExternallyAction = 0;
    if(browseMediaExternallyAction) delete browseMediaExternallyAction; browseMediaExternallyAction = 0;
    if(examineMediaExternallyAction) delete examineMediaExternallyAction; examineMediaExternallyAction = 0;
    if(forwardNextLevelAction) delete forwardNextLevelAction; forwardNextLevelAction = 0;
    if(changeReferenceNodeAction) delete changeReferenceNodeAction; changeReferenceNodeAction = 0;
    //if(stopPlaybackAction) delete stopPlaybackAction; stopPlaybackAction = 0;
    if(toggleMediaHoverAction) delete toggleMediaHoverAction; toggleMediaHoverAction = 0;
    if(resetBrowserAction) delete resetBrowserAction; resetBrowserAction = 0;
    if(rotateBrowserAction) delete rotateBrowserAction; rotateBrowserAction = 0;
    if(zoomBrowserAction) delete zoomBrowserAction; zoomBrowserAction = 0;
    if(translateBrowserAction) delete translateBrowserAction; translateBrowserAction = 0;
    if(addMediaOnTimelineTrackAction) delete addMediaOnTimelineTrackAction; addMediaOnTimelineTrackAction = 0;
    if(toggleTimelinePlaybackAction) delete toggleTimelinePlaybackAction; toggleTimelinePlaybackAction = 0;
    if(adjustTimelineHeightAction) delete adjustTimelineHeightAction; adjustTimelineHeightAction = 0;
    if(discardMediaAction) delete discardMediaAction; discardMediaAction = 0;
}

void ACOsgCompositeViewQt::clean(bool updategl){
    browser_event_handler->clean();
    timeline_event_handler->clean();
    mousedown = borderdown = 0;
    refx =  refy = refcamx = refcamy = refzoom = refrotation = 0.0f;
    //browser_renderer->clean();
    timeline_renderer->clean();
    hud_renderer->clean();
    //if (browser_event_handler) delete browser_event_handler;
    //if (timeline_event_handler) delete timeline_event_handler;
    sepy = 0;
    if (updategl)
        this->updateGL();
    library_loaded = false;
}

void ACOsgCompositeViewQt::setMediaCycle(MediaCycle* _media_cycle)
{
    media_cycle = _media_cycle;
    media_cycle->addListener(this);
    browser_renderer->setMediaCycle(media_cycle);
    timeline_renderer->setMediaCycle(media_cycle);
    browser_event_handler->setMediaCycle(media_cycle);
    ACOsgRendererFactory::getInstance().setMediaCycle(media_cycle);
    timeline_event_handler->setMediaCycle(media_cycle);
    hud_renderer->setMediaCycle(media_cycle);
}

void ACOsgCompositeViewQt::initFont()
{
    //CF this should be done once per application runtime, for instance in the browser renderer or composite viewer
    font = 0;
    std::string font_path(""),font_file("fudd.ttf");

#ifdef __WIN32__
    font_path = "C:\\Windows\\Fonts\\";
    font_file = "Arial.ttf";
#else
#ifdef USE_DEBUG
    boost::filesystem::path s_path( __FILE__ );
    font_path = s_path.parent_path().parent_path().parent_path().string() + "/data/fonts/";
#else
#ifdef __APPLE__
    boost::filesystem::path e_path( getExecutablePath() );
    font_path = e_path.parent_path().parent_path().string() + "/Resources/fonts/";
#else
    font_path = "/usr/share/mediacycle/fonts/";
#endif
#endif
#endif
    std::cout << "Current font path " << font_path << std::endl;
    font = osgText::readFontFile(font_path + font_file);
    if(!font)
        std::cerr << "ACOsgCompositeViewQt::initFont: couldn't load font " << std::endl;
}

void ACOsgCompositeViewQt::initializeGL()
{
    /*if (getGraphicsWindow()->isRealized()) {
  unsigned int _screen_width, _screen_height;
  if ( screen_width != _screen_width){
   _screen_width = timeline_view->getCamera()->getGraphicsContext()->getTraits()->width;
   std::cout << "Initial width: " << _screen_width << std::endl;
   //this->screen_width = _screen_width;
   //timeline_renderer->updateScreenWidth(_screen_width);
  }
 }*/
}

void ACOsgCompositeViewQt::resizeGL( int w, int h )
{
    if (isRealized()){
        //std::cout << "height() " << browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height() << " height " << height << std::endl;
        float prevheight = browser_view->getCamera()->getViewport()->height()+timeline_view->getCamera()->getViewport()->height();
        sepy *= h/prevheight;
        timeline_renderer->updateSize(width(),sepy);
    }

    osg_view->getEventQueue()->windowResize(0, 0, w, h);
    osg_view->resized(0,0,w,h);

    if (isRealized()){
        this->updateBrowserView(w,h);
        this->updateTimelineView(w,h);
        this->updateHUDCamera(w,h);
        //hud_renderer->getCamera()->setProjectionMatrix(osg::Matrix::ortho2D(0,w,0,h));
        /*
   if (getGraphicsWindow()->isRealized()) {

    unsigned int _screen_width, _screen_height;
    if ( screen_width != _screen_width){
     _screen_width = timeline_view->getCamera()->getGraphicsContext()->getTraits()->width;
     std::cout << "Updating width: " << _screen_width << std::endl;
     this->screen_width = _screen_width;
     timeline_renderer->updateScreenWidth(_screen_width);
    }
   }
   */
    }
}

// CF to do: understand paintGL vs updateGL to use them more correctly
void ACOsgCompositeViewQt::paintGL()
{
    browser_renderer->mutexLock();
    timeline_renderer->mutexLock();
    try{
        frame(); // put this first otherwise we don't get a clean background in the browser
    }
    catch(...){
        std::cerr << "ACOsgCompositeViewQt::paintGL: couldn't update frame "<< std::endl;
    }
    browser_renderer->mutexLock();
    timeline_renderer->mutexLock();
    if (media_cycle == 0) return;

    //CF to improve, we want to know if the view is being animated to force a frequent refresh of the positions:
    //SD 2010feb22 to allow auto update whith threaded import
    //if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getState() == AC_CHANGING)
    updateTransformsFromBrowser(media_cycle->getFrac());
}

// called according to timer
void ACOsgCompositeViewQt::updateGL()
{
    browser_renderer->mutexLock();
    timeline_renderer->mutexLock();
    double frac = 0.0;
   
    if (media_cycle == 0) return;

    if(media_cycle && media_cycle->hasBrowser())
    {
        // Automatically remove the mouse pointer if installation setting, adding it otherwise
        // Moved to ACOsgCompositeViewQt::changeSetting

        // Automatically erase the mouse pointer if outside the view for installations, keep it for other settings
        if(setting == AC_SETTING_INSTALLATION){
            if(mouseover!=this->underMouse()){
                if(this->underMouse())// && media_cycle->hasBrowser() && media_cycle->getBrowser()->hasMousePointer() == false)//Qt
                    media_cycle->getBrowser()->addMousePointer();
                else { //if(this->underMouse() == false && media_cycle->hasBrowser() && media_cycle->getBrowser()->hasMousePointer() == true){
                    media_cycle->getBrowser()->removeMousePointer();
                }
                //std::cout << "Mouse " << this->underMouse() << std::endl;
                mouseover=this->underMouse();
            }
        }

        media_cycle->updateState();
        frac = media_cycle->getFrac();

        //CF this is a temporary solution until we implement signals/slots in the core
        if(media_cycle->getBrowser()->getModeChanged()){
            //std::cout << "ACOsgCompositeViewQt::updateGL intercepted browsing mode change" << std::endl;
            //browser_renderer->clean();
        }
    }

    if (!media_cycle->getNeedsDisplay()) {
        return;
    }

    if(browser_view->getCamera() && media_cycle && media_cycle->hasBrowser())
    {

        float x=0.0, y=0.0, zoom, angle;
        float upx, upy;

        zoom = media_cycle->getCameraZoom();
        angle = media_cycle->getCameraRotation();
        media_cycle->getCameraPosition(x, y);
        upx = cos(-angle+PI/2);
        upy = sin(-angle+PI/2);

        this->updateBrowserView(width(),height());
        browser_view->getCamera()->setViewMatrixAsLookAt(Vec3(x*1.0,y*1.0,0.8 / zoom), Vec3(x*1.0,y*1.0,0), Vec3(upx, upy, 0));
        this->updateTimelineView(width(),height());
        this->updateHUDCamera(width(),height());
    }

    this->updateTransformsFromBrowser(frac);
    this->updateTransformsFromTimeline(frac);
    /*
 if (frac != 0.0)
  setMouseTracking(true); //CF necessary for the hover callback
 */
    QGLWidget::updateGL();
    media_cycle->setNeedsDisplay(false);
    browser_renderer->mutexLock();
    timeline_renderer->mutexLock();
}

void ACOsgCompositeViewQt::addInputAction(ACInputActionQt* _action)
{
    //this->addAction(dynamic_cast<QAction*>(_action));
    this->addAction(_action);
    this->inputActions.append(_action);
}

void ACOsgCompositeViewQt::pluginLoaded(std::string plugin_name){
    std::cout << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << std::endl;
    QObject* qobject = dynamic_cast<QObject*>(media_cycle->getPluginManager()->getPlugin(plugin_name));
    if(!qobject){
        //std::cerr << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " not an ACPluginQt" << std::endl;
        return;
    }
    //std::cout << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " is a QObject" << std::endl;
    ACPluginQt* plugin = qobject_cast<ACPluginQt*>(qobject);
    if(!plugin){
        //std::cerr << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " not an ACPluginQt" << std::endl;
        return;
    }
    plugin->setBrowserRenderer(this->browser_renderer);
    plugin->setTimelineRenderer(this->timeline_renderer);
    std::cout << "ACOsgCompositeViewQt::pluginLoaded: " << plugin_name << " is an ACPluginQt" << std::endl;
    std::vector<ACInputActionQt*> inputActions = plugin->providesInputActions();
    for(std::vector<ACInputActionQt*>::iterator inputAction = inputActions.begin(); inputAction != inputActions.end(); inputAction++){
        std::cout << "ACOsgCompositeViewQt::pluginLoaded: adding action from " << plugin_name << std::endl;
        if(*inputAction !=0){
            (*inputAction)->setParent(this);
            this->addInputAction(*inputAction);
        }
        else
            std::cerr << "ACOsgCompositeViewQt::pluginLoaded: warning, malformed action from plugin " << plugin_name << std::endl;

    }
}

void ACOsgCompositeViewQt::initInputActions(){

    // CF add extra shortcuts from QKeySequence::StandardKey

    openMediaExternallyAction = new ACInputActionQt(tr("Open Media File"), this);
    openMediaExternallyAction->setShortcut(Qt::Key_O);
    openMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    openMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    openMediaExternallyAction->setToolTip(tr("Open the media file with the default application"));
    connect(openMediaExternallyAction, SIGNAL(triggered()), this, SLOT(openMediaExternally()));
    this->addInputAction(openMediaExternallyAction);

    browseMediaExternallyAction = new ACInputActionQt(tr("Browse Media File"), this);
    browseMediaExternallyAction->setShortcut(Qt::Key_F);
    browseMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    browseMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    browseMediaExternallyAction->setToolTip(tr("Browse the media file with the default file browser"));
    connect(browseMediaExternallyAction, SIGNAL(triggered()), this, SLOT(browseMediaExternally()));
    this->addInputAction(browseMediaExternallyAction);

    /*examineMediaExternallyAction = new ACInputActionQt(tr("Examine Media File"), this);
    examineMediaExternallyAction->setShortcut(Qt::Key_I);
    examineMediaExternallyAction->setKeyEventType(QEvent::KeyPress);
    examineMediaExternallyAction->setMouseEventType(QEvent::MouseButtonRelease);
    examineMediaExternallyAction->setToolTip(tr("Examine the media file with the default file browser properties"));
    connect(examineMediaExternallyAction, SIGNAL(triggered()), this, SLOT(examineMediaExternally()));
    this->addInputAction(examineMediaExternallyAction);*/

    //CF separate this clusters/neighbors action into two?
    forwardNextLevelAction = new ACInputActionQt(tr("Recluster/Unwrap"), this);
    forwardNextLevelAction->setToolTip(tr("Recluster the cluster or unwrap neighbors around the selected node"));
    forwardNextLevelAction->setShortcut(Qt::Key_A);
    forwardNextLevelAction->setKeyEventType(QEvent::KeyPress);
    forwardNextLevelAction->setMouseEventType(QEvent::MouseButtonPress);
    connect(forwardNextLevelAction, SIGNAL(triggered(bool)), this, SLOT(forwardNextLevel()));
    this->addInputAction(forwardNextLevelAction);
    
    changeReferenceNodeAction = new ACInputActionQt(tr("Change reference node"), this);
    changeReferenceNodeAction->setToolTip(tr("Recluster the cluster with another reference node or unwrap neighbors around the selected node"));
    changeReferenceNodeAction->setShortcut(Qt::Key_S);
    changeReferenceNodeAction->setKeyEventType(QEvent::KeyPress);
    changeReferenceNodeAction->setMouseEventType(QEvent::MouseButtonPress);
    connect(changeReferenceNodeAction, SIGNAL(triggered(bool)), this, SLOT(changeReferenceNode()));
    this->addInputAction(changeReferenceNodeAction);    
    
    /*stopPlaybackAction = new ACInputActionQt(tr("Stop Playback"), this);
    stopPlaybackAction->setShortcut(Qt::Key_M);
    stopPlaybackAction->setKeyEventType(QEvent::KeyPress);
    stopPlaybackAction->setToolTip(tr("Stop the playback of all played media nodes"));
    connect(stopPlaybackAction, SIGNAL(triggered()), this, SLOT(stopPlayback()));
    this->addInputAction(stopPlaybackAction);*/

    toggleMediaHoverAction = new ACInputActionQt(tr("Toggle Media Hover"), this);
    toggleMediaHoverAction->setShortcut(Qt::Key_W);
    toggleMediaHoverAction->setKeyEventType(QEvent::KeyPress);
    toggleMediaHoverAction->setToolTip(tr("Toggle Media Hover (faster browsing with playback and magnification)"));
    connect(toggleMediaHoverAction, SIGNAL(toggled(bool)), this, SLOT(toggleMediaHover(bool)));
    this->addInputAction(toggleMediaHoverAction);

    triggerMediaHoverAction = new ACInputActionQt(tr("Trigger Media Hover"), this);
    triggerMediaHoverAction->setShortcut(Qt::Key_Q);
    triggerMediaHoverAction->setKeyEventType(QEvent::KeyPress);
    //triggerMediaHoverAction->setAutoRepeat(false); // works on OSX but not Ubuntu (10.04)
    triggerMediaHoverAction->setToolTip(tr("Trigger Media Hover (faster browsing with playback and magnification)"));
    connect(triggerMediaHoverAction, SIGNAL(triggered(bool)), this, SLOT(triggerMediaHover(bool)));
    this->addInputAction(triggerMediaHoverAction);

    discardMediaAction = new ACInputActionQt(tr("Toggle Discard Media File"), this);
    discardMediaAction->setToolTip(tr("When discarded, the media file stays visible in the browser in black, but won't be saved in the XML library"));
    discardMediaAction->setShortcut(Qt::Key_D);
    discardMediaAction->setKeyEventType(QEvent::KeyPress);
    discardMediaAction->setMouseEventType(QEvent::MouseButtonPress);
    connect(discardMediaAction, SIGNAL(triggered(bool)), this, SLOT(discardMedia()));
    this->addInputAction(discardMediaAction);

    resetBrowserAction = new ACInputActionQt(tr("Reset Browser"), this);
    resetBrowserAction->setShortcut(Qt::Key_C);
    resetBrowserAction->setKeyEventType(QEvent::KeyRelease);
    resetBrowserAction->setToolTip(tr("Reset the browser view (center, rotation, zoom)"));
    connect(resetBrowserAction, SIGNAL(triggered()), this, SLOT(resetBrowser()));
    this->addInputAction(resetBrowserAction);

    rotateBrowserAction = new ACInputActionQt(tr("Rotate Browser"), this);
    rotateBrowserAction->setShortcut(Qt::Key_R);
    rotateBrowserAction->setKeyEventType(QEvent::KeyPress);
    rotateBrowserAction->setMouseEventType(QEvent::MouseMove);
    rotateBrowserAction->setToolTip(tr("Rotate the browser view"));
    connect(rotateBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(rotateBrowser(float,float)));
    this->addInputAction(rotateBrowserAction);

    zoomBrowserAction = new ACInputActionQt(tr("Zoom Browser"), this);
    //zoomBrowserAction->setShortcut(Qt::Key_Z);
    zoomBrowserAction->setShortcut(tr("z"));
    zoomBrowserAction->setKeyEventType(QEvent::KeyPress);
    zoomBrowserAction->setMouseEventType(QEvent::MouseMove);
    zoomBrowserAction->setToolTip(tr("Zoom the browser view"));
    connect(zoomBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(zoomBrowser(float,float)));
    this->addInputAction(zoomBrowserAction);

    translateBrowserAction = new ACInputActionQt(tr("Translate Browser"), this);
    translateBrowserAction->setMouseEventType(ACEventQt::MousePressedMove);
    translateBrowserAction->setToolTip(tr("Translate the browser view"));
    connect(translateBrowserAction, SIGNAL(mouseMovedXY(float,float)), this, SLOT(translateBrowser(float,float)));
    this->addInputAction(translateBrowserAction);

    addMediaOnTimelineTrackAction = new ACInputActionQt(tr("Timeline Media"), this);
    addMediaOnTimelineTrackAction->setShortcut(Qt::Key_T);
    addMediaOnTimelineTrackAction->setKeyEventType(QEvent::KeyPress);
    addMediaOnTimelineTrackAction->setMouseEventType(QEvent::MouseButtonRelease);
    addMediaOnTimelineTrackAction->setToolTip(tr("Visualize media on a timeline track"));
    connect(addMediaOnTimelineTrackAction, SIGNAL(triggered()), this, SLOT(addMediaOnTimelineTrack()));
    this->addInputAction(addMediaOnTimelineTrackAction);

    toggleTimelinePlaybackAction = new ACInputActionQt(tr("Toggle Timeline Playback"), this);
    toggleTimelinePlaybackAction->setShortcut(Qt::Key_Space);
    toggleTimelinePlaybackAction->setKeyEventType(QEvent::KeyPress);
    toggleTimelinePlaybackAction->setToolTip(tr("Toggle timeline playback"));
    connect(toggleTimelinePlaybackAction, SIGNAL(toggled(bool)), this, SLOT(toggleTimelinePlayback(bool)));
    this->addInputAction(toggleTimelinePlaybackAction);

    /*adjustTimelineHeightAction = new ACInputActionQt(tr("Adjust Timeline Height"), this);
    //adjustTimelineHeightAction->setShortcut(Qt::Key_Z);
    //adjustTimelineHeightAction->setKeyEventType(QEvent::KeyPress);
    adjustTimelineHeightAction->setMouseEventType(QEvent::MouseMove);
    adjustTimelineHeightAction->setToolTip(tr("Adjust the timeline height"));
    connect(adjustTimelineHeightAction, SIGNAL(mouseMovedY(float)), this, SLOT(adjustTimelineHeight(float)));
    this->addInputAction(adjustTimelineHeightAction);*/
}

void ACOsgCompositeViewQt::openMediaExternally(){
    //std::cout << "Open externally " << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        int media_id = media_cycle->getClickedNode();
        //int media_id = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << media_id << " selected" << std::endl;

        if(media_id >= 0)
        {
#if defined (__APPLE__)
            std::stringstream command;
            ACMediaType _media_type = media_cycle->getLibrary()->getMedia(media_id)->getMediaType();
            if (_media_type == MEDIA_TYPE_IMAGE || _media_type == MEDIA_TYPE_VIDEO)
                command << "open -a Preview '";
            else if (_media_type == MEDIA_TYPE_TEXT)
                command << "open '"; // uses TextEdit or other default text application if customized OS-wide
            else
                command << "open -R '"; // no iTunes for audio! 3Dmodel default applications?
            command << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << "'" ;
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << " with the OSX-wide prefered application: " << e.what() << endl;
            }
#endif //defined (__APPLE__)
        }
    }

}

void ACOsgCompositeViewQt::browseMediaExternally(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        //int media_id = media_cycle->getClickedNode();
        int media_id = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << media_id << " selected" << std::endl;

        if(media_id >= 0)
        {
#if defined (__APPLE__)
            std::stringstream command;
            //command << "open " << fs::path(media_cycle->getLibrary()->getMedia(media_id)->getFileName()).parent_path();// opens the containing directory using the Finder
            command << "open -R '" << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << " with the OSX Finder: " << e.what() << endl;
            }
#endif //defined (__APPLE__)
        }
    }
}

/*
void ACOsgCompositeViewQt::examineMediaExternally(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        //int media_id = media_cycle->getClickedNode();
        int media_id = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        //std::cout << "node " << media_id << " selected" << std::endl;

        if(media_id >= 0)
        {
            #if defined (__APPLE__)
            std::stringstream command;
            //command << "open " << fs::path(media_cycle->getLibrary()->getMedia(media_id)->getFileName()).parent_path();// opens the containing directory using the Finder
            command << "open -R '" << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << "'" ;// opens the containing directory using the Finder and highlights the file!
            try {
                system(command.str().c_str());
            }
            catch (const exception& e) {
                cout << "ACOsgCompositeViewQt: caught exception while trying to open media file " << media_cycle->getLibrary()->getMedia(media_id)->getFileName() << " with the OSX Finder: " << e.what() << endl;
            }
            #endif //defined (__APPLE__)
        }
    }
}*/

void ACOsgCompositeViewQt::forwardNextLevel(){
    if (media_cycle == 0) return;
    std::cout << "ACOsgCompositeViewQt::forwardNextLevel" << std::endl;
    media_cycle->forwardNextLevel();
}
void ACOsgCompositeViewQt::changeReferenceNode(){
    if (media_cycle == 0) return;
    std::cout << "ACOsgCompositeViewQt::changeReferenceNode" << std::endl;
    media_cycle->changeReferenceNode();
}


/*void ACOsgCompositeViewQt::stopPlayback(){
    if (media_cycle == 0) return;
    media_cycle->setAutoPlay(0);
    media_cycle->muteAllSources();
}*/

void ACOsgCompositeViewQt::toggleMediaHover(bool toggle){
    if (media_cycle == 0) return;
    //std::cout << "Toggle Media hover " << toggle << std::endl;
    media_cycle->setAutoPlay(toggle);
}

void ACOsgCompositeViewQt::triggerMediaHover(bool trigger){
    if (media_cycle == 0) return;
    //std::cout << "Trigger Media hover " << trigger << std::endl;
    media_cycle->setAutoPlay(trigger);
}


void ACOsgCompositeViewQt::resetBrowser(){
    if (media_cycle == 0) return;
    media_cycle->setCameraRecenter();
}

void ACOsgCompositeViewQt::rotateBrowser(float x, float y){
    if (media_cycle == 0) return;
    float rotation = atan2(-(y-this->height()/2),x-this->width()/2)-atan2(-(refy-this->height()/2),refx-this->width()/2);
    media_cycle->setCameraRotation(refrotation + rotation);
}

void ACOsgCompositeViewQt::zoomBrowser(float x, float y){
    if (media_cycle == 0) return;
    //float refzoom(0),refy(0);
    media_cycle->setCameraZoom(refzoom - (y-refy)/50);
    //media_cycle->setCameraZoom(refzoom + (y-refy) / abs (y-refy) * sqrt( pow((y-refy),2) + pow((x-refx),2) )/50 );
}

void ACOsgCompositeViewQt::translateBrowser(float x, float y){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        // CF don't translate when in the timeline
        if (!( (y >= height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() ) ) && (y <= height() - ( browser_view->getCamera()->getViewport()->y() + septhick) ))) // if clicked on browser view far enough of the central border
            return;
        //if (y>sepy) // CF find better check (mouse in widget test?)

        int media_id = media_cycle->getClickedNode();
        //int media_id = media_cycle->getClosestNode();//CF to deprecate: adapt to multiple pointers
        if(media_id == -1)
        {
            //float refx(0),refy(0);
            float zoom = media_cycle->getCameraZoom();
            float angle = media_cycle->getCameraRotation();
            float xmove = (refx-x);
            float ymove =-(refy-y);
            float xmove2 = xmove*cos(-angle)-ymove*sin(-angle);
            float ymove2 = ymove*cos(-angle)+xmove*sin(-angle);
            float camera_x = refcamx + xmove2/800/zoom;
            float camera_y = refcamy + ymove2/800/zoom;
            //std::cout << "ACOsgCompositeViewQt::translateBrowser < " << x <<"->" << camera_x << " y " << y << "->" << camera_y << std::endl;
            media_cycle->setCameraPosition(camera_x , camera_y);

        }
    }
}

void ACOsgCompositeViewQt::addMediaOnTimelineTrack(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        int media_id = media_cycle->getClickedNode();
        
        if (mediaOnTrack != -1)
            this->getBrowserRenderer()->resetNodeColor(mediaOnTrack);
        
        mediaOnTrack = media_id;
        if (mediaOnTrack != -1){
            this->getBrowserRenderer()->changeNodeColor(mediaOnTrack, Vec4(1.0,1.0,1.0,1.0));//CF color the node of the media on track in white
            
            //if ( timeline_renderer->getTrack(0)!=0 )
            //{
            
            if (sepy==0)
            {
                sepy = height()/4;// CF browser/timeline proportions at startup
                timeline_renderer->updateSize(width(),sepy);
                this->updateBrowserView(width(),height());
                this->updateTimelineView(width(),height());
                this->updateHUDCamera(width(),height());
                
                media_cycle->setNeedsDisplay(true);
            }
            //if (track_playing) {
            
            //media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
            //track_playing = false;
            //}
            if (timeline_renderer->getNumberOfTracks()==0){
                //this->getTimelineRenderer()->addTrack(media_id);
                this->getTimelineRenderer()->addTrack(media_cycle->getLibrary()->getMedia(media_id));
            }
            else {
                //this->getTimelineRenderer()->getTrack(0)->updateMedia( media_id ); //media_cycle->getLibrary()->getMedia(media_id) );
                ACOsgTrackRenderer* track = 0;
                track = this->getTimelineRenderer()->getTrack(0);
                if(track)
                    track->updateMedia(media_cycle->getLibrary()->getMedia(media_id));
            }
            media_cycle->setNeedsDisplay(true);
        }
    }
}

void ACOsgCompositeViewQt::toggleTimelineVisibility(){
    if (media_cycle == 0) return;
    if (media_cycle->hasBrowser())
    {
        if (sepy==0)
            sepy = height()/4;// CF browser/timeline proportions at startup
        else
            sepy = 0;

        timeline_renderer->updateSize(width(),sepy);
        this->updateBrowserView(width(),height());
        this->updateTimelineView(width(),height());
        this->updateHUDCamera(width(),height());

        media_cycle->setNeedsDisplay(true);
    }
}

void ACOsgCompositeViewQt::adjustTimelineHeight(float _ratio_y){
    if (media_cycle == 0) return;
    if (!media_cycle->hasBrowser())return;

    if (_ratio_y<=0.0f)
        _ratio_y = 0.0f;
    if (_ratio_y>=1.0f)
        _ratio_y = 1.0f;

    this->sepy = _ratio_y*height();

    timeline_renderer->updateSize(width(),sepy);
    this->updateBrowserView(width(),height());
    this->updateTimelineView(width(),height());
    this->updateHUDCamera(width(),height());

    media_cycle->setNeedsDisplay(true);

}

void ACOsgCompositeViewQt::toggleTimelinePlayback(bool toggle){
    if (media_cycle == 0) return;
    if ( (media_cycle) && (media_cycle->hasBrowser()) && (timeline_renderer->getTrack(0)!=0) ) {
        //media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMediaIndex() );
        media_cycle->performActionOnMedia("play", timeline_renderer->getTrack(0)->getMedia()->getId());
        media_cycle->getBrowser()->toggleSourceActivity( timeline_renderer->getTrack(0)->getMedia()->getId() );
    }
}

void ACOsgCompositeViewQt::discardMedia(){
    if (media_cycle == 0) return;
    std::cout << "ACOsgCompositeViewQt::discardMedia" << std::endl;
    if (media_cycle->hasBrowser())
    {
        int media_id = media_cycle->getClickedNode();
        ACMedia* discard = media_cycle->getLibrary()->getMedia(media_id);
        if(discard){
            discard->setDiscarded(true);
            media_cycle->setNeedsDisplay(true);
        }
    }
}

void ACOsgCompositeViewQt::propagateEventToActions( QEvent* event )
{
    //int cnt = 0;
    //QListIterator<QAction*> _action(this->actions());
    QListIterator<ACInputActionQt*> _action(this->inputActions);
    while (_action.hasNext()){
        ACInputActionQt *_act = dynamic_cast<ACInputActionQt *>(_action.next());
        if(_act){
            //std::cout << "ACOsgCompositeViewQt::propagateEventToActions: " << _act->text().toStdString() << std::endl;
            _act->eventAbsorber(event);
            //_act->event(event)
        }
    }
}

void ACOsgCompositeViewQt::keyPressEvent( QKeyEvent* event )
{
    this->propagateEventToActions(event);
    if (media_cycle == 0) return;
    osg_view->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void ACOsgCompositeViewQt::keyReleaseEvent( QKeyEvent* event )
{
    this->propagateEventToActions(event);
    if (media_cycle == 0) return;
    osg_view->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void ACOsgCompositeViewQt::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    mousedown = 1;
    switch(event->button())
    {
    case(Qt::LeftButton): button = 1; break;
    case(Qt::MidButton): button = 2; break;
    case(Qt::RightButton): button = 3; break;
    case(Qt::NoButton): button = 0; break;
    default: button = 0; break;
    }
    //this->propagateEventToActions(event);
    osg_view->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
    //std::cout << "ACOsgCompositeViewQt::mousePressEvent clicked node " << media_cycle->getClickedNode() << std::endl;
    //browser_view->getEventQueue()->mouseButtonPress(event->x(), event->y()-sepy, button);

    this->propagateEventToActions(event);

    if (media_cycle == 0) return;
    // CF: for OSG y=0 is on the bottom, for Qt on the top
    // browser view top (OSG coordinates): browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y()
    // browser view bottom (OSG coordinates): browser_view->getCamera()->getViewport()->y()
    // browser view top (Qt coordinates): height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() )
    // browser view bottom (Qt coordinates): height() - ( browser_view->getCamera()->getViewport()->y() )

    refx = event->x();
    refy = event->y();
    if ( (event->y() >= height() - ( browser_view->getCamera()->getViewport()->height() + browser_view->getCamera()->getViewport()->y() ) ) && (event->y() <= height() - ( browser_view->getCamera()->getViewport()->y() + septhick) )) // if clicked on browser view far enough of the central border
    {
        media_cycle->getCameraPosition(refcamx, refcamy);
        refzoom = media_cycle->getCameraZoom();
        refrotation = media_cycle->getCameraRotation();
    }
    else if ( (event->y() >= height() - ( browser_view->getCamera()->getViewport()->y() + septhick)) && (event->y() <= height() - ( browser_view->getCamera()->getViewport()->y() - septhick)) ) // if clicked on timeline view far enough of the central border
    {
        borderdown = 1;
        refsepy = sepy;
    }
    
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
    media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseMoveEvent( QMouseEvent* event )
{
    if (media_cycle&&event->buttons() ){
        
        if (media_cycle->getClickedNode()>-1){//DRAG (click on a node and move)
            dragFlag=true;
            cout<<(event->pos() - dragStartPosition).manhattanLength();
            cout<<QApplication::startDragDistance();
            if ((event->pos() - dragStartPosition).manhattanLength()
                < QApplication::startDragDistance())
                return;
            
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
            QList<QUrl> data;
            QUrl locUrl;
            locUrl.setPath(QString::fromStdString( media_cycle->getMediaFileName(media_cycle->getClickedNode())));
            locUrl.setScheme(QString("file"));
            data.push_back(locUrl);
            mimeData->setUrls( data);
            drag->setMimeData(mimeData);
            std::string thumbName= media_cycle->getLibrary()->getMedia(media_cycle->getClickedNode())->getThumbnailFileName();
            if ( thumbName.size()>0){
                QImageReader *img=new QImageReader(QString::fromStdString(thumbName));
            
                drag->setPixmap(QPixmap::fromImageReader(img));
                delete img;
            }
            Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
            
            return;
        }
    }
    
    int button = 0;
    switch(event->button())
    {
    case(Qt::LeftButton): button = 1; break;
    case(Qt::MidButton): button = 2; break;
    case(Qt::RightButton): button = 3; break;
    case(Qt::NoButton): button = 0; break;
    default: button = 0; break;
    }
    osg_view->getEventQueue()->mouseMotion(event->x(), event->y());
    if (media_cycle == 0) return;

    float zoom, angle;
    float xmove, ymove, xmove2, ymove2;
    float x, y;
    x = event->x();
    y = event->y();
    if ( mousedown == 1 )
    {
        if ( borderdown == 1)
        {
            if ( (y<=height()-septhick) && (y>=septhick))
                sepy = refsepy + (refy-y);
            else
            {
                if ( y>height()-septhick )
                    sepy = septhick;
                else if ( y<septhick )
                    sepy = height()-septhick;
            }
            timeline_renderer->updateSize(width(),sepy);
        }
    }
    this->propagateEventToActions(event);
    media_cycle->setNeedsDisplay(true);
}

void ACOsgCompositeViewQt::mouseReleaseEvent( QMouseEvent* event )
{
    dragFlag=false;
    int button = 0;
    switch(event->button())
    {
    case(Qt::LeftButton): button = 1; break;
    case(Qt::MidButton): button = 2; break;
    case(Qt::RightButton): button = 3; break;
    case(Qt::NoButton): button = 0; break;
    default: button = 0; break;
    }
    this->propagateEventToActions(event);
    osg_view->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
    //std::cout << "ACOsgCompositeViewQt::mouseReleaseEvent clicked node " << media_cycle->getClickedNode() << std::endl;
    if (media_cycle == 0) return;
    if (media_cycle->getClickedNode()>-1)
        media_cycle->setClickedNode(-1);
    std::cout << "mouseReleaseEvent clicked node erased " << std::endl;
    mousedown = 0;
    borderdown = 0;
    media_cycle->setNeedsDisplay(true);
}

bool ACOsgCompositeViewQt::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QGLWidget::event(event);
}

bool ACOsgCompositeViewQt::gestureEvent(QGestureEvent *event)
 {
     if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
         swipeTriggered(static_cast<QSwipeGesture *>(swipe));
     else if (QGesture *pan = event->gesture(Qt::PanGesture))
         panTriggered(static_cast<QPanGesture *>(pan));
     if (QGesture *pinch = event->gesture(Qt::PinchGesture))
         pinchTriggered(static_cast<QPinchGesture *>(pinch));
     return true;
 }

// swipe is triggered when holding three fingers on the trackpad then sliding them
// (on OSX 10.6+, System Preferences > Trackpad, the related the Three Fingers Swipe to Navigate gesture must be activated)
void ACOsgCompositeViewQt::swipeTriggered(QSwipeGesture *gesture)
 {
    #ifdef USE_DEBUG
    std::cout << "ACOsgCompositeViewQt::swipeTriggered: angle " << (double) gesture->swipeAngle() << std::endl;
     if (gesture->state() == Qt::GestureFinished) {
         if (gesture->horizontalDirection() == QSwipeGesture::Left)
             std::cout << "ACOsgCompositeViewQt::swipeTriggered left" << std::endl;
         if (gesture->verticalDirection() == QSwipeGesture::Up)
             std::cout << "ACOsgCompositeViewQt::swipeTriggered up" << std::endl;
         if (gesture->horizontalDirection() == QSwipeGesture::Right)
             std::cout << "ACOsgCompositeViewQt::swipeTriggered right" << std::endl;
         if (gesture->verticalDirection() == QSwipeGesture::Down)
             std::cout << "ACOsgCompositeViewQt::swipeTriggered down" << std::endl;
         update();
     }
    #endif
 }

// CF pan is triggered when holding one finger on the trackpad then sliding it
void ACOsgCompositeViewQt::panTriggered(QPanGesture* gesture){
    #ifdef USE_DEBUG
   // std::cout << "ACOsgCompositeViewQt::panTriggered:";
   // std::cout << " offset x " << gesture->offset().x() << " y " << gesture->offset().y();
   // std::cout << " delta x " << gesture->delta().x() << " y " << gesture->delta().y();
   // std::cout << " acceleration " << gesture->acceleration() ;
   // std::cout << std::endl;
    #endif
}

//CF pinch is triggered when holding two fingers on the trackpad then moving them
void ACOsgCompositeViewQt::pinchTriggered(QPinchGesture* gesture){
    if(!media_cycle)
        return;

    //float zoom = (double) gesture->scaleFactor()/10.0f;
    float zoom = media_cycle->getCameraZoom() * ( gesture->scaleFactor() / gesture->lastScaleFactor() );
    media_cycle->setCameraZoom( zoom );

    //CF center point is updated only on a new gesture (on OSX 10.6 with the trackpad)
    // Qt doesn't care so far "QPinchGesture reports incorrect center points on Mac"
    // https://bugreports.qt-project.org/browse/QTBUG-14291
    //float center_x = (double) gesture->centerPoint().x() / (float)this->width();
    //float center_y = 1-(double) gesture->centerPoint().y() / (float)this->height();
    float center_x = (double) (gesture->centerPoint().x());// / (float)800;
    float center_y = (double) (gesture->centerPoint().y());// / (float)800;
    float start_x = (double) (gesture->startCenterPoint().x());// / (float)800;
    float start_y = (double) (gesture->startCenterPoint().y());// / (float)800;
    //media_cycle->setCameraPosition( center_x, center_y);

    float rotation = - 2.0f * (double) gesture->rotationAngle() * PI / (double)180;
    media_cycle->setCameraRotation( rotation );

    //#ifdef USE_DEBUG
    //std::cout << "ACOsgCompositeViewQt::pinchTriggered center " << center_x << " " << center_y << " or " << start_x << " " << start_y << " zoom " << zoom  << " rot " << gesture->rotationAngle() << std::endl;
    //#endif
}

void ACOsgCompositeViewQt::prepareFromBrowser()
{
    //setMouseTracking(false); //CF necessary for the hover callback
    browser_renderer->prepareNodes();
    browser_renderer->prepareLabels();
    hud_renderer->preparePointers(browser_view);
    hud_renderer->prepareLibrary(browser_view);
    hud_renderer->prepareMediaActions(browser_view);
    browser_view->setSceneData(browser_renderer->getShapes());

//    osgDB::SharedStateManager* ssm = osgDB::Registry::instance()->getSharedStateManager();
//    if ( ssm )
//        ssm->share( browser_renderer->getShapes() );

    library_loaded = true;
    //addCamera(renderer_hud->getCamera());
}

// XS TODO this is called for instance when click on node
// check that it does not do too many things
void ACOsgCompositeViewQt::updateTransformsFromBrowser( double frac)
{
    if (media_cycle == 0) return;

    browser_renderer->prepareNodes();
    browser_renderer->prepareLabels();
    hud_renderer->preparePointers();
    hud_renderer->prepareLibrary();
    hud_renderer->prepareMediaActions();

    // get screen coordinates
    //int closest_node;//CF to deprecate
    //closest_node = browser_renderer->computeScreenCoordinates(browser_view, frac);//CF to deprecate
    //media_cycle->setClosestNode(closest_node);//CF to deprecate
    browser_renderer->computeScreenCoordinates(browser_view, frac);

    // recompute scene graph
    browser_renderer->updateNodes(frac); // animation starts at 0.0 and ends at 1.0
    browser_renderer->updateLabels(frac);

    hud_renderer->updatePointers(browser_view);
    hud_renderer->updateLibrary(browser_view);
    hud_renderer->updateMediaActions(browser_view);
    /*osgViewer::ViewerBase::Views views;
 bool have_views;
 view->getViews(views,have_views);
 renderer_hud->updatePointers(views[0]);*/
}

void ACOsgCompositeViewQt::prepareFromTimeline()
{
    //setMouseTracking(false); //CF necessary for the hover callback
    timeline_renderer->prepareTracks();
    timeline_view->setSceneData(timeline_renderer->getShapes());

//    osgDB::SharedStateManager* ssm = osgDB::Registry::instance()->getSharedStateManager();
//    if ( ssm )
//        ssm->share( timeline_renderer->getShapes() );

}

void ACOsgCompositeViewQt::updateTransformsFromTimeline( double frac)
{
    if (media_cycle == 0) return;
    //int closest_track;
    // get screen coordinates
    /////////closest_track = timeline_renderer->computeScreenCoordinates(timeline_view, frac); //CF this instead of browser_view for the the simple Viewer
    ////////media_cycle->setClosestNode(closest_node);
    // recompute scene graph
    timeline_renderer->updateTracks(frac); // animation starts at 0.0 and ends at 1.0
}

void ACOsgCompositeViewQt::changeSetting(ACSettingType _setting)
{
    if(this->setting == _setting)
        return;

    this->setting = _setting;

    if(browser_renderer)
        browser_renderer->changeSetting(this->setting);
    if(hud_renderer)
        hud_renderer->changeSetting(this->setting);

    if(this->setting == AC_SETTING_INSTALLATION){
        this->setCursor(QCursor( Qt::BlankCursor ) );
        // QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
        // QApplication::restoreOverrideCursor();
        if(media_cycle->hasBrowser() && media_cycle->getBrowser()->hasMousePointer())
            media_cycle->getBrowser()->removeMousePointer();
       }
    else{
        this->setCursor(QCursor());
        if(media_cycle->hasBrowser() && media_cycle->getBrowser()->hasMousePointer() == false)
            media_cycle->getBrowser()->addMousePointer();
    }
}
