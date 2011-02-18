/*
 *  ACVideoControlsDockWidgetQt.cpp
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

#include "ACVideoControlsDockWidgetQt.h"

ACVideoControlsDockWidgetQt::ACVideoControlsDockWidgetQt(QWidget *parent) : QDockWidget(parent)
{
	#if defined (SUPPORT_VIDEO)
	ui.setupUi(this); // first thing to do
	this->media_cycle = NULL;
	this->osg_view = NULL;
	ui.comboBoxVideoSummary->setEnabled(true);
	this->show();
	#endif //defined (SUPPORT_VIDEO)
}

ACVideoControlsDockWidgetQt::~ACVideoControlsDockWidgetQt(){
	#if defined (SUPPORT_VIDEO)
	//delete media_cycle;
	#endif //defined (SUPPORT_VIDEO)
}

#if defined (SUPPORT_VIDEO)
void ACVideoControlsDockWidgetQt::on_pushButtonMuteAll_clicked()
{
	//media_cycle->muteAllSources();
}

void ACVideoControlsDockWidgetQt::on_comboBoxVideoSummary_activated(const QString & text)
{
	if (osg_view){
		std::cout << "Video Summary: " << text.toStdString() << std::endl;
		int tracks = osg_view->getTimelineRenderer()->getNumberOfTracks();
		if (tracks>0){
			ACVideoSummaryType type = AC_VIDEO_SUMMARY_NONE;
			if (text.toStdString() == "Keyframes") type = AC_VIDEO_SUMMARY_KEYFRAMES;
			else if (text.toStdString() == "Slit-scan")	type = AC_VIDEO_SUMMARY_SLIT_SCAN;
			for (int track=0; track<tracks;track++)
				osg_view->getTimelineRenderer()->getTrack(track)->setSummaryType(type);
		}
		osg_view->setFocus();
	}
}
#endif //defined (SUPPORT_VIDEO)