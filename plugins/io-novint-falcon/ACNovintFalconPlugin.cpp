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

#include "ACNovintFalconPlugin.h"
#include <MediaCycle.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

// Headers needed for sleeping.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace libnifalcon;
using namespace std;

ACNovintFalconPlugin::ACNovintFalconPlugin()
    : QObject(), ACPluginQt(), ACClientPlugin(),
      m_deviceIndex(-1),
      m_runThread(false),
      m_isInited(false),
      m_inRawMode(true),
      m_ledState(0),
      m_homingMode(false),
      m_hasUpdated(false),
      m_alwaysOutput(false),
      motor_changed(false),
      coordinate_changed(false),
      homing_state(false),
      button_state(0),
      sleep_time(.001),
      m_axisBounds(0, 0, .100),//.130
      m_plane_stiffness(1000),
      m_isInitializing(true),
      m_hasPrintedInitMsg(false),
      m_axis(2),
      m_positiveForce(true),
      m_runClickCount(0),
      m_buttonDown(false),
      m_stiffness(1000),
      m_radius(0.024),
      m_plusButtonDown(false),
      m_minusButtonDown(false),
      m_centerButtonDown(false),
      m_forwardButtonDown(false)
{
    this->mName = "Force-feedback input the Novint Falcon 3DOF device";
    this->mDescription ="Plugin for using the Novint Falcon 3DOF device for force-feedback control";
    this->mMediaType = MEDIA_TYPE_ALL;

    layouts.push_back("Desktop");
    layouts.push_back("Left-handed starfish eNTERFACE");
    layouts.push_back("Right-handed starfish eNTERFACE");

    this->addStringParameter("Layout",layouts.back(),layouts,"Layout of the Novint Falcon");

    this->active = false;
    for(int i = 0; i < 3; ++i)
    {
        m_motorVectorForce[i] = 0;
        m_motorRawForce[i] = 0;
    }

    m_falconDevice.setFalconFirmware<FalconFirmwareNovintSDK>();
    m_falconDevice.setFalconKinematic<FalconKinematicStamper>();
    m_falconDevice.setFalconGrip<FalconGripFourButton>();

    this->active = false;

    this->init_device();
}

bool ACNovintFalconPlugin::init_device(){

    int count = this->nifalcon_count();

    if(count>0){
        this->nifalcon_open();
        this->nifalcon_nvent_firmware();
        this->nifalcon_vector();

        //this->nifalcon_start_thread();
        this->active = true;
    }
    else
        return false;

    return this->StartInternalThread();
}


ACNovintFalconPlugin::~ACNovintFalconPlugin(){

    this->active = false;
    m_alwaysOutput = false;
    if(m_runThread) nifalcon_stop();
    m_falconDevice.close();

    this->WaitForInternalThreadToExit();

#ifdef WIN32
    system("pause");
#endif
}

void ACNovintFalconPlugin::Exit()
{
    m_alwaysOutput = false;
    m_falconDevice.getFalconFirmware()->setLEDStatus(libnifalcon::FalconFirmware::RED_LED);
    if(m_runThread) nifalcon_stop();
    m_falconDevice.close();
    //flext_base::Exit();
}

void ACNovintFalconPlugin::device_loop(){
    std::cout << "device loop" << std::endl;
    int res = 0;
    unsigned char buf[256];
    int i = 0;

    this->nifalcon_start_thread();
}

void ACNovintFalconPlugin::nifalcon_output_request()
{
    if(!m_alwaysOutput)
    {
        nifalcon_output();
        return;
    }
    std::cout << "ACNovintFalconPlugin: Cannot request output by bang/'output' message when auto_poll is on" << std::endl;
}

int ACNovintFalconPlugin::nifalcon_count()
{
    ScopedMutex s(m_deviceMutex);
    unsigned int count = 0;
    if(!m_falconDevice.getDeviceCount(count))
    {
        std::cout << "ACNovintFalconPlugin: Error while counting devices" << std::endl;
        return 0;
    }
    std::cout << "ACNovintFalconPlugin: Falcons Available: " << count << std::endl;
    return count;
}

void ACNovintFalconPlugin::nifalcon_auto_poll()
{
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Turning on auto polling mode " << std::endl;
    m_alwaysOutput = true;
}

void ACNovintFalconPlugin::nifalcon_manual_poll()
{
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Turning on manual polling mode" << std::endl;
    m_alwaysOutput = false;
}

void ACNovintFalconPlugin::nifalcon_raw()
{
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Falcon force input now in raw mode" << std::endl;
    m_inRawMode = true;
}

