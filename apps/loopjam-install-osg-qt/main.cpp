/*
 *  ACAudioCycleLoopJam main.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 17/05/11
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

#include <QApplication>
#include <QtGui>
#include "ACAudioCycleLoopJam.h"
#include <BreakpadHttpSender.h>
#include <BreakPadHandler.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("numediart");
    app.setOrganizationDomain("numediart.org");
    app.setApplicationName("LoopJam Installation Application");
	
    // Make Apple *.app bundles not load installed Qt Frameworks but load Qt Plugins
#ifdef __APPLE__
    QApplication::setLibraryPaths(QStringList(QApplication::applicationDirPath() + "/../PlugIns"));
#endif

	BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String("crashes"));
	BreakpadQt::GlobalHandler::instance()->setReporter(QLatin1String("../MacOS/loopjam-compose-osg-qt"));
	
    ACAudioCycleLoopJam window;

    window.setWindowTitle("LoopJam Installation Application");

    // Adding palettes
    window.addControlDock("MCOSC");
    window.addControlDock("MCBrowserControlsClusters");//"MCBrowserControlsClustersNeighbors");
    window.addControlDock("MCAudioControls");

    window.setDefaultQSettings(); // skip the first dialog about saved settings
    window.loadDefaultConfig(MEDIA_TYPE_AUDIO);
    window.show();

    window.on_actionFullscreen_triggered(true); // to be set after the window is shown
    window.autoConnectOSC(true); // to be set after loading the default config
    window.changeSetting(AC_SETTING_INSTALLATION);

    window.startLoopXML();

    return app.exec();
}

