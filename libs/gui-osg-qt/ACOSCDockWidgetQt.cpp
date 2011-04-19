/*
 *  ACOSCDockWidgetQt.cpp
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

#include "ACOSCDockWidgetQt.h"

static void osc_callback(ACOscBrowserRef, const char *tagName, void *userData)
{
	ACOSCDockWidgetQt *window = (ACOSCDockWidgetQt*)userData;
	//printf("osc received tag: %s\n", tagName);
	//std::cout << "tagName: " << tagName << std::endl;
	window->processOscMessage(tagName);
}

ACOSCDockWidgetQt::ACOSCDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACOSCDockWidgetQt")
{
	ui.setupUi(this); // first thing to do
	this->show();	
	osc_browser = new ACOscBrowser();
	osc_feedback = new ACOscFeedback();
}

ACOSCDockWidgetQt::~ACOSCDockWidgetQt(){
	if (osc_browser) {
		osc_browser->stop(mOscReceiver);		
		//osc_browser->release(mOscReceiver);//should be in destructor ?
	}
	delete osc_browser;
	delete osc_feedback;//osc_feedback destructor calls ACOscFeedback::release()
	//delete mOscReceiver;
	//delete mOscFeeder;
}

void ACOSCDockWidgetQt::on_pushButtonControlStart_clicked() {
	std::cout << "Control IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Control Port: " << ui.lineEditControlPort->text().toInt() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start") {	
		mOscReceiver = osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.lineEditControlPort->text().toInt());
		osc_browser->setUserData(mOscReceiver, this);
		osc_browser->setCallback(mOscReceiver, osc_callback);
		osc_browser->start(mOscReceiver);
		ui.pushButtonControlStart->setText("Stop");
	}	
	else if ( ui.pushButtonControlStart->text().toStdString() == "Stop") {	
		osc_browser->stop(mOscReceiver);
		osc_browser->release(mOscReceiver);	//this does delete mOscReceiver;
		ui.pushButtonControlStart->setText("Start");
	}
}	

void ACOSCDockWidgetQt::on_pushButtonFeedbackStart_clicked() {
	std::cout << "Feedback IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
	std::cout << "Feedback Port: " << ui.lineEditFeedbackPort->text().toInt() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start") {	
		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.lineEditFeedbackPort->text().toInt());
		osc_feedback->messageBegin("test");
		std::cout << "sending test messages to " << ui.lineEditFeedbackIP->text().toStdString().c_str() << " on port " << ui.lineEditFeedbackPort->text().toInt() << " ..." << endl;
		osc_feedback->messageAppendFloat(3.1415);
		osc_feedback->messageSend();
		ui.pushButtonFeedbackStart->setText("Stop");
	}	
	else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop") {	
		osc_feedback->release();//mOscFeeder);
		ui.pushButtonFeedbackStart->setText("Start");
	}
}	


void ACOSCDockWidgetQt::processOscMessage(const char* tagName) {	
	if(strcasecmp(tagName, "/mediacycle/test") == 0)
	{
		std::cout << "OSC communication established" << std::endl;
		
		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/received");
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
	}
	else if(strcasecmp(tagName, "/mediacycle/1/browser/library/load") == 0)
	{
//		char *lib_path = 0;
//		lib_path = new char[500]; // wrong magic number!
//		osc_browser->readString(mOscReceiver, lib_path, 500); // wrong magic number!
//		std::cout << "Importing file library '" << lib_path << "'..." << std::endl;
//		media_cycle->importACLLibrary(lib_path); // XS instead of getImageLibrary CHECK THIS
//		media_cycle->normalizeFeatures();
//		media_cycle->libraryContentChanged();
//		std::cout << "File library imported" << std::endl;
//		this->updateLibrary();
	}
	//std::cout << "End of OSC process messages" << std::endl;
}

