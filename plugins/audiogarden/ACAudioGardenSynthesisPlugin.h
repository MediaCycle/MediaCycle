/**
 * @brief ACAudioGardenSynthesisPlugin.h
 * @author Christian Frisson
 * @date 04/12/2012
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

#ifndef HEADER_ACAudioGardenSynthesisPlugin
#define HEADER_ACAudioGardenSynthesisPlugin

//
//  ACAudioGardenSynthesisPlugin.h
//  MediaCycle+AudioGarden
//
//  @author Christian Frisson
//  @date 01/07/10
//  @copyright (c) 2010 – UMONS - Numediart
//  
//  MediaCycle of University of Mons – Numediart institute is 
//  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
//  licence (the “License”); you may not use this file except in compliance 
//  with the License.
//  
//  This program is free software: you can redistribute it and/or 
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  Each use of this software must be attributed to University of Mons – 
//  Numediart Institute
//  
//  Any other additional authorizations may be asked to avre@umons.ac.be 
//  <mailto:avre@umons.ac.be>
//
//  Qt QGLWidget and OSG CompositeViewer that wraps 
//  a MediaCycle browser and multitrack timeline viewer 
//  customized for AudioGarden
//

#include <AGSynthesis.h>
#include <QtGui>

#include <iostream>
#include <MediaCycle.h>

#include <ACAudioEngine.h>

#include <ACPluginQt.h>
#include <ACAudioGardenDockQt.h>

class ACAudioGardenSynthesisPlugin : public QObject, public ACPluginQt, public ACMediaRendererPlugin // : public osgViewer::CompositeViewer, public QGLWidget
{

    Q_OBJECT
    Q_INTERFACES(ACPluginQt)

public:
    ACAudioGardenSynthesisPlugin();// QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );
    ~ACAudioGardenSynthesisPlugin();

private:
    ACAudioEngine *audio_engine;

public:
    void setAudioEngine(ACAudioEngine *_audio_engine){this->audio_engine = _audio_engine;}

    //AudioGarden specific members:
private:
    AGSynthesis *synth;
    int selectedRhythmPattern;
    bool autosynth;
    ACAudio* synthAudio;
    bool track_playing;

    //AudioGarden specific functions:
public:
    void setSynth(AGSynthesis* _synth){this->synth=_synth;}
    AGSynthesis* getSynth(){return this->synth;}
    int getSelectedRhythmPattern(){return selectedRhythmPattern;}
    void setAutoSynth(bool _autosynth){autosynth = _autosynth;}
    void synthesize();
    void resetSynth();
    void stopSound();

    //CF new 2012
public:
    virtual void setMediaCycle(MediaCycle* _media_cycle);
    std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::string value="");
    virtual std::map<std::string,ACMediaType> availableMediaActions();

    void changeMethod();
    void changeMapping();
    void changeRandomness();
    void changeThreshold();
    void changeAutosynth();
    void go();
    void reset();

protected:
    std::vector<std::string> synthesis_methods;
    std::vector<std::string> synthesis_mappings;

public:
    //virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL){return std::map<std::string, ACMediaType>();}
    //virtual std::map<std::string,ACMediaType> availableMediaActions(){return std::map<std::string,ACMediaType>();}
    virtual std::vector<ACInputActionQt*> providesInputActions();

public slots:
    void selectGrains();
    void selectRhythmPattern();

protected:
    ACInputActionQt* selectGrainsAction;
    ACInputActionQt* selectRhythmPatternAction;

};

#endif
