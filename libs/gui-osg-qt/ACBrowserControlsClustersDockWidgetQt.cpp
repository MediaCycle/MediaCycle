/*
 *  ACBrowserControlsClustersDockWidgetQt.cpp
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

#include "ACBrowserControlsClustersDockWidgetQt.h"

ACBrowserControlsClustersDockWidgetQt::ACBrowserControlsClustersDockWidgetQt(QWidget *parent)
    : ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACBrowserControlsClustersDockWidgetQt")
{
    ui.setupUi(this); // first thing to do
    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));
    this->show();
}

void ACBrowserControlsClustersDockWidgetQt::modifyListItem(QListWidgetItem *item)
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

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonRecenter_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->setCameraRecenter();
}

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonBack_clicked()
{
    if (media_cycle==0) return;
    this->media_cycle->goBack();
    this->synchronizeFeaturesWeights();
    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));

    // XS debug
//	this->media_cycle->dumpNavigationLevel() ;
//	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonForward_clicked()
{
    if (media_cycle==0) return;

    this->media_cycle->goForward();
    this->synchronizeFeaturesWeights();

    //	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));

    // XS debug
//	this->media_cycle->dumpNavigationLevel() ;
//	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACBrowserControlsClustersDockWidgetQt::on_spinBoxClusters_valueChanged(int _value)
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
void ACBrowserControlsClustersDockWidgetQt::on_sliderClusters_sliderReleased()
{
    if (media_cycle == 0) return;
    ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACBrowserControlsClustersDockWidgetQt::on_comboBoxClustersMethod_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Clusters Method: " << text.toStdString() << std::endl;
    media_cycle->changeClustersMethodPlugin(text.toStdString());
}

void ACBrowserControlsClustersDockWidgetQt::on_comboBoxClustersPositions_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Clusters Positions: " << text.toStdString() << std::endl;
    media_cycle->changeClustersPositionsPlugin(text.toStdString());
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACBrowserControlsClustersDockWidgetQt::synchronizeFeaturesWeights()
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
}

void ACBrowserControlsClustersDockWidgetQt::configureCheckBoxes()
{
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

}

void ACBrowserControlsClustersDockWidgetQt::cleanCheckBoxes()
{
    for(int i = 0; i < ui.featuresListWidget->count(); i++)
        delete ui.featuresListWidget->takeItem(i);

    ui.featuresListWidget->clear();
}

void ACBrowserControlsClustersDockWidgetQt::updatePluginLists()
{
    if (media_cycle == 0) return;
    //CF restore default mode
    ui.comboBoxClustersMethod->clear();
    ui.comboBoxClustersMethod->addItem(QString("KMeans (default)"));
    ui.comboBoxClustersMethod->setEnabled(false);

    ui.comboBoxClustersPositions->clear();
    ui.comboBoxClustersPositions->addItem(QString("Propeller (default)"));
    ui.comboBoxClustersPositions->setEnabled(false);

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
                //CF we don't yet deal with Visualisation Plugins (combining Methods and Positions for Clusters and/or Neighborhoods)
            }
        }
    }
}
