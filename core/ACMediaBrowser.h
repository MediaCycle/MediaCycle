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
// XS 090310 : removed struct ACMediaNode -> ACMediaNode

#ifndef __ACMEDIABROWSER_H__
#define __ACMEDIABROWSER_H__


#include <assert.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <pthread.h>

#include <string>
#include <map>
#include <set>

inline float ACRandom() { return ((float)rand()) / (float)((1LL<<31)-1L); }

#include "ACEuclideanDistance.h"
#include "ACMediaLibrary.h"
#include "ACPlugin.h"
#include "ACNeighborsManager.h"
#include "ACMediaNode.h"  // this contains ACPoint
#include "ACPointer.h"

using namespace std;

enum ACBrowserState {
    AC_IDLE=0,
    AC_CHANGING=1
};

enum ACBrowserLayout {
    AC_LAYOUT_TYPE_NONE=0,
    AC_LAYOUT_TYPE_NODELINK=1,
    AC_LAYOUT_TYPE_CONSTELLATION=2//CF: for egs. to link segments from a given media together... but what if we want to combine it with a node link layout?s
};

#include "ACEventListener.h"

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

    int mReferenceNode;
    int mNavigationLevel;
    vector<float> mFeatureWeights;
};

struct ACLabel {
    string text;
    float size;
    ACPoint pos;
    bool isDisplayed;
    ACLabel() : text(""),size(1.0),pos(ACPoint()),isDisplayed(true) {}
};

// XS 110310 added this to make the transition towards tree instead of vector
// could even be a class ?
typedef std::map<long,ACMediaNode*> ACMediaNodes;

typedef vector<ACLabel> ACLabels;

class ACMediaBrowser {

public:
    ACMediaBrowser();
    ~ACMediaBrowser();

    void clean();

    void setLibrary(ACMediaLibrary *lib) { mLibrary = lib; };
    ACMediaLibrary *getLibrary() { return mLibrary; };

    // call this when the number of nodes changes in the library
    void libraryContentChanged(int needsCluster=1, bool needsDisplay = true);

    // handy library to just assign random positions to items
    void randomizeNodePositions();

