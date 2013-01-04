/**
 * @brief Plugin for contesting in the video browser showdown
 * @author Christian Frisson
 * @date 2/01/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include <MediaCycle.h>
#include <ACPlugin.h>
#include <ACPluginQt.h>

#include <stdio.h>
#include <pthread.h>

#include "hidapi.h"

#ifndef ACVideoBrowserShowdownPlugin_H
#define	ACVideoBrowserShowdownPlugin_H

class ACVideoBrowserShowdownPlugin : public QObject, public ACPluginQt, virtual public ACClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACVideoBrowserShowdownPlugin();
    virtual ~ACVideoBrowserShowdownPlugin();

    // From ACPluginQt
    virtual std::vector<ACInputActionQt*> providesInputActions();

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread()
    {
        this->active = true;
        return (pthread_create(&hid_thread, NULL, hid_loop_func, this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitForInternalThreadToExit()
    {
        (void) pthread_join(hid_thread, NULL);
    }

protected:
    bool init_hid();
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void hid_loop();// = 0;

    void cueInCallback();
    void cueOutCallback();
    void submitCallback();
    void adjustSpeedCallback();
    void skipFrameCallback();
    void togglePlaybackCallback();

private:
    static void * hid_loop_func(void * This) {((ACVideoBrowserShowdownPlugin *)This)->hid_loop(); return NULL;}

    hid_device *handle;
    pthread_t hid_thread;
    bool active;

    /// Shuttle Pro2 specific:
    bool button01, button02, button03, button04, button05, button06, button07, button08, button09, button10, button11, button12, button13, button14, button15;
    bool button01_changed, button02_changed, button03_changed, button04_changed, button05_changed, button06_changed, button07_changed, button08_changed, button09_changed, button10_changed, button11_changed, button12_changed, button13_changed, button14_changed, button15_changed;
    int jog, wheel;
    bool jog_changed, wheel_changed;

    void reset_changed_booleans();
    void print_ascii_status();

    /// Layout as follows:
    ///   01 02 03 04
    ///  05 06 07 08 09
    /// 10   wheel   11
    ///  12  (jog)  13
    /// 14           15

    ACInputActionQt* cueInAction;
    ACInputActionQt* cueOutAction;
    ACInputActionQt* submitAction;
    ACInputActionQt* adjustSpeedAction;
    ACInputActionQt* skipFrameAction;
    ACInputActionQt* togglePlaybackAction;

    int cue_in, cue_out; // frames
    std::string url;
    int port;
    int team;
};

#endif

