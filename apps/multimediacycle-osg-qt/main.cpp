/*
 *  ACMultiMediaCycleOsgQt main.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/02/10
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

#include <QApplication>
#include <QStringListModel>
#include <QListView>

#include <QApplication>
#include <QtGui>
#include "ACMultiMediaCycleOsgQt.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	
	// Make Apple *.app bundles not load installed Qt Frameworks but load Qt Plugins
	#ifdef __APPLE__
	QApplication::setLibraryPaths(QStringList(QApplication::applicationDirPath() + "/../PlugIns"));
	#endif
	
	ACMultiMediaCycleOsgQt window;
	try {
		// Adding palettes
        window.addControlDock("MCMediaConfig");
        window.addControlDock("MCOSC");
        //#ifdef USE_DEBUG
        window.addControlDock("MCBrowserControlsComplete");
        //#else
        //window.addControlDock("MCBrowserControlsClusters");
        //#endif
#if defined (SUPPORT_AUDIO)
		window.addControlDock("MCAudioControls");
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_VIDEO)
		window.addControlDock("MCVideoControls");				 
#endif //defined (SUPPORT_VIDEO)
		
		// XS TODO
		// this has to be called after dock controls have been added
		// do we need to put all this code here ?
		window.configureSettings();
				
		// Changing the about dialog (not necessary if standard MediaCycle app)
		//window.addAboutDialog("MediaCycle");
		
		window.show();
		
	}
	catch (const exception& e) {
		cout << "** caught exception in main : " << e.what() << endl;
	}
	catch (...){
		cout << "** caught undetermined exception in main" << endl;
	}
	
	// variables used in QSettings 
	// see http://doc.qt.nokia.com/latest/qsettings.html#details
	app.setOrganizationName("numediart");
	app.setOrganizationDomain("numediart.org");
	app.setApplicationName("MediaCycle");
	
    return app.exec();
}

