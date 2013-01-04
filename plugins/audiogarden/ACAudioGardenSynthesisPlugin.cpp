//
//  ACAudioGardenSynthesisPlugin.cpp
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

#include "ACAudioGardenSynthesisPlugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#include "ACAudioData.h"

// uncomment this to parse formats dynamically
//#define PARSE_FORMATS_DYNAMICALLY

// uncomment this to parse formats in verbose mode
//#define PARSE_FORMATS_VERBOSE

ACAudioGardenSynthesisPlugin::ACAudioGardenSynthesisPlugin() : QObject(), ACPluginQt(), ACMediaRendererPlugin(),
    selectedRhythmPattern(-1),
    autosynth(false),track_playing(false)
{
    this->mName = "AudioGarden Synthesis";
    this->mDescription ="Plugin for synthesizing audio by blending rhythmic patterns and grains.";
    this->mMediaType = MEDIA_TYPE_AUDIO;

    synth = new AGSynthesis();
    synthAudio = 0;

    synthesis_methods.push_back("Simple");
    synthesis_methods.push_back("Squeezed");
    synthesis_methods.push_back("Padded");
    synthesis_mappings.push_back("Mean+Variance");
    synthesis_mappings.push_back("Mean");
    synthesis_mappings.push_back("None");

    this->addStringParameter("Method", "Simple", synthesis_methods, "Compositing method",boost::bind(&ACAudioGardenSynthesisPlugin::changeMethod,this));
    this->addStringParameter("Mapping", "Mean+Variance", synthesis_mappings, "Compositing mapping",boost::bind(&ACAudioGardenSynthesisPlugin::changeMapping,this));
    this->addNumberParameter("Randomness",0,0,100,1,"Randomness",boost::bind(&ACAudioGardenSynthesisPlugin::changeRandomness,this));
    this->addNumberParameter("Threshold",0,0,100,1,"Threshold",boost::bind(&ACAudioGardenSynthesisPlugin::changeThreshold,this));
    this->addNumberParameter("Autosynth",0,0,1,1,"Autosynthesize on pattern or grain selection",boost::bind(&ACAudioGardenSynthesisPlugin::changeAutosynth,this));
    this->addCallback("Go","Synthesize",boost::bind(&ACAudioGardenSynthesisPlugin::go,this));
    this->addCallback("Reset","Resest pattern and grain(s) selection",boost::bind(&ACAudioGardenSynthesisPlugin::reset,this));

    selectGrainsAction = new ACInputActionQt(tr("Select grains"), this);
    selectGrainsAction->setShortcut(Qt::Key_G);
    selectGrainsAction->setKeyEventType(QEvent::KeyPress);
    selectGrainsAction->setMouseEventType(QEvent::MouseButtonRelease);
    selectGrainsAction->setToolTip(tr("Select the grains for audio compositing"));
    connect(selectGrainsAction, SIGNAL(triggered()), this, SLOT(selectGrains()));

    selectRhythmPatternAction = new ACInputActionQt(tr("Select rhythm pattern"), this);
    selectRhythmPatternAction->setShortcut(Qt::Key_P);
    selectRhythmPatternAction->setKeyEventType(QEvent::KeyPress);
    selectRhythmPatternAction->setMouseEventType(QEvent::MouseButtonRelease);
    selectRhythmPatternAction->setToolTip(tr("Select the rhythm pattern for audio compositing"));
    connect(selectRhythmPatternAction, SIGNAL(triggered()), this, SLOT(selectRhythmPattern()));
}

ACAudioGardenSynthesisPlugin::~ACAudioGardenSynthesisPlugin(){
    audio_engine = 0;
    synth = 0;
    synthAudio = 0;
}

void ACAudioGardenSynthesisPlugin::changeMethod(){
    if(!synth)
        return;
    int index = this->getStringParameterValueIndex("Compositing method");
    synth->setMethod((AGMethod) index);
    float autosynth = this->getNumberParameterValue("Autosynth");
    //if(autosynth==1)
    //ui.compositeOsgView->synthesize();
    if(autosynth)
        this->synthesize();
}

