/**
 * @brief ofx3dModelLoader.cpp
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

#include "ofx3DModelLoader.h"

//-------------------------------------------
static string getExtension( string filename ){
	std::string::size_type idx;
	idx = filename.rfind('.');

	if(idx != std::string::npos){
		return filename.substr(idx+1);
	}
	else{
		return "";
	}
}

//-------------------------------------------
ofx3DModelLoader::ofx3DModelLoader(){
    numRotations = 0;
    rotAngle.clear();
    rotAxis.clear();
    scale = ofPoint(1, 1, 1);
	model = NULL;
}

//------------------------------------------
void ofx3DModelLoader::loadModel(string modelName, float scale){
	if( model != NULL)delete model;
	string ext = getExtension( modelName );
	if( ext == "3ds" || ext == "3DS" ){
		model = new model3DS();
		model->loadModel( modelName.c_str(), scale);
	}
}
void ofx3DModelLoader::loadModel(string modelName, float scale, string textureDiffusePath){
	if( model != NULL)delete model;
	string ext = getExtension( modelName );
	if( ext == "3ds" || ext == "3DS" ){
		model = new model3DS();
		model->loadModel( modelName, scale, textureDiffusePath);
	}
}

//-------------------------------------------
ofx3DModelLoader::~ofx3DModelLoader(){
	if( model != NULL )delete model;
}

//-------------------------------------------
void ofx3DModelLoader::setPosition(float x, float y, float z){
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void ofx3DModelLoader::setScale(float x, float y, float z){
    scale.x = x;
    scale.y = y;
    scale.z = z;
}

//-------------------------------------------
void ofx3DModelLoader::setRotation(int which, float angle, float rot_x, float rot_y, float rot_z){

    if(which + 1 > numRotations){
        int diff = 1 + (which - numRotations);
        for(int i = 0; i < diff; i++){
            rotAngle.push_back(0);
            rotAxis.push_back(ofPoint());
        }
        numRotations = rotAngle.size();
    }

    rotAngle[which]  = angle;
    rotAxis[which].x = rot_x;
    rotAxis[which].y = rot_y;
    rotAxis[which].z = rot_z;
}

//-------------------------------------------
void ofx3DModelLoader::draw(){

    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glScalef(scale.x, scale.y, scale.z);
        for(int i = 0; i < numRotations; i++){
            glRotatef(rotAngle[i], rotAxis[i].x, rotAxis[i].y, rotAxis[i].z);
        }

        glShadeModel(GL_SMOOTH);
		if( model->hasTexture)glEnable(GL_TEXTURE_2D);
            model->draw();
        if( model->hasTexture)glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
