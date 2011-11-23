/*
 *  ACOSCDockWidgetQt.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 05/04/11
 *  Filled by Christian Frisson since 03/05/11.
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

#include "ACOSCDockWidgetQt.h"

ACOSCDockWidgetQt::ACOSCDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACOSCDockWidgetQt")
{
	#if defined (USE_OSC)
	ui.setupUi(this); // first thing to do
	this->show();
	osc_browser = 0;
	osc_feedback = 0;
	#endif //defined (USE_OSC)
	auto_connect = false;
}

ACOSCDockWidgetQt::~ACOSCDockWidgetQt(){
	#if defined (USE_OSC)
	if (osc_browser) {
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
	}
	if (osc_feedback) {
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
	}
	#endif //defined (USE_OSC)
}

void ACOSCDockWidgetQt::autoConnect(bool _status)
{
	auto_connect = _status;
}

#if defined (USE_OSC)
void ACOSCDockWidgetQt::toggleControl(bool _status){
	if(_status){
		osc_browser = new ACOscBrowser();
		osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.spinBoxControlPort->value());
		osc_browser->setMediaCycle(media_cycle);
	#ifdef SUPPORT_AUDIO
		osc_browser->setAudioEngine(audio_engine);
	#endif//def SUPPORT_AUDIO
                osc_browser->start();
		ui.pushButtonControlStart->setText("Stop");
	}
	else {
		osc_browser->stop();
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
		ui.pushButtonControlStart->setText("Start");
	}
}

void ACOSCDockWidgetQt::toggleFeedback(bool _status){
	if(_status){
		osc_feedback = new ACOscFeedback();
                if (media_cycle)
                    media_cycle->attach(this->osc_feedback);

		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.spinBoxFeedbackPort->value());
		if (osc_browser)
			osc_browser->setFeedback(osc_feedback);
                osc_feedback->messageBegin("test mc send osc");
		std::cout << "sending test messages to " << ui.lineEditFeedbackIP->text().toStdString().c_str() << " on port " << ui.spinBoxFeedbackPort->value() << " ..." << endl;
		osc_feedback->messageSend();
		ui.pushButtonFeedbackStart->setText("Stop");	
	}
	else {
		if (osc_browser)
			osc_browser->setFeedback(0);
		osc_feedback->release(); // XS TODO for browser it was stop ?!
		delete osc_feedback;
		osc_feedback = 0;
		ui.pushButtonFeedbackStart->setText("Start");		
	}		
}

void ACOSCDockWidgetQt::on_pushButtonControlStart_clicked() {
	std::cout << "Control IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Control Port: " << ui.spinBoxControlPort->value() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start") {
		this->toggleControl(true);
	}
	else if ( ui.pushButtonControlStart->text().toStdString() == "Stop") {
		this->toggleControl(false);
	}
}

void ACOSCDockWidgetQt::on_pushButtonFeedbackStart_clicked() {
	std::cout << "Feedback IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
	std::cout << "Feedback Port: " << ui.spinBoxFeedbackPort->value() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start") {
		this->toggleFeedback(true);
	}
	else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop") {
		this->toggleFeedback(false);
	}
}

void ACOSCDockWidgetQt::disableControl(){
	if (osc_browser) {
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
	}
	ui.groupBoxOscControl->hide();
}

void ACOSCDockWidgetQt::disableFeedback(){
	if (osc_feedback) {
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
	}
	ui.groupBoxOscFeedback->hide();
}

void ACOSCDockWidgetQt::setControlPort(int _port){
	bool restart = false;
	if (osc_browser) {
		restart = true;
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
		ui.pushButtonControlStart->setText("Start");
	}
	ui.spinBoxControlPort->setValue(_port);
	if (restart)
		on_pushButtonControlStart_clicked();
}

void ACOSCDockWidgetQt::setFeedbackPort(int _port){
	bool restart = false;
	if (osc_feedback) {
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
		ui.pushButtonFeedbackStart->setText("Start");
	}
	ui.spinBoxFeedbackPort->setValue(_port);
	if (restart)
		on_pushButtonFeedbackStart_clicked();
}

void ACOSCDockWidgetQt::setMediaCycle(MediaCycle* _media_cycle)
{
	media_cycle = _media_cycle; 
        if(auto_connect){
		this->toggleControl(true);
		this->toggleFeedback(true);
	}
}

#if defined (SUPPORT_AUDIO)
void ACOSCDockWidgetQt::setAudioEngine(ACAudioEngine* _audio_engine)
{ 
	audio_engine = _audio_engine; 
	if(osc_browser) 
		osc_browser->setAudioEngine(_audio_engine);
}
#endif //defined (SUPPORT_AUDIO)

#endif //defined (USE_OSC)
