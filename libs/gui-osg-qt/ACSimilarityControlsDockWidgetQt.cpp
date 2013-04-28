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

#include "ACSimilarityControlsDockWidgetQt.h"

ACSimilarityControlsDockWidgetQt::ACSimilarityControlsDockWidgetQt(QWidget *parent)
    : ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACSimilarityControlsDockWidgetQt")
{
    ui.setupUi(this); // first thing to do

    int controlsWidth = 250; // default dock widget size

    connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));
    connect(this,SIGNAL(reconfigureCheckBoxes()),this,SLOT(configureCheckBoxes()));

    connect(this,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));

    emit this->readjustHeight();

    this->show();
}

bool ACSimilarityControlsDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACSimilarityControlsDockWidgetQt::modifyListItem(QListWidgetItem *item)
{
    // XS check
    //cout << item->text().toStdString() << endl; // isselected...
    //cout << ui.featuresListWidget->currentRow() << endl;

    // end XS check
    cout<<"osg_view "<<osg_view<<endl;
    if (osg_view && osg_view->isLibraryLoaded())
    {
        ui.featuresListWidget->setCurrentItem(item);
        float w;
        if (item->checkState() == Qt::Unchecked) w = 0.0;
        else w = 1.0 ;
        int f =  ui.featuresListWidget->currentRow(); // index of selected feature
        //std::cout << "ACSimilarityControlsDockWidgetQt::modifyListItem: currentRow " << f << std::endl;
        media_cycle->setWeight(f,w);
        media_cycle->updateDisplay(true);
        //XS 250310 was: media_cycle->updateClusters(true);
        // XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
        //osg_view->updateTransformsFromBrowser(0.0);
    }
}

void ACSimilarityControlsDockWidgetQt::resetMode(){
    if(!media_cycle) return;

    switch (media_cycle->getBrowserMode()){
    case AC_MODE_CLUSTERS:
    {
        ui.featuresListWidget->setEnabled(true); //CF until changing feature weights works in neighbors mode
    }
        break;
    case AC_MODE_NEIGHBORS:
    {
        ui.featuresListWidget->setEnabled(false); //CF until changing feature weights works in neighbors mode
    }
        break;
    default:
        break;
    }
}


void ACSimilarityControlsDockWidgetQt::updatePluginsSettings()
{
    emit this->reconfigureCheckBoxes();

    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

void ACSimilarityControlsDockWidgetQt::resetPluginsSettings()
{
    this->cleanCheckBoxes();

    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

void ACSimilarityControlsDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    if(!media_cycle) return;

    this->resetMode();

    emit this->readjustHeight();
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACSimilarityControlsDockWidgetQt::synchronizeFeaturesWeights()
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

void ACSimilarityControlsDockWidgetQt::configureCheckBoxes()
{
    //std::cout << "ACSimilarityControlsDockWidgetQt::configureCheckBoxes" << std::endl;

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

void ACSimilarityControlsDockWidgetQt::on_pushButtonAll_clicked(){
    for(int item = 0; item < ui.featuresListWidget->count(); item++){
        if(ui.featuresListWidget->item(item)){
            ui.featuresListWidget->item(item)->setCheckState (Qt::Checked);
            media_cycle->setWeight(item,1.0f);
        }
    }
    media_cycle->updateDisplay(true);
    //osg_view->updateTransformsFromBrowser(0.0);
}

void ACSimilarityControlsDockWidgetQt::on_pushButtonNone_clicked(){
    for(int item = 0; item < ui.featuresListWidget->count(); item++){
        if(ui.featuresListWidget->item(item)){
            ui.featuresListWidget->item(item)->setCheckState (Qt::Unchecked);
            media_cycle->setWeight(item,0.0f);
        }
    }
    media_cycle->updateDisplay(true);
    //osg_view->updateTransformsFromBrowser(0.0);
}

void ACSimilarityControlsDockWidgetQt::cleanCheckBoxes()
{
    //std::cout << "ACSimilarityControlsDockWidgetQt::cleanCheckBoxes" << std::endl;
    for(int i = 0; i < ui.featuresListWidget->count(); i++)
        delete ui.featuresListWidget->takeItem(i);

    ui.featuresListWidget->clear();
    emit this->readjustHeight();
}

void ACSimilarityControlsDockWidgetQt::adjustHeight(){
}
