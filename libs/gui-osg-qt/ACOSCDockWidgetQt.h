/*
 *  ACOSCDockWidgetQt.h
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

#ifndef HEADER_ACOSCDOCKWIDGETQT
#define HEADER_ACOSCDOCKWIDGETQT

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"
#include "ui_ACOSCDockWidgetQt.h"

#include <MediaCycle.h>

class ACOSCDockWidgetQt : public ACAbstractDockWidgetQt {
    Q_OBJECT

private slots:
    void on_pushButtonControlStart_clicked();
    void on_pushButtonFeedbackStart_clicked();

public:
    ACOSCDockWidgetQt(QWidget *parent = 0);
    ~ACOSCDockWidgetQt();
    virtual bool canBeVisible(ACMediaType _media_type);

    void autoConnectOSC(bool _status = true);

#if defined (USE_OSC)
    void toggleControl(bool _status);
    void toggleFeedback(bool _status);
    void disableControl();
    void disableFeedback();
    void setControlPort(int port);
    void setFeedbackPort(int port);
    void setMediaCycle(MediaCycle* _media_cycle);
#if defined (SUPPORT_AUDIO)
    void setAudioEngine(ACAudioEngine* _audio_engine);
#endif //defined (SUPPORT_AUDIO)
#endif //defined (USE_OSC)

private:
    Ui::ACOSCDockWidgetQt ui;
    bool auto_connect;
};
#endif
