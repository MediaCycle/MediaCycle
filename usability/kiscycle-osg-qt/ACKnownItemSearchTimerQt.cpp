/*
 *  ACKnownItemSearchTimerQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 18/10/2013
 *  @copyright (c) 2013 – UMONS - Numediart
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

#include "ACKnownItemSearchTimerQt.h"

ACKnownItemSearchTimerQt::ACKnownItemSearchTimerQt(QWidget *parent)
    : QLCDNumber(parent),deadline(60)
{
    setSegmentStyle(Filled);
    // Outline, Filled, Flat

    lcdpalette = new QPalette;
    this->setAutoFillBackground(true);

    lcdpalette->setColor(QPalette::Background, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::Base, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::AlternateBase, QColor(0, 0, 0));
    //lcdpalette->setColor(QPalette::Text, QColor(0, 0, 0));

    this->setPalette(*lcdpalette);

    this->setDigitCount(2);

    countdown = deadline;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    //timer->start(1000);

    //showTime();

    setWindowTitle(tr("Known Item Search Timer"));
    resize(150, 60);

}

void ACKnownItemSearchTimerQt::stop()
{
    timer->stop();
}

void ACKnownItemSearchTimerQt::start()
{
    timer->start(1000);
}

void ACKnownItemSearchTimerQt::reset()
{
    timer->stop();
    countdown = deadline;
    showTime();

    lcdpalette->setColor(QPalette::Background, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::Base, QColor(0, 0, 0));
    lcdpalette->setColor(QPalette::AlternateBase, QColor(0, 0, 0));
    this->setPalette(*lcdpalette);

    //timer->start(1000);
}

void ACKnownItemSearchTimerQt::success()
{
    lcdpalette->setColor(QPalette::Background, QColor(0, 255, 0));
    lcdpalette->setColor(QPalette::Base, QColor(0, 255, 0));
    lcdpalette->setColor(QPalette::AlternateBase, QColor(0, 255, 0));
    this->setPalette(*lcdpalette);
}

void ACKnownItemSearchTimerQt::fail()
{
    lcdpalette->setColor(QPalette::Background, QColor(255, 0, 0));
    lcdpalette->setColor(QPalette::Base, QColor(255, 0, 0));
    lcdpalette->setColor(QPalette::AlternateBase, QColor(255, 0, 0));
    this->setPalette(*lcdpalette);
}


void ACKnownItemSearchTimerQt::showTime()
{
    /*QTime time = QTime::currentTime();

    QString text = time.toString("hh:mm");
    if ((time.second() % 2) == 0)
        text[2] = ' ';
        */
    /*if(countdown==0)
        countdown = deadline;
    else*/
        countdown--;
    display(countdown);
}