void ACAudioGardenSynthesisPlugin::changeMapping(){
    if(!synth)
        return;
    int index = this->getStringParameterValueIndex("Compositing mapping");
    synth->setMapping((AGMapping) index);
    float autosynth = this->getNumberParameterValue("Autosynth");
    //if(autosynth==1)
    //ui.compositeOsgView->synthesize();
    if(autosynth)
        this->synthesize();
}

void ACAudioGardenSynthesisPlugin::changeRandomness(){
    if(!synth)
        return;
    float value = this->getNumberParameterValue("Randomness");
    synth->setRandomness(value/100.0f);
    float autosynth = this->getNumberParameterValue("Autosynth");
    //if(autosynth==1)
    //ui.compositeOsgView->synthesize();
    if(autosynth)
        this->synthesize();
}

void ACAudioGardenSynthesisPlugin::changeThreshold(){
    if(!synth)
        return;
    float value = this->getNumberParameterValue("Threshold");
    synth->setThreshold(value/100.0f);
    float autosynth = this->getNumberParameterValue("Autosynth");
    //if(autosynth==1)
    //ui.compositeOsgView->synthesize();
    if(autosynth)
        this->synthesize();
}

void ACAudioGardenSynthesisPlugin::changeAutosynth(){
    autosynth = (bool) this->getNumberParameterValue("Autosynth");
}

void ACAudioGardenSynthesisPlugin::go(){
    if(!synth)
        return;
    if(!media_cycle)
        return;
    //CF list selected rhythm pattern
    //std::cout << "Selected Rhythm Pattern: " << ui.compositeOsgView->getSelectedRhythmPattern() << std::endl;

    //CF list selected grains
    media_cycle->getBrowser()->dumpSelectedNodes();

    //ui.compositeOsgView->synthesize();
    this->synthesize();
}

void ACAudioGardenSynthesisPlugin::reset(){
    if(!synth)
        return;
    //ui.compositeOsgView->resetSynth();
    this->resetSynth();
}

void ACAudioGardenSynthesisPlugin::mediaCycleSet()
{
    synth->setMediaCycle(media_cycle);
}

