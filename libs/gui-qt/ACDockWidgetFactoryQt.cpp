/*
 *  ACDockWidgetFactoryQt.cpp
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

#include "ACDockWidgetFactoryQt.h"
#include "ACPluginQt.h"
#include "ACPluginControlsDockWidgetQt.h"

// XS watch out, here the dock_types start with "MC"
// BUT the dock's names start with "AC"

ACAbstractDockWidgetQt* ACDockWidgetFactoryQt::createDockWidget(QWidget *parent,std::string dock_type) {
    //if (dock_type == "MCOSC") {return new ACOSCDockWidgetQt(parent);}

    if (dock_type == "MCBrowserControlsComplete") {return new ACBrowserControlsCompleteDockWidgetQt(parent);}
    else if (dock_type == "MCBrowserControlsClusters") {return new ACBrowserControlsClustersDockWidgetQt(parent);}
    else if (dock_type == "MCSimilarityCheckboxControls") {return new ACSimilarityCheckboxControlsDockWidgetQt(parent);}
    else if (dock_type == "MCSimilaritySliderControls") {return new ACSimilaritySliderControlsDockWidgetQt(parent);}
    else if (dock_type == "MCMediaConfig") {return new ACMediaConfigDockWidgetQt(parent);}
    else if (dock_type == "MCMediaControls") {return new ACMediaControlsDockWidgetQt(parent);}
#if defined (SUPPORT_MULTIMEDIA)
    else if (dock_type == "MCMediaDocumentOption") {return new ACMediaDocumentOptionDockWidgetQt(parent);}
#endif //defined (SUPPORT_MULTIMEDIA)
    else if (dock_type == "MCSegmentationControls") {return new ACSegmentationControlsDockWidgetQt(parent);}

    if(this->media_cycle){
        std::vector<std::string> plugin_names = media_cycle->getPluginManager()->getListOfPlugins();
        for(std::vector<std::string>::iterator plugin_name = plugin_names.begin(); plugin_name != plugin_names.end(); plugin_name++){
            ACPluginQt* plugin = dynamic_cast<ACPluginQt*>( media_cycle->getPluginManager()->getPlugin(*plugin_name));
            if(plugin){
                if (dock_type == plugin->dockName()){
                    return plugin->createDock(parent);
                }
            }
        }
        //media_cycle->getPluginManager()

    }
    return 0;
}

ACAbstractDockWidgetQt* ACDockWidgetFactoryQt::createDockWidget(QWidget *parent,ACPluginType plugin_type){
    std::string name = "";
    if(plugin_type == PLUGIN_TYPE_CLIENT)
        name = "I/O";
    return new ACPluginControlsDockWidgetQt(plugin_type,name,parent);
}
