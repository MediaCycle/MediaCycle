/**
 * @brief Plugin for toggling autoplay on any finger touch with Apple Trackpads
 * @author Christian Frisson
 * @date 15/10/2013
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

#include "ACAppleMultitouchTrackpadAutoPlayPlugin.h"
#include<MediaCycle.h>

using namespace std;

ACAppleMultitouchTrackpadAutoPlayPlugin::ACAppleMultitouchTrackpadAutoPlayPlugin() : QObject(), ACPluginQt(), ACClientPlugin(), ACAppleMultitouchTrackpadSupport(){
    this->mName = "Autoplay plugin with Apple Trackpads";
    this->mDescription ="Plugin for toggling autoplay on any finger touch with Apple Trackpads";
    this->mMediaType = MEDIA_TYPE_ALL;

    this->start();
}


ACAppleMultitouchTrackpadAutoPlayPlugin::~ACAppleMultitouchTrackpadAutoPlayPlugin(){
    this->stop();
}

int ACAppleMultitouchTrackpadAutoPlayPlugin::callback(int device, Finger *data, int nFingers, double timestamp, int frame)
{
    if(nFingers==0){
        //std::cout << "Fingers left "<< std::endl;
        if(this->media_cycle) media_cycle->setAutoPlay(0);
    }
    else if(nFingers==1){
        //std::cout << nFingers << " fingers" << std::endl;
        if(this->media_cycle) media_cycle->setAutoPlay(1);
    }

    return 0;
}

std::vector<ACInputActionQt*> ACAppleMultitouchTrackpadAutoPlayPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    return actions;
}
