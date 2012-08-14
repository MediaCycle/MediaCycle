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

ACOscBrowser::ACOscBrowser(MediaCycle* _mc) {
    this->media_cycle = _mc;
#if defined (SUPPORT_AUDIO)
    this->audio_engine = 0;
#endif //defined (SUPPORT_AUDIO)
    this->osc_feedback = 0;
    this->server_thread = 0;
    this->active = false;
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
        std::cerr << "An OSC server is already running at port " << port << ". Please change it and restart the server." << std::endl;
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

void ACOscBrowser::start() {
    lo_server_thread_start(server_thread);
    this->active = true;
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
    widget->process_mess(path, types, argv, argc);
}

int ACOscBrowser::process_mess(const char *path, const char *types, lo_arg **argv, int argc) {
    std::string tag = std::string(path);
    std::cout << "OSC message: '" << tag << "'" << std::endl;
    bool ac = (tag.find("/audiocycle", 0) != string::npos);
    bool mc = (tag.find("/mediacycle", 0) != string::npos);
    if (!ac && !mc)//we don't process messages not containing /audiocycle or /mediacycle
        return 1;

    // test - sends back message (console + OSC feedback) if received properly
    if (tag.find("/test", 0) != string::npos) {
        std::cout << "OSC communication established" << std::endl;
        if (osc_feedback) {
            if (ac)
                osc_feedback->messageBegin("/audiocycle/received");
            else
                osc_feedback->messageBegin("/mediacycle/received");
            osc_feedback->messageEnd();
            osc_feedback->messageSend();
            return 1;
        }
    }
    // XS TODO: this one is useless, fix it ?
    else if (tag.find("/fullscreen", 0) != string::npos) {
        int fullscreen = 0;
        fullscreen = argv[0]->i;
        std::cout << "Fullscreen? " << fullscreen << std::endl;
        //if (fullscreen == 1)
        //	ui.groupControls->hide();
        //else
        //	ui.groupControls->show();
        return 1;
    }

    // MediaCycle is needed for everything below this
    if (!media_cycle)// || !this->getOsgView())
        return 1;

    // BROWSER CONTROLS
    if (tag.find("/browser", 0) != string::npos && media_cycle) {

        if (!media_cycle->getBrowser())
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

        // XS TODO clean the following messages (self-claimed to be "ugly" ?):
        // - activated
        // - released
        // - reset_pointers
        if (tag.find("/activated", 0) != string::npos) {
            std::cout << "OSC message: '" << tag << "'" << std::endl;
            //media_cycle->resetPointers();//CF temp: hack, when /released messages aren't received
            media_cycle->addPointer(id);
            //Ugly
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/released", 0) != string::npos) {
            std::cout << "OSC message: '" << tag << "'" << std::endl;
            if (media_cycle->getNumberOfPointers() < 1)//TR NEM2011
                media_cycle->setAutoPlay(0); //TR NEM
            if (media_cycle->getNumberOfPointers() == 1)//TR NEM2011
                if (media_cycle->getPointerFromIndex(0))
                    if (media_cycle->getPointerFromIndex(0)->getType() == AC_POINTER_MOUSE)
                        media_cycle->setAutoPlay(0); //TR NEM
            media_cycle->removePointer(id); //CF hack
            //Ugly
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
        } else if (tag.find("/reset_pointers", 0) != string::npos) {
            std::cout << "OSC message: '" << tag << "'" << std::endl;
            media_cycle->setAutoPlay(0);
            //media_cycle->resetPointers();
            //Ugly
            //osg_view->getHUDRenderer()->preparePointers();
            media_cycle->setNeedsDisplay(true);
            std::cout << "Reset to " << media_cycle->getNumberOfPointers() << " pointer(s)" << std::endl;
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
            std::cout << "OSC - zooming by " << zoom << std::endl;
            //zoom = zoom*600/50; // refzoom +
            media_cycle->setCameraZoom((float) zoom);
            media_cycle->setNeedsDisplay(true);
        } else if ((tag.find("/rotate", 0) != string::npos) || (tag.find("/angle", 0) != string::npos)) {
            float angle = 0.0f;
            angle = argv[0]->f;
            media_cycle->setCameraRotation((float) angle);
            media_cycle->setNeedsDisplay(true);
        }			//XS TODO clean this ? ("ugly")
        else if (tag.find("/hover/xy", 0) != string::npos) {
            float x = 0.0, y = 0.0;
            if (argc == 2 && types[0] == LO_FLOAT && types[1] == LO_FLOAT) {
                x = argv[0]->f;
                y = argv[1]->f;

                media_cycle->hoverWithPointerId(x, y, id);
                /*ACPoint p;
    p.x = x;
    p.y = y;
    p.z = 0;
    media_cycle->getPointerFromId(id)->setCurrentPosition(p);*/

                if (media_cycle->getLibrary()->getSize() > 0) {

                    ////////////////Ugly
                    media_cycle->setAutoPlay(1);

                    //CF hack
                    //int closest_node = media_cycle->getClosestNode();
                    //float distance = this->getOsgView()->getBrowserRenderer()->getDistanceMouse()[closest_node];
                    //if (osc_feedback)
                    //{
                    //	if (ac)
                    //		osc_feedback->messageBegin("/audiocycle/closest_node_at");
                    //	else
                    //		osc_feedback->messageBegin("/mediacycle/closest_node_at");
                    //	osc_feedback->messageAppendFloat(distance);
                    //	osc_feedback->messageEnd();
                    //	osc_feedback->messageSend();
                    //}
                }
                //media_cycle->setNeedsDisplay(true);
            }
        } else if (tag.find("/getknn", 0) != string::npos) {
            // returns the path to the k nearest neighbors of a specific media (XS TODO could send back Id)
            // 2 options :
            // 1) OSC message : getknn k m : specifies id of specific media
            // 2) OSC message : getknn k   : media = last one that was clicked
            std::cout << "OSC - getknn" << std::endl;
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
                std::cout << "OSC - getknn ; k= " << k << "; media m = " << m << std::endl;
                media_cycle->getKNN(m, knn, k);
                if (knn.size() != k) {
                    std::cout << "ACOscBrowser: error with knn method" << std::endl;
                    return 1;
                }
                if (osc_feedback) {
                    for (int i = 0; i < k; i++) {
                        string full_name = media_cycle->getMediaFileName(knn[i]);
                        if (full_name == "") {
                            std::cout << "ACOscBrowser: empty file name for media " << knn[i] << std::endl;
                        } else {
                            std::cout << "sending name of media " << knn[i] << std::endl;
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
        } else {
            std::cout << "ACOscBrowser: error with tag'" << tag << "'";
            for (int a = 0; a < argc; a++) {
                std::cout << " <" << types[a] << ">";
            }
            std::cout << std::endl;
        }
    } // LIBRARY
    else if (tag.find("/library", 0) != string::npos) {
        std::cout << "ACOscBrowser communication with library " << std::endl;
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
        if (!media_cycle->getLibrary())
            return -1;
        if (media_cycle->getLibrary()->getSize() == 0)
            return -1;
#if defined (SUPPORT_AUDIO)
        if (media_cycle->getLibrary()->getMediaType() == MEDIA_TYPE_AUDIO && !this->getAudioEngine())
            return 1;
#endif //defined (SUPPORT_AUDIO)
        if (tag.find("/muteall", 0) != string::npos) {
            //if(this->getMediaType()==MEDIA_TYPE_AUDIO)
            media_cycle->resetPointers(); //CF hack dirty!
            media_cycle->muteAllSources();
            return 1;
        } else if (tag.find("/bpm", 0) != string::npos) {
            float bpm;
            bpm = argv[0]->f;
            cout << "/player/bpm/" << bpm << endl;
            int node = media_cycle->getClosestNode();
#if defined (SUPPORT_AUDIO)
            if (node > -1) {
                audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
                //audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
                audio_engine->setBPM((float) bpm);
            }
#endif //defined (SUPPORT_AUDIO)
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
            cout << "/player/" << node << "/playclosestloop node " << node << endl;
            return 1;
        } else if (tag.find("/triggerclosestloop", 0) != string::npos) {
#if defined (SUPPORT_AUDIO)
            audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
            audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
            audio_engine->setScrub(0.0f);
            audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeNone);
#endif //defined (SUPPORT_AUDIO)
            media_cycle->getBrowser()->toggleSourceActivity(node);
            cout << "/player/" << node << "/triggerclosestloop node " << node << endl;
            return 1;
        } else if (tag.find("/scrub", 0) != string::npos) {
            float scrub;
            scrub = argv[0]->f;
#if defined (SUPPORT_AUDIO)
            if (node > -1) {
                audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
                audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample); //ACAudioEngineScaleModeVocode
                audio_engine->setScrub((float) scrub * 100); // temporary hack to scrub between 0 an 1
            }
#endif //defined (SUPPORT_AUDIO)
            return 1;
        } else if (tag.find("/pitch", 0) != string::npos) {
#ifdef USE_DEBUG
            float pitch;
            pitch = argv[0]->f;
            cout << "/player/" << node << "/pitch/" << pitch << endl;
#if defined (SUPPORT_AUDIO)
            if (node > -1) {
                audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
                audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
                audio_engine->setSourcePitch(node, (float) pitch);
            }
#endif //defined (SUPPORT_AUDIO)
#else //USE_DEBUG
            cout << "/player/" << node << "/pitch not yet safe, can mess up the audioengine " << endl;
#endif
            return 1;
        } else if (tag.find("/gain", 0) != string::npos) {
            float gain;
            gain = argv[0]->f;
            cout << "/player/" << node << "/gain/" << gain << endl;
#if defined (SUPPORT_AUDIO)
            if (node > -1) {
                //audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
                //audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
                audio_engine->setSourceGain(node, (float) gain);
            }
#endif //defined (SUPPORT_AUDIO)
            return 1;
        }
    }
    return 1;
    //std::cout << "End of OSC process messages" << std::endl;
}
