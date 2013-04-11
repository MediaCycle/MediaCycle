/*
 *  ACAudioEngineRendererPlugin.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 14/10/12
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

#ifndef ACAudioEngineRendererPlugin_H
#define	ACAudioEngineRendererPlugin_H

#include <MediaCycle.h>
#include <ACPlugin.h>
#include "ACAudioEngine.h"
#include <ACPluginQt.h>
#include <QtGui>

class ACAudioEngineRendererPlugin : public QObject, public ACPluginQt, public ACMediaRendererPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACAudioEngineRendererPlugin();
    virtual ~ACAudioEngineRendererPlugin();
    virtual void mediaCycleSet();
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());
    virtual std::map<std::string,ACMediaType> availableMediaActions();
    virtual std::map<std::string,ACMediaActionParameters> mediaActionsParameters();
    void updateBPM();
    void updateVolume();
    void updateSynchroMode();
    void updateScaleMode();
    void updatePreset();
    void muteAll();
protected:
    ACAudioEngine* audio_engine;
    std::vector<std::string> synchro_modes, scale_modes, presets;

public:
    virtual std::vector<ACInputActionQt*> providesInputActions();

public slots:
    void playClickedNode();
    void loopClickedNode();
    void muteAllNodes();

protected:
    ACInputActionQt* playClickedNodeAction;
    ACInputActionQt* loopClickedNodeAction;
    ACInputActionQt* muteAllNodesAction;

    std::map<std::string,ACMediaActionParameters> action_parameters;
};

#endif
