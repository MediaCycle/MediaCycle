/**
 * @brief A dock that provides a list of features whose weights can be changed by sliders
 * @author Christian Frisson
 * @date 12/07/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#ifndef HEADER_ACSimilaritySliderControlsDockWidgetQt
#define HEADER_ACSimilaritySliderControlsDockWidgetQt

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"

#include "ui_ACSimilaritySliderControlsDockWidgetQt.h"
#include <MediaCycle.h>

class ACSimilaritySliderControlsDockWidgetQt : public ACAbstractDockWidgetQt {
    Q_OBJECT

public slots:
    //virtual void modifyListItem(QListWidgetItem *item);
    virtual void modifyListItem(int row, int column);
    void on_pushButtonAll_clicked();
    void on_pushButtonNone_clicked();

public:
    ACSimilaritySliderControlsDockWidgetQt(QWidget *parent = 0);
    virtual ~ACSimilaritySliderControlsDockWidgetQt(){}
    virtual bool canBeVisible(ACMediaType _media_type);

protected:
    Ui::ACSimilaritySliderControlsDockWidgetQt ui;

signals:
    void reconfigureSliders();
    void readjustHeight();

public slots:
    void configureSliders();
    void adjustHeight();
    void changeWeight(int index);

public:
    virtual void updatePluginsSettings();
    virtual void resetPluginsSettings();
    virtual void changeMediaType(ACMediaType _media_type);

    void synchronizeFeaturesWeights();
    void cleanSliders();
    QSignalMapper* sliderMapper;

protected:
    void resetMode();
};
#endif