    // Search by similarity
    int getKNN(int id, vector<int> &ids, int k);
    int getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k);
    int getKNN(vector<FeaturesVector> &feat, vector<int> &ids, int k);
    int getClustersCenterMedia(vector<int> &ids);

    // Search on a specific feature (e.g., 10 images most red)
    int getKSortedOnFeature(int k=0, int f=0, int dim=0, bool revert=false);

    // memory/context - undefined
    void setHistory();
    void setBookmark();
    void setTag();

    // to tell if the view has to be updated.
    void setNeedsDisplay(bool val) { mNeedsDisplay = val; }
    bool getNeedsDisplay() const{ return mNeedsDisplay; }

    // to tell if the mode has just changed
    void setModeChanged(bool val) { mModeChanged = val; }
    bool getModeChanged() const{ return mModeChanged; }

    // Update audio engine sources
    void setNeedsActivityUpdateLock(int i);
    void setNeedsActivityUpdateAddMedia(int node_id);
    void setNeedsActivityUpdateRemoveMedia();
    vector<int>* getNeedsActivityUpdateMedia();

    void setNeedsNavigationUpdateLock(int i);
    // camera
    void setCameraPosition(float x, float y) { mCameraPosition[0] = x;  mCameraPosition[1] = y; setNeedsDisplay(true);}
    void getCameraPosition(float &x, float &y) 	{ x = mCameraPosition[0];  y = mCameraPosition[1]; setNeedsDisplay(true);}
    void setCameraZoom(float z){ mCameraZoom = max((double)z, 0.000001); setNeedsDisplay(true); }//CF instead of { mCameraZoom = TI_MAX(z, 0.000001); setNeedsDisplay(true); }
    void setCameraRecenter(){ mCameraPosition[0]=0.0; mCameraPosition[1]=0.0; mCameraZoom=1.0; mCameraAngle=0.0; setNeedsDisplay(true);}
    float getCameraZoom() const{ return mCameraZoom; }
    void setCameraRotation(float angle){ mCameraAngle = angle; setNeedsDisplay(true); }
    float getCameraRotation() const{ return mCameraAngle; }

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
    int getClusterNumber();
    void setClusterIndex(int mediaIdx,int clusterIdx);
    void setClusterCenter(int clusterIdx, vector<FeaturesVector >);
    void initClusterCenters();
    // XS TODO clean level / state ...
    void incrementNavigationLevels(int nodeIndex);
    int getNavigationLevel(){ return mNavigationLevel;}
    ACNavigationState getCurrentNavigationState();
    void setCurrentNavigationState(ACNavigationState state);
    // go back/forward in the navigation into clusters
    void goBack();
    void goForward();
    void storeNavigationState(); // was: pushNavigationState();
    void forwardNextLevel();
    void changeReferenceNode();

    // == Nodes
    void setClickedNode(int inode);
    int getClickedNode() {return mClickedNode;}
    bool toggleNode(int node);
    void dumpSelectedNodes();
    set<int>& getSelectedNodes(){return mSelectedNodes;}
    void unselectNodes();
    int getClosestNode(int p_index = 0);
    std::map<long int,int> setClosestNode(int node_id, int p_index); // returns a map of media id and activity
    float getClosestDistance(int p_index = 0);
    void setClosestDistance(float distance, int p_index);
    int	getLastSelectedNode(){return mLastSelectedNode;}
    void setReferenceNode(int index);
    int getReferenceNode(){return mReferenceNode;}
    const ACMediaNodes	&getMediaNodes() const { return mMediaNodes; } 	// XS 100310 is this still necessary ? const ?
    ACMediaNode* getMediaNode(int i) ; // not const because accesors to MediaNode can modify it
    ACMediaNode* getNodeFromMedia(ACMedia* _media) ; // not const because accesors to MediaNode can modify it
    void setNodeNextPosition(int node_id, ACPoint p);
    // XS TODO : displayed vs active
    void setMediaNodeDisplayed(int node_id, bool iIsDisplayed) {this->getMediaNode(node_id)->setDisplayed(iIsDisplayed);}
    void setMediaNodeActive(int node_id, int value) { this->getMediaNode(node_id)->setActivity(value);}
    int getNumberOfDisplayedNodes();
    void setNumberOfDisplayedNodes(int nd);
    int getNumberOfMediaNodes();
    bool initializeNode(long mediaId, ACBrowserMode _mode = AC_MODE_CLUSTERS);

    // == Pointers
    int getNumberOfPointers();
    ACPointerType getPointerTypeFromIndex(int i);
    ACPointerType getPointerTypeFromId(int i);
    ACPointer* getPointerFromIndex(int i); // for use when parsing pointers incrementally
    ACPointer* getPointerFromId(int i); // for use when parsing pointers from the ID set by the input device
    std::list<int> getPointerIds();
    void resetPointers();
    void addPointer(int p_id,ACPointerType _pointerType=AC_POINTER_UNKNOWN);
    void removePointer(int p_id);
    void addMousePointer();
    void removeMousePointer();
    bool hasMousePointer();

    std::map<long int, float>& getDistanceMouse(){return distance_mouse;}
    void setDistanceMouse(std::map<long int, float>& _distance_mouse){distance_mouse = _distance_mouse;}

    // == Labels
    void setClickedLabel(int ilabel);
    int getClickedLabel(){return mClickedLabel; }

    // = States : AC_ IDLE or AC_CHANGING (i.e., from current to next position)
    double getFrac() const {return mFrac;} // fraction between current and next position
    ACBrowserState getState() const {return mState;}
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
    void setAutoPlay(int auto_play) { this->auto_play = auto_play; }
    bool getAutoPlay() { return this->auto_play; }
    void setAutoDiscard(bool status) { this->auto_discard = status; }
    bool getAutoDiscard(){return this->auto_discard;}

    // sources - SD reintroduced 2009 aug 4
    int pickSource(float x, float z);
    void getSourcePosition(int node_id, float* x, float* z);
    void setSourcePosition(float _x, float _z, float* x, float* z);
    // int toggleSourceActivity(float x, float z);
    int toggleSourceActivity(int lid, int type=1); // XS deprecated
    int toggleSourceActivity(ACMediaNode* node, int _activity = 1); // XS new 150310
    int muteAllSources();

    // labels
    int getLabelSize();
    void setLabel(int i, string text, ACPoint pos);
    void setLabel(int i, string text); // without position, hidden
    void addLabel(string text, ACPoint pos);
    void addLabel(string text); // without position, hidden
    string getLabelText(int i);
    ACPoint getLabelPos(int i);
    bool isLabelDisplayed(int i);
    void removeAllLabels();
    void displayAllLabels(bool isDisplayed);

    const vector<ACLabel> &getLabelAttributes() const { return mLabelAttributes; }
    void setLabelPosition(int node_id, float x, float y, float z=0);
    int getNumberOfDisplayedLabels();
    void setNumberOfDisplayedLabels(int nd);
    int getNumberOfLabels(){return mLabelAttributes.size();}

    void setClustersMethodPlugin(ACPlugin* acpl);
    void setNeighborsMethodPlugin(ACPlugin* acpl);
    void setClustersPositionsPlugin(ACPlugin* acpl);
    void setNeighborsPositionsPlugin(ACPlugin* acpl);
    void setVisualisationPlugin(ACPlugin* acpl);

    ACClusterMethodPlugin* getClustersMethodPlugin(){return mClustersMethodPlugin;}
    ACNeighborMethodPlugin* getNeighborsMethodPlugin(){return mNeighborsMethodPlugin;}
    ACClusterPositionsPlugin* getClustersPositionsPlugin(){return mClustersPosPlugin;}
    ACNeighborPositionsPlugin* getNeighborsPositionsPlugin(){return mNeighborsPosPlugin;}

    std::string getActivePluginName(ACPluginType PluginType);

    bool changeClustersMethodPlugin(ACPlugin* acpl);
    bool changeNeighborsMethodPlugin(ACPlugin* acpl);
    bool changeClustersPositionsPlugin(ACPlugin* acpl);
    bool changeNeighborsPositionsPlugin(ACPlugin* acpl);
    //bool changeVisualisationPlugin(ACPlugin* acpl);//CF we need to sort out first what a VisualisationPlugin can contain vs Clusters/Neighbors Method/Positions plugins

    // NB: Proximity Grid moved to plugin
    ACFilteringPlugin* getFilteringPlugin(){return mFilteringPlugin;}
    void setFilteringPlugin(ACPlugin* acpl);
    bool changeFilteringPlugin(ACPlugin* acpl);

    // == Neighbors
    //ACNeighborsManager* getUserLog(); // forbidden, the neighbor manager only manages neighbor ids, not ACMediaNodes
    bool addNeighborNode(long int _parentId, long int _mediaId, int _clickTime);
    bool removeNeighborNode(long int _id);
    bool removeChildrenNeighborNodes(long int _id);
    void dumpNeighborNodes();
    std::list<long int> getNeighborNodeIds();
    std::list<long int> getNeighborNodeIds(long _id);
    long int getParentFromNeighborNode(long _id);
    long int getPreviousSiblingFromNeighborNode(long int _id);
    long int getNextSiblingFromNeighborNode(long int _id);
    long int getFirstChildFromNeighborNode(long int _id);
    long int getLastChildFromNeighborNode(long int _id);
    int getChildCountAtNeighborNode(long int _id);

    // == XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
    void updateDisplay(bool animate=false, int needsCluster=1);//, bool neighborhoods=true);
    
    
    // CF switch navigation mode while navigating
    void switchMode(ACBrowserMode _mode);

    const vector<float> &getFeatureWeights(){return mFeatureWeights;}
    const int &getClusterCount(){return mClusterCount;}
    const vector<FeaturesVector> &getClusterCenter(int i){return mClusterCenters[i];}
    vector<vector<FeaturesVector> > getClusterCenters(){return mClusterCenters;}
    void setClusterCenters(vector<vector<FeaturesVector> > clusterCenters){mClusterCenters = clusterCenters;}
    const vector<int> &getIdNodeClusterCenter(){return mIdNodeClusterCenters;}
    void setIdNodeClusterCenter(vector<int> idNodeClusterCenters){mIdNodeClusterCenters=idNodeClusterCenters;}
    void setUpdateMutex(bool value);
    
    

