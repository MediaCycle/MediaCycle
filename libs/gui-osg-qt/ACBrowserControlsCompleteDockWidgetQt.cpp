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
    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));
    connect(this,SIGNAL(reconfigureCheckBoxes()),this,SLOT(configureCheckBoxes()));
    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    this->show();
    emit this->readjustHeight();
}

bool ACBrowserControlsCompleteDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACBrowserControlsCompleteDockWidgetQt::modifyListItem(QListWidgetItem *item)
{
    // XS check
    cout << item->text().toStdString() << endl; // isselected...
    cout << ui.featuresListWidget->currentRow() << endl;

    // end XS check

    if (osg_view && osg_view->isLibraryLoaded())
    {
        float w;
        if (item->checkState() == Qt::Unchecked) w = 0.0;
        else w = 1.0 ;
        int f =  ui.featuresListWidget->currentRow(); // index of selected feature
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

void ACBrowserControlsCompleteDockWidgetQt::on_spinBoxClusters_valueChanged(int _value)
{
    if (media_cycle==0 && osg_view == 0) return;

    ui.sliderClusters->setValue(_value); 	// for synchronous display of values
    std::cout << "ClusterNumber: " << _value << std::endl;
    if (osg_view->isLibraryLoaded())
    {
        media_cycle->setClusterNumber(_value);
        // XSCF251003 added this
        media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
        // XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
        osg_view->updateTransformsFromBrowser(1.0);
    }
    osg_view->setFocus();
}

// for synchronous display of values
void ACBrowserControlsCompleteDockWidgetQt::on_sliderClusters_sliderReleased()
{
    if (media_cycle == 0) return;
    ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACBrowserControlsCompleteDockWidgetQt::on_comboBoxClustersMethod_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Clusters Method: " << text.toStdString() << std::endl;
    media_cycle->changeClustersMethodPlugin(text.toStdString());
}

void ACBrowserControlsCompleteDockWidgetQt::on_comboBoxClustersPositions_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Clusters Positions: " << text.toStdString() << std::endl;
    media_cycle->changeClustersPositionsPlugin(text.toStdString());
}

void ACBrowserControlsCompleteDockWidgetQt::on_comboBoxNeighborsMethod_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Neighbors Method: " << text.toStdString() << std::endl;
    media_cycle->changeNeighborsMethodPlugin(text.toStdString());
}

void ACBrowserControlsCompleteDockWidgetQt::on_comboBoxNeighborsPositions_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Neighbors Positions: " << text.toStdString() << std::endl;
    media_cycle->changeNeighborsPositionsPlugin(text.toStdString());
}

void ACBrowserControlsCompleteDockWidgetQt::on_radioButtonClusters_toggled( bool checked )
{
    if (media_cycle == 0) return;
    if(checked){
        //Clusters
        media_cycle->changeClustersMethodPlugin(ui.comboBoxClustersMethod->currentText().toStdString());
        media_cycle->changeClustersPositionsPlugin(ui.comboBoxClustersPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_CLUSTERS);
    }
    else{
        //Neighbors
        media_cycle->changeNeighborsMethodPlugin(ui.comboBoxNeighborsMethod->currentText().toStdString());
        media_cycle->changeNeighborsPositionsPlugin(ui.comboBoxNeighborsPositions->currentText().toStdString());
        media_cycle->changeBrowserMode(AC_MODE_NEIGHBORS);
    }
}

void ACBrowserControlsCompleteDockWidgetQt::updatePluginsSettings()
{
    emit this->reconfigureCheckBoxes();

    //Plugins according to media type
    //TODO Remember previous settings
    std::cout << "ACBrowserControlsCompleteDockWidgetQt::changeMediaType: Plugins according to media type" << std::endl;
    if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED){
        int comboBoxClustersPositionsIndex = ui.comboBoxClustersPositions->findText("Archipel Atoll");
        if (comboBoxClustersPositionsIndex > -1){
            ui.comboBoxClustersPositions->setCurrentIndex(comboBoxClustersPositionsIndex);
            media_cycle->changeClustersPositionsPlugin("Archipel Atoll");
        }
    }

    if(this->media_cycle){
        if(this->media_cycle->hasBrowser()){
            if ( media_cycle->getBrowserMode() == AC_MODE_CLUSTERS)
                ui.radioButtonClusters->setChecked(true);
            else if ( media_cycle->getBrowserMode() == AC_MODE_NEIGHBORS)
                ui.radioButtonNeighbors->setChecked(true);
        }
    }
}

