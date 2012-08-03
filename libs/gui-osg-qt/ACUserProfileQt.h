/*
 *  ACUserProfileQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 4/05/2012
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

#ifndef HEADER_ACUserProfileQt
#define HEADER_ACUserProfileQt

#include <MediaCycle.h>

#include <iostream>
#include <string.h>

#include <QtGui>

#include "ui_ACUserProfileQt.h"

class ACUserProfileQt : public QMainWindow{
    Q_OBJECT

public slots:
    void on_pushButtonPictureLocate_clicked();
    void on_lineEditEmail_editingFinished();
    void on_lineEditLocation_editingFinished();
    void on_lineEditName_editingFinished();
    void on_lineEditWebsite_editingFinished();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

public:
    ACUserProfileQt(QWidget *parent = 0);
    ~ACUserProfileQt();

    void setMediaCycle(MediaCycle* _media_cycle){ media_cycle = _media_cycle;}
    MediaCycle* getMediaCycle() {return media_cycle;}

private:
    // variables
    Ui::ACUserProfileQt ui;
    QString mediaExts;
protected:
    MediaCycle *media_cycle;
};
#endif
