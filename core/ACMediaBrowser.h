/*
 *  ACMediaBrowser.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 18/05/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

// TODO: check if all this is really common to all media browsers

#ifndef __ACMEDIABROWSER_H__
#define __ACMEDIABROWSER_H__

#include <Common/TiMath.h> // for Timax ...

#include "ACEuclideanDistance.h"

#include "ACMediaLibrary.h"
#include <vector>

// ??? extern ACAudioCycle* audio_cycle;

using namespace std;

enum ACBrowserState {
	AC_IDLE=0,
	AC_CHANGING
};

//
//enum ACNavigationStateType
//{
//	AC_NAV_SELECTION,
//};

struct ACNavigationState
{
	//ACNavigationStateType	mNavType;
	
	int 			mSelectedLoop;
	int 			mNavigationLevel;
	vector<float> 		mFeatureWeights;
};

struct ACPoint
{
	float x, y, z;
};


// XS TODO
// XS the following could be renamed but seems valid for all media
// ask SD to check...

struct ACLoopAttribute
{
	ACPoint 	currentPos, nextPos;
	ACPoint		viewPos;
	float		distanceMouse;
	int 		cluster; //cluster index
	int			active;  // plying or not - and in which mode
	int			curser;
	int 		navigationLevel; // initially all set to zero, while traversing, only the one incremented are kept
	int			hover;
	ACLoopAttribute() : cluster(0), active(false), navigationLevel(0), hover(0) {}
};

class ACMediaBrowser {
	
public:
	ACMediaBrowser();
	~ACMediaBrowser();
	
	void setLibrary(ACMediaLibrary *lib) { mLibrary = lib; };
	ACMediaLibrary *getLibrary() { return mLibrary; };

	double getFrac() const {return mFrac;};
	ACBrowserState getState() const {return mState;};

	// call this when the number of loops changes in the library
	void libraryContentChanged();
	
	// Search by similarity
	int getKNN(int id, vector<int> &ids, int k);
        int getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k);
		
	// memory/context
	void setBack();
	void setForward();
	void setHistory();
	void setBookmark();
	void setTag();
	
	void setNeedsDisplay(bool val) 				{ mNeedsDisplay = val; };
	bool getNeedsDisplay() const				{ return mNeedsDisplay; };
	
	// camera
	void setCameraPosition(float x, float y)		{ mCameraPosition[0] = x;  mCameraPosition[1] = y; };
	void getCameraPosition(float &x, float &y) 	{ x = mCameraPosition[0];  y = mCameraPosition[1]; };
	void setCameraZoom(float z)				{ mCameraZoom = TI_MAX(z, 0.000001); setNeedsDisplay(true); };
	void setCameraRecenter()				{ mCameraPosition[0]=0.0; mCameraPosition[1]=0.0; mCameraZoom=1.0; mCameraAngle=0.0;};
	float getCameraZoom() const				{ return mCameraZoom; };
	void setCameraRotation(float angle)				{ mCameraAngle = angle; setNeedsDisplay(true); };
	float getCameraRotation() const				{ return mCameraAngle; };
	
	// organization
	// XS make this more general !
	void setWeightRhythm(float weight);
	void setWeightTimbre(float weight);
	void setWeightHarmony(float weight);
	void setWeight(int i, float weight) {mFeatureWeights[i] = weight; updateClusters(true); }
	void setClusterNumber(int n); 
		
	// filtering
	void setFilterIn();
	void setFilterOut();
	void setFilterSuggest();	
	
// Cluster and Visualization methods
	// set feature weights used for clustering
	void setFeatureWeights(vector<float> &weights);

	// cluster data based on current feature weights
	void updateClusters(bool animate = false);
	
	void setClickedLoop(int index) 				{mClickedLoop = index;};
	int getClickedLoop()					{return mClickedLoop; };

	// this influences updateNextPositions
	void setSelectedObject(int index);

	// update positions based on current clustering
	void updateNextPositions();

	void updateState();
	void setState(ACBrowserState state);

	// next positions -> current positions
	void commitPositions();
	
	const vector<ACLoopAttribute>	&getLoopAttributes() const { return mLoopAttributes; }; 
	void setLoopAttributesActive(int loop_id, int value) { mLoopAttributes[loop_id].active = value; };
	//const vector<ACPoint>	&getLoopCurrentPositions() const	{ return mCurrentPos; } 
	//const vector<ACPoint>	&getLoopNextPositions()	const		{ return mNextPos; }
	void getMouse(float *mx, float *my) { *mx = mousex; *my = mousey; };
	
	int setSourceCurser(int lid, int frame_pos);
	int setHoverLoop(int lid, float x, float y);
	
	// sets all navigationLevel to 0
	void resetLoopNavigationLevels();
	void incrementLoopNavigationLevels(int loopIndex);
	int getNavigationLevel()				{ return mNavigationLevel; };
	void pushNavigationState();
	ACNavigationState getCurrentNavigationState();
	void setCurrentNavigationState(ACNavigationState state);
	
	// Quick Browser
	void setClosestLoop(int loop_id);
	void setAutoPlay(int auto_play) { this->auto_play = auto_play; };
	
	// sources - SD reintroduced 2009 aug 4
	int	 pickSource(float x, float z);
	void getSourcePosition(int loop_id, float* x, float* z);
	void setSourcePosition(float _x, float _z, float* x, float* z);
	int	 toggleSourceActivity(float x, float z);
	int toggleSourceActivity(int lid, int type=1);
	int muteAllSources();
	
protected:
	ACMediaLibrary *mLibrary; 
	
	// state management
	double 				mRefTime;
	double 				mFrac;
	ACBrowserState			mState;
	
	int 				mClickedLoop; // valid between mouseDown and mouseUp, otherwise -1
	int 				mSelectedLoop;
	
	bool 				mNeedsDisplay;
	
	float   			mViewWidth;
	float   			mViewHeight;
	float   			mCenterOffsetX;
	float				mCenterOffsetZ;
	
	float 				mCameraPosition[2];
	float 				mCameraZoom;
	float				mCameraAngle;
	
	
	// clustering data
	//vector< int > 			mObjectCluster; // cluster index for each object
	
	// displayed positions
	//vector<ACPoint>			mCurrentPos;
	//vector<ACPoint>			mNextPos;
	
	vector<ACNavigationState>	mBackwardNavigationStates;
	vector<ACNavigationState>	mForwardNavigationStates;
	
	vector <ACLoopAttribute>	mLoopAttributes; // one entry per media in the same order as in library.
	float mousex;
	float mousey;
	
	int 				mNavigationLevel;
	
	// clusters
	int				mClusterCount;
	//vector<vector <int> > 		clusters;
	vector<vector<vector <float> > >mClusterCenters; // cluster index, feature index, descriptor index
	vector<float>			mFeatureWeights; // each value must be in [0,1], important for euclidian distance.
	
	int closest_loop;
	int auto_play;
	int auto_play_toggle;
};

#endif // __ACMEDIABROWSER_H__
