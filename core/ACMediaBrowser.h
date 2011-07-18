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

#include <string>
#include <map>
#include <set>

inline float ACRandom() { return ((float)rand()) / (float)((1LL<<31)-1L); }

#include "ACEuclideanDistance.h"
#include "ACMediaLibrary.h"
#include "ACPlugin.h"
#include "ACUserLog.h"
#include "ACMediaNode.h"  // this contains ACPoint
#include "ACPointer.h"

using namespace std;

//static double getTime();

enum ACBrowserState {
	AC_IDLE=0,
	AC_CHANGING=1
};

enum ACBrowserLayout {
	AC_LAYOUT_TYPE_NONE=0,
	AC_LAYOUT_TYPE_NODELINK=1,
	AC_LAYOUT_TYPE_CONSTELLATION=2//CF: for egs. to link segments from a given media together... but what if we want to combine it with a node link layout?s
};

enum ACBrowserMode {
	AC_MODE_NONE=0,
	AC_MODE_CLUSTERS=1,
	AC_MODE_NEIGHBORS=2
};

typedef std::map<std::string, ACBrowserMode > stringToBrowserModeConverter;
const stringToBrowserModeConverter::value_type _initb[] = {
stringToBrowserModeConverter::value_type("", AC_MODE_NONE), \
stringToBrowserModeConverter::value_type("Clusters", AC_MODE_CLUSTERS), \
stringToBrowserModeConverter::value_type("Neighbors", AC_MODE_NEIGHBORS)
};
const stringToBrowserModeConverter stringToBrowserMode(_initb, _initb + sizeof _initb / sizeof *_initb);

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

struct ACLabel {
	string		text;
	float		size;
	ACPoint		pos;
	bool		isDisplayed;
	ACLabel() : isDisplayed(true) {}
};

// XS 110310 added this to make the transition towards tree instead of vector
// could even be a class ?
typedef vector<ACMediaNode> ACMediaNodes;
typedef vector<ACLabel> ACLabels;

class ACMediaBrowser {

public:
	ACMediaBrowser();
	~ACMediaBrowser();

	void clean();

	void setLibrary(ACMediaLibrary *lib) { mLibrary = lib; };
	ACMediaLibrary *getLibrary() { return mLibrary; };

	// call this when the number of loops changes in the library
	void libraryContentChanged(int needsCluster=1);

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

	// 0 of features
	void setWeight(int i, float weight);
	std::vector<float> getWeightVector(){return mFeatureWeights;}
	void setWeightVector(vector<float> &fw) {mFeatureWeights = fw;}
	float getWeight(int i);
	void initializeFeatureWeights();

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
	void incrementLoopNavigationLevels(int loopIndex);
	int getNavigationLevel()				{ return mNavigationLevel; };
	ACNavigationState getCurrentNavigationState();
	void setCurrentNavigationState(ACNavigationState state);
	// go back/forward in the navigation into clusters
	void goBack();
	void goForward();
	void storeNavigationState(); // was: pushNavigationState();

	// == Nodes
	void setClickedNode(int inode,int p_index = 0);
	int getClickedNode(int p_index = 0) {return mClickedNode; };
	bool toggleNode(int node);
	void dumpSelectedNodes();
	set<int>& getSelectedNodes(){return mSelectedNodes;}
	void unselectNodes();
	int getClosestNode(int p_index = 0) {return getPointerFromIndex(p_index)->getClosestNode(); };
	int	getLastSelectedNode(){return mLastSelectedNode;}
	void setReferenceNode(int index);
	int getReferenceNode(){return mReferenceNode;}
	const ACMediaNodes	&getLoopAttributes() const { return mLoopAttributes; } 	// XS 100310 is this still necessary ? const ?
	ACMediaNode &getMediaNode(int i) ; // not const because accesors to MediaNode can modify it
	void setNodeNextPosition(int loop_id, ACPoint p);
	// XS TODO : displayed vs active
	void setLoopIsDisplayed(int loop_id, bool iIsDisplayed) {this->getMediaNode(loop_id).setDisplayed(iIsDisplayed);}
	void setLoopAttributesActive(int loop_id, int value) { this->getMediaNode(loop_id).setActivity(value); };
	int getNumberOfDisplayedLoops();
	void setNumberOfDisplayedLoops(int nd);
	int getNumberOfMediaNodes();
	void initializeNodes(ACBrowserMode _mode = AC_MODE_CLUSTERS);

	// == Pointers
	int getNumberOfPointers();
	ACPointerType getPointerTypeFromIndex(int i);
	ACPointerType getPointerTypeFromId(int i);	
	ACPointer* getPointerFromIndex(int i); // for use when parsing pointers incrementally
	ACPointer* getPointerFromId(int i); // for use when parsing pointers from the ID set by the input device
	void resetPointers();
	void addPointer(int p_id,ACPointerType _pointerType=AC_POINTER_UNKNOWN);
	void removePointer(int p_id);
	void addMousePointer();
	void removeMousePointer();

	// == Labels
	void setClickedLabel(int ilabel);
	int getClickedLabel()					{return mClickedLabel; };

	// = States : AC_ IDLE or AC_CHANGING (i.e., from current to next position)
	double getFrac() const {return mFrac;} // fraction between current and next position
	ACBrowserState getState() const {return mState;};
	void setState(ACBrowserState state);
	void updateState();

