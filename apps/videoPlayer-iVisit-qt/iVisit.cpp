/*
 *  iVisit.cpp
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

#include <QtCore/QString>

#include "iVisit.h"

#include <iostream>
using namespace std;

iVisit::iVisit(QWidget *parent) : QMainWindow(parent) {
	iVui.setupUi(this); // first thing to do
	
	connect(iVui.pushButton, SIGNAL(clicked()), this, SLOT(testButton()));

}

iVisit::~iVisit() {
	delete mp1;
	delete mp2;
}

void iVisit::setupPlayers(){
	QString s1("");
	mp1 = new MediaPlayer(s1,false,iVui.frame1);
	mp1->setParent(iVui.frame1);
	mp1->show();
	
	QString s2("");
	mp2 = new MediaPlayer(s2,false,iVui.frame2);
	mp2->show();
	cout <<"done" << endl;
}

void iVisit::seekVideos(int t1, int t2){
	// XS todo add tests !
	mp1->seekTime(t1*40);
	mp2->seekTime(t2*40);
}

// SLOTS 
void iVisit::testButton(){
	this->seekVideos(1,1);
}


// ---------------------------------------------------------------------------

MainForm::MainForm() {
	scene = new QGraphicsScene();
	view = new QGraphicsView(scene);
	QHBoxLayout* hl = new QHBoxLayout();
	QPushButton* loadimg_pb = new QPushButton("Load Image ...");
	connect(loadimg_pb,SIGNAL(clicked()),this,SLOT(LoadImage()));
	hl->addWidget(loadimg_pb,0);
	hl->addStretch(1);
	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(view);
	vl->addLayout(hl);
	setLayout(vl);
}

MainForm::~MainForm() {
}

void MainForm::LoadImage() {
	QString filename = QFileDialog::getOpenFileName(0, "Open Image", "", "Images (*.jpg;*.jpeg;*.bmp;*.png)");
	QImage img(filename);
	
	QRgb value;
	value = qRgb(0, 0, 0); // 0xff7aa327
	img.setColor(0, value);

	for (int i=0; i<50;i++){
		for (int j=0; j<500;j++){
			img.setPixel(i, j, 0);
		}
	}
	
	if (!img.isNull()) {
		QGraphicsPixmapItem* pi = scene->addPixmap(QPixmap::fromImage(img));
		pi->setFlag(QGraphicsItem::ItemIsMovable,false);
		pi->setFlag(QGraphicsItem::ItemIsSelectable,false);
		pi->setPos(0,0);
	}
	
	
}

void MainForm::setVideoApp(iVisit* _videoApp){
	this->videoApp = _videoApp;
}

void MainForm::mousePressEvent( QMouseEvent *e ){
	videoApp->seekVideos( e->x() , e->y() );

	cout << "click: " <<  e->x() << " - " << e->y() << endl;
}
