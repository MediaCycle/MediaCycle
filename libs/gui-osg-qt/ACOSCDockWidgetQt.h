/*
 *  ACOSCDockWidgetQt.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 05/04/11
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

#ifndef HEADER_ACOSCDOCKWIDGETQT
#define HEADER_ACOSCDOCKWIDGETQT

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"
#include "ui_ACOSCDockWidgetQt.h"

#include <MediaCycle.h>

#if defined (USE_OSC)
#include <ACOscBrowser.h>
#include <ACOscFeedback.h>
#endif //defined (USE_OSC)

class ACOSCDockWidgetQt : public ACAbstractDockWidgetQt {
Q_OBJECT

private slots:
	void on_pushButtonControlStart_clicked();
	void on_pushButtonFeedbackStart_clicked();

public:
	ACOSCDockWidgetQt(QWidget *parent = 0);
	~ACOSCDockWidgetQt();

#if defined (USE_OSC)	
	void disableControl();
	void disableFeedback();
	void setControlPort(int port);
	void setFeedbackPort(int port);
	ACOscBrowser* getControlHandler(){return osc_browser;}
	ACOscFeedback* getFeedbackHandler(){return osc_feedback;}

	void setMediaCycle(MediaCycle* _media_cycle){media_cycle = _media_cycle; if (osc_browser) osc_browser->setMediaCycle(_media_cycle);}
	void setAudioEngine(ACAudioEngine* _audio_engine){ audio_engine = _audio_engine; if (osc_browser) osc_browser->setAudioEngine(_audio_engine);}
#endif //defined (USE_OSC)

private:
	Ui::ACOSCDockWidgetQt ui;
#if defined (USE_OSC)
	ACOscBrowser *osc_browser;
	ACOscFeedback *osc_feedback;
#endif //defined (USE_OSC)
};

#endif
