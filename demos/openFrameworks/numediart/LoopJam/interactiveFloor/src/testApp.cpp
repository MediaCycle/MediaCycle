/**
 * @brief testApp.cpp
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

#include "testApp.h"

#include <iostream>
#include <sstream> // for int -> string conversion
#include <cmath>
#include <algo.h>
#define PI 3.14159265

#include "UserTrackInfo.h"

using namespace std;
//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::ImageGenerator g_ImageGenerator;
xn::UserGenerator g_UserGenerator;
XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;
XnBool g_bPause = false;
XnBool g_bRecord = false;
XnBool g_bQuit = false;

bool drawcloud=true;

XnPoint3D pt;
XnPoint3D handPos;

XnUserID g_nPlayer = 0;
XnBool g_bCalibrated = FALSE;
bool cal=false;

GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0};
GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0};

//KINECT GESTURE CALLBACK
//#define SAMPLE_XML_PATH "@executable_path/../MacOS/SamplesConfig.xml"
#define SAMPLE_XML_PATH "SamplesConfig.xml"

//KINECT CALLBACK


void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
 	
	
	if (cal==false) {
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
	if (cal==true) {
		g_UserGenerator.GetSkeletonCap().LoadCalibrationData(nId, 0);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}

}
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
//	printf("Lost user %d\n", nId);
	
	//CF
	string IP="localhost";
	const char* newIP=IP.c_str();
	ofxOscSender sender;
	sender.setup(newIP, 12345);
	
	ofxOscMessage m;
	std::stringstream tmp_i;
	tmp_i << nId;
	string adres = "/mediacycle/browser/" + tmp_i.str() + "/released" ;
	cout << adres << endl;
	m.setAddress(adres);
	sender.sendMessage( m );
}
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	printf("Pose %s detected for user %d\n", strPose, nId);
	
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	printf("Calibration started for user %d\n", nId);
}
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
		g_UserGenerator.GetSkeletonCap().SaveCalibrationData(nId, 0);
		cal=true;
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);

		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}
//--------------------------------------------------------------
void testApp::setup(){
	
#ifdef TARGET_OSX      
    // Get the absolute location of the executable file in the bundle.  
    CFBundleRef appBundle     = CFBundleGetMainBundle();  
    CFURLRef    executableURL = CFBundleCopyExecutableURL(appBundle);  
    char execFile[4096];  
    if (CFURLGetFileSystemRepresentation(executableURL, TRUE, (UInt8 *)execFile, 4096))  
    {  
        // Strip out the filename to just get the path  
        string strExecFile = execFile;  
        int found = strExecFile.find_last_of("/");  
        string strPath = strExecFile.substr(0, found);  
		
        // Change the working directory to that of the executable  
        if(-1 == chdir(strPath.c_str())) {  
            ofLog(OF_LOG_ERROR, "Unable to change working directory to executable's directory.");  
        }  
    }  
    else {  
        ofLog(OF_LOG_ERROR, "Unable to identify executable's directory.");  
    }  
    CFRelease(executableURL);  
#endif  
	
	//OSC SETUP
	
	//string IP ="192.168.1.245";	// TCTS
	// string IP ="169.254.87.118"; // local network Seneffe
//	string IP="192.168.1.132";
	string IP="localhost";
	const char* newIP=IP.c_str();
	sender.setup(newIP, 12345);
	
	
	//sender.setup( HOST, PORTS );
	receiver.setup( PORTR );
	//END OSC SETUP
	
	//KINECT SETUP
	XnStatus nRetVal = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	g_Context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
	g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_ImageGenerator);
	g_UserGenerator.Create(g_Context);
	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)){
		printf("Supplied user generator doesn't support skeleton\n");
	}
	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);
	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration()){
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)){
			printf("Pose required, but not supported\n");
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}
	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	nRetVal = g_Context.StartGeneratingAll();
	//END KINECT SETUP
	
    quadratic = NULL;
	
    cameraForConfiguration = true;
	headTracking = false;
    action = -1;
	
    campos.set(0,0,0);
    camstarepos.set(0,0,0);
	
	kinectpos.set(0,0,0);
    
	camanglz = -(std::atan(1.0)*4)*0.5f;
    camdistz = 3000.f;
	
    testAxis = 0;
	
    eyesCount = 0;
	halGridOffset.set(0,220,0);
	halGridWidth = 6000.f;	//largeur ecran
    halGridHeight = 3500.f;	//hauteur ecran
	halGridDepth = 2000.f;	//courbure profondeur

		
    stareDistanceMultiplier = 0.1f;

    stareAtCamera = false;
	//stare.set(0,0,0);
	//stare2.set(0,0,0);
	
    ofSetFrameRate( 30 );
	
    // ofEnableAlphaBlending();
    ofDisableArbTex();
	
  
	ofSetVerticalSync(true);
    /// 3D world setup
    quadratic = gluNewQuadric();  // Create A Pointer To The Quadric Object
    gluQuadricTexture(quadratic, GL_TRUE);       // Create Texture Coords
    gluQuadricNormals(quadratic, GLU_SMOOTH);    // Create Smooth Normals
	
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
    glEnable(GL_COLOR_MATERIAL );
}

//--------------------------------------------------------------
void testApp::update(){
	
	UpdateUserData();

	//KINECT PROCESSING DATA 
	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_Context.WaitAndUpdateAll();
	g_DepthGenerator.GetMetaData(depthMD);
	g_UserGenerator.GetUserPixels(0, sceneMD);
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);

		
	
	}

//--------------------------------------------------------------
void testApp::draw() {
	
    campos.x = (cosf(camanglz) * camdistz);
    campos.z = (sinf(camanglz) * camdistz);
	
	// ofSetColor(0x000000);
	// videoTexture.draw(20,20,camWidth,camHeight);
	
	glColor3f(255.f,255.f,255.f);
	
    if (!cameraForConfiguration) {
        ofBackground(0, 0, 0);
    } else 
	{
        ofBackground(150, 150, 150);
        ofDrawBitmapString("Configuration mode", 5, ofGetHeight()-195);
        ofDrawBitmapString("Camera x: "+ofToString(campos.x,2)+", y: "+ofToString(campos.y,2)+", z: "+ofToString(campos.z,2), 5, ofGetHeight()-180);
        if (action == 0) {
            ofDrawBitmapString("Current action: 0 >> camera Z angle on arrow left & right", 5, ofGetHeight()-165);
        } else if (action == 1) {
            ofDrawBitmapString("Current action: 1 >> camera Y position on arrow left & right", 5, ofGetHeight()-165);
        } else if (action == 2) {
            ofDrawBitmapString("Current action: 2 >> kinect X position on arrow left & right", 5, ofGetHeight()-165);
			ofDrawBitmapString("Kinect x: "+ofToString(kinectpos.x,2)+", y: "+ofToString(kinectpos.y,2)+", z: "+ofToString(kinectpos.z,2), 5, ofGetHeight()-150);
			//ofDrawBitmapString("Stare x: "+ofToString(stare.x,2)+", y: "+ofToString(stare.y,2)+", z: "+ofToString(stare.z,2), 5, ofGetHeight()-135);
		} else if (action == 3) {
            ofDrawBitmapString("Current action: 3 >> kinect Y position on arrow left & right", 5, ofGetHeight()-165);
			ofDrawBitmapString("Kinect x: "+ofToString(kinectpos.x,2)+", y: "+ofToString(kinectpos.y,2)+", z: "+ofToString(kinectpos.z,2), 5, ofGetHeight()-150);
			//ofDrawBitmapString("Stare x: "+ofToString(stare.x,2)+", y: "+ofToString(stare.y,2)+", z: "+ofToString(stare.z,2), 5, ofGetHeight()-135);
		} else if (action == 4) {
            ofDrawBitmapString("Current action: 4 >> kinect Z position on arrow left & right", 5, ofGetHeight()-165);
			ofDrawBitmapString("Kinect x: "+ofToString(kinectpos.x,2)+", y: "+ofToString(kinectpos.y,2)+", z: "+ofToString(kinectpos.z,2), 5, ofGetHeight()-150);
			//ofDrawBitmapString("Stare x: "+ofToString(stare.x,2)+", y: "+ofToString(stare.y,2)+", z: "+ofToString(stare.z,2), 5, ofGetHeight()-135);
		} else {
            ofDrawBitmapString("Current action: "+ofToString(action,0), 5, ofGetHeight()-165);
        }
    }
	
    /// reseting the world
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (!cameraForConfiguration) 
	{
		
    } 
	else 
	{
		gluPerspective(70,640/480,1,40000);
       gluLookAt(-camdistz*1.5f,-camdistz*1.5f,-camdistz*1.5f,0,0,0,0,1,0);
		//gluLookAt(0,0,0,0,0,-4500,0,1,0);
    }
    glDisable(GL_TEXTURE_2D);
	
    glBlendFunc(GL_ONE,GL_ZERO);
	
    if (cameraForConfiguration) {
		
		if (drawcloud==true) {
			glPushMatrix();
			//glTranslatef(kinectpos.x,kinectpos.y,kinectpos.z);
			drawPointCloud();
			glPopMatrix();
		}
		
		
        /// render orgin (0,0,0)
        glColor3f(255.f,255.f,255.f);
        glBegin(GL_LINES); glVertex3f( -50, 0, 0); glVertex3f( 50, 0, 0); glEnd();
        glBegin(GL_LINES); glVertex3f( 0, -50, 0); glVertex3f( 0, 50, 0); glEnd();
        glBegin(GL_LINES); glVertex3f( 0, 0, -50); glVertex3f( 0, 0, 50); glEnd();
   
		/*
        /// render stare point
        glColor3f(255.f,0,0);
        glBegin(GL_LINES);
		glVertex3f( 0, 0, 0);
		glVertex3f( 0, 0, stare.z);
		glVertex3f( 0, 0, stare.z);
		glVertex3f( stare.x, 0, stare.z);
		glVertex3f( stare.x, 0, stare.z);
		glVertex3f( stare.x, stare.y, stare.z);
        glEnd();
		*/
        glPushMatrix();
		//glTranslatef(stare.x,stare.y,stare.z);
		gluSphere(quadratic, 25, 5, 5);
        glPopMatrix();
		
        /// render camera
        glColor3f(255.f,255.f,255.f);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f( camstarepos.x, camstarepos.y, camstarepos.z);
		glVertex3f( campos.x,campos.y, campos.z);
		glEnd();
        glPushMatrix();
		glTranslatef(camstarepos.x,camstarepos.y,camstarepos.z);
		gluQuadricDrawStyle(quadratic,GLU_FILL);
		gluSphere(quadratic, 20, 5, 5);
        glPopMatrix();
        glPushMatrix();
		glTranslatef(campos.x,campos.y,campos.z);
		gluSphere(quadratic, 50, 5, 5);
        glPopMatrix();
		/// render camera
        glColor3f(0,255.f,0);
		glBegin(GL_LINES);
		glVertex3f( 0, 0, 0);
		glVertex3f( 0, 0, kinectpos.z);
		glVertex3f( 0, 0, kinectpos.z);
		glVertex3f( kinectpos.x, 0, kinectpos.z);
		glVertex3f( kinectpos.x, 0, kinectpos.z);
		glVertex3f( kinectpos.x, kinectpos.y, kinectpos.z);
        glEnd();
		glPushMatrix();
		glTranslatef(kinectpos.x,kinectpos.y,kinectpos.z);
		gluSphere(quadratic, 25, 5, 5);
        glPopMatrix();
		
		glPushMatrix();
		glTranslatef(kinectpos.x,kinectpos.y,kinectpos.z);
		DrawSkel();
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(kinectpos.x,kinectpos.y,kinectpos.z);
		DrawSphere();
		glPopMatrix();

    }
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    if (key == 357 /*arrow up*/)
	{
        camdistz -= 100;

    }


	else if (key == 359 /*arrow down*/)
	{
        camdistz += 100;

    }
	else if (key == 356 /*arrow left*/)
	{
        if (action == 0)
		{
            camanglz+=0.02f;
        } else if (action == 1)
		{
            campos.y+=50;
        } else if (action == 2) 
		{
			kinectpos.x+=10;
		} else if (action == 3) 
		{
			kinectpos.y+=10;
		} else if (action == 4) 
		{
			kinectpos.z+=10;
		}
    } else if (key == 358 /*arrow right*/)
	{
        if (action == 0)
		{
            camanglz-=0.02f;
        } else if (action == 1)
		{
            campos.y-=50;
        } else if (action == 2)
		{
			kinectpos.x-=10;
		} else if (action == 3)
		{
			kinectpos.y-=10;
		} else if (action == 4)
		{
			kinectpos.z-=10;
		}
    } 
	else if (key == 115 /* 's' */)
	{
		drawcloud=!drawcloud;
	} 
	else if (key == 32 /*spacebar*/)
	{
		cameraForConfiguration = !cameraForConfiguration;
	} 
	else if (key >= 48 && key <= 57 /*numpad [0,9]*/)
	{
		action = key-48;
	} 
	else if (key == 'h') 
	{
		headTracking = !headTracking;
	} 
	else 
	{
		// vidGrabber.videoSettings();
	}
	cout << key << "\n";
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

