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

//#include <Common/TiMath.h> // for Timax only ...

#include <assert.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <sys/time.h>
#include <pthread.h>

inline float ACRandom() { return ((float)rand()) / (float)((1LL<<31)-1L); }

#include "ACEuclideanDistance.h"
#include "ACMediaLibrary.h"
#include "ACPlugin.h"
#include "ACUserLog.h"
#include "ACMediaNode.h"  // this contains ACPoint

using namespace std;

// XS 110310 added this to make the transition towards tree instead of vector
// could even be a class ?
typedef vector<ACMediaNode> ACMediaNodes;


enum ACBrowserState {
	AC_IDLE=0,
	AC_CHANGING=1
};

enum ACBrowserLayout {
	AC_LAYOUT_TYPE_NONE=0,
	AC_LAYOUT_TYPE_NODELINK=1
};

enum ACBrowserMode {
	AC_MODE_CLUSTERS=0,
	AC_MODE_NEIGHBORS=1
};

//
//enum ACNavigationStateType
//{
//	AC_NAV_SELECTION,
//};

struct ACNavigationState
{
	//ACNavigationStateType	mNavType;
	
	int 			mReferenceNode;
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

	// call this when the number of loops changes in the library
	void libraryContentChanged();
	
	// handy library to just assign random positions to items
	void randomizeNodePositions();

	// Search by similarity
	int getKNN(int id, vector<int> &ids, int k);
	int getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k);
		
	// memory/context - undefined
	void setHistory();
	void setBookmark();
	void setTag();
	
	// to tell if the view (e.g., ACOsgBrowserViewQT) has to be updated.
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
	void setCameraZoom(float z)				{ mCameraZoom = max((double)z, 0.000001); setNeedsDisplay(true); }//CF instead of { mCameraZoom = TI_MAX(z, 0.000001); setNeedsDisplay(true); }
	void setCameraRecenter()				{ mCameraPosition[0]=0.0; mCameraPosition[1]=0.0; mCameraZoom=1.0; mCameraAngle=0.0; setNeedsDisplay(true);}
	float getCameraZoom() const				{ return mCameraZoom; }
	void setCameraRotation(float angle)				{ mCameraAngle = angle; setNeedsDisplay(true); }
	float getCameraRotation() const				{ return mCameraAngle; }
	
	// weights of features
	void setWeight(int i, float weight);
	void setFeatureWeights(vector<float> &weights);
		
	// filtering
	void setFilterIn();
	void setFilterOut();
	void setFilterSuggest();	
	
	// == Cluster Mode == (based on features weights)
	void setClusterNumber(int n); 
	void setClusterIndex(int mediaIdx,int clusterIdx);
	void setClusterCenter(int clusterIdx, vector< vector<float> >);
	void initClusterCenters();
	// XS TODO clean level / state ...
	void resetLoopNavigationLevels();
	void incrementLoopNavigationLevels(int loopIndex);
	int getNavigationLevel()				{ return mNavigationLevel; };
	void pushNavigationState();
	ACNavigationState getCurrentNavigationState();
	void setCurrentNavigationState(ACNavigationState state);
	// go back/forward in the navigation into clusters 
	void goBack();
	void goForward();
		
	// == Nodes
	void setClickedNode(int inode);
	int getClickedNode()					{return mClickedNode; };
	int getClosestNode()					{return mClosestNode; };
	void setReferenceNode(int index);
	const ACMediaNodes	&getLoopAttributes() const { return mLoopAttributes; } 	// XS 100310 is this still necessary ? const ?
	ACMediaNode &getMediaNode(int i) ; // not const because accesors to MediaNode can modify it
	void setNodeNextPosition(int loop_id, float x, float y, float z=0);
	// XS TODO : displayed vs active
	void setLoopIsDisplayed(int loop_id, bool iIsDisplayed) {this->getMediaNode(loop_id).setDisplayed(iIsDisplayed);}	
	void setLoopAttributesActive(int loop_id, int value) { this->getMediaNode(loop_id).setActivity(value); };
	int getNumberOfDisplayedLoops();
	void setNumberOfDisplayedLoops(int nd);
	int getNumberOfMediaNodes(){return mLoopAttributes.size() ;} // XS TODO getsize; this should be the same as mLibrary->getSize(), but this way it is more similar to getNumberOfLabels // CF not true in non-explatory mode (one loop can be displayed more than once at a time)
	// XS TODO: define this one
	void initializeNodes(int _defaultNodeId = 0); 	
	
	// == Labels 
	void setClickedLabel(int ilabel);
	int getClickedLabel()					{return mClickedLabel; };

	// = States : AC_ IDLE or AC_CHANGING (i.e., from current to next position)
	double getFrac() const {return mFrac;} // fraction between current and next position
	ACBrowserState getState() const {return mState;};
	void setState(ACBrowserState state);
	void updateState();
	// next positions -> current positions
	void commitPositions();
	
	// interaction with mouse
	void getMouse(float *mx, float *my) { *mx = mousex; *my = mousey; };
	int setSourceCursor(int lid, int frame_pos);
	int setHoverLoop(int lid, float x, float y);
	
	ACBrowserLayout getLayout();
	void setLayout(ACBrowserLayout _layout);
	ACBrowserMode getMode();
	void setMode(ACBrowserMode _mode);
		
	// Quick Browser
	void setClosestNode(int loop_id);
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
	void setNeighborhoodsPlugin(ACPlugin* acpl){
		mNeighborsPlugin=acpl;
		this->setMode(AC_MODE_NEIGHBORS);//CF this stays until it is correctly used on main applications
	};	
	
	// NB: Proximity Grid moved to plugin
	
	// == User Log
	ACUserLog* getUserLog(){return mUserLog;};

	// == XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
	void updateDisplay(bool animate=false);
	
//XS this should be private when we use updateDisplay
public:
	// update positions based on current clustering

	// == Cluster Mode 
	void updateClusters(bool animate = false);
	// default cluster : 
	// - neighborhood = Kmeans
	// - position = propeller
	void updateNextPositionsPropeller();
	void updateClustersKMeans(bool animate);

	// == Neighbors Mode ==
	void updateNeighborhoods();

	void updateNextPositions();
	void updateNextPositions2dim();

protected:
	ACMediaLibrary *mLibrary; 
	// state management
	double 				mRefTime;
	double 				mFrac;
	ACBrowserState		mState;
	ACBrowserLayout		mLayout;
	ACBrowserMode		mMode;
	
	int 				mClickedNode; // valid between mouseDown and mouseUp, otherwise -1
	int 				mReferenceNode;
	int					mClosestNode;

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
	
	vector<ACNavigationState>	mBackwardNavigationStates;
	vector<ACNavigationState>	mForwardNavigationStates;
	
	// XS TODO 1 generalize to tree
	// XS TODO 2 make this vector of pointers
	ACMediaNodes mLoopAttributes; 
	
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
	
	int auto_play;
	int auto_play_toggle;

	ACPlugin* mVisPlugin;
	ACPlugin* mPosPlugin;
	ACPlugin* mNeighborsPlugin;
	ACUserLog* mUserLog;
	

	
};

#endif // __ACMEDIABROWSER_H__
