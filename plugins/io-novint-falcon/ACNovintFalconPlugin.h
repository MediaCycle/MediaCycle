/**
 * @brief Plugin for using the Novint Falcon 3DOF force-feedback device for browsing
 * @author Christian Frisson
 * @date 10/07/2013
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

#include "falcon/core/FalconDevice.h"
#include "falcon/grip/FalconGripFourButton.h"
#include "falcon/kinematic/FalconKinematicStamper.h"
#include "falcon/firmware/FalconFirmwareNovintSDK.h"
#include "falcon/util/FalconFirmwareBinaryNvent.h"

#ifndef ACNovintFalconPlugin_H
#define	ACNovintFalconPlugin_H

class ScopedMutex
{
    ScopedMutex() {}

public:
    ScopedMutex(pthread_mutex_t& tm)
    {
        m = &tm;
        pthread_mutex_init(m,NULL);
        pthread_mutex_lock(m);
    }

    ~ScopedMutex()
    {
        pthread_mutex_unlock(m);
    }
private:
    pthread_mutex_t* m;
};

class ACNovintFalconPlugin : public QObject, public ACPluginQt, virtual public ACClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACNovintFalconPlugin();
    virtual ~ACNovintFalconPlugin();

    // From ACPluginQt
    virtual std::vector<ACInputActionQt*> providesInputActions();

 public:

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread()
    {
        this->active = true;
        return (pthread_create(&device_thread, NULL, device_loop_func, this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitForInternalThreadToExit()
    {
        (void) pthread_join(device_thread, NULL);
    }

protected:
    bool init_device();
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void device_loop();// = 0;

private:
    static void * device_loop_func(void * This) {((ACNovintFalconPlugin *)This)->device_loop(); return NULL;}

    // Adapted from Kyle Machulis' np_nifalcon flext object
    // https://github.com/qdot/np_nifalcon/blob/master/np_nifalcon.flext.cpp
    libnifalcon::FalconDevice m_falconDevice;
    int m_deviceIndex;
    bool m_inRawMode;
    bool m_isInited;
    int m_ledState;
    boost::array<double,3> m_motorVectorForce;
    boost::array<int,3> m_motorRawForce;
    bool m_homingMode;

    bool m_runThread;
    pthread_mutex_t m_deviceMutex;
    pthread_mutex_t m_updateMutex;
    pthread_mutex_t m_ioMutex;
    pthread_mutex_t m_runMutex;
    bool m_hasUpdated;
    bool m_alwaysOutput;

    int motor_list[3];
    float coordinate_list[3];
    int button_list[4];
    bool motor_changed;
    bool coordinate_changed;
    bool homing_state_changed;
    bool button_state_changed;
    bool old_homing_state;
    bool old_button_state;
    bool homing_state;
    uint8_t button_state;
    double sleep_time;

public:
    void Exit();
    void nifalcon_output_request();
    int nifalcon_count();
    void nifalcon_auto_poll();
    void nifalcon_manual_poll();
    void nifalcon_raw();
    void nifalcon_vector();
    void nifalcon_nvent_firmware();
    void nifalcon_open();
    void nifalcon_close();
    //void nifalcon_anything(const t_symbol *msg,int argc,t_atom *argv);
    void nifalcon_stop();
    void nifalcon_update_loop();
    void nifalcon_output();
    void nifalcon_start_thread();
    void nifalcon_motor_raw(int motor_1, int motor_2, int motor_3);
    void nifalcon_motor_vector(float x, float y, float z);
    void nifalcon_led(int red, int green, int blue);
    void nifalcon_homing_mode(long t);

private:
    unsigned int m_axis;
    unsigned long m_runClickCount;
    bool m_positiveForce;
    double m_plane_stiffness;
    gmtl::Vec3f m_axisBounds;
    bool m_isInitializing;
    bool m_hasPrintedInitMsg;
    bool m_buttonDown;

    double m_stiffness;
    double m_radius;
    bool m_plusButtonDown;
    bool m_minusButtonDown;
    bool m_centerButtonDown;
    bool m_forwardButtonDown;

private:
    pthread_t device_thread;
    bool active;

    std::vector<std::string> layouts;

};

#endif