void ACNovintFalconPlugin::nifalcon_vector()
{
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Falcon force input now in vector mode" << std::endl;
    m_inRawMode = false;
}

void ACNovintFalconPlugin::nifalcon_nvent_firmware()
{
    if(!m_falconDevice.isOpen())
    {
        std::cout << "ACNovintFalconPlugin: Falcon not open" << std::endl;
        return;
    }
    /*if(m_falconDevice.isFirmwareLoaded())
    {
        m_isInited = true;
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Firmware already loaded, no need to reload..." << std::endl;
        return;
    }*/
    ScopedMutex s(m_deviceMutex);
    for(int i = 0; i < 10; ++i)
    {
        if(!m_falconDevice.getFalconFirmware()->loadFirmware(false, NOVINT_FALCON_NVENT_FIRMWARE_SIZE, const_cast<uint8_t*>(NOVINT_FALCON_NVENT_FIRMWARE)))
        {
            //Completely close and reopen
            m_falconDevice.close();
            if(!m_falconDevice.open(m_deviceIndex))
            {
                std::cout << "ACNovintFalconPlugin: Cannot open falcon device index "<< m_deviceIndex << " - Lib Error Code: " << m_falconDevice.getErrorCode() << " Device Error Code: " << m_falconDevice.getFalconComm()->getDeviceErrorCode() << std::endl;
                break;
            }
        }
        else
        {
            m_isInited = true;
            break;
        }
    }
    if(m_isInited)
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": loading nvent firmware finished" << std::endl;
    else
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": loading nvent firmware FAILED" << std::endl;
}

void ACNovintFalconPlugin::nifalcon_open()
{
    if(m_falconDevice.isOpen())
    {
        std::cout << "np_nifalcon " << m_deviceIndex << ": Falcon already open" << std::endl;
        return;
    }
    ScopedMutex s(m_deviceMutex);
    int index = /*-1;
                                                                                                                                                    if(argc == 1) index = GetInt(argv[0]);
                                                                                                                                                    else index =*/ 0;
    std::cout << "np_nifalcon: Opening first falcon found" << std::endl;
    if(!m_falconDevice.open(index))
    {
        std::cout << "np_nifalcon: Cannot open falcon " << index <<" Error: " <<  m_falconDevice.getErrorCode() << std::endl;
        return;
    }
    m_deviceIndex = index;
    std::cout << "np_nifalcon" << index <<" Opened" << std::endl;
    return;
}

void ACNovintFalconPlugin::nifalcon_close()
{
    if(!m_falconDevice.isOpen())
    {
        std::cout << "ACNovintFalconPlugin: Falcon not open" << std::endl;
        return;
    }

    m_isInited = false;
    if(m_runThread) nifalcon_stop();
    //ScopedMutex s(m_deviceMutex);
    m_falconDevice.close();
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Falcon device closed" << std::endl;
    m_deviceIndex = -1;
    return;
}

/*void ACNovintFalconPlugin::nifalcon_anything(const t_symbol *msg,int argc,t_atom *argv)
{
    if(!strcmp(msg->s_name, "open"))
    {
        if(m_falconDevice.isOpen())
        {
            std::cout << "ACNovintFalconPlugin: Falcon "<< m_deviceIndex <<" already open" << std::endl;
            return;
        }
        ScopedMutex s(m_deviceMutex);
        int index = -1;
        if(argc == 1) index = GetInt(argv[0]);
        else index = 0;
        std::cout << "ACNovintFalconPlugin: Opening first falcon found" << std::endl;
        if(!m_falconDevice.open(index))
        {
            std::cout << "ACNovintFalconPlugin: Cannot open falcon " << index << " - Error: " << m_falconDevice.getErrorCode() << std::endl;
            return;
        }
        m_deviceIndex = index;
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<" Opened"<< std::endl;
        return;
    }
    else if (!strcmp(msg->s_name, "init"))
    {
        if(!m_falconDevice.isOpen())
        {
            std::cout << "ACNovintFalconPlugin: Falcon not open" << std::endl;
            return;
        }
        if(m_falconDevice.isFirmwareLoaded())
        {
            m_isInited = true;
            std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Firmware already loaded, no need to reload..." << std::endl;
            return;
        }
        if(!m_falconDevice.setFirmwareFile(GetString(argv[0])))
        {
            std::cout << "ACNovintFalconPlugin " << m_deviceIndex << ": Cannot find firmware file " << GetString(argv[0]) << std::endl;
            return;
        }
        ScopedMutex s(m_deviceMutex);
        if(!m_falconDevice.loadFirmware(10, false))
        {
            std::cout << "ACNovintFalconPlugin " << m_deviceIndex << ": Could not load firmware: " << m_falconDevice.getErrorCode() << std::endl;
            return;
        }
        m_isInited = true;
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Falcon init finished" << std::endl;
    }
    else if(!strcmp(msg->s_name, "sleep_time"))
    {
        //We take microseconds as arguments. Not sure if we have that level of granularity, but it never hurts to try.
        if(argc == 1)
        {
            sleep_time = (double)GetInt(argv[0]) / (double)1000000;
            post("Set sleep time to " << sleep_time << std::endl;
        }
        else
        {
            std::cout << "ACNovintFalconPlugin: sleep_usec message takes one argument (microseconds to sleep in polling thread)" << std::endl;
        }
    }
    else
    {
        std::cout << "ACNovintFalconPlugin: not a valid np_nifalcon message: " << msg->s_name << std::endl;
    }
}*/

