/*
 *  ACAudioControlsDockWidgetQt.cpp
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

#include "ACAudioControlsDockWidgetQt.h"

ACAudioControlsDockWidgetQt::ACAudioControlsDockWidgetQt(QWidget *parent) : QDockWidget(parent)
{
	#if defined (SUPPORT_AUDIO)
	ui.setupUi(this); // first thing to do
	this->media_cycle = NULL;
	this->audio_engine = NULL;	
	this->show();
	#endif //defined (SUPPORT_AUDIO)
}

ACAudioControlsDockWidgetQt::~ACAudioControlsDockWidgetQt(){
	#if defined (SUPPORT_AUDIO)
	//delete audio_engine;
	//delete media_cycle;
	#endif //defined (SUPPORT_AUDIO)
}

#if defined (SUPPORT_AUDIO)
void ACAudioControlsDockWidgetQt::on_pushButtonMuteAll_clicked()
{
	if (media_cycle)
		media_cycle->muteAllSources();
}

void ACAudioControlsDockWidgetQt::on_pushButtonQueryRecord_toggled()
{
	if (audio_engine){
		if (ui.pushButtonQueryRecord->isChecked() == 1)
		{
			if (audio_engine->isCaptureAvailable())
			{
				ui.pushButtonQueryReplay->setEnabled(false);
				ui.pushButtonQueryKeep->setEnabled(false);
				ui.pushButtonQueryReferent->setEnabled(false);
				std::cout <<"Recording..."<<std::endl;
				// CF There is a delay before the recording actually starts: work around with a countdown on a modal window?
				audio_engine->startCapture();
			}
			else
				ui.pushButtonQueryRecord->setChecked(false);
		}
		else
		{
			if (audio_engine->isCaptureAvailable())
			{	
				audio_engine->stopCapture();	
				std::cout <<"Recording done."<<std::endl;
				ui.pushButtonQueryReplay->setEnabled(true);
				ui.pushButtonQueryKeep->setEnabled(true);
				ui.pushButtonQueryReferent->setEnabled(true);
			}	
		}	
	}	
}	

void ACAudioControlsDockWidgetQt::on_pushButtonQueryReplay_clicked()
{
	
}

// Pops up a modal window for saving the query as wavefile and add it to the library
void ACAudioControlsDockWidgetQt::on_pushButtonQueryKeep_clicked()
{
	//...
	
	//CF then only re-recording can re-enable the keeping
	ui.pushButtonQueryKeep->setEnabled(false);
}

void ACAudioControlsDockWidgetQt::on_pushButtonQueryReferent_clicked()
{
	
}
#endif //defined (SUPPORT_AUDIO)