bool ACAudioGardenSynthesisPlugin::performActionOnMedia(std::string action, long int mediaId, std::string value){
    std::cout << "ACAudioGardenSynthesisPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << " value " << value << std::endl;
    //2012
    if(!media_cycle){
        std::cerr << "ACAudioGardenSynthesisPlugin::performActionOnMedia: mediacycle not set " << std::endl;
        return false;
    }
    if(!browser_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::performActionOnMedia: browser renderer not set " << std::endl;

        return false;
    }
    if(!timeline_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::performActionOnMedia: timeline renderer not set " << std::endl;
        return false;
    }
    /*           int media_id = media_cycle->getClickedNode();
                        std::cout << "node " << media_id << " selected" << std::endl;
            //media_cycle->hoverWithPointerId(event->x(),event->y(),-1);//mouse
                        //int media_id = media_cycle->getClosestNode();

                        if(media_id >= 0)
            {*/
    /*if(mediaId < 0) { //2012
        std::cerr << "ACAudioGardenSynthesisPlugin::performActionOnMedia: no media selected " << std::endl;
        return false;
    }*/

    /*if (forwarddown==1)
                {
                    if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
                                                media_cycle->incrementNavigationLevels(media_id);
                                        media_cycle->setReferenceNode(media_id);

                    // XSCF 250310 added these 3
                    if (media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS)
                        media_cycle->storeNavigationState();
                    //media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
                    //media_cycle->getBrowser()->setState(AC_CHANGING);

                    //			media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
                    //			media_cycle->getBrowser()->setState(AC_CHANGING);

                    media_cycle->updateDisplay(true); //XS250310 was: media_cycle->updateClusters(true);
                    // XSCF 250310 removed this:
                    // media_cycle->updateNeighborhoods();
                    //					media_cycle->updateClusters(false);// CF was true, equivalent to what's following

                    // remainders from updateClusters(true)
//					media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
//					media_cycle->getBrowser()->setState(AC_CHANGING);
                }*/

    if(action == "select rhythm pattern") //else if (selectRhythmPatternAction == true)
    {
        if (selectedRhythmPattern != -1)
            this->getBrowserRenderer()->resetNodeColor(selectedRhythmPattern);

        selectedRhythmPattern = mediaId;
        if (selectedRhythmPattern != -1)
            this->getBrowserRenderer()->changeNodeColor(selectedRhythmPattern, osg::Vec4(1.0,1.0,1.0,1.0));//CF color the rhythm pattern in white

        if (timeline_renderer->getNumberOfTracks()==0){
            //this->getTimelineRenderer()->addTrack(media_id);
            this->getTimelineRenderer()->addTrack(media_cycle->getLibrary()->getMedia(mediaId));
        }
        else{
            //this->getTimelineRenderer()->getTrack(0)->updateMedia(media_id);
            this->getTimelineRenderer()->getTrack(0)->updateMedia(media_cycle->getLibrary()->getMedia(mediaId));
        }

        //					if ( timeline_renderer->getTrack(0)!=0 )
        //					{
        //						if (track_playing) {
        //							audio_engine->getFeedback()->stopExtSource();
        //							audio_engine->getFeedback()->deleteExtSource();
        //							track_playing = false;
        //						}

        //						//CF possible only for audio? then do some tests
        //                                                ACAudio* tempAudio = (ACAudio*) media_cycle->getLibrary()->getMedia(media_id);

        //						//delete synthAudio;
        //						synthAudio = new ACAudio( *tempAudio, false);
        //						float* tempBuffer = (float*)synthAudio->getMonoSamples();
        //						audio_engine->getFeedback()->createExtSource(tempBuffer, synthAudio->getNFrames() );

        //                                                this->getTimelineRenderer()->getTrack(0)->updateMedia( synthAudio ); //media_cycle->getLibrary()->getMedia(media_id) );
        //						delete[] tempBuffer;
        //						media_cycle->setNeedsDisplay(true);
        //					}
        media_cycle->setNeedsDisplay(true);
        return true; // 2012
    }

    else if(action == "select grains") //else if (selectGrainsAction == true)
    {
        media_cycle->getBrowser()->toggleNode(mediaId);
        media_cycle->getBrowser()->dumpSelectedNodes();
        return true; // 2012
    }
    /*}
        }
        //media_cycle->setClickedNode(-1);
    }
    mousedown = 0;
    borderdown = 0;
    media_cycle->setNeedsDisplay(true);*/
    return false;
}

std::map<std::string,ACMediaType> ACAudioGardenSynthesisPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["select rhythm pattern"] = MEDIA_TYPE_AUDIO;
    media_actions["select grains"] = MEDIA_TYPE_AUDIO;
    return media_actions;
}

