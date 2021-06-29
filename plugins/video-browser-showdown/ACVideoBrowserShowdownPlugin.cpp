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

#include "ACVideoBrowserShowdownPlugin.h"
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

#include <curl/curl.h>

using namespace std;

ACVideoBrowserShowdownPlugin::ACVideoBrowserShowdownPlugin() : QObject(), ACPluginQt(), ACClientPlugin(){
    this->mName = "Video Browser Showdown";
    this->mDescription ="Plugin for contesting in the video browser showdown: Contour Design Shuttle Pro 2 jogwheel bound to the video timeline thru HIDAPI, cue in/out and submit the target segment";
    this->mMediaType = MEDIA_TYPE_VIDEO;

    this->button01 = false;
    this->button02 = false;
    this->button03 = false;
    this->button04 = false;
    this->button05 = false;
    this->button06 = false;
    this->button07 = false;
    this->button08 = false;
    this->button09 = false;
    this->button10 = false;
    this->button11 = false;
    this->button12 = false;
    this->button13 = false;
    this->button14 = false;
    this->button15 = false;

    this->reset_changed_booleans();

    this->jog = 0;
    this->wheel = 0;
    this->jog_changed = false;
    this->jog_clockwise = true;
    this->wheel_changed = false;

    this->active = false;
    this->handle = 0;

    QString device("Contour Design Shuttle Pro2");
    cueInAction = new ACInputActionQt(tr("Cue in the beginning of the segment"), this);
    cueInAction->setToolTip(tr("Cue in the beginning of the segment"));
    cueInAction->setShortcut(Qt::LeftArrow);
    cueInAction->setKeyEventType(QEvent::KeyPress);
    cueInAction->setDeviceName(device);
    cueInAction->setDeviceEvent("button 10 pressed");

    cueOutAction = new ACInputActionQt(tr("Cue out the end of the segment"), this);
    cueOutAction->setToolTip(tr("Cue out the end of the segment"));
    cueOutAction->setShortcut(Qt::RightArrow);
    cueOutAction->setKeyEventType(QEvent::KeyPress);
    cueOutAction->setDeviceName(device);
    cueOutAction->setDeviceEvent("button 11 pressed");

    submitAction = new ACInputActionQt(tr("Submit the segment"), this);
    submitAction->setToolTip(tr("Submit the segment"));
    submitAction->setShortcut(Qt::Key_S);
    submitAction->setKeyEventType(QEvent::KeyPress);
    submitAction->setDeviceName(device);
    submitAction->setDeviceEvent("button 09 pressed");

    adjustSpeedAction = new ACInputActionQt(tr("Adjust the playback speed"), this);
    adjustSpeedAction->setToolTip(tr("Adjust the playback speed"));
    adjustSpeedAction->setDeviceName(device);
    adjustSpeedAction->setDeviceEvent("wheel moved");

    skipFrameAction = new ACInputActionQt(tr("Skip frame(s)"), this);
    skipFrameAction->setToolTip(tr("Skip frame(s)"));
    skipFrameAction->setDeviceName(device);
    skipFrameAction->setDeviceEvent("jog moved");

    togglePlaybackAction = new ACInputActionQt(tr("Toggle playback"), this);
    togglePlaybackAction->setToolTip(tr("Toggle playback"));
    togglePlaybackAction->setShortcut(Qt::Key_Space);
    togglePlaybackAction->setKeyEventType(QEvent::KeyPress);
    togglePlaybackAction->setDeviceName(device);
    togglePlaybackAction->setDeviceEvent("button 13 pressed");

    cue_in = 0;
    cue_out = 0;
    url = "10.0.0.7";
    port = 8080;
    team = 5;

    this->addStringParameter("Server",this->url,std::vector<std::string>(),"Server",boost::bind(&ACVideoBrowserShowdownPlugin::changeServer,this));
    this->addNumberParameter("Port",this->port,1,65535,1,"Port",boost::bind(&ACVideoBrowserShowdownPlugin::changePort,this));
    this->addNumberParameter("Team",this->team,0,16,1,"Team",boost::bind(&ACVideoBrowserShowdownPlugin::changeTeam,this));

    this->init_hid();
}

void ACVideoBrowserShowdownPlugin::changeServer(){
    this->url = this->getStringParameterValue("Server");
    std::cout << "ACVideoBrowserShowdownPlugin::changeServer: " << this->url << std::endl;
}

void ACVideoBrowserShowdownPlugin::changePort(){
    this->port = this->getNumberParameterValue("Port");
    std::cout << "ACVideoBrowserShowdownPlugin::changePort: " << this->port << std::endl;
}