void ACNovintFalconPlugin::nifalcon_stop()
{
    if(!m_falconDevice.isOpen())
    {
        std::cout << "ACNovintFalconPlugin: Falcon not open"<< std::endl;
        return;
    }

    if(!m_runThread)
    {
        std::cout << "ACNovintFalconPlugin " << m_deviceIndex << ": No thread running" << std::endl;
        return;
    }
    bool ao = m_alwaysOutput;
    m_alwaysOutput = false;
    m_runThread = false;
    //ScopedMutex r(m_runMutex);
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Input thread stopped" << std::endl;
    m_alwaysOutput = ao;
    return;
}

void ACNovintFalconPlugin::nifalcon_update_loop()
{
    bool ret = false;

    //Run the IO Loop, locking the device while we do
    {
        //ScopedMutex m(m_deviceMutex);
        ret = m_falconDevice.runIOLoop(FalconDevice::FALCON_LOOP_FIRMWARE | FalconDevice::FALCON_LOOP_GRIP | (m_inRawMode ? 0 : FalconDevice::FALCON_LOOP_KINEMATIC));
    }

    //If we didn't get anything out of the IO loop, return
    if(!ret)
    {
        return;
    }

    //Put together the information
    {
        //lock to make sure we don't try to bang out half updated information
        //ScopedMutex m(m_ioMutex);
        int i = 0, buttons = 0;
        //t_atom analog_list[];
        //Output encoder values
        for(i = 0; i < 3; ++i)
        {
            if(motor_list[i] == (m_falconDevice.getFalconFirmware()->getEncoderValues())[i]) continue;
            motor_changed = true;
            motor_list[i] = m_falconDevice.getFalconFirmware()->getEncoderValues()[i];
        }

        //Output kinematics values
        for(i = 0; i < 3; ++i)
        {
            if(fabs(coordinate_list[i] - m_falconDevice.getPosition()[i]) < .000001 ) continue;
            coordinate_changed = true;
            coordinate_list[i] = m_falconDevice.getPosition()[i];
        }
        if(coordinate_changed){

            boost::array<double, 3> pos = m_falconDevice.getPosition();
            if(m_isInitializing)
            {
                if(!m_hasPrintedInitMsg)
                {
                    std::cout << "Move the end effector out of the wall area" << std::endl;
                    m_hasPrintedInitMsg = true;
                    m_falconDevice.getFalconFirmware()->setLEDStatus(libnifalcon::FalconFirmware::RED_LED);
                }
                if(((pos[m_axis] > m_axisBounds[m_axis]) && m_positiveForce) || ((pos[m_axis] < m_axisBounds[m_axis]) && !m_positiveForce))
                {
                    std::cout << "Starting wall simulation..." << std::endl;
                    m_isInitializing = false;
                    m_falconDevice.getFalconFirmware()->setLEDStatus(libnifalcon::FalconFirmware::GREEN_LED);
                    //tstart();
                }
            }
            else{

                float x(0.0f),y(0.0f);
                if(media_cycle){
                    media_cycle->setAutoPlay(1);
                    if(this->getStringParameterValue("Layout") == "Desktop"){
                        // Falcon x,y -> Screen x,y : Desktop
                        x = 20*pos[0];
                        y = 20*pos[1];
                    }
                    else if(this->getStringParameterValue("Layout") == "Left-handed starfish eNTERFACE"){
                        // Falcon y, -x -> Screen x,y : Left-handed starfish eNTERFACE
                        x = 20*pos[1];
                        y = -20*pos[0];
                    }
                    else /*if(this->getStringParameterValue("Layout") == "Right-handed starfish eNTERFACE")*/{
                        // Falcon -y, x -> Screen x,y : Right-handed starfish eNTERFACE
                        x = -20*pos[1];
                        y = 20*pos[0];
                    }
                    if(media_cycle->hasBrowser()) media_cycle->getBrowser()->removeMousePointer();
                    media_cycle->hoverWithPointerId(x,y,0);
                    //std::cout << pos[0] << " " << pos[1] << " " << pos[2] << " "<< std::endl;
                }

                boost::array<double, 3> force;
                force[0] = 0.0;
                force[1] = 0.0;
                force[2] = 0.0;

                //CF: Z-wall from libnifalcon FalconWallTest

                double plane_dist = 10000;
                int closest = -1, outside=3;

                // For each axis, check if the end effector is inside
                // the cube.  Record the distance to the closest wall.

                force[m_axis] = 0;
                if(((pos[m_axis] < m_axisBounds[m_axis]) && m_positiveForce) || ((pos[m_axis] > m_axisBounds[m_axis]) && !m_positiveForce))
                {
                    double dA = pos[m_axis]-m_axisBounds[m_axis];
                    plane_dist = dA;
                    closest = m_axis;
                }

                // If so, add a proportional force to kick it back
                // outside from the nearest wall.

                if (closest > -1)
                    force[closest] = -m_plane_stiffness*plane_dist;

                if(media_cycle){
                    if(media_cycle->getLibrarySize()>0 && this->getBrowserRenderer()){
                        ACMediaNode* node = media_cycle->getMediaNode( media_cycle->getClosestNode(0) );
                        if(node){
                            ACPoint p = node->getCurrentPosition(); // getNextPosition();//
                            float distance = 0;
                            if( this->getBrowserRenderer())
                                distance = this->getBrowserRenderer()->getDistanceMouse()[media_cycle->getClosestNode(0)];

                            if(distance < 0.03){

                                //CF: node friction from libnifalcon FalconSphereTest

                                //make sphere soft radius or "slippery"
                                //m_stiffness = -300.0;//attraction
                                //m_stiffness = 500.0;//friction
                                m_stiffness = 0.225;

                                if(this->getStringParameterValue("Layout") == "Desktop"){
                                    // Falcon x,y -> Screen x,y : Desktop
                                    force[0] = (p.x - pos[0]) * m_stiffness * 10.0;
                                    force[1] = (p.y - pos[1]) * m_stiffness * 10.0;
                                }
                                else if(this->getStringParameterValue("Layout") == "Left-handed starfish eNTERFACE"){
                                    // Falcon y, -x -> Screen x,y : Left-handed starfish eNTERFACE
                                    force[0] = (p.y - pos[1]) * m_stiffness * 10.0;
                                    force[1] = -(p.x - pos[0]) * m_stiffness * 10.0;

                                }
                                else /*if(this->getStringParameterValue("Layout") == "Right-handed starfish eNTERFACE")*/{
                                    // Falcon -y, x -> Screen x,y : Right-handed starfish eNTERFACE
                                    force[0] = -(p.y - pos[1]) * m_stiffness * 10.0;
                                    force[1] = (p.x - pos[0]) * m_stiffness * 10.0;
                                }
                                //Alt
                                //force[0] = (pos[0] / dist) * (m_radius - dist) * m_stiffness;
                                //force[1] = (pos[1] / dist) * (m_radius - dist) * m_stiffness;
                            }
                        }
                    }
                }
                m_falconDevice.setForce(force);
            }
        }

        //Output digital values
        buttons = m_falconDevice.getFalconGrip()->getDigitalInputs();
        if(button_state != buttons)
        {
            for(i = 0; i < 4; ++i)
            {
                button_list[i] = buttons & (1 << i);
            }
            button_state = buttons;
            button_state_changed = true;
        }

        //Output analog values
        //We don't have analog values yet. Nothing will leave this output until I figured out analog. Implement later.

        //Output homing values
        if(m_falconDevice.getFalconFirmware()->isHomed() != homing_state)
        {
            homing_state = m_falconDevice.getFalconFirmware()->isHomed();
            homing_state_changed = true;
        }

        //Confirm that we're ready to output if we have something new
        if(motor_changed || coordinate_changed || button_state_changed || homing_state_changed)
            m_hasUpdated = true;
    }
    //If we're autopolling and we got this far, output
    if(m_alwaysOutput && m_hasUpdated) nifalcon_output();

    //Update the device with the information from the patch
    {
        //lock to make sure we don't try to update information from a patch while it's written to the device object
        //ScopedMutex t(m_updateMutex);
        //Now that we're done parsing what we got back, set the new internal values
        //        if(!m_inRawMode)
        //        {
        //            m_falconDevice.setForce(m_motorVectorForce);
        //        }
        //        else
        //        {
        //            m_falconDevice.getFalconFirmware()->setForces(m_motorRawForce);
        //        }

        m_falconDevice.getFalconFirmware()->setHomingMode(m_homingMode);

        m_falconDevice.getFalconFirmware()->setLEDStatus(m_ledState);
    }
}

