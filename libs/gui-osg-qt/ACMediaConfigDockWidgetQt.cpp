/*
 *  ACMediaConfigDockWidgetQt.cpp
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

#include "ACMediaConfigDockWidgetQt.h"

ACMediaConfigDockWidgetQt::ACMediaConfigDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACMediaConfigDockWidgetQt")
{
	ui.setupUi(this); // first thing to do
	
	// Media types
    std::vector< std::string > mediaTypes = ACMediaFactory::getInstance().listAvailableMediaTypes();
    for (std::vector< std::string >::iterator mediaType = mediaTypes.begin(); mediaType!=mediaTypes.end(); ++mediaType)
        ui.comboLibrary->addItem(QString((*mediaType).c_str()));

	this->show();

    ui.labelSimilarity->hide();
    ui.comboSimilarity->hide();
    this->adjustSize();
}

bool ACMediaConfigDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACMediaConfigDockWidgetQt::on_comboLibrary_activated(const QString & text)
{
    emit mediaTypeChanged(text);
}

void ACMediaConfigDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    std::string mediaType = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(_media_type);
    int mediaIndex = ui.comboLibrary->findText(QString(mediaType.c_str()));
    if (mediaIndex > -1){
        ui.comboLibrary->setCurrentIndex(mediaIndex);
    }
}

void ACMediaConfigDockWidgetQt::resetMediaType(ACMediaType _media_type)
{
    string sMedia = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(_media_type);
    int comboIndex = ui.comboLibrary->findText(QString(sMedia.c_str()));
    if (comboIndex > -1)
        ui.comboLibrary->setCurrentIndex(comboIndex);//stay with the current config without reloading
    else
        ui.comboLibrary->setCurrentIndex(0);//display the startup combo value "-- Config --"
}