void ACAudioGardenSynthesisPlugin::synthesize()
{
    //2012
    if(!media_cycle){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: mediacycle not set " << std::endl;
        return;
    }
    /*if(!audio_engine){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: audio engine not set " << std::endl;
        return;
    }*/
    if(!browser_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: browser renderer not set " << std::endl;
        return;
    }
    if(!timeline_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: timeline renderer not set " << std::endl;
        return;
    }

    if ( this->getSelectedRhythmPattern() > -1 && media_cycle->getBrowser()->getSelectedNodes().size() > 0)
    {
        // Stop the track playback
//        if (track_playing) {
//            audio_engine->getFeedback()->stopExtSource();
//            audio_engine->getFeedback()->deleteExtSource();
//            track_playing = false;
//        }

        // Synthesize
        synth->compute(this->getSelectedRhythmPattern(), media_cycle->getBrowser()->getSelectedNodes());
        //synth->saveAsWav("./synthesis.wav");
        // Display the synthesis
        if (synthAudio)
            delete synthAudio;
        synthAudio = new ACAudio();
        ACAudioFloatPtrDataContainer* data = new ACAudioFloatPtrDataContainer();
        data->setData(synth->getSound());
        data->setNumberOfFrames(synth->getLength());
        ACAudioData* media_data = new ACAudioGardenData();
        media_data->setData(data);
        synthAudio->setMediaData(media_data);
        //synthAudio->computeWaveform( this->getSynth()->getSound()  );
        this->getTimelineRenderer()->getTrack(0)->updateMedia( synthAudio ); //media_cycle->getLibrary()->getMedia(media_id) );
        media_cycle->setNeedsDisplay(true);

        // Playback the synthesis
        media_cycle->getLibrary()->addMedia(synthAudio);
        int mediaId = synthAudio->getId();
//        audio_engine->getFeedback()->createExtSource(this->getSynth()->getSound(), this->getSynth()->getLength());
//        audio_engine->getFeedback()->loopExtSource();
        track_playing = true;
    }
}

void ACAudioGardenSynthesisPlugin::resetSynth()
{
    //2012
    if(!media_cycle){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: mediacycle not set " << std::endl;
        return;
    }
    if(!audio_engine){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: mediacycle not set " << std::endl;
        return;
    }
    if(!browser_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: browser renderer not set " << std::endl;
        return;
    }
    if(!timeline_renderer){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: timeline renderer not set " << std::endl;
        return;
    }

    // Stop the track playback
    if (track_playing) {
        audio_engine->getFeedback()->stopExtSource();
        audio_engine->getFeedback()->deleteExtSource();
        track_playing = false;
    }

    // Unselect pattern and grains
    if (selectedRhythmPattern != -1 )
        this->getBrowserRenderer()->resetNodeColor(selectedRhythmPattern);
    selectedRhythmPattern = -1;
    media_cycle->getBrowser()->unselectNodes();

    // Empty the synthesizer buffer but keep the synthesis parameters
    synth->resetSound();

    // Empty the visual track
    this->getTimelineRenderer()->getTrack(0)->clearMedia();
    media_cycle->setNeedsDisplay(true);
}

void ACAudioGardenSynthesisPlugin::stopSound()
{
    //2012
    if(!audio_engine){
        std::cerr << "ACAudioGardenSynthesisPlugin::synthesize: mediacycle not set " << std::endl;
        return;
    }

    // Stop the track playback
    if (track_playing) {
        audio_engine->getFeedback()->stopExtSource();
        audio_engine->getFeedback()->deleteExtSource();
        track_playing = false;
    }
}

