/**
 * @brief Generic dock for setting parameters of all plugin from a given plugin type
 * @author Christian Frisson
 * @date 6/01/2013
 * @copyright (c) 2013 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef HEADER_ACPluginControlsDockWidgetQt
#define HEADER_ACPluginControlsDockWidgetQt

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"
#include "ACPluginControlsWidgetQt.h"

//#include "ui_ACPluginControlsDockWidgetQt.h" // uncomment if a *.ui file exists

class ACPluginControlsDockWidgetQt : public ACAbstractDockWidgetQt {
    Q_OBJECT

public slots:
    void adjustHeight();

public:
    ACPluginControlsDockWidgetQt(ACPluginType pluginType, std::string dockName, QWidget *parent = 0);
    virtual ~ACPluginControlsDockWidgetQt();
    virtual bool canBeVisible(ACMediaType _media_type);

    virtual void changeMediaType(ACMediaType media_type);
    virtual void updatePluginsSettings();
    virtual void resetPluginsSettings();
    virtual void resetMediaType(ACMediaType _media_type);

protected:
    //Ui::ACPluginControlsDockWidgetQt ui; // uncomment if a *.ui file exists
    ACPluginControlsWidgetQt* widget;
};
#endif
