/*
 *  ACOscBrowser.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 03/04/11
 *
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

#include "ACOscBrowser.h"
#include <sstream>

using namespace std;

void error(int num, const char *msg, const char *path) {
    std::cout << "ACOscBrowser: liblo server error " << num << " in path " << path << ": " << msg << std::endl;
    fflush(stdout);
}

ACOscBrowser::ACOscBrowser() {
    this->media_cycle = 0;
    this->callback = 0;
    this->user_data = 0;
    this->osc_feedback = 0;
    this->server_thread = 0;
    this->active = false;
    this->verbosity = false;
    this->forward_messages = true;
}

ACOscBrowser::~ACOscBrowser() {
    release();
}

bool ACOscBrowser::create(const char *hostname, int port) {
    this->release();
    char portchar[6];
    sprintf(portchar, "%d", port);
    server_thread = lo_server_thread_new(portchar, error);
    if (!server_thread) {
        std::cerr << "ACOscBrowser: An OSC server is already running at port " << port << ". Please change it and restart the server." << std::endl;
        return false;
    }
    this->setUserData(this);
    this->setCallback(static_mess_handler);
    return true;
}

void ACOscBrowser::release() {
    if (server_thread) {
        lo_server_thread_free(server_thread);
        server_thread = 0;
    }
    this->active = false;
}

bool ACOscBrowser::start() {
    if(server_thread){
        lo_server_thread_start(server_thread);
        this->active = true;
        return true;
    }
    return false;
}

void ACOscBrowser::stop() {
    lo_server_thread_stop(server_thread);
    this->active = false;
}

void ACOscBrowser::setUserData(void *_user_data) {
    user_data = _user_data;
}

void ACOscBrowser::setCallback(ACOscBrowserCallback* _callback) {
    callback = _callback;
    lo_server_thread_add_method(server_thread, NULL, NULL, *_callback, user_data);
}

// these can be called only from the callback

void ACOscBrowser::readFloat(float *val) {
}

void ACOscBrowser::readInt(int *val) {
}

void ACOscBrowser::readString(char *val, int maxlen) {
}

int ACOscBrowser::static_mess_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
    ACOscBrowser* widget = (ACOscBrowser*) user_data;
    return widget->process_mess(path, types, argv, argc, widget->getVerbosity());
}

int ACOscBrowser::process_mess(const char *path, const char *types, lo_arg **argv, int argc, bool verbose) {
    std::string tag = std::string(path);
    //std::cout << "ACOscBrowser: OSC message: '" << tag << "'" << std::endl;
    bool mc = (tag.find("/mediacycle", 0) != string::npos);
    bool tuio = (tag.find("/tuio", 0) != string::npos);
    if (!mc && !tuio)//we don't process messages not containing /mediacycle or /tuio
        return 1;

    if (osc_feedback and forward_messages) {
        osc_feedback->messageBegin(path);
        for(int arg=0; arg<argc; arg++){
            if(types[arg]=='f'){
                osc_feedback->messageAppendFloat(argv[arg]->f);
            }
            else if(types[arg]=='i'){
                osc_feedback->messageAppendInt(argv[arg]->i);
            }
            else if(types[arg]=='s'){
                osc_feedback->messageAppendString(&argv[arg]->s);
            }
        }
        //osc_feedback->messageAppendFloat();
        osc_feedback->messageEnd();
        osc_feedback->messageSend();
    }


    // test - sends back message (console + OSC feedback) if received properly
    if (tag.find("/test", 0) != string::npos) {
        std::cout << "ACOscBrowser: OSC communication established" << std::endl;
        if (osc_feedback) {
            osc_feedback->messageBegin("/mediacycle/received");
            osc_feedback->messageEnd();
            osc_feedback->messageSend();
        }
        return 1;
    }
    // XS TODO: this one is useless, fix it ?
    else if (tag.find("/fullscreen", 0) != string::npos) {
        int fullscreen = 0;
        fullscreen = argv[0]->i;
        std::cout << "ACOscBrowser: Fullscreen? " << fullscreen << std::endl;
        //if (fullscreen == 1)
        //	ui.groupControls->hide();
        //else
        //	ui.groupControls->show();
        return 1;
    }

    // MediaCycle is needed for everything below this
    if (!this->media_cycle)// || !this->getOsgView())
        return 1;

    if (tag.find("/list_media_actions", 0) != string::npos || tag == "/who") {
        std::cout << "ACOscBrowser: available media actions " << std::endl;
        for(std::map<std::string,std::string>::iterator media_action = media_actions.begin();media_action!=media_actions.end();media_action++){
            std::string _message = "/mediacycle/pointer/<id>/" + media_action->first;
            std::cout << "ACOscBrowser: media action "<< _message << std::endl;
            /*if (osc_feedback) {
                osc_feedback->messageBegin(_message.c_str());
                osc_feedback->messageEnd();
                osc_feedback->messageSend();
            }*/
        }
        std::cout << "ACOscBrowser: available media actions with parameters " << std::endl;
        std::map<std::string,std::string>::iterator media_action = media_actions.begin();
        for(std::map<std::string,ACMediaActionParameters>::iterator action_parameters = actions_parameters.begin();action_parameters!=actions_parameters.end();action_parameters++){
            std::cout << "ACOscBrowser: media action '" << action_parameters->first << "'";
            std::string _message = "/mediacycle/pointer/<id>/" + media_action->first;
            media_action++;
            ACMediaActionParameters parameters = action_parameters->second;
            if(parameters.size()==0)
                std::cout << " without parameters";
            else if(parameters.size()==1)
                std::cout << " with parameter:";
            else
                std::cout << " with parameters:";
            for(ACMediaActionParameters::iterator parameter = parameters.begin();parameter != parameters.end();parameter++){
                std::cout << " '" << parameter->getName() << "' (type: %" << (char)(parameter->getType()) << ")";
                _message += " <" + parameter->getName() + " (%" + (char)(parameter->getType()) + ")>";
            }
            std::cout << std::endl;
            if (osc_feedback) {
                osc_feedback->messageBegin(_message.c_str());
                osc_feedback->messageEnd();
                osc_feedback->messageSend();
            }
        }
        return 1;
    }
    // BROWSER CONTROLS
    else if (tag.find("/browser", 0) != string::npos && this->media_cycle) {

        if (!this->media_cycle->getBrowser())
            return 1;

        //Extracting the pointer/device integer id between "/browser/" and subsequent "/..." strings
        // XS TODO: what default value should be sent if only one application, one browser ?
        // (0 ? nothing ?)
        int id = -1;
        std::string prefix = ("/browser/");
        std::string suffix = ("/");
        size_t prefix_found = tag.find(prefix, 0);
        if (prefix_found != string::npos) {
            size_t suffix_found = tag.find(suffix, prefix_found + prefix.size());
            if (suffix_found != string::npos) {
                std::string id_string = tag.substr(prefix_found + prefix.size(), suffix_found - (prefix_found + prefix.size()));
                istringstream id_ss(id_string);
                if (!(id_ss >> id))
                    std::cerr << "ACOscBrowser: wrong pointer id" << std::endl;
            }
        }

        if (tag.find("/activated", 0) != string::npos) {
            std::cout << "ACOscBrowser: OSC message: '" << tag << "'" << std::endl;
            //media_cycle->resetPointers();//CF hack when /released messages aren't received
            media_cycle->addPointer(id);
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/released", 0) != string::npos) {
            std::cout << "ACOscBrowser: OSC message: '" << tag << "'" << std::endl;
            if (media_cycle->getNumberOfPointers() < 1)//TR NEM2011
                media_cycle->setAutoPlay(0); //TR NEM
            if (media_cycle->getNumberOfPointers() == 1)//TR NEM2011
                if (media_cycle->getPointerFromIndex(0))
                    if (media_cycle->getPointerFromIndex(0)->getType() == AC_POINTER_MOUSE)
                        media_cycle->setAutoPlay(0); //TR NEM
            media_cycle->removePointer(id); //CF hack
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/reset_pointers", 0) != string::npos) {
            std::cout << "ACOscBrowser: OSC message: '" << tag << "'" << std::endl;
            media_cycle->setAutoPlay(0);
            //media_cycle->resetPointers();
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
            std::cout << "ACOscBrowser: reset to " << media_cycle->getNumberOfPointers() << " pointer(s)" << std::endl;
        } else if (tag.find("/move/xy", 0) != string::npos) {
            float x = 0.0, y = 0.0;
            media_cycle->getCameraPosition(x, y);
            x = argv[0]->f;
            y = argv[1]->f;
            float zoom = media_cycle->getCameraZoom();
            float angle = media_cycle->getCameraRotation();
            float xmove = x * cos(-angle) - y * sin(-angle);
            float ymove = y * cos(-angle) + x * sin(-angle);
            media_cycle->setCameraPosition(xmove / 2 / zoom, ymove / 2 / zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/zoom", 0) != string::npos) {
            float zoom = 0.0f;
            zoom = argv[0]->f;
            std::cout << "ACOscBrowser: zoom of " << zoom << std::endl;
            //zoom = zoom*600/50; // refzoom +
            media_cycle->setCameraZoom((float) zoom);
            media_cycle->setNeedsDisplay(true);
        } else if ((tag.find("/rotate", 0) != string::npos) || (tag.find("/angle", 0) != string::npos)) {
            float angle = 0.0f;
            angle = argv[0]->f;
            media_cycle->setCameraRotation((float) angle);
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/hover/xy", 0) != string::npos) {
            float x = 0.0, y = 0.0;
            if (argc == 2 && types[0] == LO_FLOAT && types[1] == LO_FLOAT) {
                x = argv[0]->f;
                y = argv[1]->f;

                media_cycle->hoverWithPointerId(x, y, id);
                //    ACPoint p;
                //    p.x = x;
                //    p.y = y;
                //    p.z = 0;
                //    media_cycle->getPointerFromId(id)->setCurrentPosition(p);

                if (media_cycle->getLibrary()->getSize() > 0) {
                    media_cycle->setAutoPlay(1);

                    //CF required for force-feedback, to restore (need to move "distancemouse" calculation in the core)
                    /*int closest_node = media_cycle->getClosestNode();
                    float distance = this->getOsgView()->getBrowserRenderer()->getDistanceMouse()[closest_node];
                    if (osc_feedback)
                    {
                        osc_feedback->messageBegin("/mediacycle/closest_node_at");
                        osc_feedback->messageAppendFloat(distance);
                        osc_feedback->messageEnd();
                        osc_feedback->messageSend();
                    }*/
                }
                //media_cycle->setNeedsDisplay(true);
            }
        } else if (tag.find("/getknn", 0) != string::npos) {
            // returns the path to the k nearest neighbors of a specific media (XS TODO could send back Id)
            // 2 options :
            // 1) OSC message : getknn k m : specifies id of specific media
            // 2) OSC message : getknn k   : media = last one that was clicked
            std::cout << "ACOscBrowser: getKNN" << std::endl;
            int k = 0;
            int m = 0;
            std::vector<int> knn;
            bool knn_ok = false;
            if (argc == 1) {
                k = argv[0]->i;
                m = media_cycle->getLastSelectedNode(); // get the id of the last node we clicked on
                if (m < 0) {
                    std::cerr << "ACOscBrowser: error getting clicked node" << std::endl;
                    return -1;
                }
                knn_ok = true;
            } else if (argc == 2) {
                k = argv[0]->i;
                m = argv[1]->i;
                knn_ok = true;
            }
            if (!knn_ok) {
                std::cerr << "ACOscBrowser: error with knn message" << std::endl;
                return -1;
            } else {
                std::cout << "ACOscBrowser: getKNN ; k= " << k << "; media m = " << m << std::endl;
                media_cycle->getKNN(m, knn, k);
                if (knn.size() != k) {
                    std::cout << "ACOscBrowser: error with getKNN method" << std::endl;
                    return 1;
                }
                if (osc_feedback) {
                    for (int i = 0; i < k; i++) {
                        string full_name = media_cycle->getMediaFileName(knn[i]);
                        if (full_name == "") {
                            std::cout << "ACOscBrowser: empty file name for media " << knn[i] << std::endl;
                        } else {
                            std::cout << "ACOscBrowser: sending name of media " << knn[i] << std::endl;
                            osc_feedback->messageBegin("/mediacycle/knn");
                            osc_feedback->messageAppendString(full_name);
                            osc_feedback->messageEnd();
                            osc_feedback->messageSend();
                        }
                    }
                }
                else {
                    std::cerr << "ACOscBrowser: no OSC feedback connected" << std::endl;
                    return -1;
                }
            }
        } else if (tag.find("/recenter", 0) != string::npos) {
            media_cycle->setCameraRecenter();
        } else if (tag.find("/recluster", 0) != string::npos) {
            //int node = media_cycle->getClickedNode();
            int node = media_cycle->getClosestNode();
            if (media_cycle->getLibrary()->getSize() > 0 && media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS && node > -1) {
                media_cycle->setReferenceNode(node);
                // media_cycle->pushNavigationState(); XS 250810 removed
                media_cycle->updateDisplay(true);
            }
        } else if (tag.find("/back", 0) != string::npos) {
            media_cycle->goBack();
        } else if (tag.find("/forward", 0) != string::npos) {
            media_cycle->goForward();
        } else if ((tag.find("/gointocluster", 0) != string::npos)) {
            // XS NOTE if you put "forwardNextLevel" as OSC message, it will be mistaken for "forward"
            media_cycle->forwardNextLevel();
        } else if ((tag.find("/changerefNode", 0) != string::npos)) {
            media_cycle->changeReferenceNode();
        } else {
            std::cout << "ACOscBrowser: error with tag'" << tag << "'";
            for (int a = 0; a < argc; a++) {
                std::cout << " <" << types[a] << ">";
            }
            std::cout << std::endl;
        }
    } // LIBRARY
    else if (tag.find("/library", 0) != string::npos) {
        std::cout << "ACOscBrowser: library command " << std::endl;
        if (tag.find("/addfile", 0) != string::npos) {
            char* c = &(argv[0]->s);
            stringstream ss;
            string s;
            ss << c;
            ss >> s;
            std::cout << "ACOscBrowser: adding file : " << s << std::endl;
            std::vector<std::string> vs; // so that I can use importDirectories, not directory
            vs.push_back(s);
            media_cycle->importDirectories(vs, 0);
            // XS TODO : should not call these methods from outside -- clean up !!
            //media_cycle->normalizeFeatures(1);
            //media_cycle->libraryContentChanged(1);
        } else if (tag.find("/load", 0) != string::npos) {
            std::cerr << "ACOscBrowser: Library loading thru OSC not yet implemented" << std::endl;
        } else if (tag.find("/clear", 0) != string::npos) {
            std::cerr << "ACOscBrowser: Library cleaning thru OSC not yet implemented" << std::endl;

            //this->media_cycle->cleanLibrary();
            //this->media_cycle->cleanBrowser();

            //CF make the following accessible from a dock manager

            //was cleanCheckBoxes()
            //for (int d=0;d<dockWidgets.size();d++){
            //	if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt") {
            //		((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
            //	}
            //	if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt") {
            //		((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
            //	}
            //}

            // XS TODO : remove the boxes specific to the media that was loaded
            // e.g. ACAudioControlDockWidgets
            // modify the DockWidget's API to allow this
            //plugins_scanned = false;

            //this->getOsgView()->clean();
            //this->getOsgView()->setFocus();
        }
    }// PLAYER
    else if (tag.find("/player", 0) != string::npos) {
        if (!media_cycle->getLibrary()){
            std::cerr << "ACOscBrowser: no library" << std::endl;
            return 1;
        }
        if (media_cycle->getLibrary()->getSize() == 0){
            std::cerr << "ACOscBrowser: library empty, /player messages useless" << std::endl;
            return 1;
        }
        if (tag.find("/muteall", 0) != string::npos) {
            media_cycle->resetPointers(); //CF hack dirty!
            media_cycle->muteAllSources();
            return 1;
        }
        if (media_cycle->getLibrary()->getMediaType() != MEDIA_TYPE_AUDIO){
            std::cerr << "ACOscBrowser: /player messages only for audio for now" << std::endl;
            return 1;
        }
        ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin("Audio Engine");
        if(!plugin){
            std::cerr << "ACOscBrowser: Audio Engine plugin not set, can't bind /player messages" << std::endl;
            return 1;
        }
        ACMediaRendererPlugin* audio_engine_plugin = dynamic_cast<ACMediaRendererPlugin*>(plugin);
        if(!audio_engine_plugin){
            std::cerr << "ACOscBrowser: Audio Engine plugin not a media renderer plugin, can't bind /player messages" << std::endl;
            return -1;
        }
        if (tag.find("/bpm", 0) != string::npos) {
            float bpm;
            bpm = argv[0]->f;
            cout << "ACOscBrowser: /player/bpm/" << bpm << endl;
            int node = media_cycle->getClosestNode();
            if (node > -1) {
                audio_engine_plugin->performActionOnMedia("scale mode",node,"AutoBeat");
                ////audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample)
                //audio_engine_plugin->performActionOnMedia("synchro mode",node,"Resample");
                audio_engine_plugin->setNumberParameterValue("BPM",(float) bpm);
            }
            return 1;
        }
        //Extracting the pointer/device integer id between "/player/" and subsequent "/..." strings
        int id = -1;
        std::string prefix = ("/player/");
        std::string suffix = ("/");
        size_t prefix_found = tag.find(prefix, 0);
        if (prefix_found != string::npos) {
            size_t suffix_found = tag.find(suffix, prefix_found + prefix.size());
            if (suffix_found != string::npos) {
                std::string id_string = tag.substr(prefix_found + prefix.size(), suffix_found - (prefix_found + prefix.size()));
                istringstream id_ss(id_string);
                if (!(id_ss >> id))
                    std::cerr << "ACOscBrowser: wrong pointer id" << std::endl;
            }
        }
        int node = -1;
        if (media_cycle->getBrowser()->getPointerFromId(id))
            node = media_cycle->getBrowser()->getPointerFromId(id)->getClosestNode();
        if (tag.find("/playclosestloop", 0) != string::npos) {
            //media_cycle->pickedObjectCallback(-1);
            media_cycle->getBrowser()->toggleSourceActivity(node);
            cout << "ACOscBrowser: /player/" << node << "/playclosestloop node " << node << endl;
            return 1;
        } else if (tag.find("/triggerclosestloop", 0) != string::npos) {
            audio_engine_plugin->performActionOnMedia("synchro mode",node,"Manual");
            audio_engine_plugin->performActionOnMedia("scale mode",node,"Resample");
            audio_engine_plugin->performActionOnMedia("scrub",node,"0");
            audio_engine_plugin->performActionOnMedia("synchro mode",node,"None");
            media_cycle->getBrowser()->toggleSourceActivity(node);
            cout << "ACOscBrowser: /player/" << node << "/triggerclosestloop node " << node << endl;
            return 1;
        } else if (tag.find("/scrub", 0) != string::npos) {
            float scrub;
            scrub = argv[0]->f;
            if (node > -1) {
                audio_engine_plugin->performActionOnMedia("synchro mode",node,"Manual");
                audio_engine_plugin->performActionOnMedia("scale mode",node,"Resample");// Vocode
                stringstream s;
                s << (float) scrub * 100; // temporary hack to scrub between 0 an 1
                audio_engine_plugin->performActionOnMedia("scrub",node,s.str());
            }
            return 1;
        } else if (tag.find("/pitch", 0) != string::npos) {
#ifdef USE_DEBUG
            float pitch;
            pitch = argv[0]->f;
            cout << "ACOscBrowser: /player/" << node << "/pitch/" << pitch << endl;
            if (node > -1) {
                audio_engine_plugin->performActionOnMedia("synchro mode",node,"AutoBeat");
                audio_engine_plugin->performActionOnMedia("scale mode",node,"Resample");// or Vocode
                stringstream p;
                p << (float) pitch;
                audio_engine_plugin->performActionOnMedia("pitch",node, p.str());
            }
#else //USE_DEBUG
            cout << "ACOscBrowser: /player/" << node << "/pitch not yet safe, can mess up the audioengine " << endl;
#endif
            return 1;
        } else if (tag.find("/gain", 0) != string::npos) {
            float gain;
            gain = argv[0]->f;
            cout << "ACOscBrowser: /player/" << node << "/gain/" << gain << endl;
            if (node > -1) {
                stringstream g;
                g << (float) gain;
                audio_engine_plugin->performActionOnMedia("gain",node, g.str());
            }
            return 1;
        }
    } // TUIO
    else if(tuio){ //else if (tag.find("/tuio", 0) != string::npos) {
        // TUIO 1.1
        //http://www.tuio.org/?specification

        // Dimension: defines 2D/2.5D/3D Interactive Surfaces or custom profiles
        float dim = 0;
        if (tag.find("/2D", 0) != string::npos) {
            dim = 2;
        }
        else if (tag.find("/25D", 0) != string::npos) {
            dim = 2.5;
        }
        else if (tag.find("/3D", 0) != string::npos) {
            dim = 3;
        }
        else if (tag.find("/_", 0) != string::npos) {
            dim = 1;
            if(verbose)
                std::cout << "ACOscBrowser: received custom TUIO message: " << tag << std::endl;
            return 1;
        }
        else{
            dim = 0;
            if(verbose)
                std::cout << "ACOscBrowser: received unknown TUIO message: " << tag << std::endl;
            return 1;
        }
        if(verbose)
            std::cout << "ACOscBrowser: received "<< dim << "D TUIO message";//: " << tag << std::endl;

        // Type: object, curser or blob
        std::string type("");
        if (tag.find("obj", 0) != string::npos) {
            type = "obj";
        }
        else if (tag.find("cur", 0) != string::npos) {
            type = "cur";
        }
        else if (tag.find("blb", 0) != string::npos) {
            type = "blb";
        }
        if(type!="")
            if(verbose)
                std::cout << " of type " << type;
        //std::cout << "ACOscBrowser: received " << type << " TUIO message: " << tag << std::endl;

        // Status:
        // - source (starts message bundles, with name@address source identification),
        // - alive (lists alive elements ids),
        // - set (outputs parameters of each element),
        // - fseq (ends message bundles, with an incremental frame id)
        std::string status("");
        if(argc > 0 && types[0]=='s'){
            status = std::string( &(argv[0]->s) );
        }
        if(status!=""){
            if(verbose)
                std::cout << " of status '" << status << "'";
            //std::cout << "ACOscBrowser: received " << type << " TUIO message: " << tag << std::endl;
        }
        else{
            std::cerr << "ACOscBrowser: TUIO message with unknown status: " << tag << std::endl;
            return 1;
        }
        if(status == "source"){
            if( argc == 2 && types[1]=='s'){
                if(verbose)
                    std::cout << " '" << &(argv[1]->s) << "'";
            }
            else {
                std::cerr << "ACOscBrowser: TUIO message with source status with incorrect number of arguments: " << tag << std::endl;
                return 1;
            }
        }
        else if(status == "alive"){
            if(verbose)
                std::cout << "alive " << std::endl;
            /*if(argc-1 == 0){
                media_cycle->resetPointers();
                media_cycle->setAutoPlay(0);
                media_cycle->muteAllSources();
                media_cycle->setNeedsDisplay(true);
            }*/
            bool refresh = false;

            std::list<int> ids = media_cycle->getPointerIds();

            for(int arg = 0; arg<argc;arg++){
                if(types[arg]=='i'){
                    if(verbose)
                        std::cout << " " << argv[arg]->i32;

                    std::list<int>::iterator _id = std::find(ids.begin(),ids.end(),argv[arg]->i32);
                    if(_id != ids.end())
                        ids.erase(_id);

                    if(!media_cycle->getPointerFromId( argv[arg]->i32 )){
                        media_cycle->addPointer( argv[arg]->i32 );
                        refresh = true;
                    }
                }
                /*else
                    if(verbose)
                        std::cout << " err";*/
            }

            for(std::list<int>::iterator _id = ids.begin(); _id != ids.end(); _id++){
                media_cycle->removePointer(*_id);
                if(verbose)
                    std::cout << " remove " << *_id;
            }
            if(verbose)
                std::cout << std::endl;

            if(refresh)
                media_cycle->setNeedsDisplay(true);
        }
        else if(status == "set"){
            if(dim <= 1){
                std::cerr << "ACOscBrowser: TUIO message with set status of unimplemented custom type: " << tag << std::endl;
                return 1;
            }
            int arg = 1;
            // Session ID (temporary object ID)
            int sid = argv[arg++]->i32;
            if(verbose)
                std::cout << " s:" << sid;
            // Class ID (e.g. marker ID)
            if(type =="obj"){
                int cid = argv[arg++]->i32;
                if(verbose)
                    std::cout << " i:" << cid;
            }
            // Positions (range 0...1)
            float x = argv[arg++]->f32;
            if(verbose)
                std::cout << " x:" << x;
            float y = argv[arg++]->f32;
            if(verbose)
                std::cout << " y:" << y;
            if(dim > 2.0f){
                float z = argv[arg++]->f32;
                if(verbose)
                    std::cout << " z:" << z;
            }
            // Angles (range 0..2PI)
            if(type == "obj" || type == "blb"){
                float a = argv[arg++]->f32;
                if(verbose)
                    std::cout << " a:" << a;
                if(dim == 3){
                    float b = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " b:" << b;
                    float c = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " c:" << c;
                }
            }
            // Dimensions (range 0..1)
            if(type == "blb"){
                // Width (range 0..1)
                float w = argv[arg++]->f32;
                if(verbose)
                    std::cout << " w:" << w;
                // Height (range 0..1)
                float h = argv[arg++]->f32;
                if(verbose)
                    std::cout << " h:" << h;
                if(dim == 3.0f){
                    // Depth (range 0..1)
                    float d = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " d:" << d;
                    // Volume (range 0..1)
                    float v = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " v:" << v;
                }
                else if(dim >=2.0f && dim < 3.0f){
                    // Area (range 0..1)
                    float f = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " f:" << f;
                }
            }
            // Velocity vector (motion speed & direction
            float X = argv[arg++]->f32;
            if(verbose)
                std::cout << " X:" << X;
            float Y = argv[arg++]->f32;
            if(verbose)
                std::cout << " Y:" << Y;
            if(dim > 2.0f){
                float Z = argv[arg++]->f32;
                if(verbose)
                    std::cout << " Z:" << Z;
            }
            // Rotation velocity vector (rotation speed & direction)
            if(type == "obj" || type == "blb"){
                float A = argv[arg++]->f32;
                if(verbose)
                    std::cout << " A:" << A;
                if(dim == 3){
                    float B = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " B:" << B;
                    float C = argv[arg++]->f32;
                    if(verbose)
                        std::cout << " C:" << C;
                }
            }
            // Motion acceleration
            float m = argv[arg++]->f32;
            if(verbose)
                std::cout << " m:" << m;
            // Rotation acceleration
            if(type == "obj" || type == "blb"){
                float r = argv[arg++]->f32;
                if(verbose)
                    std::cout << " r:" << r;
            }
            media_cycle->hoverWithPointerId(-1.0f+2.0f*x, 1.0f-2.0f*y, sid);
            if (media_cycle->getLibrary()->getSize() > 0)
                media_cycle->setAutoPlay(1);
        }
        else if(status == "fseq"){
            // The FSEQ frame ID is incremented for each delivered bundle, while redundant bundles can be marked using the frame sequence ID -1.
            if( argc == 2 && types[1]=='i'){
                if(verbose)
                    std::cout << " '" << argv[1]->i << "'";
            }
            else {
                std::cerr << " incorrect number of arguments: " << tag << std::endl;
                return 1;
            }
        }
        if(verbose){
            std::cout << " (" << argc-1 << " arg(s)) ";
            std::cout << std::endl;
        }
    }
    else{
        size_t action_start = tag.find_last_of ("/");
        std::string action("");
        if(action_start != string::npos){
            action = tag.substr(action_start+1);
        }
        std::map<std::string,std::string>::iterator media_action = media_actions.find(action);
        if(media_action!=media_actions.end()){
            //Extracting the pointer/device integer id before the action
            int id = -1;
            std::string app("/mediacycle");
            std::string prefix("");
            //std::string prefix = "/" + media_action->second + "/";
            prefix = app + "/pointer/";
            std::string suffix = "/" + media_action->first;//("/");
            size_t prefix_found = tag.find(prefix, 0);
            bool allMedia = false;
            if (prefix_found != string::npos) {
                size_t suffix_found = tag.find(suffix, prefix_found + prefix.size());
                if (suffix_found != string::npos) {
                    std::string id_string = tag.substr(prefix_found + prefix.size(), suffix_found - (prefix_found + prefix.size()));

                    if(id_string == "*")
                        allMedia = true;

                    if(!allMedia){
                        istringstream id_ss(id_string);
                        if (!(id_ss >> id)){
                            std::cerr << "ACOscBrowser: wrong pointer id" << std::endl;
                            return -1;
                        }
                    }
                }
                else{
                    return -1;
                }
            }
            else{
                return -1;
            }

            int node = -1;
            if(allMedia){
                node = -2;
            }
            else{
                if (media_cycle->getBrowser()->getPointerFromId(id))
                    node = media_cycle->getBrowser()->getPointerFromId(id)->getClosestNode();
                if(node==-1){
                    std::cerr << "ACOscBrowser: no closest node for pointer id " << id << std::endl;
                    return -1;
                }
            }

            std::vector<boost::any> arguments;

            for(int argn = 0; argn<argc;argn++){
                if(types[argn]=='s'){
                    std::string arg = std::string( &(argv[argn]->s) );
                    arguments.push_back(arg);
                }
                else if(types[argn]=='i'){
                    int arg = argv[argn]->i;
                    float argf = (float)arg; // CF workaround: PureData might send int for sliders boundaries while required to send floats
                    arguments.push_back(argf);
                }
                else if(types[argn]=='f'){
                    float arg = argv[argn]->f;
                    arguments.push_back(arg);
                }
                else if(types[argn]=='d'){
                    double arg = argv[argn]->d;
                    arguments.push_back(arg);
                }
                else{
                    std::cerr << "ACOscBrowser: unsuported argument number " << argn << " of type " << types[argn] << std::endl;
                    return -1;
                }
            }
            media_cycle->performActionOnMedia(media_action->second,node,arguments);
        }
    }
    return 1;
    //std::cout << "ACOscBrowser: end of OSC process messages" << std::endl;
}

