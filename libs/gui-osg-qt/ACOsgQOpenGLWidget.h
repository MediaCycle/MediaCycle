/*
*  ACOsgQOpenGLWidget.h
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

#include <osgQOpenGL/osgQOpenGLWidget>

#include "ACOsgQtRenderer.h"

#ifndef HEADER_AC_OSG_QOPENGL_WIDGET
#define HEADER_AC_OSG_QOPENGL_WIDGET

class ACOsgQOpenGLWidget : public osgQOpenGLWidget
{
    Q_OBJECT

public:
    ACOsgQOpenGLWidget(QWidget* parent = nullptr);
    ACOsgQOpenGLWidget(osg::ArgumentParser* arguments, QWidget* parent = nullptr);
    virtual ~ACOsgQOpenGLWidget();

    /** Get osgViewer View */
    virtual osgViewer::View* getOsgView(unsigned i);

    virtual void addOsgView(osgViewer::View* view);

    ACOsgQtRenderer* getCompositeViewer();

protected:

    //! call createRender. If overloaded, this method must send initialized signal at end
    void initializeGL() override; // so that we can call our own createRenderer()

    void resizeGL(int w, int h) override;

    //! lock scene graph and call osgViewer::frame()
    void paintGL() override;

    void createRenderer();
};

#endif // HEADER_AC_OSG_QOPENGL_WIDGET