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
// XS 090310 : removed struct ACLoopAttribute -> ACMediaNode

#ifndef __ACMEDIABROWSER_H__
#define __ACMEDIABROWSER_H__

#include <Common/TiMath.h> // for Timax only ...

#include "ACEuclideanDistance.h"
#include "ACMediaLibrary.h"
#include "ACPlugin.h"
#include "ACUserLog.h"

#include "ACMediaNode.h"  // this contains ACPoint

#include <vector>

using namespace std;

// XS 110310 added this to make the transition towards tree instead of vector
// could even be a class ?
typedef vector<ACMediaNode> ACMediaNodes;


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

struct ACLabelAttribute {
	string		text;
	float		size;
	ACPoint		pos;
	bool		isDisplayed;
	ACLabelAttribute() : isDisplayed(true) {}
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
	
	// handy library to just assign random positions to items
	void randomizeLoopPositions();

	// Search by similarity
	int getKNN(int id, vector<int> &ids, int k);
	int getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k);
		
	// memory/context
	void setBack();
	void setForward();
	void setHistory();
	void setBookmark();
	void setTag();
	
	void setNeedsDisplay(bool val) 				{ mNeedsDisplay = val; }
	bool getNeedsDisplay() const				{ return mNeedsDisplay; }
	
	// Update audio engine sources		
	void setNeedsActivityUpdateLock(int i);
	void setNeedsActivityUpdateAddMedia(int loop_id);	
	void setNeedsActivityUpdateRemoveMedia();	
	vector<int>* getNeedsActivityUpdateMedia();

	// camera
	void setCameraPosition(float x, float y)		{ mCameraPosition[0] = x;  mCameraPosition[1] = y; setNeedsDisplay(true);}
	void getCameraPosition(float &x, float &y) 	{ x = mCameraPosition[0];  y = mCameraPosition[1]; setNeedsDisplay(true);}
	void setCameraZoom(float z)				{ mCameraZoom = TI_MAX(z, 0.000001); setNeedsDisplay(true); }
	void setCameraRecenter()				{ mCameraPosition[0]=0.0; mCameraPosition[1]=0.0; mCameraZoom=1.0; mCameraAngle=0.0; setNeedsDisplay(true);}
	float getCameraZoom() const				{ return mCameraZoom; }
	void setCameraRotation(float angle)				{ mCameraAngle = angle; setNeedsDisplay(true); }
	float getCameraRotation() const				{ return mCameraAngle; }
	
	// organization
	// XS 091009 : this unique one replaces rhythm, timbre, harmony
	void setWeight(int i, float weight);
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
	void setClusterIndex(int mediaIdx,int clusterIdx);
	void setClusterCenter(int clusterIdx, vector< vector<float> >);
	void initClusterCenters();
	void kmeans(bool animate);
	
	// neighbors
	void updateNeighborhoods();
	
	void setClickedLoop(int iloop);
	int getClickedLoop()					{return mClickedLoop; };
	int getClosestLoop()					{return closest_loop; };

	void setClickedLabel(int ilabel);
	int getClickedLabel()					{return mClickedLabel; };

	// this influences updateNextPositions
	void setSelectedObject(int index);

	// update positions based on current clustering
	void updateNextPositions();
	void setNextPositionsPropeller();
	void setNextPositions2dim();

	void updateState();
	void setState(ACBrowserState state);

	// next positions -> current positions
	void commitPositions();
	
	// loops (or items) 
	// XS 100310 is this still necessary ? const ?
	const ACMediaNodes	&getLoopAttributes() const { return mLoopAttributes; }; 
	
	// XS NEW 100310
	ACMediaNode &getMediaNode(int i) ; // not const because accesors to MediaNode can modify it
	void initializeNodes(int _defaultNodeId = 0); 

	
	void setLoopPosition(int loop_id, float x, float y, float z=0);
	// XS 100310 MediaNode
	void setLoopIsDisplayed(int loop_id, bool iIsDisplayed) {this->getMediaNode(loop_id).setDisplayed(iIsDisplayed);}

	int getNumberOfDisplayedLoops();
	void setNumberOfDisplayedLoops(int nd);

	// XS TODO getsize
	int getNumberOfMediaNodes(){return mLoopAttributes.size() ;} // XS this should be the same as mLibrary->getSize(), but this way it is more similar to getNumberOfLabels // CF not true in non-explatory mode (one loop can be displayed more than once at a time)
	
	// XS 100310 MediaNode
	void setLoopAttributesActive(int loop_id, int value) { this->getMediaNode(loop_id).setActivity(value); };
	//const vector<ACPoint>	&getLoopCurrentPositions() const	{ return mCurrentPos; } 
	//const vector<ACPoint>	&getLoopNextPositions()	const		{ return mNextPos; }
	
	void getMouse(float *mx, float *my) { *mx = mousex; *my = mousey; };
	
	int setSourceCursor(int lid, int frame_pos);
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
	// int	 toggleSourceActivity(float x, float z);
	int toggleSourceActivity(int lid, int type=1); // XS deprecated
	int toggleSourceActivity(ACMediaNode &node, int _activity = 1); // XS new 150310
	int muteAllSources();
	
	// labels
	int getLabelSize();
	void setLabel(int i, string text, ACPoint pos);
	string getLabelText(int i);
	ACPoint getLabelPos(int i);
	
	const vector<ACLabelAttribute> &getLabelAttributes() const { return mLabelAttributes; }; 
	void setLabelPosition(int loop_id, float x, float y, float z=0);
	int getNumberOfDisplayedLabels();
	void setNumberOfDisplayedLabels(int nd);
	int getNumberOfLabels(){return mLabelAttributes.size();}

	void setVisualisationPlugin(ACPlugin* acpl){mVisPlugin=acpl;};
	void setPositionsPlugin(ACPlugin* acpl){mPosPlugin=acpl;};
	void setNeighborhoodsPlugin(ACPlugin* acpl){mNeighborsPlugin=acpl;};	
	
	// Proximity Grid		- SD TODO - eventually to be moved in visualization plugin chain
	float proxgridstepx;
	float proxgridstepy;
	float proxgridaspectratio;
	int proxgridlx;
	int proxgridly;
	float proxgridl;
	float proxgridr;
	float proxgridb;
	float proxgridt;
	int proxgridmaxdistance;
	float proxgridjitter;
	int proxgridboundsset;
	vector<int> proxgrid;
	void setProximityGrid();
	void setProximityGridQuantize(ACPoint p, ACPoint *pgrid);	
	void setProximityGridUnquantize(ACPoint pgrid, ACPoint *p);
	void setProximityGridBounds(float l, float r, float b, float t);
	void setRepulsionEngine();
	
	ACUserLog* getUserLog(){return mUserLog;};