void ACVideoBrowserShowdownPlugin::changeTeam(){
    this->team = this->getNumberParameterValue("Team");
    std::cout << "ACVideoBrowserShowdownPlugin::changeTeam: " << this->team << std::endl;
}

bool ACVideoBrowserShowdownPlugin::init_hid(){

    struct hid_device_info *devs, *cur_dev;
    if (hid_init())
        return -1;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;

    while (cur_dev) {
        //        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        //        printf("\n");
        //        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        //        printf("  Product:      %ls\n", cur_dev->product_string);
        //        printf("  Release:      %hx\n", cur_dev->release_number);
        //        printf("  Interface:    %d\n",  cur_dev->interface_number);
        //        printf("\n");

        if(cur_dev->vendor_id != 0x0b33 && cur_dev->product_id != 0x0030)
            cur_dev = cur_dev->next;
        else
            break;
    }

    //hid_free_enumeration(devs);

    if(!cur_dev){
        std::cerr << "Couldn't find a Shuttle Pro2 device, aborting." << std::endl;
        return 0;
    }

    // Open the device using the VID, PID,
    // and optionally the Serial number.
    handle = hid_open(0x0b33,0x0030, NULL);
    if (!handle) {
        printf("unable to open device\n");
        hid_free_enumeration(devs);
        return 1;
    }

    return this->StartInternalThread();
}

ACVideoBrowserShowdownPlugin::~ACVideoBrowserShowdownPlugin(){
    this->active = false;
    this->WaitForInternalThreadToExit();
    hid_close(handle);

    /* Free static HIDAPI objects. */
    hid_exit();

#ifdef WIN32
    system("pause");
#endif
}

void ACVideoBrowserShowdownPlugin::hid_loop(){
    std::cout << "hid loop" << std::endl;
    int res = 0;
    unsigned char buf[256];
    int i = 0;
    while(this->active){
        res = hid_read_timeout(handle, buf, sizeof(buf),10 /*msec*/);
        if (res > 0){
            //            printf("Data read:   ");
            //            for (i = 0; i < res; i++)
            //                printf("%02hhx ", buf[i]);
            //            printf("\n");

            if(res == 5){
                if(wheel != (int) (buf[0]+8)%16-8) wheel_changed = true; wheel = (int) (buf[0]+8)%16-8;
                if(jog != (int) buf[1]) {
                    jog_clockwise = ( (float)((int) buf[1]-jog)/255.0f > 0 ? true : false);
                    jog_changed = true;
                    jog = (int) buf[1];
                }
                if(button01 != ((int) (buf[3] & 0x01) != 0)) button01_changed = true; button01 = ((int) (buf[3] & 0x01) != 0);
                if(button02 != ((int) (buf[3] & 0x02) != 0)) button02_changed = true; button02 = ((int) (buf[3] & 0x02) != 0);
                if(button03 != ((int) (buf[3] & 0x04) != 0)) button03_changed = true; button03 = ((int) (buf[3] & 0x04) != 0);
                if(button04 != ((int) (buf[3] & 0x08) != 0)) button04_changed = true; button04 = ((int) (buf[3] & 0x08) != 0);
                if(button05 != ((int) (buf[3] & 0x10) != 0)) button05_changed = true; button05 = ((int) (buf[3] & 0x10) != 0);
                if(button06 != ((int) (buf[3] & 0x20) != 0)) button06_changed = true; button06 = ((int) (buf[3] & 0x20) != 0);
                if(button07 != ((int) (buf[3] & 0x40) != 0)) button07_changed = true; button07 = ((int) (buf[3] & 0x40) != 0);
                if(button08 != ((int) (buf[3] & 0x80) != 0)) button08_changed = true; button08 = ((int) (buf[3] & 0x80) != 0);
                if(button09 != ((int) (buf[4] & 0x01) != 0)) button09_changed = true; button09 = ((int) (buf[4] & 0x01) != 0);
                if(button10 != ((int) (buf[4] & 0x20) != 0)) button10_changed = true; button10 = ((int) (buf[4] & 0x20) != 0);
                if(button11 != ((int) (buf[4] & 0x40) != 0)) button11_changed = true; button11 = ((int) (buf[4] & 0x40) != 0);
                if(button12 != ((int) (buf[4] & 0x02) != 0)) button12_changed = true; button12 = ((int) (buf[4] & 0x02) != 0);
                if(button13 != ((int) (buf[4] & 0x04) != 0)) button13_changed = true; button13 = ((int) (buf[4] & 0x04) != 0);
                if(button14 != ((int) (buf[4] & 0x08) != 0)) button14_changed = true; button14 = ((int) (buf[4] & 0x08) != 0);
                if(button15 != ((int) (buf[4] & 0x10) != 0)) button15_changed = true; button15 = ((int) (buf[4] & 0x10) != 0);

                //                this->print_ascii_status();

                /// Allowing submission from adjacent buttons, for realtime craze
                if(button04 && button04_changed) this->submitFastCallback();
                if(button08 && button08_changed) this->submitFastCallback();
                if(button09 && button09_changed) this->submitFastCallback();

                if(button01 && button01_changed) this->submitFastCallback();
                if(button05 && button05_changed) this->submitFastCallback();
                if(button06 && button06_changed) this->submitFastCallback();

                if(button10 && button10_changed) this->cueInCallback();
                if(button11 && button11_changed) this->cueOutCallback();
                if(jog_changed) this->adjustSelectionWidth();
                /*if(wheel_changed)*/ this->adjustSpeedCallback();

                if(button12 && button12_changed) this->skipToPreviousSegment();
                if(button13 && button13_changed) this->skipToNextSegment();
                if(button14 && button14_changed) this->skipToPreviousSegment();
                if(button15 && button15_changed) this->skipToNextSegment();
            }
            this->reset_changed_booleans();
        }
    }
}

