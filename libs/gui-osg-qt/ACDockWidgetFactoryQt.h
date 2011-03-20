/*
 *  ACDockWidgetFactoryQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 20/02/11
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

#ifndef ACDOCKWIDGETFACTORYQT_H
#define ACDOCKWIDGETFACTORYQT_H

#include "ACAbstractDockWidgetQt.h"
#if defined (SUPPORT_AUDIO)
#include "ACAudioControlsDockWidgetQt.h"
#endif //defined (SUPPORT_AUDIO)
#include "ACBrowserControlsClustersDockWidgetQt.h"
#include "ACBrowserControlsClustersNeighborsDockWidgetQt.h"
#include "ACMediaConfigDockWidgetQt.h"
#if defined (SUPPORT_VIDEO)
#include "ACVideoControlsDockWidgetQt.h"
#endif //defined (SUPPORT_VIDEO)

class ACDockWidgetFactoryQt{
public:
	ACDockWidgetFactoryQt(){};
	~ACDockWidgetFactoryQt(){};
	ACAbstractDockWidgetQt* createDockWidget(QWidget *parent = 0,std::string dock_type="");
};
#endif // ACDOCKWIDGETFACTORYQT_H