private: // better not let the ouside world know about internal cooking
    void resetNavigation();
    void resetNavigationLevels(int l=0);
    void resetCamera();
	void normalizePositions();
public:
    // next positions -> current positions
    void commitPositions();

    //XS the update() methods should remain private
    // all what is needed from outside is *updateDisplay*
private:
    bool updateMutex;
    // update positions based on current clustering

    // == Cluster Mode
    void updateClusters(bool animate=false, int needsCluster=1);
    // == Neighbors Mode ==
    void updateNeighborhoods();
    bool initializeUpdateNeighborhoods();

public: // XS TODO so far this one is still called from outside
    void updateNextPositions();

protected:
    ACMediaLibrary *mLibrary; // just a pointer to the library, no "new"
    // state management
    double mRefTime;
    double mFrac;
    ACBrowserState mState;
    ACBrowserLayout mLayout;
    ACBrowserMode mMode;

    int mClickedNode; // valid between mouseDown and mouseUp, otherwise -1
    set<int> mSelectedNodes;
    int mReferenceNode;
    int	mClosestNode;
    int	mLastSelectedNode;

    bool mNeedsDisplay;
    bool mModeChanged;
    //bool mNeedsActivityUpdate;
    vector<int> mNeedsActivityUpdateMedia;
    pthread_mutex_t activity_update_mutex;
    pthread_mutexattr_t activity_update_mutex_attr;
    pthread_mutex_t navigation_update_mutex;
    pthread_mutexattr_t navigation_update_mutex_attr;

    float mViewWidth;
    float mViewHeight;
    float mCenterOffsetX;
    float mCenterOffsetZ;

    float mCameraPosition[2];
    float mCameraZoom;
    float mCameraAngle;

    vector<ACNavigationState>	mBackwardNavigationStates;
    vector<ACNavigationState>	mForwardNavigationStates;

    // XS TODO 1 generalize to tree
    // XS TODO 2 make this vector of pointers
    ACMediaNodes mMediaNodes;
    ACPointers mPointers;
    int mPointersActiveNumber;

    std::map<long int, float> distance_mouse;

    int nbDisplayedNodes;

    ACLabels mLabelAttributes;
    int nbDisplayedLabels;
    int mClickedLabel;

    int mNavigationLevel;

    // XS TODO: make a class clusters
    int mClusterCount;
    //vector<vector <int> >  clusters;
    //vector<vector<vector <float> > > mClusterCenters; // cluster index, feature index, descriptor index
    vector<vector<FeaturesVector> > mClusterCenters; // cluster index, feature index, descriptor index
    vector<int> mIdNodeClusterCenters;
    vector<float> mFeatureWeights; // each value must be in [0,1], important for euclidian distance.
    
    
    int auto_play;
    int auto_play_toggle;
    bool auto_discard;

    ACClusterMethodPlugin* mClustersMethodPlugin;
    ACNeighborMethodPlugin* mNeighborsMethodPlugin;

    ACClusterPositionsPlugin* mClustersPosPlugin;
    ACNeighborPositionsPlugin* mNeighborsPosPlugin;
    ACNoMethodPositionsPlugin* mNoMethodPosPlugin;

    ACFilteringPlugin* mFilteringPlugin;

    ACNeighborsManager* mNeighborsManager;

    long mLastInitializedNodeId;
    //void checkFormLastInitializedNodeId(long);
};

#endif // __ACMEDIABROWSER_H__