void ACNovintFalconPlugin::nifalcon_output()
{
    //If we haven't run a new successful loop yet, we have nothing to output
    if(!m_hasUpdated) return;

    //Since this is the only function we output in, do a system level lock
    //This calls critical_enter/exit on max and sys_lock on Pd
    //Thanks to ClaudiusMaximus on freenode #dataflow for pointing this out.
    //See also http://lists.puredata.info/pipermail/pd-list/2005-01/025473.html

    //ONLY NEEDED FOR PD. LOCKS MAX
    //Lock();

    //Make sure we don't collide with the I/O loop if we're manually polling
    ScopedMutex s(m_ioMutex);
    if(motor_changed)
    {
        motor_changed = false;
        //ToOutList(1, 3, motor_list);
    }
    if(coordinate_changed && !m_inRawMode)
    {
        coordinate_changed = false;
        //ToOutList(2, 3, coordinate_list);
    }
    if(button_state_changed)
    {
        button_state_changed = false;
        //ToOutList(3, 4, button_list);
    }
    if(homing_state_changed)
    {
        homing_state_changed = false;
        //ToOutInt(5, homing_state);
    }
    //ToOutBang(0);

    //ONLY NEEDED FOR PD. LOCKS MAX
    //Unlock();

    m_hasUpdated = false;
}

