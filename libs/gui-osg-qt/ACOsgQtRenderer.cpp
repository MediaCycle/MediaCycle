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

#include <QThread>

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

void OSGRenderer::setupOSG(int windowWidth, int windowHeight, float windowScale)
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
    setKeyEventSetsDone(0);
    setReleaseContextAtEndOfFrameHint(false);
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    osgViewer::CompositeViewer::Windows windows;
    getWindows(windows);

    _timerId = startTimer(10, Qt::PreciseTimer);
    _lastFrameStartTime.setStartTick(0);
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

