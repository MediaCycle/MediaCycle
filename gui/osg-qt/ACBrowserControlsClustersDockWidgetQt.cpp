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

void ACBrowserControlsClustersDockWidgetQt::modifyListItem(QListWidgetItem *item) {
	// XS check
	cout << item->text().toStdString() << endl; // isselected...
	cout << ui.featuresListWidget->currentRow() << endl;
	
	// end XS check 
	
	if (osg_view && osg_view->isLibraryLoaded()){
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

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonRecenter_clicked() {
	if (media_cycle==NULL) return; 
	this->media_cycle->setCameraRecenter();
}	

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonBack_clicked() {
	if (media_cycle==NULL) return; 
	this->media_cycle->goBack();
	this->synchronizeFeaturesWeights();
	//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	this->media_cycle->dumpNavigationLevel() ;
	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACBrowserControlsClustersDockWidgetQt::on_pushButtonForward_clicked() {
	if (media_cycle==NULL) return; 
	
	this->media_cycle->goForward();
	this->synchronizeFeaturesWeights();
	
	//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	this->media_cycle->dumpNavigationLevel() ;
	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACBrowserControlsClustersDockWidgetQt::on_spinBoxClusters_valueChanged(int _value){
	if (media_cycle==NULL && osg_view == NULL) return; 
	
	ui.sliderClusters->setValue(_value); 	// for synchronous display of values 
	std::cout << "ClusterNumber: " << _value << std::endl;
	if (osg_view->isLibraryLoaded()){
		media_cycle->setClusterNumber(_value);
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
		osg_view->updateTransformsFromBrowser(1.0);
	}
	osg_view->setFocus();
}

// for synchronous display of values 
void ACBrowserControlsClustersDockWidgetQt::on_sliderClusters_sliderReleased(){
	if (media_cycle == NULL) return; 
	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACBrowserControlsClustersDockWidgetQt::synchronizeFeaturesWeights(){
	if (media_cycle == NULL) return; 
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	if (nw==0){
		cout << "features not yet computed from plugins; setting all weights to 0" << endl;
		for (int i=0; i< ui.featuresListWidget->count(); i++){
			ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
		}
		return;
	}
	else if (ui.featuresListWidget->count() != nw){
		cerr << "Warning: Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
		return;
		//exit(1);
	}
	else {
		for (int i=0; i< nw; i++){
			if (w[i]==0) 
				ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
			else
				ui.featuresListWidget->item(i)->setCheckState (Qt::Checked);		
		}
	}	
}

void ACBrowserControlsClustersDockWidgetQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins found by plugin manager
	if (media_cycle == NULL) return;
	ACPluginManager *acpl = this->media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES && acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == media_cycle->getLibrary()->getMediaType()) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
					item->setCheckState (Qt::Unchecked);
				}
			}
		}
	}
	this->synchronizeFeaturesWeights();
	
	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(modifyListItem(QListWidgetItem*)));
	
}

void ACBrowserControlsClustersDockWidgetQt::cleanCheckBoxes(){
	for(int i = 0; i < ui.featuresListWidget->count(); i++)
		delete ui.featuresListWidget->takeItem(i);
	
	ui.featuresListWidget->clear();
}