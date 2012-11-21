/**
 * @brief ACMediaDocumentOptionDockWidgetQt.cpp
 * @author Christian Frisson
 * @date 24/07/2012
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

#include "ACMediaDocumentOptionDockWidgetQt.h"

ACMediaDocumentOptionDockWidgetQt::ACMediaDocumentOptionDockWidgetQt(QWidget *parent)
#ifdef SUPPORT_MULTIMEDIA
    :ACAbstractDockWidgetQt(parent, MEDIA_TYPE_MIXED,"ACMediaDocumentOptionDockWidgetQt")
    #endif//def SUPPORT_MULTIMEDIA
{
    ui.setupUi(this);
#ifdef SUPPORT_MULTIMEDIA
    connect(ui.mediaTypeComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(changeMediaType(QString)));
#endif//def SUPPORT_MULTIMEDIA

}

ACMediaDocumentOptionDockWidgetQt::~ACMediaDocumentOptionDockWidgetQt(){
}

bool ACMediaDocumentOptionDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return (_media_type == this->getMediaType());
}

#ifdef SUPPORT_MULTIMEDIA
void ACMediaDocumentOptionDockWidgetQt::updatePluginsSettings()
{
    // according to plugins actually used to compute the features
    if(media_cycle == 0) return;
    if(media_cycle->getMediaType()!=MEDIA_TYPE_MIXED) return;
    if(media_cycle->getLibrary()->getSize()==0) return;
    vector<int> indexMedia=this->media_cycle->getLibrary()->getParentIds();
    if(indexMedia.size() == 0) return;
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

void ACMediaDocumentOptionDockWidgetQt::resetPluginsSettings(){

    if(media_cycle == 0) return;
    if(media_cycle->getMediaType()!=MEDIA_TYPE_MIXED) return;
    if(media_cycle->getLibrary()->getSize()==0) return;

    ui.mediaTypeComboBox->setEnabled(false);
    //for(int i = 0; i < ui.mediaTypeComboBox->count(); i++)
    //  ui.mediaTypeComboBox->removeItem(i);
    ui.mediaTypeComboBox->clear();

    //ui.mediaTypeListWidget->clear();

}
#endif//def SUPPORT_MULTIMEDIA

void ACMediaDocumentOptionDockWidgetQt::changeMediaType(QString name){
    #ifdef SUPPORT_MULTIMEDIA
    if (initOn)
        return;
    emit activeMediaTypeChanged(name);
    #endif//def SUPPORT_MULTIMEDIA
}
