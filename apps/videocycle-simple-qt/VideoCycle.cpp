/*
 *  VideoCycle.cpp
 *  tryxcodeqt
 *
 *  @author Xavier Siebert
 *  @date 2/01/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "VideoCycle.h"
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "ACMediaLibrary.h"

#include <iostream>
#include <exception>
#include <string>
using namespace std;

VideoCycle::VideoCycle(QWidget* parent) : QMainWindow(parent) {
	setupUi(this);
// XS TODO: choose type of media...
//	const string video_plugin_path = "/Users/xavier/development/workingDirectory/ticore-app/Applications/Numediart/MediaCycle/src/Builds/mac/plugins/video/Debug/";
	mediacycle = new MediaCycle(MEDIA_TYPE_AUDIO); // XS hack
//	mediacycle->loadPluginLibraryFromBasename("video");

	connect(GOpushButton, SIGNAL(clicked()), this, SLOT(Calculate()));
	connect(BrowseVideoDirpushButton, SIGNAL(clicked()), this, SLOT(BrowseVideoDir()));
	connect(BrowseOutputFilepushButton, SIGNAL(clicked()), this, SLOT(BrowseOutputFile()));
	connect(BrowseACLFilepushButton, SIGNAL(clicked()), this, SLOT(BrowseACLFile()));
}

VideoCycle::~VideoCycle() {
	delete mediacycle;
}

// SLOTS 
void VideoCycle::Calculate(){
	string videoDirectory = VideoDirectorylineEdit->text().toStdString() ;
	bool ok = true;
//	if (videoDirectory == "") {
//		cerr << "missing video directory" << endl;
//		ok = false;
//	}
//	else
//		cout << videoDirectory << endl;

	string outputFile =  OutputFilelineEdit->text().toStdString() ;
	if (outputFile == "") {
		cout << "missing output file" << endl;
		ok = false;
	}
	else
		cout << outputFile << endl;
	
//	if (ok) {
//		mediacycle->importDirectory(videoDirectory, 1);
//		mediacycle->getLibrary()->saveSorted(outputFile); // XS not clean
//	}
	
	string ACLFile =  ACLFilelineEdit->text().toStdString() ;
	if (ACLFile == "") {
		cout << "missing acl file" << endl;
		ok = false;
	}
	else
		cout << ACLFile << endl;
	if (ok) {
		mediacycle->importACLLibrary(ACLFile);
		mediacycle->getLibrary()->saveSorted(outputFile); // XS not clean
	}
	
	cout << "end of analysis" << endl;
	QMessageBox msgBox;
	msgBox.setText("Done !");
	msgBox.setStandardButtons(QMessageBox::Cancel);
	int ret = msgBox.exec();
}


void VideoCycle::BrowseVideoDir(){
	QString video_dir = QFileDialog::getExistingDirectory();
	try {
			if (video_dir.isEmpty()) throw(string("<BrowseVideoDir> empty directory !"));
			std::string s = video_dir.toStdString();
	}
	catch (const string& empty_file) {
			cerr << empty_file << endl;
			return;
	}	
	VideoDirectorylineEdit->setText(video_dir);
}

void VideoCycle::BrowseOutputFile(){
	QString output_file = QFileDialog::getOpenFileName();
	try {
		if (output_file.isEmpty()) throw(string("<BrowseOutputFile> empty file !"));
		std::string s = output_file.toStdString();
	}
	catch (const string& empty_file) {
		cerr << empty_file << endl;
		return;
	}	
	OutputFilelineEdit->setText(output_file);
}

void VideoCycle::BrowseACLFile(){
	QString acl_file = QFileDialog::getOpenFileName();
	try {
		if (acl_file.isEmpty()) throw(string("<ACLFile> empty file !"));
		std::string s = acl_file.toStdString();
	}
	catch (const string& empty_file) {
		cerr << empty_file << endl;
		return;
	}	
	ACLFilelineEdit->setText(acl_file);
}

