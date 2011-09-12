/*
 *  halEye.cpp
 *  hellohal
 *
 *  @author Julien Leroy
 *  @date 14/03/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "halEye.h"


halEye::halEye() {
    //ctor
    size = 50;
    texCurrent = 0;
    texTarget = 0;
    stareDefined = false;
}

halEye::~halEye() {
    //dtor
}

/*
 void halEye::init(GLUquadricObj *n_quadratic, ofOGLImage* n_mask, ofOGLImage* n_pupille) {
 quadratic = n_quadratic;
 mask = n_mask;
 pupille = n_pupille;
 }
 */

void halEye::init() {
    position.set(0,0,0);
    positionTarget.set(0,0,0);
    rotation.set(0,0,0);
    rotationTarget.set(0,0,0);
    maskRotation.set(0,0,0);
    eyeSize = size * eyeSizeRatio;
    maskTextureRatio = eyeSize/size;
    maskTextureGap = (1.f-maskTextureRatio)*0.25f;
}

void halEye::update() {
    stare();
    if (texCurrent < texTarget) { texCurrent++; }
    else if (texCurrent > texTarget) { texCurrent--; }
    if (position.x != positionTarget.x) { position.x = interpolate(position.x, positionTarget.x,0.05f,false); }
    if (position.y != positionTarget.y) { position.y = interpolate(position.y, positionTarget.y,0.05f,false); }
    if (position.z != positionTarget.z) { position.z = interpolate(position.z, positionTarget.z,0.05f,false); }
    if (rotation.x != rotationTarget.x) { rotation.x = interpolate(rotation.x, rotationTarget.x,0.6f,false); }
    if (rotation.y != rotationTarget.y) { rotation.y = interpolate(rotation.y, rotationTarget.y,0.6f,false); }
    if (rotation.z != rotationTarget.z) { rotation.z = interpolate(rotation.z, rotationTarget.z,0.6f,false); }
}

float halEye::interpolate(float v1, float v2, float speed, bool isAngle) {
    if (v1 != v2) {
        if (!isAngle) {
            v1 += (v2-v1)*speed;
            if (abs(v1 - v2) < 0.001f) { v1 = v2; }
        } else {
            float a1 = v1*3.14f/180;
            float a2 = v2*3.14f/180;
            float c1 = cosf(a1);
            float s1 = sinf(a1);
            float c2 = cosf(a2);
            float s2 = sinf(a2);
            v1 = atan2f( (s2-s1)*speed, (c2-c1)*speed );
            if (abs(c2-c1) < 0.001f && abs(s2-s1) < 0.001f) { v1 = v2; }
        }
    }
    return v1;
}

void halEye::stareAt(ofPoint* n_starePoint) {
    starePoint = n_starePoint;
    stareDefined = true;
}

void halEye::stare() {
    if (stareDefined) {
        // glTranslatef(0, cosf(testAxis)*20, 0);
        float zangl = atan2f(position.z - starePoint->z, position.x - starePoint->x);
        rotationTarget.z = (-zangl*180.f/3.14f);
        // glRotatef((-zangl*180.f/3.14f),  0, 0, 1);
        // glTranslatef(0, 0, cosf(testAxis)*20);
        // glRotatef(testAxis,  0, 1, 0);
        float xangl = atan2f(
							 -starePoint->y + position.y,
                             sqrtf(
                                   (starePoint->x - position.x)*(starePoint->x - position.x)
                                   +
                                   (starePoint->z - position.z)*(starePoint->z - position.z)
                                   )
                             );
        rotationTarget.x = (xangl*180.f/3.14f);
        // glRotatef((yangl*180.f/3.14f),  1, 0, 0);
    }
}

float halEye::rotateZMask(ofPoint* cameraPoint) {
	return atan2f(position.z - cameraPoint->z, position.x - cameraPoint->x)*180.f/3.14f*-1.f;
}
