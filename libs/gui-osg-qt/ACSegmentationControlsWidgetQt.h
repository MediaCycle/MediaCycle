/*
 *  ACSegmentationControlsWidgetQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 12/06/2012
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

#ifndef HEADER_ACSegmentationControlsWidgetQt
#define HEADER_ACSegmentationControlsWidgetQt

#include <iostream>
#include <string.h>

#include "ACAbstractWidgetQt.h"

//#include "ui_ACSegmentationControlsWidgetQt.h"

class ACSegmentationControlsWidgetQt : public QWidget, public ACAbstractWidgetQt {
    Q_OBJECT

public slots:
    void on_comboBoxPlugins_activated(const QString & text);

protected slots:
    void buildPluginList();
    void adjustHeight();

signals:
    void rebuildPluginList();
    void readjustHeight();

public:
    ACSegmentationControlsWidgetQt(QWidget *parent = 0);
    virtual ~ACSegmentationControlsWidgetQt();

    void changeMediaType(ACMediaType media_type);
    void updatePluginsSettings();
    void resetPluginsSettings();
    void resetMediaType(ACMediaType _media_type);

protected:
    //Ui::ACSegmentationControlsWidgetQt ui;
    QComboBox* comboBoxPlugins;
    QVBoxLayout* vboxLayout;
    QVBoxLayout* parametersLayout;
    QWidget* parametersContainer;
    void cleanPluginList();
};
#endif
