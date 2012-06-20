/*
 *  ACSegmentationControlsDockWidgetQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 2/03/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACSegmentationControlsDockWidgetQt.h"

ACSegmentationControlsDockWidgetQt::ACSegmentationControlsDockWidgetQt(QWidget *parent)
    : ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACSegmentationControlsDockWidgetQt")
{
    //ui.setupUi(this); // first thing to do if a *.ui file exists
    widget = new ACSegmentationControlsWidgetQt();
    widget->setFixedWidth(250);
    this->setWidget(widget);
    this->setWindowTitle("Segmentation Controls");
    connect(widget,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));
    this->show();
}

ACSegmentationControlsDockWidgetQt::~ACSegmentationControlsDockWidgetQt()
{
    disconnect(widget);
    if(widget)
        delete widget;
    widget = 0;
}

bool ACSegmentationControlsDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return (_media_type == MEDIA_TYPE_AUDIO || _media_type == MEDIA_TYPE_VIDEO);
}

void ACSegmentationControlsDockWidgetQt::adjustHeight(){
    widget->setMaximumHeight( widget->minimumHeight() );
    this->adjustSize();
}

void ACSegmentationControlsDockWidgetQt::changeMediaType(ACMediaType _media_type)
{
    widget->setMediaCycle(this->media_cycle);
    if(!media_cycle) return;
    if(this->canBeVisible(media_cycle->getMediaType()))
        widget->changeMediaType(_media_type);
}

void ACSegmentationControlsDockWidgetQt::updatePluginsSettings()
{
    widget->setMediaCycle(this->media_cycle);
    if(!media_cycle) return;
    if(this->canBeVisible(media_cycle->getMediaType()))
        widget->updatePluginsSettings();
}

void ACSegmentationControlsDockWidgetQt::resetPluginsSettings()
{
    /*if(!media_cycle) return;
    if(this->canBeVisible(media_cycle->getMediaType()))
        widget->resetPluginsSettings();*/
}

void ACSegmentationControlsDockWidgetQt::resetMediaType(ACMediaType _media_type)
{
    /*widget->setMediaCycle(this->media_cycle);
    if(this->canBeVisible(media_cycle->getMediaType()))
        widget->resetMediaType(_media_type);*/
}