void ACNovintFalconPlugin::nifalcon_start_thread()
{
    if(!m_falconDevice.isOpen())
    {
        std::cout << "ACNovintFalconPlugin: Falcon not open" << std::endl;
        return;
    }
    if(m_runThread)
    {
        std::cout << "ACNovintFalconPlugin "<< m_deviceIndex << ": Thread already running" << std::endl;
        return;
    }
    if(!m_isInited)
    {
        std::cout << "ACNovintFalconPlugin " << m_deviceIndex << ": Falcon must be initialized to start" << std::endl;
        return;
    }
    ScopedMutex r(m_runMutex);
    m_runThread = true;
    //Lock; // unused if FLEXT_SYS_PD
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Input thread started" << std::endl;
    //Unlock(); // unused if FLEXT_SYS_PD
    while(m_runThread && m_falconDevice.isOpen())
    {
        nifalcon_update_loop();
#ifdef __WIN32__
        Sleep(sleep_time);
#else
        sleep(sleep_time);
#endif
    }
    //Lock(); // unused if FLEXT_SYS_PD
    std::cout << "ACNovintFalconPlugin "<< m_deviceIndex <<": Input thread exiting" << std::endl;
    //Unlock(); // unused if FLEXT_SYS_PD
}

void ACNovintFalconPlugin::nifalcon_motor_raw(int motor_1, int motor_2, int motor_3)
{
    ScopedMutex s(m_updateMutex);
    if(!m_inRawMode)
    {
        std::cout << "ACNovintFalconPlugin: Falcon in vector force mode, raw input ignored" << std::endl;
        return;
    }
    m_motorRawForce[0] = motor_1;
    m_motorRawForce[1] = motor_2;
    m_motorRawForce[2] = motor_3;

}

void ACNovintFalconPlugin::nifalcon_motor_vector(float x, float y, float z)
{
    ScopedMutex s(m_updateMutex);
    if(m_inRawMode)
    {
        std::cout << "ACNovintFalconPlugin: Falcon in raw mode, vector force input ignored" << std::endl;
        return;
    }
    m_motorVectorForce[0] = x;
    m_motorVectorForce[1] = y;
    m_motorVectorForce[2] = z;
}

void ACNovintFalconPlugin::nifalcon_led(int red, int green, int blue)
{
    ScopedMutex s(m_updateMutex);
    if(red > 0) m_ledState |= FalconFirmware::RED_LED;
    else m_ledState &= ~FalconFirmware::RED_LED;
    if(green > 0) m_ledState |= FalconFirmware::GREEN_LED;
    else m_ledState &= ~FalconFirmware::GREEN_LED;
    if(blue > 0) m_ledState |= FalconFirmware::BLUE_LED;
    else m_ledState &= ~FalconFirmware::BLUE_LED;
}

void ACNovintFalconPlugin::nifalcon_homing_mode(long t)
{
    ScopedMutex s(m_updateMutex);
    m_homingMode = (t > 0);
}

std::vector<ACInputActionQt*> ACNovintFalconPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    return actions;
}