void ACBrowserControlsCompleteDockWidgetQt::resetPluginsSettings()
{
    this->cleanCheckBoxes();
}

void ACBrowserControlsCompleteDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    this->updatePluginLists();
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
    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::configureCheckBoxes()
{
    std::cout << "ACBrowserControlsCompleteDockWidgetQt::configureCheckBoxes" << std::endl;

    // dynamic config of checkboxes
    // according to plugins actually used to compute the features
    if (media_cycle == 0) return;

    vector<string> plugins_list = this->media_cycle->getListOfActivePlugins();
    vector<string> ::iterator list_iter;

    for (list_iter = plugins_list.begin(); list_iter != plugins_list.end(); list_iter++)
    {
        QString s((*list_iter).c_str());
        QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
        item->setCheckState (Qt::Unchecked);
    }

    this->synchronizeFeaturesWeights();

    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));

    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::cleanCheckBoxes()
{
    std::cout << "ACBrowserControlsCompleteDockWidgetQt::cleanCheckBoxes" << std::endl;

    for(int i = 0; i < ui.featuresListWidget->count(); i++)
        delete ui.featuresListWidget->takeItem(i);

    ui.featuresListWidget->clear();
    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::updatePluginLists()
{
    std::cout << "ACBrowserControlsCompleteDockWidgetQt::updatePluginLists" << std::endl;
    if (media_cycle == 0) return;
    //CF restore default mode
    ui.comboBoxClustersMethod->clear();
    ui.comboBoxClustersMethod->addItem(QString("KMeans (default)"));
    ui.comboBoxClustersMethod->setEnabled(false);

    ui.comboBoxClustersPositions->clear();
    ui.comboBoxClustersPositions->addItem(QString("Propeller (default)"));
    ui.comboBoxClustersPositions->setEnabled(false);

    ui.comboBoxNeighborsMethod->clear();
    ui.comboBoxNeighborsMethod->addItem(QString("None available"));
    ui.comboBoxNeighborsMethod->setEnabled(false);

    ui.comboBoxNeighborsPositions->clear();
    ui.comboBoxNeighborsPositions->addItem(QString("None available"));
    ui.comboBoxNeighborsPositions->setEnabled(false);

    //setPreProcessPlugin

    ACPluginManager *acpl = media_cycle->getPluginManager();
    if (acpl)
    {
        for (int i=0; i<acpl->getSize(); i++)
        {
            for (int j=0; j<acpl->getPluginLibrary(i)->getSize(); j++)
            {
                //if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == media_cycle->getLibrary()->getMediaType()) {
                if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_CLUSTERS_METHOD))
                {
                    //CF on the first detected Clusters Method plugin
                    if (ui.comboBoxClustersMethod->count() == 1 && ui.comboBoxClustersMethod->currentText().toStdString() == "KMeans (default)")
                    {
                        ui.comboBoxClustersMethod->setEnabled(true);
                        //CF default settings: no Clusters Method plugin, use KMeans
                    }
                    ui.comboBoxClustersMethod->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
                    //if (media_cycle->getMediaType() == MEDIA_TYPE_TEXT && ui.comboBoxClustersMethod->currentText().toStdString() == "KMeans (default)")
                }
                else if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_CLUSTERS_POSITIONS))
                {
                    //CF on the first detected Clusters Positions plugin
                    if (ui.comboBoxClustersPositions->count() == 1 && ui.comboBoxClustersPositions->currentText().toStdString() == "Propeller (default)")
                    {
                        ui.comboBoxClustersPositions->setEnabled(true);
                        //CF default settings: no Clusters Positions plugin, use Propeller
                    }
                    ui.comboBoxClustersPositions->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));

                }
                else if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_METHOD))
                {
                    //CF on the first detected Neighbors Method plugin
                    if (ui.comboBoxNeighborsMethod->count() == 1 && ui.comboBoxNeighborsMethod->currentText().toStdString() == "None available")
                    {
                        ui.comboBoxNeighborsMethod->clear();
                        ui.comboBoxNeighborsMethod->setEnabled(true);
                    }
                    ui.comboBoxNeighborsMethod->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
                }
                else if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_POSITIONS))
                {
                    //CF on the first detected Neighbors Positions plugin
                    if (ui.comboBoxNeighborsPositions->count() == 1 && ui.comboBoxNeighborsPositions->currentText().toStdString() == "None available")
                    {
                        ui.comboBoxNeighborsPositions->clear();
                        ui.comboBoxNeighborsPositions->setEnabled(true);
                    }
                    ui.comboBoxNeighborsPositions->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
                }
                //CF we don't yet deal with Visualisation Plugins (combining Methods and Positions for Clusters and/or Neighborhoods)
            }
        }
    }

    if (ui.comboBoxNeighborsMethod->currentText().toStdString() != "None available" && ui.comboBoxNeighborsPositions->currentText().toStdString() != "None available")
    {
        ui.radioButtonClusters->setEnabled(true);
        ui.radioButtonNeighbors->setEnabled(true);
    }

    if(media_cycle->getMediaType() == MEDIA_TYPE_TEXT || media_cycle->getMediaType() == MEDIA_TYPE_MIXED){
        int comboBoxClustersMethodIndex = ui.comboBoxClustersMethod->findText("ACSparseCosKMeans");
        if (comboBoxClustersMethodIndex > -1){
            ui.comboBoxClustersMethod->setCurrentIndex(comboBoxClustersMethodIndex);
            media_cycle->changeClustersMethodPlugin("ACSparseCosKMeans");
        }
        int comboBoxClustersPositionsIndex = ui.comboBoxClustersPositions->findText("ACSparseCosClustPosition");
        if (comboBoxClustersPositionsIndex > -1){
            ui.comboBoxClustersPositions->setCurrentIndex(comboBoxClustersPositionsIndex);
            media_cycle->changeClustersPositionsPlugin("ACSparseCosClustPosition");
        }
    }
    /*else{
        ui.comboBoxClustersMethod->setCurrentIndex(1);
        ui.comboBoxClustersPositions->setCurrentIndex(1);
    }*/
    emit this->readjustHeight();
}

