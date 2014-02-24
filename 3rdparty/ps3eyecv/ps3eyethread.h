/**
 * @brief Thread wrapper for the Sony PS3 EYE Camera Driver using libusb
 * @author Christian Frisson
 * @date 21/11/2013
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

#include <iostream>
#include <vector>
#include <pthread.h>

class ps3eyethread
{
public:
    ps3eyethread() {/* empty */}
    virtual ~ps3eyethread() {/* empty */}

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool start();

    /** Will not return until the internal thread has exited. */
    void waitExit();

protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void thread() = 0;

private:
    static void * threadFunc(void * This) {((ps3eyethread *)This)->thread(); return NULL;}
    pthread_t _thread;
};
