/**
 * @brief A plugin that provides an audio engine based on STK and RtAudio.
 * @author Christian Frisson
 * @date 10/11/2012
 * @copyright (c) 2012 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef ACAudioStkEngineRendererPlugin_H
#define	ACAudioStkEngineRendererPlugin_H

#include <MediaCycle.h>
#include <ACPlugin.h>
#include <ACPluginQt.h>
#include <QtGui>

#include <FileWvIn.h>
#include <FileLoop.h>
#include <RtAudio.h>

#include <ACAudioStkFileWvIn.h>
#include <ACAudioStkFileLoop.h>
#include <ACAudioStkGranulate.h>

class ACAudioStkEngineRendererPlugin : public QObject, public ACPluginQt, public ACMediaRendererPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACAudioStkEngineRendererPlugin();
    virtual ~ACAudioStkEngineRendererPlugin();
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());
    virtual std::map<std::string,ACMediaType> availableMediaActions();
    virtual std::map<std::string,ACMediaActionParameters> mediaActionsParameters();
    void updateVolume();
    void updateRate();
    void muteAll();
    void updateGrainVoices();
    void updateGrainRandomness();
    void updateGrainStretch();
    void updateGrainParameters();

public:
    RtAudio dac;
    std::map< long int, ACAudioStkFileWvIn*> inputs;
    std::map< long int, ACAudioStkFileLoop*> loops;
    std::map< long int, ACAudioStkGranulate*> grains;
    std::map< long int, int> current_frames;
    void justReadFrames(long int mediaId, int nFrames);
    void removeInput(long int mediaId);
    void removeLoop(long int mediaId);
    std::map< long int, stk::StkFrames*> frames;
    int outputChannels();

public:
    virtual std::vector<ACInputActionQt*> providesInputActions();

public slots:
    void playClickedNode();
    void loopClickedNode();
    void granulateClickedNode();
    void muteAllNodes();

protected:
    ACInputActionQt* playClickedNodeAction;
    ACInputActionQt* loopClickedNodeAction;
    ACInputActionQt* granulateClickedNodeAction;
    ACInputActionQt* muteAllNodesAction;

    std::map<std::string,ACMediaActionParameters> action_parameters;
};

#endif
