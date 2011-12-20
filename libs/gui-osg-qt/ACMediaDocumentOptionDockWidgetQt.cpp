/**
 * @brief ACMediaDocumentOptionDockWidgetQt.cpp
 * @author Thierry Ravet
 * @date 20/12/2011
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

#include "ACMediaDocumentOptionDockWidgetQt.h"
#include "ui_ACMediaDocumentOptionDockWidgetQt.h"

ACMediaDocumentOptionDockWidgetQt::ACMediaDocumentOptionDockWidgetQt(QWidget *parent) 
:ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACMediaDocumentOptionDockWidgetQt")
{
    ui.setupUi(this);	
	connect(ui.mediaTypeComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(changeMediaType(QString)));

}

ACMediaDocumentOptionDockWidgetQt::~ACMediaDocumentOptionDockWidgetQt(){
}

void ACMediaDocumentOptionDockWidgetQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins actually used to compute the features
	if (media_cycle == 0) return;
	vector<int> indexMedia=this->media_cycle->getLibrary()->getParentIds();
	ACMediaDocument* temp=static_cast<ACMediaDocument*> (this->media_cycle->getLibrary()->getMedia(indexMedia[0]));
	if (temp==0)
		return;
	vector<string> plugins_list = temp->getActivableMediaKeys();
	vector<string> ::iterator list_iter;
	string actMediaKey=temp->getActiveMediaKey();

	initOn=true;
	ui.mediaTypeComboBox->clear();
	for (list_iter = plugins_list.begin(); list_iter != plugins_list.end(); list_iter++) {
		QString s((*list_iter).c_str());
//		QRadioButton * item = new QRadioButton(s,ui.groupBoxOption);
//		item->setChecked(false);
		ui.mediaTypeComboBox->addItem(s);
		if ((*list_iter)==actMediaKey)
		{
			ui.mediaTypeComboBox->setCurrentIndex(ui.mediaTypeComboBox->count()-1);
		}

	}
	ui.mediaTypeComboBox->setEnabled(true);
	initOn=false;
	
//	this->synchronizeFeaturesWeights();
	
//	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
//	this, SLOT(modifyListItem(QListWidgetItem*)));
	
}


void ACMediaDocumentOptionDockWidgetQt::cleanCheckBoxes(){
	
	
	ui.mediaTypeComboBox->setEnabled(false);
    //for(int i = 0; i < ui.mediaTypeComboBox->count(); i++)
      //  ui.mediaTypeComboBox->removeItem(i);
	ui.mediaTypeComboBox->clear();
	
    //ui.mediaTypeListWidget->clear();
	
}
void ACMediaDocumentOptionDockWidgetQt::changeMediaType(QString name){
	if (initOn)
		return;
	string nameStr=string(name.toAscii());
	media_cycle->setActiveMediaType(nameStr);
	media_cycle->initializeFeatureWeights(); 
	media_cycle->normalizeFeatures(1);
	
	
	media_cycle->libraryContentChanged(1); 
	emit changeLibraryMediaType();
	
	
}