/*
*  ACOsgQtRenderer.cpp
*  MediaCycle
*
*  @author Christian Frisson
*  @date 25/06/2021
*  @copyright (c) 2021 – UMONS - Numediart
*  @brief Class deriving from osgQt/OSGRenderer and osgViewer/CompositeViewer
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

#include "ACOsgQtRenderer.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QThread>

namespace
{

    class QtKeyboardMap
    {
    public:
        QtKeyboardMap()
        {
            mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
            mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
            mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
            mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
            mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
            mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
            mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
            mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
            mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
            mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
            mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
            mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
            mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
            mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
            mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
            mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
            mKeyMap[Qt::Key_Alt        ] = osgGA::GUIEventAdapter::KEY_Alt_L;
            mKeyMap[Qt::Key_Shift      ] = osgGA::GUIEventAdapter::KEY_Shift_L;
            mKeyMap[Qt::Key_Control    ] = osgGA::GUIEventAdapter::KEY_Control_L;
            mKeyMap[Qt::Key_Meta       ] = osgGA::GUIEventAdapter::KEY_Meta_L;
            mKeyMap[Qt::Key_F1             ] = osgGA::GUIEventAdapter::KEY_F1;
            mKeyMap[Qt::Key_F2             ] = osgGA::GUIEventAdapter::KEY_F2;
            mKeyMap[Qt::Key_F3             ] = osgGA::GUIEventAdapter::KEY_F3;
            mKeyMap[Qt::Key_F4             ] = osgGA::GUIEventAdapter::KEY_F4;
            mKeyMap[Qt::Key_F5             ] = osgGA::GUIEventAdapter::KEY_F5;
            mKeyMap[Qt::Key_F6             ] = osgGA::GUIEventAdapter::KEY_F6;
            mKeyMap[Qt::Key_F7             ] = osgGA::GUIEventAdapter::KEY_F7;
            mKeyMap[Qt::Key_F8             ] = osgGA::GUIEventAdapter::KEY_F8;
            mKeyMap[Qt::Key_F9             ] = osgGA::GUIEventAdapter::KEY_F9;
            mKeyMap[Qt::Key_F10            ] = osgGA::GUIEventAdapter::KEY_F10;
            mKeyMap[Qt::Key_F11            ] = osgGA::GUIEventAdapter::KEY_F11;
            mKeyMap[Qt::Key_F12            ] = osgGA::GUIEventAdapter::KEY_F12;
            mKeyMap[Qt::Key_F13            ] = osgGA::GUIEventAdapter::KEY_F13;
            mKeyMap[Qt::Key_F14            ] = osgGA::GUIEventAdapter::KEY_F14;
            mKeyMap[Qt::Key_F15            ] = osgGA::GUIEventAdapter::KEY_F15;
            mKeyMap[Qt::Key_F16            ] = osgGA::GUIEventAdapter::KEY_F16;
            mKeyMap[Qt::Key_F17            ] = osgGA::GUIEventAdapter::KEY_F17;
            mKeyMap[Qt::Key_F18            ] = osgGA::GUIEventAdapter::KEY_F18;
            mKeyMap[Qt::Key_F19            ] = osgGA::GUIEventAdapter::KEY_F19;
            mKeyMap[Qt::Key_F20            ] = osgGA::GUIEventAdapter::KEY_F20;
            mKeyMap[Qt::Key_hyphen         ] = '-';
            mKeyMap[Qt::Key_Equal         ] = '=';
            mKeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
            mKeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
            mKeyMap[Qt::Key_Minus         ] = '-';
            mKeyMap[Qt::Key_Plus          ] = '+';
            mKeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        }

        ~QtKeyboardMap()
        {
        }

        int remapKey(QKeyEvent* event)
        {
            KeyMap::iterator itr = mKeyMap.find(event->key());

            if(itr == mKeyMap.end())
            {
                return int(*(event->text().toLatin1().data()));
            }
            else
                return itr->second;
        }

    private:
        typedef std::map<unsigned int, int> KeyMap;
        KeyMap mKeyMap;
    };

    static QtKeyboardMap s_QtKeyboardMap;
} // namespace


ACOsgQtRenderer::ACOsgQtRenderer(QObject* parent)
    : 
    // QObject(parent), 
    OSGRenderer(parent), osgViewer::CompositeViewer()
{
}

ACOsgQtRenderer::ACOsgQtRenderer(osg::ArgumentParser* arguments, QObject* parent)
    : 
    // QObject(parent), 
    OSGRenderer(arguments, parent), osgViewer::CompositeViewer(*arguments)
{
}

ACOsgQtRenderer::~ACOsgQtRenderer()
{
    
}

void ACOsgQtRenderer::resize(int windowWidth, int windowHeight, float windowScale)
{
    if(!m_osgInitialized)
        return;

    m_windowScale = windowScale;

    /*  _camera->setViewport(new osg::Viewport(0, 0, windowWidth * windowScale,
                                           windowHeight * windowScale));*/

    m_osgWinEmb->resized(0, 0,
                         windowWidth * windowScale,
                         windowHeight * windowScale);
    m_osgWinEmb->getEventQueue()->windowResize(0, 0,
                                               windowWidth * windowScale,
                                               windowHeight * windowScale);

    update();
}

