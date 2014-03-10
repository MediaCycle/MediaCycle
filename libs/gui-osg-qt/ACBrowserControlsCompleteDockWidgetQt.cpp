/*
 *  ACBrowserControlsClustersNeighborsDockWidgetQt.cpp
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

#include "ACBrowserControlsCompleteDockWidgetQt.h"

ACBrowserControlsCompleteDockWidgetQt::ACBrowserControlsCompleteDockWidgetQt(QWidget *parent)
    : ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACBrowserControlsCompleteDockWidgetQt")
{
    ui.setupUi(this); // first thing to do

    preProcessControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_PREPROCESS);
    clustersMethodControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_CLUSTERS_METHOD);
    clustersPositionsControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_CLUSTERS_POSITIONS);
    clustersFilteringControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_FILTERING);
    neighborsMethodControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_NEIGHBORS_METHOD);
    neighborsPositionsControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_NEIGHBORS_POSITIONS);

    ui.groupBoxPreProcess->layout()->addWidget(preProcessControls);
    ui.groupBoxClustersMethod->layout()->addWidget(clustersMethodControls);
    ui.groupBoxClustersPositions->layout()->addWidget(clustersPositionsControls);
    ui.groupBoxFiltering->layout()->addWidget(clustersFilteringControls);
    ui.groupBoxNeighborsMethod->layout()->addWidget(neighborsMethodControls);
    ui.groupBoxNeighborsPositions->layout()->addWidget(neighborsPositionsControls);

    int controlsWidth = 250; // default dock widget size
    preProcessControls->setFixedWidth(controlsWidth);
    clustersMethodControls->setFixedWidth(controlsWidth);
    clustersPositionsControls->setFixedWidth(controlsWidth);
    clustersFilteringControls->setFixedWidth(controlsWidth);
    neighborsMethodControls->setFixedWidth(controlsWidth);
    neighborsPositionsControls->setFixedWidth(controlsWidth);

    // There are no previous/next navigation levels when starting the application
    ui.pushButtonBack->setEnabled(false);
    ui.pushButtonForward->setEnabled(false);

    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(preProcessControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(clustersMethodControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(clustersPositionsControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(clustersFilteringControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(neighborsMethodControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(neighborsPositionsControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));

    emit this->readjustHeight();

    this->show();
}

bool ACBrowserControlsCompleteDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACBrowserControlsCompleteDockWidgetQt::on_pushButtonRecenter_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->setCameraRecenter();
}

void ACBrowserControlsCompleteDockWidgetQt::on_pushButtonBack_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->goBack();
    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
}

void ACBrowserControlsCompleteDockWidgetQt::on_pushButtonForward_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->goForward();
    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
}

void ACBrowserControlsCompleteDockWidgetQt::on_tabWidgetModes_currentChanged(int index){

    if (media_cycle == 0) return;
    if(index == 0){//if(checked){
        //Clusters
        //media_cycle->changeClustersMethodPlugin(ui.comboBoxClustersMethod->currentText().toStdString());
        //media_cycle->changeClustersPositionsPlugin(ui.comboBoxClustersPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_CLUSTERS);
    }
    else{
        //Neighbors
        //media_cycle->changeNeighborsMethodPlugin(ui.comboBoxNeighborsMethod->currentText().toStdString());
        //media_cycle->changeNeighborsPositionsPlugin(ui.comboBoxNeighborsPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_NEIGHBORS);
    }

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::resetMode(){
    if(!media_cycle) return;

    switch (media_cycle->getBrowserMode()){
    case AC_MODE_CLUSTERS:
    {
        ui.tabWidgetModes->setCurrentIndex(0);
    }
        break;
    case AC_MODE_NEIGHBORS:
    {
        ui.tabWidgetModes->setCurrentIndex(1);
    }
        break;
    default:
        break;
    }
}


void ACBrowserControlsCompleteDockWidgetQt::updatePluginsSettings()
{
    if(!media_cycle) return;

    this->resetMode();

    preProcessControls->setMediaCycle(this->media_cycle);
    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    clustersFilteringControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    preProcessControls->updatePluginsSettings();
    clustersMethodControls->updatePluginsSettings();
    clustersPositionsControls->updatePluginsSettings();
    clustersFilteringControls->updatePluginsSettings();
    neighborsMethodControls->updatePluginsSettings();
    neighborsPositionsControls->updatePluginsSettings();

    if(media_cycle->getAvailablePluginNames(PLUGIN_TYPE_PREPROCESS,this->getMediaType()).size()>1) // ACNormalizePlugin is loaded by default
        ui.groupBoxPreProcess->show();
    else
        ui.groupBoxPreProcess->hide();

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::resetPluginsSettings()
{
    if(!media_cycle) return;

    this->resetMode();

    preProcessControls->setMediaCycle(this->media_cycle);
    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    clustersFilteringControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    preProcessControls->resetPluginsSettings();
    clustersMethodControls->resetPluginsSettings();
    clustersPositionsControls->resetPluginsSettings();
    clustersFilteringControls->resetPluginsSettings();
    neighborsMethodControls->resetPluginsSettings();
    neighborsPositionsControls->resetPluginsSettings();

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    if(!media_cycle) return;

    this->resetMode();

    preProcessControls->setMediaCycle(this->media_cycle);
    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    clustersFilteringControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    preProcessControls->changeMediaType(_media_type);
    clustersMethodControls->changeMediaType(_media_type);
    clustersPositionsControls->changeMediaType(_media_type);
    clustersFilteringControls->changeMediaType(_media_type);
    neighborsMethodControls->changeMediaType(_media_type);
    neighborsPositionsControls->changeMediaType(_media_type);

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::adjustHeight(){
    this->adjustSize();

    ui.groupBoxPreProcess->setMinimumHeight( preProcessControls->minimumHeight());
    ui.groupBoxClustersMethod->setMinimumHeight( clustersMethodControls->minimumHeight());
    ui.groupBoxClustersPositions->setMinimumHeight( clustersPositionsControls->minimumHeight());
    ui.tabClusters->setMinimumHeight(
                ui.groupBoxPreProcess->minimumHeight()
                + ui.groupBoxClustersMethod->minimumHeight()
                + ui.groupBoxClustersPositions->minimumHeight()
                + ui.groupBoxFiltering->minimumHeight()
                + 128
                );

    ui.groupBoxNeighborsMethod->setMinimumHeight( neighborsMethodControls->minimumHeight());
    ui.groupBoxNeighborsPositions->setMinimumHeight( neighborsPositionsControls->minimumHeight());
    ui.tabNeighbors->setMinimumHeight(
                ui.groupBoxNeighborsMethod->minimumHeight()
                + ui.groupBoxNeighborsPositions->minimumHeight()
                );
    int mode = ui.tabWidgetModes->currentIndex();

    if(mode == 0){
        ui.tabWidgetModes->setMinimumHeight(ui.tabClusters->minimumHeight());
    }
    else if(mode == 1){
        ui.tabWidgetModes->setMinimumHeight(ui.tabNeighbors->minimumHeight());
    }

    ui.dockWidgetContents->setMinimumHeight(
                + ui.tabWidgetModes->minimumHeight()
                + ui.groupBoxNavigation->minimumHeight()
                + 64
                );

    this->setMinimumHeight(ui.dockWidgetContents->minimumHeight() + 64);
    this->setMaximumHeight(ui.dockWidgetContents->minimumHeight() + 64);

    /*std::cout << "ACBrowserControlsCompleteDockWidgetQt::adjustHeight exiting" << std::endl;
    std::cout << "  tabWidgetModes min " << ui.tabWidgetModes->minimumHeight() << " actual " << ui.tabWidgetModes->height() << std::endl;
    std::cout << "    ui.tabClusters min " << ui.tabClusters->minimumHeight() << " actual " << ui.tabClusters->height() << std::endl;
    //std::cout << "      clustersMethodBox min " << clustersMethodBox->minimumHeight() << " actual " << clustersMethodBox->height() << std::endl;
    std::cout << "       clustersMethodControls min " << clustersMethodControls->minimumHeight() << " actual " << clustersMethodControls->height() << std::endl;
    //std::cout << "      clustersPositionsBox min " << clustersPositionsBox->minimumHeight() << " actual " << clustersPositionsBox->height() << std::endl;
    std::cout << "       clustersPositionsControls min " << clustersPositionsControls->minimumHeight() << " actual " << clustersPositionsControls->height() << std::endl;
    std::cout << "    ui.tabNeighbors min " << ui.tabNeighbors->minimumHeight() << " actual " << ui.tabNeighbors->height() << std::endl;
    std::cout << "      neighborsMethodControls min " << neighborsMethodControls->minimumHeight() << " actual " << neighborsMethodControls->height() << std::endl;
    std::cout << "      neighborsPositionsControls min " << neighborsPositionsControls->minimumHeight() << " actual " << neighborsPositionsControls->height() << std::endl;
    std::cout << "  ui.dockWidgetContents min " << ui.dockWidgetContents->minimumHeight() << " actual " << ui.dockWidgetContents->height() << std::endl;
    std::cout << "  this min " << this->minimumHeight() << " actual " << this->height() << std::endl;*/
}
