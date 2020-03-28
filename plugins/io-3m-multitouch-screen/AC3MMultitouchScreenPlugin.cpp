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

#include "AC3MMultitouchScreenPlugin.h"
#include<MediaCycle.h>

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

using namespace std;

AC3MMultitouchScreenPlugin::AC3MMultitouchScreenPlugin() : QObject(), ACPluginQt(), ACClientPlugin(){
    this->mName = "Multitouch input with 3M MicroTouch Display C1968PW";
    this->mDescription ="Plugin for using the 3M MicroTouch Display C1968PW screen as multitouch input";
    this->mMediaType = MEDIA_TYPE_ALL;

    this->active = false;
    this->handle = 0;

    number_of_fingers = 0;

    this->init_hid();
}

bool AC3MMultitouchScreenPlugin::init_hid(){

    struct hid_device_info *devs, *cur_dev;
    if (hid_init())
        return -1;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;

    std::string cur_path("");

    while (cur_dev) {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("\n");

        if(cur_dev->vendor_id != 0x0596 && cur_dev->product_id != 0x0500 && cur_dev->release_number != 0x106)
            cur_dev = cur_dev->next;
        else{
            cur_path = std::string(cur_dev->path);
            std::cout << "AC3MMultitouchScreenPlugin: using path " << cur_path << std::endl;
            break;
        }
    }

    //hid_free_enumeration(devs);

    if(!cur_dev){
        std::cerr << "AC3MMultitouchScreenPlugin: couldn't find a 3M MicroTouch Display C1968PW device, aborting." << std::endl;
        return 0;
    }

    if(cur_path == ""){
        std::cerr << "AC3MMultitouchScreenPlugin: wrong 3M MicroTouch Display C1968PW subdevice, aborting." << std::endl;
        return 0;
    }

    if(cur_path != "USB_0596_0500_6200000"){
        //std::cerr << "AC3MMultitouchScreenPlugin: wrong 3M MicroTouch Display C1968PW subdevice, aborting." << std::endl;
        //return 0;
        std::cerr << "AC3MMultitouchScreenPlugin: wrong 3M MicroTouch Display C1968PW subdevice path " << cur_path << ", adjusting to USB_0596_0500_6200000" << std::endl;
        cur_path = "USB_0596_0500_6200000";
    }

    // Open the device using the VID, PID,
    // and optionally the Serial number.
    //handle = hid_open(0x0596,0x0500, NULL);
    // Open the device using its path, since subdevices can't be differenciated with their empty serial numbers
    handle = hid_open_path(cur_path.c_str());
    if (!handle) {
        printf("AC3MMultitouchScreenPlugin: unable to open device\n");
        hid_free_enumeration(devs);
        return 1;
    }

    return this->StartInternalThread();
}

AC3MMultitouchScreenPlugin::~AC3MMultitouchScreenPlugin(){
    this->active = false;
    this->WaitForInternalThreadToExit();
    hid_close(handle);

    /* Free static HIDAPI objects. */
    hid_exit();

#ifdef WIN32
    system("pause");
#endif
}

void AC3MMultitouchScreenPlugin::hid_loop(){
    std::cout << "hid loop" << std::endl;
    int res = 0;
    unsigned char buf[256];
    int i = 0;
    while(this->active){
        res = hid_read_timeout(handle, buf, sizeof(buf),10 /*msec*/);
        if (res > 0){
            //                        printf("Data read:   ");
            //                        for (i = 0; i < res; i++)
            //                            printf("%02hhx ", buf[i]);
            //                        printf("\n");

            if(res == 62){
                number_of_fingers = (int)buf[61];
                std::cout << "AC3MMultitouchScreenPlugin: number of fingers " << number_of_fingers << std::endl;

                // Building the list of new ids
                std::vector<int> new_ids;
                for(int finger = 1; finger<=number_of_fingers;finger++){
                    new_ids.push_back((int)buf[2+6*(finger-1)]);
                }

                // Comparing with the list of former ids, deleting pointers when no match with new ids
                for(std::map<int,ACPoint>::iterator pointer = pointers.begin(); pointer != pointers.end(); pointer++){
                    bool keep_id = false;
                    for(std::vector<int>::iterator new_id = new_ids.begin(); new_id != new_ids.end(); new_id++){
                        if(pointer->first == *new_id)
                            keep_id = true;
                    }
                    if(!keep_id){
                        std::cout << "Removing pointer of id " << pointer->first << std::endl;
                        if(media_cycle){
                            media_cycle->removePointer(pointer->first);
                        }
                        pointers.erase(pointer);
                    }
                }

                if(media_cycle)
                    media_cycle->setAutoPlay(true);

                // Updating the pointers coordinates
                std::cout << "AC3MMultitouchScreenPlugin: ";
                for(int finger = 1; finger<=number_of_fingers;finger++){
                    std::cout << " finger " << finger;
                    int id = (int)buf[2+6*(finger-1)];
                    if(buf[1+6*(finger-1)] == 0x07){
                        std::cout << " touched";
                        if(media_cycle){
                            media_cycle->addPointer(id);
                        }
                    }
                    else if(buf[1+6*(finger-1)] == 0x04){
                        std::cout << " released";
                        if(media_cycle){
                            media_cycle->removePointer(id);
                        }
                    }
                    //std::cout << " id " << (int)buf[2+6*(finger-1)] << " values " << (int)buf[3+6*(finger-1)] << " " << (int)buf[4+6*(finger-1)] << " " << (int)buf[5+6*(finger-1)] << " " << (int)buf[6+6*(finger-1)] << " ";
                    std::cout << " id " << (int)buf[2+6*(finger-1)];
                    //                    std::cout << " x " << (int)buf[3+6*(finger-1)] + 128*(int)buf[4+6*(finger-1)] ;
                    //                    std::cout << " y " << (int)buf[5+6*(finger-1)] + 128*(int)buf[6+6*(finger-1)] ;
                    std::cout << " x " << (float)((int)buf[3+6*(finger-1)] + 128*(int)buf[4+6*(finger-1)])/(float)(128+128*128);
                    std::cout << " y " << (float)((int)buf[5+6*(finger-1)] + 128*(int)buf[6+6*(finger-1)])/(float)(128+128*128);

                    ACPoint p;
                    p.x = (float)((int)buf[3+6*(finger-1)] + 128*(int)buf[4+6*(finger-1)])/(float)(128+128*128);
                    p.y = (float)((int)buf[5+6*(finger-1)] + 128*(int)buf[6+6*(finger-1)])/(float)(128+128*128);
                    pointers[id] = p;
                    if(media_cycle)
                        media_cycle->hoverWithPointerId(2.0f*(p.x-0.5f),2.0f*(0.5f-p.y),id);
                }
                std::cout << std::endl;

                // Removing all pointers if all have been released
                bool clear_pointers = true;
                for(int finger = 1; finger<=number_of_fingers;finger++){
                    if(buf[1+6*(finger-1)] == 0x07){
                        clear_pointers = false;
                        break;
                    }
                }
                if(clear_pointers){
                    std::cout << "AC3MMultitouchScreenPlugin: releasing all pointers" << std::endl;
                    pointers.clear();
                    for(int finger = 1; finger<=number_of_fingers;finger++){
                        int id = (int)buf[2+6*(finger-1)];
                        if(media_cycle){
                            media_cycle->removePointer(id);
                        }
                    }
                    if(media_cycle)
                        media_cycle->setAutoPlay(false);
                }
            }
        }
    }
}

void AC3MMultitouchScreenPlugin::print_ascii_status(){

}


std::vector<ACInputActionQt*> AC3MMultitouchScreenPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    return actions;
}