protected:
	ACMediaLibrary *mLibrary; 
	// state management
	double 				mRefTime;
	double 				mFrac;
	ACBrowserState		mState;
	
	int 				mClickedLoop; // valid between mouseDown and mouseUp, otherwise -1
	int 				mSelectedLoop;
	
	bool 				mNeedsDisplay;
	//bool 				mNeedsActivityUpdate;
	vector<int>			mNeedsActivityUpdateMedia;
	pthread_mutex_t		activity_update_mutex;
	pthread_mutexattr_t activity_update_mutex_attr;

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
	
	// XS TODO 1 generalize to tree
	// XS TODO 2 make this vector of pointers
	ACMediaNodes mLoopAttributes; 
	// XS was: one entry per media in the same order as in library.
	
	int nbDisplayedLoops;
	
	float mousex;
	float mousey;
	
	vector <ACLabelAttribute>	mLabelAttributes;
	int nbDisplayedLabels;
	int mClickedLabel;
	
	int 				mNavigationLevel;
	
	// XS TODO: make a class clusters
	int				mClusterCount;
	//vector<vector <int> > 		clusters;
	vector<vector<vector <float> > > mClusterCenters; // cluster index, feature index, descriptor index
	vector<float>			mFeatureWeights; // each value must be in [0,1], important for euclidian distance.
	
	int closest_loop;
	int auto_play;
	int auto_play_toggle;

	ACPlugin* mVisPlugin;
	ACPlugin* mPosPlugin;
	ACPlugin* mNeighborsPlugin;
	ACUserLog* mUserLog;
	
};

#endif // __ACMEDIABROWSER_H__