void ACVideoBrowserShowdownPlugin::print_ascii_status(){
    std::cout << " Wheel " << wheel;
    std::cout << " Jog " << jog;
    std::cout << std::endl;
    std::cout << " " << button01 << " " << button02 << " " << button03 << " " << button04 << std::endl;
    std::cout << button05 << " " << button06 << " " << button07 << " " << button08 << " " << button09 << std::endl;
    std::cout << button10 << "       " << button11 << std::endl;
    std::cout << " " << button12 << "     " << button13 << std::endl;
    std::cout << " " << button14 << "     " << button15 << std::endl;
}

void ACVideoBrowserShowdownPlugin::cueInCallback(){
    std::cout << "cueInCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueInCallback: mediacycle not set" << std::endl;
        return;
    }
    if(!timeline_renderer){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueInCallback: timeline renderer not set" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0) == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueInCallback: couldn't access the video track" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0)->getMedia() == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueInCallback: couldn't access the video track media" << std::endl;
        return;
    }
    float duration = timeline_renderer->getTrack(0)->getMedia()->getDuration();
    float current_pos = timeline_renderer->getTrack(0)->getSelectionPosX();
	float fps = (timeline_renderer->getTrack(0)->getMedia()->getFrameRate());
    cue_in = (int)((float)duration*(float)current_pos*(float)fps);
    std::cout << "ACVideoBrowserShowdownPlugin::cueInCallback: frame " << cue_in << " at current pos " << current_pos << " ([0;1]) / " << duration << " (total duration) "<< fps << std::endl;
}

void ACVideoBrowserShowdownPlugin::cueOutCallback(){
    std::cout << "cueOutCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueOutCallback: mediacycle not set" << std::endl;
        return;
    }
    if(!timeline_renderer){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueOutCallback: timeline renderer not set" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0) == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueOutCallback: couldn't access the video track" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0)->getMedia() == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::cueOutCallback: couldn't access the video track media" << std::endl;
        return;
    }
    float duration = timeline_renderer->getTrack(0)->getMedia()->getDuration();
	float current_pos = timeline_renderer->getTrack(0)->getSelectionPosX();
    float fps = (timeline_renderer->getTrack(0)->getMedia()->getFrameRate());
    cue_out = (int)((float)duration*(float)current_pos*(float)fps);
    std::cout << "ACVideoBrowserShowdownPlugin::cueOutCallback: frame " << cue_out << " at current pos " << current_pos << " ([0;1]) / " << duration << " (total duration) " << fps << std::endl;
}

void ACVideoBrowserShowdownPlugin::submitCallback(){
    std::cout << "ACVideoBrowserShowdownPlugin::submitCallback" << std::endl;

    if(!timeline_renderer){
        std::cerr << "ACVideoBrowserShowdownPlugin::submitCallback: timeline renderer not set" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0) == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::submitCallback: couldn't access the video track" << std::endl;
        return;
    }
    if(timeline_renderer->getTrack(0)->getMedia() == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::submitCallback: couldn't access the video track media" << std::endl;
        return;
    }

    std::stringstream command;
    command << url << ":" << port << "/team=" << team << "/video=";

    std::stringstream basename;
    boost::filesystem::path media_path(timeline_renderer->getTrack(0)->getMedia()->getFileName().c_str());
    basename << media_path.stem().string();
    std::string numbers = basename.str();
    command << numbers.substr(0,3);

    command << "/segstart=" << cue_in << "/segstop=" << cue_out;

    std::cout << "ACVideoBrowserShowdownPlugin::submitCallback: sending HTTP request: '" << command.str() << "'"<< std::endl;

    // Put this inside a thread, blocks the plugin if the server can't be reached
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, command.str().c_str());

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

}

