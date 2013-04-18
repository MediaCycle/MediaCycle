/*
 *  ACAbstractDefaultConfig.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/11/2012
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

#ifndef __ACAbstractDefaultConfig_H__
#define __ACAbstractDefaultConfig_H__


#include "ACMediaBrowser.h"
#include "ACEventListener.h"
#include "ACPlugin.h"

// Setting: where/how the application is used
typedef unsigned int ACSettingType;
const ACSettingType AC_SETTING_NONE = 0x0000;
const ACSettingType AC_SETTING_DESKTOP = 0x0001;
const ACSettingType AC_SETTING_INSTALLATION = 0x0002;
const ACSettingType AC_SETTING_MOBILE = 0x0004;
const ACSettingType AC_SETTING_WEB = 0x0008;

class ACAbstractDefaultConfig {
	
public:
    ACAbstractDefaultConfig(){}
    virtual ~ACAbstractDefaultConfig(){}
    virtual std::string name()=0;
    virtual std::string description()=0;
    virtual ACMediaType mediaType()=0;
    virtual ACBrowserMode browserMode()=0;
    //virtual ACBrowserLayout browserLayout()=0;
    virtual std::vector<std::string> pluginLibraries()=0;
    virtual bool staticLibraries()=0;
    virtual ACPluginLibrary* createPluginLibrary(std::string _name)=0;
    virtual bool useNeighbors(){return true;}
    virtual std::string clustersMethodPlugin()=0;
    virtual std::string clustersPositionsPlugin()=0;
    virtual std::string neighborsMethodPlugin(){return "";}
    virtual std::string neighborsPositionsPlugin(){return "";}
    virtual std::string preProcessPlugin(){return "";}
    virtual bool useSegmentation()=0;
    virtual std::string segmentationPlugin(){return "";}
    #if defined (SUPPORT_MULTIMEDIA)
    virtual std::string mediaReaderPlugin(){return "";}//=0;
    virtual std::string activeMediaType(){return "";}//=0;
    #endif
    virtual bool useOSC(){return false;}
    virtual bool connectOSC(){return false;}
    virtual ACSettingType settingType(){return AC_SETTING_DESKTOP;}
};

#endif //__ACAbstractDefaultConfig_H__