void testApp::DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}
	
	XnSkeletonJointPosition joint1, joint2;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);
	
	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;
	glVertex3d(-pt[0].X,pt[0].Y, -pt[0].Z );
	glVertex3d(-pt[1].X,pt[1].Y, -pt[1].Z );
}
void testApp::DrawSkel()
{
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			glBegin(GL_LINES);
			glColor3ub(255, 255, 255);
			
			DrawLimb(aUsers[i], XN_SKEL_HEAD, XN_SKEL_NECK);
			
			DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);
			
			DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);
			
			DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);
			
			DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);
			
			DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);
			
			DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);
			
			glEnd();
		}
	}
}	


void testApp::DrawSphere()
{
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	GLUquadric* params;
	params = gluNewQuadric();

	for (int i=0; i<nUsers; i++) {
		
		XnPoint3D com;
		g_UserGenerator.GetCoM(aUsers[i], com);
		glColor3ub(0,0,255);
		glPushMatrix();
		glTranslated(-com.X,com.Y,-com.Z);
		
		glRotated(90, 1, 0, 0);
		gluQuadricDrawStyle(params,GLU_FILL);
		gluSphere(params,250,20,20);
		glPopMatrix();
	}	
}
void testApp::drawPointCloud()
{
	DepthMetaData g_depthMD;
	ImageMetaData g_imageMD;
	//g_image.GetAlternativeViewPointCap().SetViewPoint(g_depth);
	g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_ImageGenerator);
	//g_UserGenerator.GetAlternativeViewPointCap().SetViewPoint(g_image);
	g_DepthGenerator.GetMetaData(g_depthMD);
	g_ImageGenerator.GetMetaData(g_imageMD);
	const XnDepthPixel* pDepth = g_depthMD.Data(); 
	const XnRGB24Pixel* pImage = g_imageMD.RGB24Data(); 
	
	XnUInt32 nAllIndex = 640*480; 
	XnPoint3D nProjection[nAllIndex]; 
	int k=0;
	for (double i=0; i<480; i++) {
		for (double j=0; j<640; j++) {
			nProjection[k].X=j;
			nProjection[k].Y=i;
			nProjection[k].Z=pDepth[k];
			k++;
		}
	}
	XnPoint3D nRealWorld[nAllIndex]; 
	
	g_DepthGenerator.ConvertProjectiveToRealWorld(nAllIndex, nProjection,nRealWorld);           
	
	//glMatrixMode( GL_MODELVIEW );
	//glLoadIdentity( );
	glBegin(GL_POINTS);
	for (int i=0;i<nAllIndex;i++)
	{
		
		int red=(int)pImage[i].nRed;
		int green=(int)pImage[i].nGreen;
		int blue=(int)pImage[i].nBlue;
		glColor3ub(red,green,blue); 
		glVertex3d(-nRealWorld[i].X,nRealWorld[i].Y,-nRealWorld[i].Z);
		
	}
	glEnd();

}
void testApp::UpdateUserData()
{
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	int dx,dy,dz;
	
	XnPoint3D com[nUsers];	//vecteur de points 3D pour les diff�rents centres de gravit� 
	for (int i=0; i<nUsers; i++) 
	{
		g_UserGenerator.GetCoM(aUsers[i], com[i]);
	
	
		ostringstream oss;
		oss << i+1;
		std::string result = oss.str();
		
		string a="userpos"+result;
		string b="userdelta"+result;
		// don't send 0,0,0
		// limit to a square 3x3 for Seneffe
		std::stringstream tmp_i;
		tmp_i << aUsers[i];
		if (com[i].X!=0 && com[i].Y!=0 && com[i].Z > 1000 && com[i].X > -1500 && com[i].X < 1500 && com[i].Z < 4000)
		{
			ofxOscMessage m;
			
			string adres = "/mediacycle/browser/" + tmp_i.str() + "/hover/xy" ;
			cout << adres << endl;
			cout << com[i].X << "  " << com[i].Z << endl;
			m.setAddress(adres);
			//m.setAddress( "/mediacycle/browser/hover/xy");
//			m.addStringArg( a );
//			m.addIntArg(aUsers[i] );
			m.addFloatArg( (com[i].X) * 1.0 / 1500);
//			m.addIntArg( com[i].Y );
			m.addFloatArg( (2500 - com[i].Z) * 1.0 / 1500);
			sender.sendMessage( m );
//			m.setAddress( "/Send Centroid" );
//			m.addStringArg( a );
//			m.addIntArg(aUsers[i] );
//			m.addIntArg( com[i].X );
//			m.addIntArg( com[i].Y );
//			m.addIntArg( com[i].Z );
//			sender.sendMessage( m );
		}
		else{
			ofxOscMessage m;
			string adres = "/mediacycle/browser/" + tmp_i.str() + "/released" ;
			cout << adres << endl;
			m.setAddress(adres);
			sender.sendMessage( m );
		}
		
		XnPoint3D pt;
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])) 
		{
			XnSkeletonJointPosition joint1;
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, joint1);
			pt=joint1.position;

			string c="handR"+result;
			
			if (pt.X !=0 && pt.Y !=0 && pt.Z!=0) {
				ofxOscMessage m2;
				m2.setAddress( "/Send Hand Right" );
				m2.addStringArg( c );
				m2.addIntArg( aUsers[i] );
				m2.addIntArg( pt.X );
				m2.addIntArg( pt.Y );
				m2.addIntArg( pt.Z );
				sender.sendMessage( m2 );
			}
			
			g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, joint1);
			pt=joint1.position;
			
			string d="handL"+result;
			
			if (pt.X !=0 && pt.Y !=0 && pt.Z!=0)
			{
				ofxOscMessage m3;
				m3.setAddress( "/Send Hand Left" );
				m3.addStringArg( d );
				m3.addIntArg(aUsers[i]);
				m3.addIntArg( pt.X );
				m3.addIntArg( pt.Y );
				m3.addIntArg( pt.Z );
				sender.sendMessage( m3 );
			}
		}
	}
}