void ACOsgQtRenderer::setupOSG(int windowWidth, int windowHeight, float windowScale)
{
    m_osgInitialized = true;
    m_windowScale = windowScale;
    m_osgWinEmb = new osgViewer::GraphicsWindowEmbedded(0, 0,
                                                        windowWidth * windowScale, windowHeight * windowScale);
    //m_osgWinEmb = new osgViewer::GraphicsWindowEmbedded(0, 0, windowWidth * windowScale, windowHeight * windowScale);
    // make sure the event queue has the correct window rectangle size and input range
    m_osgWinEmb->getEventQueue()->syncWindowRectangleWithGraphicsContext();
    _camera->setViewport(new osg::Viewport(0, 0, windowWidth * windowScale,
                                           windowHeight * windowScale));
    _camera->setGraphicsContext(m_osgWinEmb.get());
    // disable key event (default is Escape key) that the viewer checks on each
    // frame to see
    // if the viewer's done flag should be set to signal end of viewers main
    // loop.
    osgViewer::CompositeViewer::setKeyEventSetsDone(0);
    osgViewer::CompositeViewer::setReleaseContextAtEndOfFrameHint(false);
    osgViewer::CompositeViewer::setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    osgViewer::CompositeViewer::Windows windows;
    osgViewer::CompositeViewer::getWindows(windows);

    _timerId = startTimer(10, Qt::PreciseTimer);
    _lastFrameStartTime.setStartTick(0);
}

void ACOsgQtRenderer::setKeyboardModifiers(QInputEvent* event)
{
    unsigned int modkey = event->modifiers() & (Qt::ShiftModifier |
                                                Qt::ControlModifier |
                                                Qt::AltModifier);
    unsigned int mask = 0;

    if(modkey & Qt::ShiftModifier) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;

    if(modkey & Qt::ControlModifier) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;

    if(modkey & Qt::AltModifier) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;

    m_osgWinEmb->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

void ACOsgQtRenderer::keyPressEvent(QKeyEvent* event)
{
    setKeyboardModifiers(event);
    int value = s_QtKeyboardMap.remapKey(event);
    m_osgWinEmb->getEventQueue()->keyPress(value);
}

void ACOsgQtRenderer::keyReleaseEvent(QKeyEvent* event)
{
    if(event->isAutoRepeat())
    {
        event->ignore();
    }
    else
    {
        setKeyboardModifiers(event);
        int value = s_QtKeyboardMap.remapKey(event);
        m_osgWinEmb->getEventQueue()->keyRelease(value);
    }
}

void ACOsgQtRenderer::mousePressEvent(QMouseEvent* event)
{
    int button = 0;

    switch(event->button())
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MidButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    case Qt::NoButton:
        button = 0;
        break;

    default:
        button = 0;
        break;
    }

    setKeyboardModifiers(event);
    m_osgWinEmb->getEventQueue()->mouseButtonPress(event->x() * m_windowScale,
                                                   event->y() * m_windowScale, button);
}

void ACOsgQtRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    int button = 0;

    switch(event->button())
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MidButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    case Qt::NoButton:
        button = 0;
        break;

    default:
        button = 0;
        break;
    }

    setKeyboardModifiers(event);
    m_osgWinEmb->getEventQueue()->mouseButtonRelease(event->x() * m_windowScale,
                                                     event->y() * m_windowScale, button);
}

void ACOsgQtRenderer::mouseDoubleClickEvent(QMouseEvent* event)
{
    int button = 0;

    switch(event->button())
    {
    case Qt::LeftButton:
        button = 1;
        break;

    case Qt::MidButton:
        button = 2;
        break;

    case Qt::RightButton:
        button = 3;
        break;

    case Qt::NoButton:
        button = 0;
        break;

    default:
        button = 0;
        break;
    }

    setKeyboardModifiers(event);
    m_osgWinEmb->getEventQueue()->mouseDoubleButtonPress(event->x() * m_windowScale,
                                                         event->y() * m_windowScale, button);
}

void ACOsgQtRenderer::mouseMoveEvent(QMouseEvent* event)
{
    setKeyboardModifiers(event);
    m_osgWinEmb->getEventQueue()->mouseMotion(event->x() * m_windowScale,
                                              event->y() * m_windowScale);
}

void ACOsgQtRenderer::wheelEvent(QWheelEvent* event)
{
    setKeyboardModifiers(event);
    m_osgWinEmb->getEventQueue()->mouseMotion(event->x() * m_windowScale,
                                              event->y() * m_windowScale);
    m_osgWinEmb->getEventQueue()->mouseScroll(
        event->orientation() == Qt::Vertical ?
        (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_UP :
         osgGA::GUIEventAdapter::SCROLL_DOWN) :
        (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_LEFT :
         osgGA::GUIEventAdapter::SCROLL_RIGHT));
}


// called from ViewerWidget paintGL() method
void ACOsgQtRenderer::frame(double simulationTime)
{
    // limit the frame rate
    if(osgViewer::CompositeViewer::getRunMaxFrameRate() > 0.0)
    {
        double dt = _lastFrameStartTime.time_s();
        double minFrameTime = 1.0 / osgViewer::CompositeViewer::getRunMaxFrameRate();

        if(dt < minFrameTime)
            QThread::usleep(static_cast<unsigned int>(1000000.0 * (minFrameTime - dt)));
    }

    // avoid excessive CPU loading when no frame is required in ON_DEMAND mode
    if(osgViewer::CompositeViewer::getRunFrameScheme() == osgViewer::ViewerBase::ON_DEMAND)
    {
        double dt = _lastFrameStartTime.time_s();

        if(dt < 0.01)
            OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0 *
                                                                      (0.01 - dt)));
    }

    // record start frame time
    _lastFrameStartTime.setStartTick();
    // make frame

#if 1
    osgViewer::CompositeViewer::frame(simulationTime);
#else

    if(_done) return;

    // OSG_NOTICE<<std::endl<<"CompositeViewer::frame()"<<std::endl<<std::endl;

    if(_firstFrame)
    {
        viewerInit();

        if(!isRealized())
        {
            realize();
        }

        _firstFrame = false;
    }

    advance(simulationTime);

    eventTraversal();
    updateTraversal();
    //    renderingTraversals();
#endif
}

