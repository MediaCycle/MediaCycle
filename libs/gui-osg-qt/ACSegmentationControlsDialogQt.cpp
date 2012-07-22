/*
 *  ACSegmentationControlsDialogQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/06/2012
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

#include "ACSegmentationControlsDialogQt.h"

ACSegmentationControlsDialogQt::ACSegmentationControlsDialogQt(QWidget *parent)
    : QDialog(parent), ACAbstractWidgetQt(),widget(0),label(0),buttonBox(0)
{
    //ui.setupUi(this); // first thing to do if a *.ui file exists

    this->setFixedWidth(250);
    //this->setFixedHeight(500);
    this->setWindowTitle("Segmentation?");
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    label = new QLabel("Do you want to segment the media files?");
    label->setWordWrap(true);
    label->setFixedWidth(250);
    label->adjustSize();
    label->setFixedHeight( label->height() );
    layout->addWidget(label);

    widget = new ACPluginControlsWidgetQt(PLUGIN_TYPE_SEGMENTATION);
    layout->addWidget(widget);
    connect(widget,SIGNAL(readjustHeight()),this,SLOT(adjustHeight()));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes|QDialogButtonBox::No, Qt::Horizontal);
    layout->addWidget(buttonBox);
    connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

    this->setLayout(layout);
    this->adjustSize();

    this->show();
}

ACSegmentationControlsDialogQt::~ACSegmentationControlsDialogQt()
{
    disconnect(widget);
    disconnect(buttonBox);
    if(widget)
        delete widget;
    widget = 0;
    if(label)
        delete label;
    label = 0;
    if(buttonBox)
        delete buttonBox;
    buttonBox = 0;
}

void ACSegmentationControlsDialogQt::adjustHeight(){
    this->setMinimumHeight( widget->minimumHeight() + label->minimumHeight() + buttonBox->minimumHeight() );
    this->adjustSize();
}

void ACSegmentationControlsDialogQt::changeMediaType(ACMediaType _media_type)
{
    widget->setMediaCycle(this->media_cycle);
    widget->changeMediaType(_media_type);
}

void ACSegmentationControlsDialogQt::updatePluginsSettings()
{
    widget->setMediaCycle(this->media_cycle);
    widget->updatePluginsSettings();
    this->adjustSize();
}

void ACSegmentationControlsDialogQt::resetPluginsSettings()
{
    widget->resetPluginsSettings();
}

void ACSegmentationControlsDialogQt::resetMediaType(ACMediaType _media_type)
{
    widget->setMediaCycle(this->media_cycle);
    widget->resetMediaType(_media_type);
}
