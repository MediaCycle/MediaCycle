/*
 *  ACBrowserControlsClustersNeighborsDockWidgetQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 8/01/11
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

#ifndef HEADER_ACBROWSERCONTROLSCOMPLETEDOCKWIDGETQT
#define HEADER_ACBROWSERCONTROLSCOMPLETEDOCKWIDGETQT

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"

#include "ui_ACBrowserControlsCompleteDockWidgetQt.h"
#include <MediaCycle.h>

class ACBrowserControlsCompleteDockWidgetQt : public ACAbstractDockWidgetQt {
Q_OBJECT

public slots:
    virtual void modifyListItem(QListWidgetItem *item);
		
protected slots:
    // Browser controls
    void on_pushButtonRecenter_clicked();
    void on_pushButtonBack_clicked();
    void on_pushButtonForward_clicked();

    // Clustering controls
    void on_spinBoxClusters_valueChanged(int _value);
    void on_sliderClusters_sliderReleased();
    void on_comboBoxClustersMethod_activated(const QString & text);
    void on_comboBoxClustersPositions_activated(const QString & text);

// Neighborhoods controls
    void on_comboBoxNeighborsMethod_activated(const QString & text);
    void on_comboBoxNeighborsPositions_activated(const QString & text);

    void on_radioButtonClusters_toggled( bool checked );

    void resizePluginList();
	
public:
    ACBrowserControlsCompleteDockWidgetQt(QWidget *parent = 0);
    virtual ~ACBrowserControlsCompleteDockWidgetQt(){};
    virtual bool canBeVisible(ACMediaType _media_type);

protected:
    Ui::ACBrowserControlsCompleteDockWidgetQt ui;

public:
    virtual void updatePluginsSettings();
    virtual void resetPluginsSettings();
    virtual void changeMediaType(ACMediaType _media_type);

    void synchronizeFeaturesWeights();
    void configureCheckBoxes();
    void cleanCheckBoxes();
    void updatePluginLists();
    QListWidget* getFeaturesListWidget(){return ui.featuresListWidget;}
};
#endif
