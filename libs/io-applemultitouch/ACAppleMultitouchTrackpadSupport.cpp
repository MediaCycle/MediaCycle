/**
 * @brief ACAppleMultitouchTrackpadSupport.cpp
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

#include "ACAppleMultitouchTrackpadSupport.h"

static int _callback(int device, Finger *data, int nFingers, double timestamp, int frame) {

    std::vector<Finger*> fingers;
	fingers.resize(nFingers);
	
	 for (int i=0; i<nFingers; i++) {
	/*	 
	 Finger *f = &data[i];
	 
	 printf("Frame %7d: Angle %6.2f, ellipse %6.3f x%6.3f; "
	 "position (%6.3f,%6.3f) vel (%6.3f,%6.3f) "
	 "ID %d, state %d [%d %d?] size %6.3f, %6.3f?\n",
	 f->frame,
	 f->angle * 90 / atan2(1,0),
	 f->majorAxis,
	 f->minorAxis,
	 f->normalized.pos.x,
	 f->normalized.pos.y,
	 f->normalized.vel.x,
	 f->normalized.vel.y,
	 f->identifier, f->state, f->foo3, f->foo4,
	 f->size, f->unk2);
	 }
	 printf("\n");
	 */
	 fingers[i] = &data[i];
	 }	 
	
	for (std::vector< ACAppleMultitouchTrackpadSupport* >::iterator instance = instances.begin(); instance < instances.end(); instance++) 
	{	
        (*instance)->callback(device, data, nFingers, timestamp, frame);
	}	
	return 0;
}

void ACAppleMultitouchTrackpadSupport::start()
{	
	if (instances.empty()) {
		dev = MTDeviceCreateDefault();
        MTRegisterContactFrameCallback(dev, _callback);
		MTDeviceStart(dev, 0);
	}
	if (find(instances.begin(), instances.end(), this) == instances.end())
		instances.push_back(this);
}

void ACAppleMultitouchTrackpadSupport::stop()
{
	std::vector<ACAppleMultitouchTrackpadSupport*>::iterator it = find(instances.begin(), instances.end(), this);
	if (it != instances.end())
	instances.erase(it);
	assert(find(instances.begin(), instances.end(), this) == instances.end());
	if (instances.empty()) {
        MTUnregisterContactFrameCallback(dev, _callback);
		MTDeviceStop(dev);
		MTDeviceRelease(dev);
		dev = 0;
	}
}
