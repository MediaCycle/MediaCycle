/*
*  ACOsgQOpenGLWidget.cpp
*  MediaCycle
*
*  @author Christian Frisson
*  @date 25/06/2021
*  @copyright (c) 2021 – UMONS - Numediart
*  @brief Class deriving from osgQt/osgQOpenGLWidget
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

#include "ACOsgQOpenGLWidget.h"

#include <QApplication>
#include <QScreen>
#include <QWindow>


ACOsgQOpenGLWidget::ACOsgQOpenGLWidget(QWidget* parent)
    : osgQOpenGLWidget(parent)
{
    // m_renderer = new ACOsgQtRenderer(this);
}

ACOsgQOpenGLWidget::ACOsgQOpenGLWidget(osg::ArgumentParser* arguments,
                                   QWidget* parent) :
    osgQOpenGLWidget(arguments,parent)
{
    // m_renderer = new ACOsgQtRenderer(this);
}

ACOsgQOpenGLWidget::~ACOsgQOpenGLWidget()
{
}

ACOsgQtRenderer* ACOsgQOpenGLWidget::getCompositeViewer()
{
    if (m_renderer) return dynamic_cast<ACOsgQtRenderer*>(m_renderer);
    return nullptr;
}


osgViewer::View* ACOsgQOpenGLWidget::getOsgView(unsigned i)
{
    if (!m_renderer)
        return nullptr;

    ACOsgQtRenderer* _renderer = dynamic_cast<ACOsgQtRenderer*>(m_renderer);

    if (!_renderer)
        return nullptr;

    return _renderer->getView(i);
}

void ACOsgQOpenGLWidget::addOsgView(osgViewer::View* view)
{
    if (!m_renderer)
        return;

    ACOsgQtRenderer* _renderer = dynamic_cast<ACOsgQtRenderer*>(m_renderer);

    if (!_renderer)
        return;

    _renderer->addView(view);
}

void ACOsgQOpenGLWidget::initializeGL()
{
    // Initializes OpenGL function resolution for the current context.
    initializeOpenGLFunctions();
    createRenderer();
    emit initialized();
}

void ACOsgQOpenGLWidget::paintGL()
{
    OpenThreads::ScopedReadLock locker(_osgMutex);
	if (_isFirstFrame) {
		_isFirstFrame = false;
        ACOsgQtRenderer* _renderer = getCompositeViewer();
		// m_renderer->getCamera()->getGraphicsContext()->setDefaultFboId(defaultFramebufferObject());
        if(_renderer){
            for (unsigned int i = 0; i < _renderer->getNumViews(); ++i)
            {
                _renderer->getView(i)->getCamera()->getGraphicsContext()->setDefaultFboId(defaultFramebufferObject());
            }
        }
	}
	m_renderer->frame();
}

void ACOsgQOpenGLWidget::createRenderer()
{
    // call this before creating a View...
    setDefaultDisplaySettings();
	if (!_arguments) {
		m_renderer = new ACOsgQtRenderer(this);
	} else {
		m_renderer = new ACOsgQtRenderer(_arguments, this);
	}
	QScreen* screen = windowHandle()
                      && windowHandle()->screen() ? windowHandle()->screen() :
                      qApp->screens().front();
    m_renderer->setupOSG(width(), height(), screen->devicePixelRatio());
}
