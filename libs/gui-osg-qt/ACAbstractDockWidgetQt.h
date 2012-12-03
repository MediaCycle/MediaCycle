/*
 *  ACAbstractDockWidget.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/02/11
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

#ifndef ACABSTRACTDOCKWIDGETQT_H
#define ACABSTRACTDOCKWIDGETQT_H

#include <MediaCycle.h>
#include <string>

#include <QtGui>

#include "ACAbstractWidgetQt.h"
#include "ACAbstractDefaultConfig.h"

class ACAbstractDockWidgetQt : public QDockWidget, public ACAbstractWidgetQt {
    Q_OBJECT

signals:
    void mediaConfigChanged(QString);
    #ifdef SUPPORT_MULTIMEDIA
    void activeMediaTypeChanged(QString);
    #endif//def SUPPORT_MULTIMEDIA
public:
    ACAbstractDockWidgetQt(QWidget *parent = 0, ACMediaType _media_type = MEDIA_TYPE_NONE, std::string _class_name = "")
        : QDockWidget(parent), ACAbstractWidgetQt(), media_type(_media_type),class_name(_class_name)
    {
        this->setFeatures(QDockWidget::DockWidgetClosable);
        this->setAllowedAreas(Qt::LeftDockWidgetArea);
    }
    virtual ~ACAbstractDockWidgetQt(){}
    virtual bool canBeVisible(ACMediaType _media_type) = 0;

    ACMediaType getMediaType(){return this->media_type;}
    void setClassName(std::string _class_name){this->class_name=_class_name;}
    std::string getClassName(){return this->class_name;}

    virtual void changeMediaType(ACMediaType media_type){}
    virtual void updatePluginsSettings(){}
    virtual void resetPluginsSettings(){}
    virtual void resetMediaType(ACMediaType _media_type){}
    virtual bool addDefaultConfig(ACAbstractDefaultConfig* _config){return false;}

private:
    ACMediaType media_type; // media type of the dock, not of the library
    std::string class_name;
};

#endif // ACABSTRACTDOCKWIDGETQT_H