void ACVideoBrowserShowdownPlugin::submitFastCallback(){
    this->cueInCallback();
    this->cueOutCallback();
    this->submitCallback();
}

void ACVideoBrowserShowdownPlugin::adjustSpeedCallback(){
    if(this->media_cycle == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback: mediacycle not set" << std::endl;
        return;
    }
    if(this->media_cycle->getLibrarySize() == 0){
        //std::cerr << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback: no media element loaded" << std::endl;
        return;
    }
    if(this->timeline_renderer == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback: couldn't access timeline" << std::endl;
        return;
    }
    if(this->timeline_renderer->getTrack(0) == 0){
        //std::cerr << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback: couldn't access timeline track" << std::endl;
        return;
    }
    if(this->timeline_renderer->getTrack(0)->getSharedThumbnailStream() == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback: couldn't access timeline track shared thumbnail stream" << std::endl;
        return;
    }
    int tm = 1;
    if(this->wheel > 0)
        tm = this->wheel+1;
    else if(this->wheel < 0)
        tm = this->wheel;
    if(this->wheel!=0)
        std::cout << "ACVideoBrowserShowdownPlugin::adjustSpeedCallback " << this->wheel << " -> " << tm << std::endl;
    if(this->wheel!=0){
        float current_time = this->timeline_renderer->getTrack(0)->getSharedThumbnailStream()->getCurrentTime();
        this->timeline_renderer->getTrack(0)->getSharedThumbnailStream()->seek(current_time+tm);
    }
}

void ACVideoBrowserShowdownPlugin::adjustSelectionWidth(){
    if(this->timeline_renderer == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::adjustSelectionWidth: couldn't access timeline" << std::endl;
        return;
    }
    if(this->timeline_renderer->getTrack(0) == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::adjustSelectionWidth: couldn't access timeline track" << std::endl;
        return;
    }
    float selection_width = this->timeline_renderer->getTrack(0)->getSelectionWidth();
    float ratio = 0.25f*(selection_width);
    float new_selection_width = selection_width + (this->jog_clockwise ? 1 : -1) * ratio;

    std::cout << "ACVideoBrowserShowdownPlugin::adjustSelectionWidth from " << selection_width << " to " << new_selection_width << std::endl;
    this->timeline_renderer->getTrack(0)->resizeSelectionWidth(new_selection_width);
}

void ACVideoBrowserShowdownPlugin::skipFrameCallback(){
    std::cout << "ACVideoBrowserShowdownPlugin::skipFrameCallback " << this->jog << std::endl;
}

void ACVideoBrowserShowdownPlugin::togglePlaybackCallback(){
    std::cout << "ACVideoBrowserShowdownPlugin::togglePlaybackCallback " << std::endl;
}

void ACVideoBrowserShowdownPlugin::skipToNextSegment(){
   if(this->timeline_renderer == 0){
       std::cerr << "ACVideoBrowserShowdownPlugin::skipToNextSegment: couldn't access timeline" << std::endl;
       return;
   }
   if(this->timeline_renderer->getTrack(0) == 0){
       std::cerr << "ACVideoBrowserShowdownPlugin::skipToNextSegment: couldn't access timeline track" << std::endl;
       return;
   }
   this->timeline_renderer->getTrack(0)->skipToNextSegment();
}

void ACVideoBrowserShowdownPlugin::skipToPreviousSegment(){
    if(this->timeline_renderer == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::skipToPreviousSegment: couldn't access timeline" << std::endl;
        return;
    }
    if(this->timeline_renderer->getTrack(0) == 0){
        std::cerr << "ACVideoBrowserShowdownPlugin::skipToPreviousSegment: couldn't access timeline track" << std::endl;
        return;
    }
    this->timeline_renderer->getTrack(0)->skipToPreviousSegment();
}

std::vector<ACInputActionQt*> ACVideoBrowserShowdownPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    actions.push_back(cueInAction);
    actions.push_back(cueOutAction);
    actions.push_back(submitAction);
    actions.push_back(adjustSpeedAction);
    actions.push_back(skipFrameAction);
    return actions;
}

void ACVideoBrowserShowdownPlugin::reset_changed_booleans(){
    this->button01_changed = false;
    this->button02_changed = false;
    this->button03_changed = false;
    this->button04_changed = false;
    this->button05_changed = false;
    this->button06_changed = false;
    this->button07_changed = false;
    this->button08_changed = false;
    this->button09_changed = false;
    this->button10_changed = false;
    this->button11_changed = false;
    this->button12_changed = false;
    this->button13_changed = false;
    this->button14_changed = false;
    this->button15_changed = false;

    this->jog_changed = false;
    this->wheel_changed = false;
}
