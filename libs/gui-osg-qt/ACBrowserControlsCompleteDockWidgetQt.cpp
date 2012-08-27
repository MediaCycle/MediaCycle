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

    clustersMethodControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_CLUSTERS_METHOD);
    clustersPositionsControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_CLUSTERS_POSITIONS);
    neighborsMethodControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_NEIGHBORS_METHOD);
    neighborsPositionsControls = new ACPluginControlsWidgetQt(PLUGIN_TYPE_NEIGHBORS_POSITIONS);

    ui.groupBoxClustersMethod->layout()->addWidget(clustersMethodControls);
    ui.groupBoxClustersPositions->layout()->addWidget(clustersPositionsControls);
    ui.groupBoxNeighborsMethod->layout()->addWidget(neighborsMethodControls);
    ui.groupBoxNeighborsPositions->layout()->addWidget(neighborsPositionsControls);

    int controlsWidth = 250; // default dock widget size
    clustersMethodControls->setFixedWidth(controlsWidth);
    clustersPositionsControls->setFixedWidth(controlsWidth);
    neighborsMethodControls->setFixedWidth(controlsWidth);
    neighborsPositionsControls->setFixedWidth(controlsWidth);

    // There are no previous/next navigation levels when starting the application
    ui.pushButtonBack->setEnabled(false);
    ui.pushButtonForward->setEnabled(false);

    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));
    connect(this,SIGNAL(reconfigureCheckBoxes()),this,SLOT(configureCheckBoxes()));

    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(clustersMethodControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(clustersPositionsControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(neighborsMethodControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    connect(neighborsPositionsControls,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));

    emit this->readjustHeight();

    this->show();
}

bool ACBrowserControlsCompleteDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACBrowserControlsCompleteDockWidgetQt::modifyListItem(QListWidgetItem *item)
{
    // XS check
    //cout << item->text().toStdString() << endl; // isselected...
    //cout << ui.featuresListWidget->currentRow() << endl;

    // end XS check

    if (osg_view && osg_view->isLibraryLoaded())
    {
        ui.featuresListWidget->setCurrentItem(item);
        float w;
        if (item->checkState() == Qt::Unchecked) w = 0.0;
        else w = 1.0 ;
        int f =  ui.featuresListWidget->currentRow(); // index of selected feature
        //std::cout << "ACBrowserControlsCompleteDockWidgetQt::modifyListItem: currentRow " << f << std::endl;
        media_cycle->setWeight(f,w);
        media_cycle->updateDisplay(true);
        //XS 250310 was: media_cycle->updateClusters(true);
        // XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
        osg_view->updateTransformsFromBrowser(0.0);
    }
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
    //this->synchronizeFeaturesWeights();
    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));

}

void ACBrowserControlsCompleteDockWidgetQt::on_pushButtonForward_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->goForward();
    //this->synchronizeFeaturesWeights();
    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
}

