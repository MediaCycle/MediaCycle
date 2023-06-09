/*
 *  ACOscBrowser.h
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

#ifndef HEADER_ACOSCBROWSER
#define HEADER_ACOSCBROWSER

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lo/lo.h>

#include <ACOscFeedback.h>
#include <MediaCycle.h>

typedef int (ACOscBrowserCallback) (const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

class ACOscBrowser {
public:
    ACOscBrowser();
    ~ACOscBrowser();

    // pass 0 to receive from any host
    bool create(const char *hostname, int port);
    void release();

    // Starts a background thread and listens to socket from there
    bool start();
    void stop();

    void setUserData(void *_user_data);
    void setCallback(ACOscBrowserCallback _callback);

    bool isActive(){return active;}

    // these can be called only from the callback
    void readFloat(float *val);
    void readInt(int *val);
    void readString(char *val, int maxlen);

    bool getVerbosity(){return this->verbosity;}

    void clearMediaActions();
    bool addMediaAction(std::string _action);
    void clearActionParameters();
    bool addActionParameters(std::string _action, ACMediaActionParameters _parameters);

private:
    lo_server_thread server_thread;
    void* user_data;
    ACOscBrowserCallback* callback;
    ACOscFeedback *osc_feedback;
    bool active;
    bool verbosity;

public:
    static int static_mess_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
    int process_mess(const char *path, const char *types, lo_arg **argv, int argc, bool verbose);

public:

    void setMediaCycle(MediaCycle* _media_cycle) {
        this->media_cycle = _media_cycle;
    }

    MediaCycle* getMediaCycle() {
        return this->media_cycle;
    }
    void setFeedback(ACOscFeedback *_osc_feedback) {
        this->osc_feedback = _osc_feedback;
    }

    ACOscFeedback* getFeedback() {
        return this->osc_feedback;
    }

    void forwardMessages(bool _forward_messages){forward_messages = _forward_messages;}

protected:
    MediaCycle *media_cycle;
    std::map<std::string,std::string> media_actions;
    std::map<std::string,ACMediaActionParameters> actions_parameters;
    bool forward_messages;
};

#endif /* _ACOSCBROWSER_H_ */