std::map<std::string, ACMediaType> ACAudioGardenSynthesisPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_AUDIO || media_type == MEDIA_TYPE_ALL){
#ifdef PARSE_FORMATS_DYNAMICALLY
#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering audio file extensions from sndfile..." << std::endl;
#endif//def PARSE_OSG_PLUGINS_VERBOSE
        //CF ripped from sndfile's examples/list_formats.c
        SF_FORMAT_INFO	info ;
        SF_INFO 		sfinfo ;
        char buffer [128] ;
        int format, major_count, subtype_count, m, s;

        memset (&sfinfo, 0, sizeof (sfinfo));
        buffer [0] = 0 ;
        sf_command (0, SFC_GET_LIB_VERSION, buffer, sizeof (buffer));
        if (strlen (buffer) < 1)
        {	printf ("sndfile line %d: could not retrieve lib version.\n", __LINE__);
            //exit (1);
        } ;
        //printf ("Adding libsnfile supported extension, version : %s\n\n", buffer);// CF enable for debug mode

        sf_command (0, SFC_GET_FORMAT_MAJOR_COUNT, &major_count, sizeof (int)) ;
        sf_command (0, SFC_GET_FORMAT_SUBTYPE_COUNT, &subtype_count, sizeof (int)) ;

        sfinfo.channels = 1 ;
        for (m = 0 ; m < major_count ; m++)
        {
            info.format = m ;
            sf_command (0, SFC_GET_FORMAT_MAJOR, &info, sizeof (info)) ;

            std::string ext =  std::string(".") + std::string(info.extension);

            if (ext == ".aif" || ext == ".aiff"){
                extensions[".aif"];
                extensions[".aiff"];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extensions .aiff and .aif" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            else if (ext == ".oga"){
                extensions[".oga"];
                extensions[".ogg"];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extensions .oga and .ogg" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            else{
                extensions[ext];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extension " << ext << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            //CF the following could be used to add format metadata
            /*
    printf ("%s  (extension \"%s\")\n", info.name, info.extension) ;
    format = info.format ;
    for (s = 0 ; s < subtype_count ; s++)
    {	info.format = s ;
        sf_command (0, SFC_GET_FORMAT_SUBTYPE, &info, sizeof (info)) ;

        format = (format & SF_FORMAT_TYPEMASK) | info.format ;

        sfinfo.format = format ;
        if (sf_format_check (&sfinfo))
            printf ("   %s\n", info.name) ;
    } ;
    */
        }
#else // PARSE_FORMATS_DYNAMICALLY
        // from libsndfile 1.0.21
        extensions[".aif"] = MEDIA_TYPE_AUDIO;
        extensions[".aiff"] = MEDIA_TYPE_AUDIO;
        extensions[".au"] = MEDIA_TYPE_AUDIO;
        extensions[".avr"] = MEDIA_TYPE_AUDIO;
        extensions[".caf"] = MEDIA_TYPE_AUDIO;
        extensions[".flac"] = MEDIA_TYPE_AUDIO;
        extensions[".htk"] = MEDIA_TYPE_AUDIO;
        extensions[".iff"] = MEDIA_TYPE_AUDIO;
        extensions[".mat"] = MEDIA_TYPE_AUDIO;
        extensions[".mpc"] = MEDIA_TYPE_AUDIO;
        extensions[".oga"] = MEDIA_TYPE_AUDIO;
        extensions[".ogg"] = MEDIA_TYPE_AUDIO;
        extensions[".paf"] = MEDIA_TYPE_AUDIO;
        extensions[".pvf"] = MEDIA_TYPE_AUDIO;
        extensions[".raw"] = MEDIA_TYPE_AUDIO;
        extensions[".rf64"] = MEDIA_TYPE_AUDIO;
        extensions[".sd2"] = MEDIA_TYPE_AUDIO;
        extensions[".sds"] = MEDIA_TYPE_AUDIO;
        extensions[".sf"] = MEDIA_TYPE_AUDIO;
        extensions[".voc"] = MEDIA_TYPE_AUDIO;
        extensions[".w64"] = MEDIA_TYPE_AUDIO;
        extensions[".wav"] = MEDIA_TYPE_AUDIO;
        extensions[".wve"] = MEDIA_TYPE_AUDIO;
        extensions[".xi"] = MEDIA_TYPE_AUDIO;
#endif // PARSE_FORMATS_DYNAMICALLY
    }
    return extensions;
}


void ACAudioGardenSynthesisPlugin::selectGrains(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("select grains", media_id,"");
}

void ACAudioGardenSynthesisPlugin::selectRhythmPattern(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("select rhythm pattern", media_id,"");
}

std::vector<ACInputActionQt*> ACAudioGardenSynthesisPlugin::providesInputActions(){
    std::vector<ACInputActionQt*> inputActions;
    inputActions.push_back(selectGrainsAction);
    inputActions.push_back(selectRhythmPatternAction);
    return inputActions;
}

