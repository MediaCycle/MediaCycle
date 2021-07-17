/*
*  ACOsgQtRenderer.h
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

#ifndef HEADER_AC_OSG_QT_RENDERER
#define HEADER_AC_OSG_QT_RENDERER

#include <osgQOpenGL/OSGRenderer>
#include <osgViewer/CompositeViewer>

class QInputEvent;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class ACOsgQtRenderer : 
// public QObject, 
public OSGRenderer, public osgViewer::CompositeViewer
{
    bool                                       m_osgInitialized {false};
    osg::ref_ptr<osgViewer::GraphicsWindow>    m_osgWinEmb;
    float                                      m_windowScale {1.0f};

    int                                        _timerId{0};
    osg::Timer                                 _lastFrameStartTime;

public:
    explicit ACOsgQtRenderer(QObject* parent = nullptr);
    explicit ACOsgQtRenderer(osg::ArgumentParser* arguments, QObject* parent = nullptr);

    ~ACOsgQtRenderer() override;

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

    virtual void resize(int windowWidth, int windowHeight, float windowScale) override;

    void setupOSG(int windowWidth, int windowHeight, float windowScale);

    osg::ref_ptr<osgViewer::GraphicsWindow> getGraphicsWindow(){return m_osgWinEmb;}

    // overrided from osgViewer::ViewerBase
    void frame(double simulationTime = USE_REFERENCE_TIME) override;

    virtual osg::Object* clone(const osg::CopyOp& obj) const override {return osgViewer::CompositeViewer::clone(obj);}
    virtual bool isSameKindAs(const osg::Object* obj) const override {return osgViewer::CompositeViewer::isSameKindAs(obj);}
    virtual const char* libraryName() const override {return osgViewer::CompositeViewer::libraryName();}
    virtual const char* className() const override {return osgViewer::CompositeViewer::className();}
    virtual osg::Object* cloneType() const override {return osgViewer::CompositeViewer::cloneType();}

protected:
    void setKeyboardModifiers(QInputEvent* event);

};

#endif // HEADER_AC_OSG_QT_RENDERER
