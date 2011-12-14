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
	#if defined (SUPPORT_AUDIO)
		ui.comboDefaultSettings->addItem(QString("Audio"));
	#endif //defined (SUPPORT_AUDIO)
	#if defined (SUPPORT_IMAGE)
		ui.comboDefaultSettings->addItem(QString("Image"));
	#endif //defined (SUPPORT_IMAGE)
	#if defined (SUPPORT_VIDEO)
		ui.comboDefaultSettings->addItem(QString("Video"));
	#endif //defined (SUPPORT_VIDEO)
    #if defined (SUPPORT_3DMODEL)
		ui.comboDefaultSettings->addItem(QString("3DModel"));
	#endif //defined (SUPPORT_3DMODEL)
	#if defined (SUPPORT_TEXT)
		ui.comboDefaultSettings->addItem(QString("Text"));
	#endif //defined (SUPPORT_TEXT)
    #if defined (SUPPORT_MULTIMEDIA)
	ui.comboDefaultSettings->addItem(QString("Mixed"));
    #endif //defined (SUPPORT_MULTIMEDIA)
	this->show();
}
