/**
 * @brief Plugin for using the 3M MicroTouch Display C1968PW screen as input device
 * @author Christian Frisson
 * @date 23/01/2013
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
#include <ACPluginControlsDockWidgetQt.h>

#include <stdio.h>
#include <pthread.h>

#include "hidapi.h"

#ifndef AC3MMultitouchScreenPlugin_H
#define	AC3MMultitouchScreenPlugin_H

class AC3MMultitouchScreenPlugin : public QObject, public ACPluginQt, virtual public ACClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    AC3MMultitouchScreenPlugin();
    virtual ~AC3MMultitouchScreenPlugin();

    // From ACPluginQt
    virtual std::vector<ACInputActionQt*> providesInputActions();

 public:

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

private:
    static void * hid_loop_func(void * This) {((AC3MMultitouchScreenPlugin *)This)->hid_loop(); return NULL;}

    hid_device *handle;
    pthread_t hid_thread;
    bool active;

    void print_ascii_status();

    int number_of_fingers;
    std::map<int,ACPoint> pointers;
};

#endif

