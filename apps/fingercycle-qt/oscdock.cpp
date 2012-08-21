/*
 *  oscdock.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 21/08/12
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

#include "oscdock.h"

simpleoscfeedback::simpleoscfeedback() {
    sendto = 0;
    message = 0;
    this->active = false;
}

simpleoscfeedback::~simpleoscfeedback() {
    release();
}

void simpleoscfeedback::create(const char *hostname, int port) {
    /* an address to send messages to. sometimes it is better to let the server
     * pick a port number for you by passing NULL as the last argument */
    this->release();
    char portchar[6];
    sprintf(portchar, "%d", port);
    sendto = lo_address_new(hostname, portchar);
    this->active = true;
}

void simpleoscfeedback::release() {
    if (sendto) {
        lo_address_free(sendto);
        sendto = 0;
    }
    this->active = false;
}

void simpleoscfeedback::messageBegin(const char *_tag) {
    if (message) {
        lo_message_free(message);
    }
    message = lo_message_new();
    tag = _tag;
}

void simpleoscfeedback::messageEnd() {
}

void simpleoscfeedback::messageSend() {
    if(this->isActive())
        lo_send_message(sendto, tag, message);
}

void simpleoscfeedback::messageAppendFloat(float val) {
    lo_message_add_float(message, val);
}

void simpleoscfeedback::messageAppendInt(int val) {
    if(this->isActive())
        lo_message_add_int32(message, val);
}

void simpleoscfeedback::messageAppendString(const char *val) {
    if(this->isActive())
        lo_message_add_string(message, val);
}

void simpleoscfeedback::messageAppendString(std::string val) {
    if(this->isActive())
        lo_message_add_string(message, val.c_str());
}

void simpleoscfeedback::sendMessage(std::string _message){
    if(this->isActive())
        lo_send(sendto, _message.c_str(),"");
}

oscdock::oscdock(QWidget *parent)
    : QDockWidget(parent)
{
#if defined (USE_OSC)
    ui.setupUi(this); // first thing to do
    osc_feedback = 0;
    this->show();
#endif //defined (USE_OSC)
}

oscdock::~oscdock(){
#if defined (USE_OSC)
    osc_feedback = 0;
#endif //defined (USE_OSC)
}

#if defined (USE_OSC)
void oscdock::toggleFeedback(bool _status){
    if(!osc_feedback){
        std::cerr << "oscdock::toggleFeedback: osc feedback not created" << std::endl;
        return;
    }
    if(_status){
        //osc_feedback = new simpleoscfeedback();

        osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.spinBoxFeedbackPort->value());
        osc_feedback->messageBegin("test mc send osc");
        std::cout << "sending test messages to " << ui.lineEditFeedbackIP->text().toStdString().c_str() << " on port " << ui.spinBoxFeedbackPort->value() << " ..." << endl;
        osc_feedback->messageSend();
        ui.pushButtonFeedbackStart->setText("Stop");
    }
    else {
        osc_feedback->release(); // XS TODO for browser it was stop ?!
        //delete osc_feedback;
        //osc_feedback = 0;
        ui.pushButtonFeedbackStart->setText("Start");
    }
}

void oscdock::on_pushButtonFeedbackStart_clicked() {
    if(!osc_feedback){
        std::cerr << "oscdock::feedbackStart: osc feedback not created" << std::endl;
        return;
    }
    std::cout << "Feedback IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
    std::cout << "Feedback Port: " << ui.spinBoxFeedbackPort->value() << std::endl;
    if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start") {
        this->toggleFeedback(true);
    }
    else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop") {
        this->toggleFeedback(false);
    }
}

void oscdock::setFeedbackPort(int _port){
    if(!osc_feedback){
        std::cerr << "oscdock::setFeedbackPort: osc feedback not created" << std::endl;
        return;
    }
    bool restart = false;
    if (osc_feedback->isActive()) {
        osc_feedback->release();
        //delete osc_feedback;
        //osc_feedback = 0;
        ui.pushButtonFeedbackStart->setText("Start");
    }
    ui.spinBoxFeedbackPort->setValue(_port);
    if (restart)
        on_pushButtonFeedbackStart_clicked();
}

#endif //defined (USE_OSC)
