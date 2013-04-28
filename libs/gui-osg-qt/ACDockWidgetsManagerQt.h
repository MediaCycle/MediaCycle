/*
 *  ACDockWidgetsManagerQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/02/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#ifndef ACDOCKWIDGETSMANAGERQT_H
#define ACDOCKWIDGETSMANAGERQT_H

#include <QtGui>

#include "ACAbstractWidgetQt.h"
#include "ACDockWidgetFactoryQt.h"

class ACDockWidgetsManagerQt : public QObject, public ACAbstractWidgetQt {
Q_OBJECT

public slots:
    void syncControlToggleWithDocks();
signals:
    void toggleControls(bool);
    void mediaConfigChanged(ACMediaType);

public:
    ACDockWidgetsManagerQt(QMainWindow* _mainWindow = 0);
    virtual ~ACDockWidgetsManagerQt();

    virtual void setMediaCycle(MediaCycle* _media_cycle);
    virtual void setOsgView(ACOsgCompositeViewQt* _osg_view);
    void updateDocksVisibility(bool visibility);
    void updateDockHeight();

    ACAbstractDockWidgetQt* addControlDock(ACAbstractDockWidgetQt* dock);
    ACAbstractDockWidgetQt* addControlDock(std::string dock_type);
    ACAbstractDockWidgetQt* addControlDock(ACPluginType plugin_type);

    void changeMediaType(ACMediaType _media_type);
    void updatePluginsSettings();
    void resetPluginsSettings();
    void resetMediaType(ACMediaType _media_type); // loadDefaultConfig if no video/pdf osg plug

protected:
    ACAbstractDockWidgetQt* getDockFromClassName(std::string _name);

protected:
    QMainWindow* mainWindow;
    ACDockWidgetFactoryQt* dockWidgetFactory;
    vector<ACAbstractDockWidgetQt*> dockWidgets;
    std::map<std::string,int> lastDocksVisibilities; //state stored before hiding all docks with the toggle
    bool plugins_scanned,previous_docks_visibility,current_docks_visibility;
    ACMediaType media_type;
    int appOrigMinHeight;
};
#endif // ACDOCKWIDGETSMANAGERQT_H
