/*
 *  iVisit.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 6/09/10
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

#ifndef IVISIT_H
#define IVISIT_H

#include <QtGui>
#include "ui_iVisit.h"
#include "mediaplayer.h"

class iVisit : public QMainWindow {
	Q_OBJECT

public slots: // or private ? or protected ?
	void testButton();
	
public:
	iVisit(QWidget *parent = 0);
	~iVisit();
	void setupPlayers();
	void seekVideos(int t1, int t2);

private:
	MediaPlayer *mp1;
	MediaPlayer *mp2;

	Ui::iVisitMainWindow iVui;
	
};

// from http://www.qtcentre.org/archive/index.php/t-29236.html
class MainForm : public QWidget { 
	Q_OBJECT
public:
	MainForm();
	~MainForm();
	void setVideoApp(iVisit* _videoApp);

private:
	QGraphicsScene* scene;
	QGraphicsView* view;
	iVisit* videoApp;

protected slots:
	void LoadImage();
	void mousePressEvent( QMouseEvent *e );
};

#endif // IVISIT_H
