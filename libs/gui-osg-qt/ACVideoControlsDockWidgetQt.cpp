/*
 *  ACVideoControlsDockWidgetQt.cpp
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

#include "ACVideoControlsDockWidgetQt.h"

ACVideoControlsDockWidgetQt::ACVideoControlsDockWidgetQt(QWidget *parent)
	: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_VIDEO,"ACVideoControlsDockWidgetQt")
{
	#if defined (SUPPORT_VIDEO)
	ui.setupUi(this); // first thing to do
	ui.comboBoxSummary->setEnabled(true);
	ui.comboBoxSelection->setEnabled(true);
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

void ACVideoControlsDockWidgetQt::on_comboBoxSummary_activated(const QString & text)
{
	if (osg_view){
		//std::cout << "Video Summary: " << text.toStdString() << std::endl;		
		ACVideoSummaryType type = osg_view->getTimelineRenderer()->getVideoSummaryType();
		if (text.toStdString() == "Keyframes") type = AC_VIDEO_SUMMARY_KEYFRAMES;
		else if (text.toStdString() == "Slit-scan")	type = AC_VIDEO_SUMMARY_SLIT_SCAN;
		else if (text.toStdString() == "None")	type = AC_VIDEO_SUMMARY_NONE;
		osg_view->getTimelineRenderer()->updateVideoSummaryType(type);
		osg_view->setFocus();
	}
}

void ACVideoControlsDockWidgetQt::on_comboBoxSelection_activated(const QString & text)
{
	if (osg_view){
		//std::cout << "Video Selection: " << text.toStdString() << std::endl;
		ACVideoSelectionType type = osg_view->getTimelineRenderer()->getVideoSelectionType();
		if (text.toStdString() == "Keyframes") type = AC_VIDEO_SELECTION_KEYFRAMES;
		else if (text.toStdString() == "Slit-scan")	type = AC_VIDEO_SELECTION_SLIT_SCAN;
		else if (text.toStdString() == "None")	type = AC_VIDEO_SELECTION_NONE;
		osg_view->getTimelineRenderer()->updateVideoSelectionType(type);
		osg_view->setFocus();
	}
}

void ACVideoControlsDockWidgetQt::on_checkBoxPlayback_stateChanged(int state)
{
	if (osg_view){
		//std::cout << "Video Playback: " << (bool)state << std::endl;
		osg_view->getTimelineRenderer()->updateVideoPlaybackVisibility((bool)state);
		osg_view->setFocus();
	}
}	
#endif //defined (SUPPORT_VIDEO)