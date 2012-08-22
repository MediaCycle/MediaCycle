/*
   Copyright (C) 2011:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include "QKinectWrapper.h"

#include "oscdock.h"

#include <XnFPSCalculator.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void kinectData();
    void kinectStatus(QKinect::KinectStatus);
    void kinectUser(unsigned,bool);
    void kinectPose(unsigned,QString);
    void kinectCalibration(unsigned,QKinect::CalibrationStatus);

private slots:
    void on_pushButton_KinectStartStop_clicked();

private:
    Ui::MainWindow *ui;
    QKinect::QKinectWrapper kreader;
    QLabel *sbKinectStatus;
    QLabel *sbKinectFrame;
    QLabel *sbKinectTime;
    QLabel *sbKinectFPS;
    QLabel *sbKinectNumBody;
    XnFPSData xnFPS;

protected:
    oscdock *oscDockWidget;
    simpleoscfeedback *osc_feedback;
};

#endif // MAINWINDOW_H