void ACBrowserControlsCompleteDockWidgetQt::on_tabWidgetModes_currentChanged(int index){

    if (media_cycle == 0) return;
    if(index == 0){//if(checked){
        //Clusters
        //media_cycle->changeClustersMethodPlugin(ui.comboBoxClustersMethod->currentText().toStdString());
        //media_cycle->changeClustersPositionsPlugin(ui.comboBoxClustersPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_CLUSTERS);

        ui.featuresListWidget->setEnabled(true); //CF until changing feature weights works in neighbors mode
    }
    else{
        //Neighbors
        //media_cycle->changeNeighborsMethodPlugin(ui.comboBoxNeighborsMethod->currentText().toStdString());
        //media_cycle->changeNeighborsPositionsPlugin(ui.comboBoxNeighborsPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_NEIGHBORS);

        ui.featuresListWidget->setEnabled(false); //CF until changing feature weights works in neighbors mode
    }

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::resetMode(){
    if(!media_cycle) return;

    switch (media_cycle->getBrowserMode()){
    case AC_MODE_CLUSTERS:
    {
        ui.tabWidgetModes->setCurrentIndex(0);
        ui.featuresListWidget->setEnabled(true); //CF until changing feature weights works in neighbors mode
    }
    break;
    case AC_MODE_NEIGHBORS:
    {
        ui.tabWidgetModes->setCurrentIndex(1);
        ui.featuresListWidget->setEnabled(false); //CF until changing feature weights works in neighbors mode
    }
    break;
    }
}


void ACBrowserControlsCompleteDockWidgetQt::updatePluginsSettings()
{
    emit this->reconfigureCheckBoxes();

    if(!media_cycle) return;

    this->resetMode();

    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    clustersMethodControls->updatePluginsSettings();
    clustersPositionsControls->updatePluginsSettings();
    neighborsMethodControls->updatePluginsSettings();
    neighborsPositionsControls->updatePluginsSettings();

    //Plugins according to media type
    //TODO Remember previous settings
    //std::cout << "ACBrowserControlsCompleteDockWidgetQt::changeMediaType: Plugins according to media type" << std::endl;
    if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED){
        if(!media_cycle->getLibrary())
            return;
        if(!media_cycle->getLibrary()->getMediaReaderPlugin())
            return;
        if(media_cycle->getLibrary()->getMediaReaderPlugin()->getName() == "ArchipelReader")
            clustersPositionsControls->on_comboBoxPlugins_activated("Archipel Atoll");
        // or media_cycle->changeClustersPositionsPlugin("Archipel Atoll"); ?
    }

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::resetPluginsSettings()
{
    this->cleanCheckBoxes();

    if(!media_cycle) return;

    this->resetMode();

    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    clustersMethodControls->resetPluginsSettings();
    clustersPositionsControls->resetPluginsSettings();
    neighborsMethodControls->resetPluginsSettings();
    neighborsPositionsControls->resetPluginsSettings();

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    if(!media_cycle) return;

    this->resetMode();

    clustersMethodControls->setMediaCycle(this->media_cycle);
    clustersPositionsControls->setMediaCycle(this->media_cycle);
    neighborsMethodControls->setMediaCycle(this->media_cycle);
    neighborsPositionsControls->setMediaCycle(this->media_cycle);

    clustersMethodControls->changeMediaType(_media_type);
    clustersPositionsControls->changeMediaType(_media_type);
    neighborsMethodControls->changeMediaType(_media_type);
    neighborsPositionsControls->changeMediaType(_media_type);

    emit this->readjustHeight();
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACBrowserControlsCompleteDockWidgetQt::synchronizeFeaturesWeights()
{
    if (media_cycle == 0) return;
    vector<float> w = media_cycle->getWeightVector();
    int nw = w.size();
    if (nw==0)
    {
        cout << "features not yet computed from plugins; setting all weights to 0" << endl;
        for (int i=0; i< ui.featuresListWidget->count(); i++)
        {
            ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
        }
        return;
    }
    else if (ui.featuresListWidget->count() != nw)
    {
        cerr << "Warning: Checkboxes in GUI do not match Features in MediaCycle" << endl;
        cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
        return;
        //exit(1);
    }
    else
    {
        for (int i=0; i< nw; i++)
        {
            if (w[i]==0)
                ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
            else
                ui.featuresListWidget->item(i)->setCheckState (Qt::Checked);
        }
    }
    //emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::configureCheckBoxes()
{
    //std::cout << "ACBrowserControlsCompleteDockWidgetQt::configureCheckBoxes" << std::endl;

    // dynamic config of checkboxes
    // according to plugins actually used to compute the features
    if (media_cycle == 0) return;

    vector<string> plugins_list = this->media_cycle->getListOfActivePlugins();
    vector<string> ::iterator list_iter;
    this->cleanCheckBoxes();

    for (list_iter = plugins_list.begin(); list_iter != plugins_list.end(); list_iter++)
    {
        QString s((*list_iter).c_str());
        QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
        item->setCheckState (Qt::Unchecked);
    }

    this->synchronizeFeaturesWeights();

    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));

    //emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::cleanCheckBoxes()
{
    //std::cout << "ACBrowserControlsCompleteDockWidgetQt::cleanCheckBoxes" << std::endl;

    for(int i = 0; i < ui.featuresListWidget->count(); i++)
        delete ui.featuresListWidget->takeItem(i);

    ui.featuresListWidget->clear();
    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::adjustHeight(){
    int line_heigth = 16;
    int max_number_of_lines = 5;

    int featureListHeight = line_heigth+8;
    int groupBoxSimilarityHeight = featureListHeight + 32;
    if (ui.featuresListWidget->count() >0){
        // CF resize the feature list along the number of features with threshold
        int current_number_of_lines = min(max_number_of_lines,ui.featuresListWidget->count())+1;
        //int current_number_of_lines = ui.featuresListWidget->count(); // uncomment this to remove the threshold
        featureListHeight = line_heigth*current_number_of_lines+8; // 8 px approx to avoid the scrollbar
        groupBoxSimilarityHeight = featureListHeight + 32;
    }
    ui.featuresListWidget->setMinimumHeight(featureListHeight);
    ui.featuresListWidget->setMaximumHeight(featureListHeight);
    ui.groupBoxSimilarity->setMinimumHeight(groupBoxSimilarityHeight);
    ui.groupBoxSimilarity->setMaximumHeight(groupBoxSimilarityHeight);

    ui.groupBoxClustersMethod->setMinimumHeight( clustersMethodControls->minimumHeight());
    ui.groupBoxClustersPositions->setMinimumHeight( clustersPositionsControls->minimumHeight());
    ui.tabClusters->setMinimumHeight(
                ui.groupBoxClustersMethod->minimumHeight()
                + ui.groupBoxClustersPositions->minimumHeight()
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
                ui.groupBoxSimilarity->minimumHeight()
                + ui.tabWidgetModes->minimumHeight()
                + ui.groupBoxNavigation->minimumHeight()
                + 64
                );

    this->setMinimumHeight(ui.dockWidgetContents->minimumHeight() + 64);
    this->setMaximumHeight(ui.dockWidgetContents->minimumHeight() + 64);

    /*std::cout << "ACBrowserControlsCompleteDockWidgetQt::adjustHeight exiting" << std::endl;
    std::cout << "  groupBoxNavigation min " << ui.groupBoxNavigation->minimumHeight() << " actual " << ui.groupBoxNavigation->height() << std::endl;
    std::cout << "  groupBoxSimilarity min " << ui.groupBoxSimilarity->minimumHeight() << " actual " << ui.groupBoxSimilarity->height() << std::endl;
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
