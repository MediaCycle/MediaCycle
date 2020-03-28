/*
 *  ACAppleMultitouchTrackpadSupport.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 26/03/10
 *  Based on Steike's hack.
 *  http://www.steike.com/code/multitouch/
 *
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef _AC_APPLE_MULTITOUCH_TRACKPAD_SUPPORT_H_
#define _AC_APPLE_MULTITOUCH_TRACKPAD_SUPPORT_H_

#include <math.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include <mach/mach.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { float x,y; } mtPoint;
typedef struct { mtPoint pos,vel; } mtReadout;

typedef struct {
    int frame;
    double timestamp;
    int identifier, state, foo3, foo4;
    mtReadout normalized;
    float size;
    int zero1;
    float angle, majorAxis, minorAxis; // ellipsoid
    mtReadout mm;
    int zero2[2];
    float unk2;
} Finger;

typedef void* MTDeviceRef;
typedef int (*MTContactCallbackFunction)(int,Finger*,int,double,int);

CFArrayRef MTDeviceCreateList();
MTDeviceRef MTDeviceCreateDefault();
MTDeviceRef MTDeviceCreateFromDeviceID(int64_t);
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTUnregisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int);
void MTDeviceStop(MTDeviceRef);
void MTDeviceRelease(MTDeviceRef);

#ifdef __cplusplus
}
#endif

typedef void* ACAppleMultitouchTrackpadSupportRef;

static MTDeviceRef dev;

class ACAppleMultitouchTrackpadSupport {
public:
    ACAppleMultitouchTrackpadSupport(){}
    ~ACAppleMultitouchTrackpadSupport(){}
    void start();
    void stop();
    virtual int callback(int device, Finger *data, int nFingers, double timestamp, int frame){return 0;}
};

static std::vector<ACAppleMultitouchTrackpadSupport*> instances;

#endif /* _AC_APPLE_MULTITOUCH_TRACKPAD_SUPPORT_H_ */