	// interaction with pointers
	void setSourceCursor(int lid, int frame_pos);
	void setCurrentFrame(int lid, int frame_pos);
	void hoverWithPointerId(float mxx, float myy, int p_id = -1);
	void hoverWithPointerIndex(float mxx, float myy, int p_index = 0);

	ACBrowserLayout getLayout();
	void setLayout(ACBrowserLayout _layout);
	ACBrowserMode getMode();
	void setMode(ACBrowserMode _mode);

	// Quick Browser
	void setClosestNode(int loop_id, int p_index);
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
	void addLabel(string text, ACPoint pos);
	string getLabelText(int i);
	ACPoint getLabelPos(int i);

	const vector<ACLabel> &getLabelAttributes() const { return mLabelAttributes; };
	void setLabelPosition(int loop_id, float x, float y, float z=0);
	int getNumberOfDisplayedLabels();
	void setNumberOfDisplayedLabels(int nd);
	int getNumberOfLabels(){return mLabelAttributes.size();}

	void setClustersMethodPlugin(ACPlugin* acpl){mClustersMethodPlugin=dynamic_cast<ACClusterMethodPlugin*> (acpl) ;}
	void setNeighborsMethodPlugin(ACPlugin* acpl){mNeighborsMethodPlugin=dynamic_cast<ACNeighborMethodPlugin*> (acpl);}
	void setClustersPositionsPlugin(ACPlugin* acpl){mClustersPosPlugin=dynamic_cast<ACClusterPositionsPlugin*> (acpl);}
	void setNeighborsPositionsPlugin(ACPlugin* acpl){mNeighborsPosPlugin=dynamic_cast<ACNeighborPositionsPlugin*> (acpl);}
	void setVisualisationPlugin(ACPlugin* acpl){mNoMethodPosPlugin=dynamic_cast<ACNoMethodPositionsPlugin*> (acpl);}

	bool changeClustersMethodPlugin(ACPlugin* acpl);
	bool changeNeighborsMethodPlugin(ACPlugin* acpl);
	bool changeClustersPositionsPlugin(ACPlugin* acpl);
	bool changeNeighborsPositionsPlugin(ACPlugin* acpl);
	//bool changeVisualisationPlugin(ACPlugin* acpl);//CF we need to sort out first what a VisualisationPlugin can contain vs Clusters/Neighbors Method/Positions plugins

	// NB: Proximity Grid moved to plugin

	// == User Log
	ACUserLog* getUserLog(){return mUserLog;};

	// == XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
	void updateDisplay(bool animate=false, int needsCluster=1);//, bool neighborhoods=true);

	// CF switch navigation mode while navigating
	void switchMode(ACBrowserMode _mode);

	const vector<float> &getFeatureWeights(){return mFeatureWeights;};
	const int &getClusterCount(){return mClusterCount;};
	const vector<vector <float> > &getClusterCenter(int i){return mClusterCenters[i];};
	
	
private: // better not let the ouside world know about internal cooking
	void resetNavigation();
	void resetLoopNavigationLevels(int l=0);
	void resetCamera();
	// next positions -> current positions
	void commitPositions();

//XS the update() methods should remain private
// all what is needed from outside is *updateDisplay*
private:
	// update positions based on current clustering

	// == Cluster Mode
	void updateClusters(bool animate=false, int needsCluster=1);
	// default cluster :
	// - neighborhood = Kmeans
	// - position = propeller
	void updateNextPositionsPropeller();
	void updateClustersKMeans(bool animate, int needsCluster=1);

	// == Neighbors Mode ==
	void updateNeighborhoods();
public: // XS TODO so far this one is still called from outside
	void updateNextPositions();

protected:
	ACMediaLibrary *mLibrary; // just a pointer to the library, no "new"
	// state management
	double 				mRefTime;
	double 				mFrac;
	ACBrowserState		mState;
	ACBrowserLayout		mLayout;
	ACBrowserMode		mMode;

	int 				mClickedNode; // valid between mouseDown and mouseUp, otherwise -1
	set<int>			mSelectedNodes;
	int 				mReferenceNode;
	int					mClosestNode;
	int					mLastSelectedNode;

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
	ACPointers   mPointers;
	int mPointersActiveNumber;

	int nbDisplayedLoops;

	ACLabels	mLabelAttributes;
	int nbDisplayedLabels;
	int mClickedLabel;

	int mNavigationLevel;

	// XS TODO: make a class clusters
	int mClusterCount;
	//vector<vector <int> > 		clusters;
	vector<vector<vector <float> > > mClusterCenters; // cluster index, feature index, descriptor index
	vector<float>			mFeatureWeights; // each value must be in [0,1], important for euclidian distance.

	int auto_play;
	int auto_play_toggle;

	ACClusterMethodPlugin* mClustersMethodPlugin;
	ACNeighborMethodPlugin* mNeighborsMethodPlugin;

	ACClusterPositionsPlugin* mClustersPosPlugin;
	ACNeighborPositionsPlugin* mNeighborsPosPlugin;
	ACNoMethodPositionsPlugin* mNoMethodPosPlugin;

	ACUserLog* mUserLog;

	int prevLibrarySize;
};

#endif // __ACMEDIABROWSER_H__