void ACBrowserControlsCompleteDockWidgetQt::adjustHeight(){
    if (ui.featuresListWidget->sizeHintForRow(0) >-1 && ui.featuresListWidget->count() >0){
        // CF resize the feature list along the number of features with threshold
        int max_number_of_lines = 9; // allows the current 18 audio features to be accessed on 2 pages
        int current_number_of_lines = min(max_number_of_lines,ui.featuresListWidget->count());
        //int current_number_of_lines = ui.featuresListWidget->count(); // uncomment this to remove the threshold
        int featureListHeight = ui.featuresListWidget->sizeHintForRow(0)*current_number_of_lines+8; // 8 px approx to avoid the scrollbar
        int groupBoxSimilarityHeight = ui.featuresListWidget->sizeHintForRow(0)*(current_number_of_lines+2); // 2 more lines as security factor

        ui.featuresListWidget->setMinimumHeight(featureListHeight);
        ui.featuresListWidget->setFixedHeight(featureListHeight);
        ui.groupBoxSimilarity->setMinimumHeight(groupBoxSimilarityHeight);
        ui.groupBoxSimilarity->setFixedHeight(groupBoxSimilarityHeight);
    }
    else{
        // CF resize the feature list to a default min size
        ui.featuresListWidget->setMinimumHeight(16);
        ui.featuresListWidget->setFixedHeight(16);
        ui.groupBoxSimilarity->setMinimumHeight(64);
        ui.groupBoxSimilarity->setFixedHeight(64);
    }
    ui.featuresListWidget->adjustSize();
    ui.groupBoxSimilarity->adjustSize();

    ui.dockWidgetContents->setMinimumHeight(
        ui.groupBoxSimilarity->minimumHeight()
        + ui.groupBoxClusters->minimumHeight()
        + ui.groupBoxNeighbors->minimumHeight()
        + ui.widgetModes->minimumHeight()
        + ui.widgetNavigation->minimumHeight()
        + 64
    );
    ui.dockWidgetContents->adjustSize();
    ui.verticalLayoutWidget->adjustSize();
    this->adjustSize();
}