void ACOscBrowser::clearMediaActions(){
    this->media_actions.clear();
}

bool ACOscBrowser::addMediaAction(std::string _action){
    if(_action=="")
        return false;

    std::string _tag(_action);
    size_t _space = 0;
    while(_space != std::string::npos){
        _space = _tag.find_first_of(" ",_space);
        if(_space != std::string::npos){
            _tag = _tag.replace(_space,1,"_");
        }
    }

    std::map<std::string,std::string>::iterator listed = media_actions.find(_tag);
    if(listed!=this->media_actions.end())
        return false;

    this->media_actions[_tag] = _action;
    std::cout << "ACOscBrowser::addMediaAction: adding action '" << _action << "' (" << _tag << ")" << std::endl;
    return true;
}

void ACOscBrowser::clearActionParameters(){
    this->actions_parameters.clear();
}

bool ACOscBrowser::addActionParameters(std::string _action, ACMediaActionParameters _parameters){
    if(_action=="")
        return false;
    std::map<std::string,ACMediaActionParameters>::iterator listed = actions_parameters.find(_action);
    if(listed!=this->actions_parameters.end())
        return false;

    this->actions_parameters[_action] = _parameters;
    std::cout << "ACOscBrowser::addActionParameters: adding action '" << _action << "' parameters " << std::endl;
    return true;
}
