/**
 * @brief testApp.h
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

#ifndef _TEST_APP
#define _TEST_APP

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "ofMain.h"

#include "ofOGLImage.h"
#include "halEye.h"

#include "ofxOsc.h"

#define HOST "localhost"
#define PORTS 12000 //port d'envoi
#define PORTR 9110	//port de reception

#include "ofx3D/ofx3DModelLoader.h"

class testApp : public ofBaseApp {
	
public:
	void setup();
	void buildGrid();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2);
	void DrawSkel();
	void DrawSphere();
	
	void DrawFloor();
	void DrawRoom();
	void UpdateUserData();
	void drawPointCloud();
	
	float referenceSize;
	
	ofPoint halGridOffset;
	float halGridWidth;
	float halGridHeight;
	float halGridDepth;
	int halNumCols;
	int halNumRows;
	int halEyeSize;
	float halEyeRatio;
	
	float halGridXsegment; // x angle (from *-1 to * 1) for grid deformation
	float halGridYsegment; // y angle (from *-1 to * 1) for grid deformation
	
	
	int eyesCount;
	
	vector<ofOGLImage> images;
	ofOGLImage borderImage;
	ofOGLImage glassImage;
	ofOGLImage* pupilles;
	
	GLUquadricObj *quadratic;   // Storage For Our Quadratic Object
	ofx3DModelLoader model;
	
	ofPoint kinectpos;
	ofPoint campos;
	ofPoint camstarepos;
	float camanglz;
	float camdistz;
	
	float testAxis;
	
	
	float stareDistanceMultiplier;
	bool stareAtCamera;
	bool headTracking;
	
	bool cameraForConfiguration;
	int action;
	
	ofxOscSender sender;
	ofxOscReceiver	receiver;
	int userToTrack;
	

};

#endif
