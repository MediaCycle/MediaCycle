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
    ui.setupUi(this); // first thing to do
    this->show();

    QSettings settings("numediart", "MediaCycle");

    QString osc_control_ip = settings.value("osc.control.ip").toString();
    if(osc_control_ip.isEmpty())
        ui.lineEditControlIP->setText("localhost");
    else
        ui.lineEditControlIP->setText(osc_control_ip);

    QString osc_feedback_ip = settings.value("osc.feedback.ip").toString();
    if(osc_feedback_ip.isEmpty())
        ui.lineEditFeedbackIP->setText("localhost");
    else
        ui.lineEditFeedbackIP->setText(osc_feedback_ip);

    int osc_control_port = settings.value("osc.control.port").toInt();
    if (osc_control_port == 0)
        ui.spinBoxControlPort->setValue(12345);
    else
        ui.spinBoxControlPort->setValue(osc_control_port);

    int osc_feedback_port = settings.value("osc.feedback.port").toInt();
    if (osc_feedback_port == 0)
        ui.spinBoxFeedbackPort->setValue(12346);
    else
        ui.spinBoxFeedbackPort->setValue(osc_feedback_port);
    auto_connect = false;
}

ACOSCDockWidgetQt::~ACOSCDockWidgetQt(){
    QSettings settings("numediart", "MediaCycle");
    settings.setValue("osc.control.ip",ui.lineEditControlIP->text());
    settings.setValue("osc.feedback.ip",ui.lineEditFeedbackIP->text());
    settings.setValue("osc.control.port",ui.spinBoxControlPort->text());
    settings.setValue("osc.feedback.port",ui.spinBoxFeedbackPort->text());

    osc_browser = 0;
    osc_feedback = 0;
}

bool ACOSCDockWidgetQt::canBeVisible(ACMediaType _media_type){
    return true;
}

void ACOSCDockWidgetQt::autoConnectOSC(bool _status)
{
    auto_connect = _status;
    if(!media_cycle)
        return;
    if(!osc_browser || !osc_feedback){
        std::cerr << "ACOSCDockWidgetQt::autoConnectOSC: osc";
        if(!osc_browser){
            std::cerr << "browser";
            if(!osc_feedback)
                std::cerr << " / ";
        }
        if(!osc_feedback)
            std::cerr << "feedback";
        std::cerr << " not created" << std::endl;
        return;
    }
    if(auto_connect){
        if(!osc_browser->isActive())
            this->toggleControl(true);
        if(!osc_feedback->isActive())
            this->toggleFeedback(true);
    }
}

void ACOSCDockWidgetQt::toggleControl(bool _status){
    if(!osc_browser){
        std::cerr << "ACOSCDockWidgetQt::toggleControl: osc browser not created" << std::endl;
        return;
    }
    if(_status){
        //osc_browser = new ACOscBrowser();
        if(osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.spinBoxControlPort->value())){
            osc_browser->setMediaCycle(media_cycle);
            osc_browser->start();
            ui.pushButtonControlStart->setText("Stop");
        }
        else{
            osc_browser->release();
            //delete osc_browser;
            //osc_browser = 0;
            QString msg = QString("The OSC port ") + ui.spinBoxControlPort->text() + QString(" is already in use. Please change the OSC control port and start the OSC control again.");
            int warn_button = QMessageBox::warning(this->osg_view, "Error", msg);
        }
    }
    else {
        osc_browser->stop();
        osc_browser->release();
        //delete osc_browser;
        //osc_browser = 0;
        ui.pushButtonControlStart->setText("Start");
    }
}

void ACOSCDockWidgetQt::toggleFeedback(bool _status){
    if(!osc_feedback){
        std::cerr << "ACOSCDockWidgetQt::toggleFeedback: osc feedback not created" << std::endl;
        return;
    }
    if(_status){
        //osc_feedback = new ACOscFeedback();
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
        //delete osc_feedback;
        //osc_feedback = 0;
        ui.pushButtonFeedbackStart->setText("Start");
    }
}

void ACOSCDockWidgetQt::on_pushButtonControlStart_clicked() {
    if(!osc_browser){
        std::cerr << "ACOSCDockWidgetQt::controlStart: osc browser not created" << std::endl;
        return;
    }
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
    if(!osc_feedback){
        std::cerr << "ACOSCDockWidgetQt::feedbackStart: osc feedback not created" << std::endl;
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

void ACOSCDockWidgetQt::disableControl(){
    if (osc_browser) {
        osc_browser->release();
        //delete osc_browser;
        //osc_browser = 0;
    }
    ui.pushButtonControlStart->hide();
    ui.lineEditControlIP->hide();
    ui.spinBoxControlPort->hide();
    ui.labelControl->hide();
    ui.spinBoxControlPort->hide();
}

void ACOSCDockWidgetQt::disableFeedback(){
    if (osc_feedback) {
        osc_feedback->release();
        //delete osc_feedback;
        //osc_feedback = 0;
    }
    ui.pushButtonFeedbackStart->hide();
    ui.lineEditFeedbackIP->hide();
    ui.spinBoxFeedbackPort->hide();
    ui.labelFeedback->hide();
    ui.spinBoxFeedbackPort->hide();
}

void ACOSCDockWidgetQt::setControlPort(int _port){
    if(!osc_browser){
        std::cerr << "ACOSCDockWidgetQt::setControlPort: osc browser not created" << std::endl;
        return;
    }
    bool restart = false;
    if (osc_browser->isActive()) {
        restart = true;
        osc_browser->release();
        //delete osc_browser;
        //osc_browser = 0;
        ui.pushButtonControlStart->setText("Start");
    }
    ui.spinBoxControlPort->setValue(_port);
    if (restart)
        on_pushButtonControlStart_clicked();
}

void ACOSCDockWidgetQt::setFeedbackPort(int _port){
    if(!osc_feedback){
        std::cerr << "ACOSCDockWidgetQt::setFeedbackPort: osc feedback not created" << std::endl;
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

void ACOSCDockWidgetQt::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle = _media_cycle;
    if(auto_connect){
        this->toggleControl(true);
        this->toggleFeedback(true);
    }
}