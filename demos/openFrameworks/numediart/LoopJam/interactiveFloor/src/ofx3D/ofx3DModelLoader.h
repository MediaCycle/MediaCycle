/**
 * @brief ofx3DModelLoader.h
 * @author Christian Frisson
 * @date 12/09/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#pragma once

#include "ofMain.h"
#include "3DS/model3DS.h"

class ofx3DModelLoader{

    public:
    ofx3DModelLoader();
	~ofx3DModelLoader();

    void loadModel(string modelName, float scale = 1.0);
    void loadModel(string modelName, float scale, string textureDiffusePath);
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setRotation(int which, float angle, float rot_x, float rot_y, float r_z);
    void draw();

    vector <float> rotAngle;
    vector <ofPoint> rotAxis;
    ofPoint scale;
    ofPoint pos;

    int numRotations;
    ofx3DBaseLoader * model;

};
