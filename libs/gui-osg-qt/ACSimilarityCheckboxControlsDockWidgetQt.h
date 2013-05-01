/**
 * @brief A dock that provides a list of features whose weights can be changed by checkboxes
 * @authors Xavier Siebert, Christian Frisson
 * @date 8/01/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#ifndef HEADER_ACSimilarityCheckboxControlsDockWidgetQt
#define HEADER_ACSimilarityCheckboxControlsDockWidgetQt

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"
#include "ACPluginControlsWidgetQt.h"

#include "ui_ACSimilarityCheckboxControlsDockWidgetQt.h"
#include <MediaCycle.h>

class ACSimilarityCheckboxControlsDockWidgetQt : public ACAbstractDockWidgetQt {
    Q_OBJECT

public slots:
    virtual void modifyListItem(QListWidgetItem *item);
    void on_pushButtonAll_clicked();
    void on_pushButtonNone_clicked();

public:
    ACSimilarityCheckboxControlsDockWidgetQt(QWidget *parent = 0);
    virtual ~ACSimilarityCheckboxControlsDockWidgetQt(){}
    virtual bool canBeVisible(ACMediaType _media_type);
    virtual Qt::DockWidgetArea favoriteArea(){return Qt::LeftDockWidgetArea;}

protected:
    Ui::ACSimilarityCheckboxControlsDockWidgetQt ui;

signals:
    void reconfigureCheckBoxes();
    void readjustHeight();

public slots:
    void configureCheckBoxes();
    void adjustHeight();

public:
    virtual void updatePluginsSettings();
    virtual void resetPluginsSettings();
    virtual void changeMediaType(ACMediaType _media_type);

    void synchronizeFeaturesWeights();
    void cleanCheckBoxes();

protected:
    void resetMode();
};
#endif
