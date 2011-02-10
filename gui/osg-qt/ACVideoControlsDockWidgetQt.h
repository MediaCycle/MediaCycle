/*
 *  ACVideoControlsDockWidgetQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 8/11/11
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef HEADER_ACVIDEOCONTROLSDOCKWIDGETQT
#define HEADER_ACVIDEOCONTROLSDOCKWIDGETQT

#include <iostream>
#include <string.h>

#include <QtGui>

#include "ui_ACVideoControlsDockWidgetQt.h"
#include <MediaCycle.h>
#include <ACOsgCompositeViewQt.h>
#include <ACAudioEngine.h>

class ACVideoControlsDockWidgetQt : public QDockWidget {
Q_OBJECT
	
private slots:
	// Video controls
	void on_pushButtonMuteAll_clicked();
	void on_comboBoxVideoSummary_activated(const QString & text);
	
public:
	ACVideoControlsDockWidgetQt(QWidget *parent = 0);
	~ACVideoControlsDockWidgetQt();
	
	void setMediaCycle(MediaCycle* _media_cycle){ media_cycle = _media_cycle;}
	MediaCycle* getMediaCycle() {return media_cycle;}
	void setOsgView(ACOsgCompositeViewQt* _osg_view){ osg_view = _osg_view;}
	ACOsgCompositeViewQt* getOsgView() {return osg_view;}
	
private:
	Ui::ACVideoControlsDockWidgetQt ui;
	MediaCycle* media_cycle;
	ACOsgCompositeViewQt* osg_view;
};
#endif
