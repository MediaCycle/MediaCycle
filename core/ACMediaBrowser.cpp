/*
 *  ACMediaBrowser.cpp
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

#include "ACMediaBrowser.h"

// XS : this is for initRandomFeatures() ?
#include <Common/TiMath.h>
#include <Common/TiTime.h>

#include <assert.h>
#include <math.h>
#include <algorithm>
#include <vector>

using namespace std;

// XS TODO this will need to be changed
// once we define a class with methods to compute distances
// ....

static double compute_distance(const vector< vector<float> > &obj1, const vector<vector <float> > &obj2, const vector<float> &weights, bool inverse_features)
{
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	
	int feature_count = obj1.size();
	double dis = 0.0;
	
	/*if(inverse_features)
	 {
	 total_weight = double(feature_count) - total_weight;
	 }*/
	
	int l, r;
	
	for(int f=0; f<feature_count; f++)
	{
		/*
		 if (f==INDEX_RYTHMO) {
		 l=0; r=24;
		 }
		 if (f==INDEX_TIMBRO) {
		 l=1; r=12;
		 }
		 if (f==INDEX_CHROMA) {
		 l=0; r=12;
		 }*/
		
		// Image version
		// XS TODO: does this work for image too ?
		l = 0; r = obj1[f].size();
		
		for(int d=l; d<r; d++)
		{
			float df = obj1[f][d] - obj2[f][d]; 
			
			dis += df*df * (inverse_features?(1.0-weights[f]):weights[f]);
		}
	}
	
	dis = sqrt(dis);
	
	return dis;
}

static double compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f], obj2[f]);
		dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	dis = sqrt(dis);
	
	return dis;
}

static double compute_distance(vector<ACMediaFeatures*> &obj1, const vector<vector <float> > &obj2, const vector<float> &weights, bool inverse_features)
{
	assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
	int feature_count = obj1.size();
	
	double dis = 0.0;
	
	for (int f=0; f<feature_count; f++) {
		//ACEuclideanDistance* E = new ACEuclideanDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
		//FeaturesVector tmp  = obj1[f]->getFeaturesVector();
		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
		dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
		delete E;
	}
	dis = sqrt(dis);
	
	return dis;
}

/*void ACAudioBrowser::initRandomFeatures()
 {
 int i, d;
 
 objects.resize(DEFAULT_OBJECT_COUNT);
 mObjectCluster.resize(DEFAULT_OBJECT_COUNT);
 
 srandom(156);
 for(i=0; i<DEFAULT_OBJECT_COUNT; i++)
 {
 objects[i].resize(DEFAULT_FEATURE_COUNT);
 
 for(d=0; d<DEFAULT_FEATURE_COUNT; d++)
 {
 objects[i][d] = ((float)random()) / (float)((1<<31)-1);
 }
 }
 
 
 mCurrentPos.resize(DEFAULT_OBJECT_COUNT);
 mNextPos.resize(DEFAULT_OBJECT_COUNT);
 
 for(i=0; i<DEFAULT_OBJECT_COUNT; i++)
 {
 mCurrentPos[i].x = TiRandom();
 mCurrentPos[i].y = TiRandom();
 mCurrentPos[i].z = TiRandom() / 10.0;
 
 mNextPos[i].x = mCurrentPos[i].x + TiRandom() / 100.0;
 mNextPos[i].y = mCurrentPos[i].y + TiRandom() / 100.0;
 mNextPos[i].z = mCurrentPos[i].z + TiRandom() / 100.0;
 
 }
 
 
 mFeatureWeights.resize(DEFAULT_FEATURE_COUNT);
 
 for(i=0; i<DEFAULT_FEATURE_COUNT; i++)
 {
 mFeatureWeights[i] = i<6?1.0:0.0;
 }
 }
 */


ACMediaBrowser::ACMediaBrowser() {
	
	mViewWidth = 820;
	mViewHeight = 365;
	mCenterOffsetX = mViewWidth / 2;
	mCenterOffsetZ = mViewHeight / 2;
	
	mCameraPosition[0] = 0.0;
	mCameraPosition[1] = 0.0;
	mCameraZoom = 1.0;
	mCameraAngle = 0.0;
	
	mClickedLoop = -1;
	mClickedLabel = -1;
	
	mClusterCount = 5;
	mNavigationLevel = 0;
	
	mState = AC_IDLE;
	mRefTime = TiGetTime();
	mFrac = 0.0;
	
	mousex = 0.0;
	mousey = 0.0;
	
	closest_loop = -1;
	auto_play = 0;
	auto_play_toggle = 0;
	
	mLabelAttributes.resize(0); // XS leave it like this or also make a tree ?
	nbDisplayedLabels = 0;
	
//	mLoopAttributes.resize(0); // XS TODO make this a tree
	nbDisplayedLoops = 20;
	
	mVisPlugin = NULL;
	mPosPlugin = NULL;
	mNeighborsPlugin = NULL;
	mUserLog = new ACUserLog();
	
	proxgridboundsset = 0;
	
	pthread_mutexattr_init(&activity_update_mutex_attr);
	pthread_mutex_init(&activity_update_mutex, &activity_update_mutex_attr);
	pthread_mutexattr_destroy(&activity_update_mutex_attr);	
}

ACMediaBrowser::~ACMediaBrowser() {
	// XS TODO delete mLoopAttributes
	pthread_mutex_destroy(&activity_update_mutex);
	delete mUserLog;
}

int ACMediaBrowser::getLabelSize() {
	return mLabelAttributes.size();
}

void ACMediaBrowser::setLabel(int i, string text, ACPoint pos) {
	if (mLabelAttributes.size()<=i) {
		mLabelAttributes.resize(i+1);
	}
	mLabelAttributes[i].text = text;
	mLabelAttributes[i].size = 1.0;
	mLabelAttributes[i].pos = pos;
}

string ACMediaBrowser::getLabelText(int i) {
	return mLabelAttributes[i].text;
}

ACPoint ACMediaBrowser::getLabelPos(int i) {
	return mLabelAttributes[i].pos;
}

// memory/context
void ACMediaBrowser::setBack()
{
	printf("backward\n");
	
	if (mBackwardNavigationStates.size() > 1) {
		mForwardNavigationStates.push_back(mBackwardNavigationStates.back());
		mBackwardNavigationStates.pop_back();
		
		setCurrentNavigationState(mBackwardNavigationStates.back());
	}
}

void ACMediaBrowser::setForward()
{
	printf("forward\n");
	
	if (mForwardNavigationStates.size() > 0) {
		mBackwardNavigationStates.push_back(mForwardNavigationStates.back());
		mForwardNavigationStates.pop_back();
		
		setCurrentNavigationState(mBackwardNavigationStates.back());
	}
}

void ACMediaBrowser::setHistory()
{
	// SD TODO
}

void ACMediaBrowser::setBookmark()
{
	// SD TODO
}

void ACMediaBrowser::setTag()
{
	// SD TODO
}

void ACMediaBrowser::setFilterIn()
{
	// SD TODO
}

void ACMediaBrowser::setFilterOut()
{
	// SD TODO
}

void ACMediaBrowser::setFilterSuggest()
{
	// SD TODO
}

void ACMediaBrowser::setWeight(int i, float weight) {
	mFeatureWeights[i] = weight; 
	updateClusters(true); 
	setNeedsDisplay(true);
}

void ACMediaBrowser::setClusterNumber(int n)
{
	// SD TODO	
	mClusterCount = n;
	updateClusters(true);
	setNeedsDisplay(true);
}

void ACMediaBrowser::setClickedLoop(int iloop){
	if (iloop < -1 || iloop >= this->getNumberOfMediaNodes())
		cerr << "<ACMediaBrowser::setClickedLoop> : index " << iloop << " out of bounds (nb loop = " << this->getNumberOfMediaNodes() << ")"<< endl;
	else
		mClickedLoop = iloop;
}

void ACMediaBrowser::setClickedLabel(int ilabel){
	if (ilabel < -1 || ilabel >= this->getNumberOfLabels())
		cerr << "<ACMediaBrowser::setClickedLabel> : index " << ilabel << "out of bounds" << endl;
	else
		mClickedLabel = ilabel;
}

// XS TODO: setNodePosition -- or even NodeNextPosition ??
void ACMediaBrowser::setLoopPosition(int loop_id, float x, float y, float z){
	this->getMediaNode(loop_id).setNextPosition(x,y,z);
}

void ACMediaBrowser::setLabelPosition(int label_id, float x, float y, float z){
// XS todo change this too
	ACPoint p;
	p.x = x;
	p.y = y;
	p.z = z;
	mLabelAttributes[label_id].pos = p;
}

int ACMediaBrowser::getNumberOfDisplayedLoops(){
	return nbDisplayedLoops;
	// should be the same as:	
	//	int cnt=0;
	//	for (int i=0; i < getNumberOfLoops();i++){
	//		if (mLoopAttributes[i].isDisplayed) cnt++;
	//	}
	//	return cnt;
}

int ACMediaBrowser::getNumberOfDisplayedLabels(){
	return nbDisplayedLabels;
}

void ACMediaBrowser::setNumberOfDisplayedLoops(int nd){
	if (nd < 0 || nd > this->getNumberOfMediaNodes())
		cerr << "<ACMediaBrowser::setNumberOfDisplayedLoops> : too many loops to display: " << nd << endl;
	else
		nbDisplayedLoops = nd;
}

void ACMediaBrowser::setNumberOfDisplayedLabels(int nd){
	if (nd < 0 || nd > this->getNumberOfLabels())
		cerr << "<ACMediaBrowser::setNumberOfDisplayedLabels> : too many labels to display: " << nd << endl;
	else
		nbDisplayedLabels = nd;
}

void ACMediaBrowser::resetLoopNavigationLevels() {
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		(*node).setNavigationLevel (0);
	}	
}

void ACMediaBrowser::incrementLoopNavigationLevels(int loopIndex) {
	int n=getNumberOfMediaNodes(),clusterIndex;
	
	// XS which if goes first ?
	if (mNavigationLevel==0)
		resetLoopNavigationLevels();
	
	if(!(loopIndex >= 0 && loopIndex < n)) return;
	
	clusterIndex = this->getMediaNode(loopIndex).getClusterId();	
	if(clusterIndex < 0 || clusterIndex >= mClusterCount) return;
	
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		if ((*node).getClusterId() == clusterIndex){
			(*node).increaseNavigationLevel();
		}
	}
	
	mNavigationLevel++;
}

ACNavigationState ACMediaBrowser::getCurrentNavigationState()
{
	ACNavigationState state;
	
	//state.mNavType = AC_NAV_SELECTION;
	state.mSelectedLoop = mSelectedLoop;
	state.mNavigationLevel = mNavigationLevel;
	state.mFeatureWeights = mFeatureWeights;
	
	return state;
}


void ACMediaBrowser::setCurrentNavigationState(ACNavigationState state)
{
	mSelectedLoop = state.mSelectedLoop;
	mNavigationLevel = state.mNavigationLevel;
	mFeatureWeights = state.mFeatureWeights;
	
	// re-cluster, blabla
	updateNeighborhoods();
	updateClusters(true);
}

void ACMediaBrowser::pushNavigationState()
{
	mForwardNavigationStates.clear();
	
	mBackwardNavigationStates.push_back(getCurrentNavigationState());
}

// SD TODO - This is not used anymore I think
// XS - it is still used by 
//    void ACOsgBrowserEventHandler::hover_callback(float x, float y) {
//        media_cycle->hoverCallback(x, y);
//    }

int ACMediaBrowser::setHoverLoop(int lid, float mx, float my)
{
	int loop_id;
	
	mousex = mx;
	mousey = my;
	
	// In this case, the loops to be played wil be selected according to distance from the mouse pointer in the view
	// audio_cycle->getAudioFeedback()->createDynamicSourcesWithPosition();
	
	loop_id = lid;
	
	if ( (loop_id>=0) && (loop_id<getLibrary()->getSize()) ) {
		this->getMediaNode(loop_id).setHover(1);
	}
	else {
		return 0;
	}
}


int ACMediaBrowser::setSourceCursor(int lid, int frame_pos) {
	this->getMediaNode(lid).setCursor(frame_pos);
}

// XS Node
void ACMediaBrowser::randomizeLoopPositions(){
	if(mLibrary == NULL) return;
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		(*node).setCurrentPosition (TiRandom() * mViewWidth, 
									TiRandom() * mViewHeight, 
									0);
		(*node).setNextPosition((*node).getCurrentPositionX() + TiRandom() * mViewWidth / 100.0, 
								(*node).getCurrentPositionY() + TiRandom() * mViewHeight / 100.0, 
								0);
	}	
}

// XS rename to updateLibrary() ?
void ACMediaBrowser::libraryContentChanged() {
	// update initial positions and resize other vector structures dependent on loop count.
	
	// XS 150310 TODO: check this one
	initializeNodes(1); // media_ID = loop_ID

	
	// XS 27/10/09 TODO this should use the randomizePositions defined above
	if(mLibrary == NULL) return;
	else if(mLibrary->isEmpty()) {
		setNeedsDisplay(true);
		return;
	}
	
	if (mVisPlugin==NULL && mPosPlugin==NULL) {
		for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
			
			(*node).setCurrentPosition (TiRandom(), 
										TiRandom(), 
										TiRandom() / 10.0);
			
			(*node).setNextPosition ((*node).getCurrentPositionX() + TiRandom() / 100.0,
									 (*node).getCurrentPositionY() + TiRandom() / 100.0, 
									 (*node).getCurrentPositionZ() + TiRandom() / 100.0);		
			(*node).setDisplayed (true);
		}
	}
	
	// XS what if all media don't have the same number of features as the first one ?
	int fc = mLibrary->getMedia(0)->getNumberOfFeaturesVectors();
	mFeatureWeights.resize(fc);
	
	// XS if (config_file)...
	printf("setting all feature weights to 1.0 (count=%d)\n", (int) mFeatureWeights.size());
	for(int i=0; i<fc; i++) {
		mFeatureWeights[i] = 0.0;//SD temporary hack before config filing
	}
	mFeatureWeights[0] = 1.0;//SD temporary hack before config filing
	updateNeighborhoods();
	updateClusters(false);
	updateNextPositions();
	
	setNeedsDisplay(true);
}

	
// SD - Brute Force KNN
// SD TODO - Different kNN algorithms should have their own classes
int ACMediaBrowser::getKNN(int id, vector<int> &ids, int k) {
	
	int i, j;
	int el;
	int min_pos;
	double min_distance, max_distance;
	int kcount;
	
	if(mLibrary == NULL) return -1; 
	
	// XS TODO simplify this
	vector<ACMedia*> loops = mLibrary->getAllMedia();
	int object_count = loops.size(); if(object_count == 0) return -1;
	int feature_count = loops.back()->getNumberOfFeaturesVectors();
	assert(mFeatureWeights.size() == feature_count);
	
	double inv_weight = 0.0;
	vector<float> distances;
	
	el = -1;
	for (i=0;i<loops.size();i++) { // XS TODO iterator
		if (id==loops[i]->getId()) {
			el=i;
			break;
		}
	}
	if (el==-1) {
		return 0;
	}
	
	for(i=0; i<feature_count; i++) {
		inv_weight += mFeatureWeights[i];
	}
	if(inv_weight > 0.0) inv_weight = 1.0 / inv_weight;
	else return -1;
	
	distances.resize(object_count);
	
	for (i=0; i<object_count; i++) {		
		distances[i] = compute_distance(loops[el]->getAllFeaturesVectors(), loops[i]->getAllFeaturesVectors(), mFeatureWeights, false);
		if (distances[i]>max_distance) {
			max_distance = distances[i];
		}
	}
	max_distance++;
	distances[el] = max_distance;
	
	kcount = 0;
	for (j=0;j<k;j++) {
		min_distance = max_distance;
		min_pos = -1;
		for (i=0;i<object_count;i++) {	
			if (distances[i]<min_distance) {
				min_distance = distances[i];
				min_pos = i;
			}
		}
		if (min_pos>=0) {
			int tmpid = loops[min_pos]->getId();
			ids.push_back(tmpid);
			distances[min_pos] = max_distance;
			kcount++;
		}
		else {
			break;
		}
	}
	
	return kcount;
}

int ACMediaBrowser::getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k) {
	
    int i, j;
    //int el;
    int min_pos;
    double min_distance, max_distance;
    int kcount;
	
    if (mLibrary == NULL) return -1;
	
	// XS TODO simplify this (same as above)
    vector<ACMedia*> loops = mLibrary->getAllMedia();
    //assert(loops.size() == mLoopAttributes.size());
    int object_count = loops.size();
    if (object_count == 0) return -1;
    int feature_count = loops.back()->getNumberOfFeaturesVectors();
    assert(mFeatureWeights.size() == feature_count);
	
    double inv_weight = 0.0;
    vector<float> distances;
	
    for (i = 0; i < feature_count; i++) {
        inv_weight += mFeatureWeights[i];
    }
    if (inv_weight > 0.0) inv_weight = 1.0 / inv_weight;
    else return -1;
	
    distances.resize(object_count);
	
    for (i = 0; i < object_count; i++) {
        distances[i] = compute_distance(aMedia->getAllFeaturesVectors(), loops[i]->getAllFeaturesVectors(), mFeatureWeights, false);
        if (distances[i] > max_distance) {
            max_distance = distances[i];
        }
    }
    max_distance++;
    //distances[el] = max_distance;
	
    kcount = 0;
    result.clear();
    for (j = 0; j < k; j++) {
        min_distance = max_distance;
        min_pos = -1;
        for (i = 0; i < object_count; i++) {
            if (distances[i] < min_distance) {
                min_distance = distances[i];
                min_pos = i;
            }
        }
        if (min_pos >= 0) {
            //int tmpid = loops[min_pos]->getId();
            result.push_back(loops[min_pos]);
            distances[min_pos] = max_distance;
            kcount++;
        } else {
            break;
        }
    }
	
    return kcount;
}
void ACMediaBrowser::setFeatureWeights(vector<float> &weights)
{
	//assert(weights.size() == objects.back().size());
// XS todo check sizes without assert	
	mFeatureWeights = weights;
}

// XS node, not media
void ACMediaBrowser::setClusterIndex(int nodeIdx,int clusterIdx){
	// XS TODO check bounds
	this->getMediaNode(nodeIdx).setClusterId (clusterIdx);
}

void ACMediaBrowser::setClusterCenter(int clusterIdx, vector< vector<float> > clusterCenter){
	// XS TODO check bounds
	this->mClusterCenters[clusterIdx] = clusterCenter;
}

void ACMediaBrowser::initClusterCenters(){
	vector<ACMedia*> loops = mLibrary->getAllMedia();
	int feature_count = loops.back()->getNumberOfFeaturesVectors();
	int desc_count;
	mClusterCenters.resize(mClusterCount);  
	for(int j=0; j<mClusterCount; j++){
		mClusterCenters[j].resize(feature_count);
		for(int f=0; f<feature_count; f++){
			desc_count = loops.back()->getFeaturesVector(f)->getSize();  
			mClusterCenters[j][f].resize(desc_count);
			for(int d=0; d<desc_count; d++){
				mClusterCenters[j][f][d] = 0;
			}
		}
	}
}

void ACMediaBrowser::setProximityGridQuantize(ACPoint p, ACPoint *pgrid) {
	pgrid->x = (int)round( (p.x-proxgridl)/proxgridstepx );
	pgrid->y = (int)round( (p.y-proxgridb)/proxgridstepy );		
}

void ACMediaBrowser::setProximityGridUnquantize(ACPoint pgrid, ACPoint *p) {
	p->x = pgrid.x * proxgridstepx + proxgridl;
	p->y = pgrid.y * proxgridstepy + proxgridb;		
}

void ACMediaBrowser::setProximityGridBounds(float l, float r, float b, float t) {
	proxgridl = l;
	proxgridr = r;
	proxgridb = b;
	proxgridt = t;
	proxgridboundsset = 1;
}

// XS TODO clean this and sent to plugin
void ACMediaBrowser::setProximityGrid() {
	
	float jitter;
	
	int i, j, k, l;
	int n;
	int found_slot;
	
	ACPoint p, pgrid, p2, curpos;
	int index, pgridindex, curposindex;
	
	float langle, orientation, spiralstepx, spiralstepy, lorientation;
	
	n = getNumberOfMediaNodes(); // mLoopAttributes.size(); // XS TODO getsize
	
	// Proximity Grid Size	
	if (!proxgridboundsset) {
		if (n>0) {
			p = getMediaNode(0).getNextPosition();
			proxgridl = p.x;
			proxgridr = p.x;
			proxgridb = p.y;
			proxgridt = p.y;
		}
		for(i=1; i<n; i++) {
			p = getMediaNode(i).getNextPosition();
			if (p.x<proxgridl) {
				proxgridl = p.x;
			}
			if (p.x>proxgridr) {
				proxgridr = p.x;
			}
			if (p.y<proxgridb) {
				proxgridb = p.y;
			}
			if (p.y>proxgridt) {
				proxgridt = p.y;
			}
		}
	}
		
	// Proximity Grid Density
	proxgridlx = 2;
	proxgridstepx = (proxgridr-proxgridl)/(proxgridlx-1);
	proxgridaspectratio = 9.0/16.0;
	proxgridstepy = proxgridstepx * proxgridaspectratio;
	proxgridly = (proxgridt-proxgridb)/proxgridstepy + 1;	
	proxgridmaxdistance = 2;
	proxgridjitter = 0.25;
	
	// Init
	proxgrid.resize((proxgridlx)*(proxgridly));
	fill(proxgrid.begin(), proxgrid.end(), -1);
	
	// SD TODO - maybe need to compute MST (Minimum Spanning Tree)
	
	for(i=0; i<n; i++) {
		
		found_slot = 0;
		
		p = getMediaNode(i).getNextPosition();
		
		// grid quantization
		setProximityGridQuantize(p, &pgrid);
		pgridindex =  pgrid.y * proxgridlx + pgrid.x;
		
		// no further processing for this media if out of grid
		if ( (pgrid.x<0) || (pgrid.x>=proxgridlx) || (pgrid.y<0) || (pgrid.y>=proxgridly) ) {
			continue;
		}
		
		// spiral search
	    setProximityGridUnquantize(pgrid, &p2);
		langle = atan( (p.y-p2.y) / (p.x-p2.x) );
		orientation = fmod(ceil(langle/(M_PI/4.0)), 2) * 2 - 1; // +1 means positive angles
		spiralstepx = fmod(round((langle-M_PI/2.0)/(M_PI/2.0)), 2);
		if ((p.x-p2.x)<0) spiralstepx = - spiralstepx;
		spiralstepy = fmod(round(langle/(M_PI/2.0)), 2);
		if ((p.y-p2.y)<0) spiralstepy = - spiralstepy;
		lorientation = atan(spiralstepy/spiralstepx);
		
		curpos.x = pgrid.x;
		curpos.y = pgrid.y;
		curposindex = curpos.y * proxgridlx + curpos.x;
		if (proxgrid[curposindex]==-1) {
			l = proxgridmaxdistance;
			found_slot = 1;
		}
		else {
			l = 1;
		}
		
		while (l<proxgridmaxdistance) {
			for (j=0;j<2;j++) {	
				for (k=0;k<l;k++) {
					curpos.x += spiralstepx;
					curpos.y += spiralstepy;
					curposindex = curpos.y * proxgridlx + curpos.x;
					if ( (curpos.x<0) || (curpos.x>=proxgridlx) || (curpos.y<0) || (curpos.y>=proxgridly) ) {
						continue;
					}
					else {
						if (proxgrid[curposindex]==-1) {
							l=proxgridmaxdistance;
							found_slot = 1;
							k=l;
							j=2;
						}
					}
				}
				// minus PI or plus PI depending on orientation
				lorientation += (orientation * M_PI / 2.0);
				lorientation = fmod((double)lorientation, 2.0*M_PI);
				spiralstepx = cos(lorientation);
				spiralstepy = sin(lorientation);
			}	
			l++;
		}
		
		// empty stategy
		if (found_slot) {
			proxgrid[curposindex] = i;
		}
		
		// swap strategy
		// SD TODO
		
		// bump strategy
		// SD TODO
		
	}
	
	// XS TODO iter
	for(i=0; i<n; i++) {
		getMediaNode(i).setNextPositionGrid (getMediaNode(i).getNextPosition());
	}
	
	for(i=0; i<proxgrid.size(); i++) {
		index = proxgrid[i];
		if ( (index>=0) && (index<n) ) {
			curpos.x = fmod((float)i,proxgridlx);
			curpos.y = floor((float)i/(proxgridlx));
			setProximityGridUnquantize(curpos, &p2);
			p2.z = getMediaNode(index).getNextPosition().z;
			getMediaNode(index).setNextPositionGrid (p2);
		}
	}
	
	// XS TODO iter
	for(i=0; i<n; i++) {
		getMediaNode(i).setNextPosition(getMediaNode(i).getNextPositionGrid());
	}
	
	if (proxgridjitter>0) {
		for(i=0; i<n; i++) {
			// XS heavy ?
			jitter = TiRandom()-0.5;
			mLoopAttributes[i].setNextPositionX( mLoopAttributes[i].getNextPositionX() +
												jitter*proxgridjitter*proxgridstepx);
			jitter = TiRandom()-0.5;
			mLoopAttributes[i].setNextPositionY( mLoopAttributes[i].getNextPositionY() + 
												jitter*proxgridjitter*proxgridstepy);
		}
		for(i=0; i<n; i++) {
			mLoopAttributes[i].setNextPositionX( max(min(mLoopAttributes[i].getNextPositionX(),proxgridr), proxgridl));
			mLoopAttributes[i].setNextPositionY( max(min(mLoopAttributes[i].getNextPositionY(),proxgridt), proxgridb));
		}
	}

	
	return;
}


void ACMediaBrowser::setRepulsionEngine() {
	return;
}

// SD TODO - Different clustering algorithms should have their own classes
// SD TODO - DIfferent dimensionality reduction too
// This function make the kmeans and set some varaibles : 
// mClusterCenters
// mLoopAttributes -> ACMediaNode
void ACMediaBrowser::updateClusters(bool animate){
	if (mVisPlugin==NULL && mNeighborsPlugin==NULL)
		kmeans(animate);
	else{
		if (mNeighborsPlugin==NULL) {
			initClusterCenters();
			std::cout << "UpdateClusters : Plugin" << std::endl;
			mVisPlugin->updateClusters(this);
			if(animate) {
				updateNextPositions();
				//commitPositions();
				setState(AC_CHANGING);
			}
			
		}
	}
}	

void ACMediaBrowser::updateNeighborhoods(){
	if (mNeighborsPlugin==NULL)
		std::cout << "No neighboorhood plugin set" << std::endl; // CF: waiting for one!
	else{
		std::cout << "UpdateNeighborhoods : Plugin" << std::endl;
		mNeighborsPlugin->updateNeighborhoods(this);
	}
}

void ACMediaBrowser::updateNextPositions(){
	if (mVisPlugin==NULL && mPosPlugin==NULL)
		setNextPositionsPropeller();
	else{
		if (mPosPlugin){
			std::cout << "updateNextPositions : Positions Plugin" << std::endl;
			mPosPlugin->updateNextPositions(this);	
		}	
		else{	
			std::cout << "updateNextPositions : Visualisation Plugin" << std::endl;
			mVisPlugin->updateNextPositions(this);
		}	
	}
//	setProximityGrid();
}

void ACMediaBrowser::setNextPositions2dim(){
	// XS TODO clean this one !
	if(mLibrary == NULL) 
		return; 
	if(mLibrary->isEmpty()) 
		return;
	
	ACPoint p;
	vector<float> tmpFeatures;
		
	int nbMedia = mLibrary->getSize(); 
	// XS TODO problem if all media don't have the same number of features
	int nbFeature = mLibrary->getMedia(0)->getNumberOfFeaturesVectors();
	int featDim;
	int totalDim = 0;
	assert(mFeatureWeights.size() == nbFeature); // XS TODO change this
	
	// XS why recalculate totaldim ?
	// ACMediaLibrary should know it
	// XS and, you can do a +getSize() instead of ++ !!!
	for(int f=0; f< nbFeature; f++){
		// XS TODO again, problem if all media don't have the same number of features
		featDim = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
		for(int d=0; d < featDim; d++){
			totalDim++;
		}
	}
	assert(totalDim > 1);
	
	tmpFeatures.resize(totalDim);
	std::cout << "Total dimension = " << totalDim << std::endl;
	
	// XS TODO iterator
	for(int i=0; i<nbMedia; i++) {    
		int tmpIdx = 0;
		for(int f=0; f< nbFeature; f++){
			// XS TODO again, problem if all media don't have the same number of features
			featDim = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
			for(int d=0; d < featDim; d++){
				tmpFeatures[tmpIdx] = mLibrary->getMedia(i)->getFeaturesVector(f)->getFeatureElement(d);
				tmpIdx++;
			}
		}
		// DT : Problem if there is less than 2 dims
		p.x = tmpFeatures[1]/10;
		p.y = tmpFeatures[2]/10;
		this->getMediaNode(i).setNextPosition(p);
	}
}

// XS 150310
// . removed assert size
// . included ACMediaNode 
void ACMediaBrowser::kmeans(bool animate) {
	int i,j,d,f;
	
	if(mLibrary == NULL) {
		cerr << "<ACMediaBrowser::kmeans> : Media Library NULL" << endl;
		return;
	}
	else if(mLibrary->isEmpty()) {
		cerr << "<ACMediaBrowser::kmeans> : empty Media Library " << endl;
		return;
	}

	int object_count = mLibrary->getSize();
	
	// XS TODO problem if all media don't have the same number of features
	int feature_count = mLibrary->getMedia(0)->getNumberOfFeaturesVectors();
	double inv_weight = 0.0;
		
	vector< int > 			cluster_counts;
	vector<vector<vector <float> > >cluster_accumulators; // cluster, feature, desc
	vector< float > 		cluster_distances; // for computation
	
	for(i=0; i<feature_count; i++)
	{
		inv_weight += mFeatureWeights[i];
	}
	
	if(inv_weight > 0.0) inv_weight = 1.0 / inv_weight;
	else return;
	
	// picking random object as initial cluster center
	srandom(15);
	mClusterCenters.resize(mClusterCount);
	cluster_counts.resize(mClusterCount);
	cluster_accumulators.resize(mClusterCount);
	cluster_distances.resize(mClusterCount);
	
	for(i=0; i<mClusterCount; i++)
	{
		mClusterCenters[i].resize(feature_count);
		cluster_accumulators[i].resize(feature_count);
		
		// initialize cluster center with a randomly chosen object
		int r = random() % object_count;
		int l = 100;
		
		// TODO SD - Avoid selecting the same twice
		while(l--)
		{
			if(this->getMediaNode(r).getNavigationLevel() >= mNavigationLevel) break;
			else r = random() % object_count;
		}
		
		// couldn't find center in this nav level...
		if(l <= 0) return;
		
		for(f=0; f<feature_count; f++)
		{
			// XS again, what if all media don't have the same number of features ?
			int desc_count = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
			
			mClusterCenters[i][f].resize(desc_count);
			cluster_accumulators[i][f].resize(desc_count);
			
			for(d=0; d<desc_count; d++)
			{
				mClusterCenters[i][f][d] = mLibrary->getMedia(r)->getFeaturesVector(f)->getFeatureElement(d);
				
				//printf("cluster  %d center: %f\n", i, mClusterCenters[i][f][d]);
			}
		}
	}
	
	int n_iterations = 20, it;
	
	printf("feature weights:");
	for (int fw=0; fw < mFeatureWeights.size(); fw++)
		printf("%f ", mFeatureWeights[fw]);
	printf("\n");
	
	// applying a few K-means iterations
	for(it = 0; it < n_iterations; it++)
	{
		// reset accumulators and counts
		for(i=0; i<mClusterCount; i++)
		{
			cluster_counts[i] = 0;
			for(f=0; f<feature_count; f++)
			{
				// XS again, what if all media don't have the same number of features ?
				int desc_count = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
				
				for(d=0; d<desc_count; d++)
				{
					cluster_accumulators[i][f][d] = 0.0;
				}
			}
		}
		
		for(i=0; i<object_count; i++)
		{
			// check if we should include this object
			if(this->getMediaNode(i).getNavigationLevel() < mNavigationLevel) continue;
			
			// compute distance between this object and every cluster
			for(j=0; j<mClusterCount; j++)
			{
				/*float d = 0.0;
				 
				 for(f=0; f<feature_count; f++)
				 {
				 float df = mClusterCenters[j][f] - objects[i][f]; 
				 
				 d += df*df * mFeatureWeights[f] * inv_weight;
				 }
				 
				 d = sqrt(d);
				 
				 
				 cluster_distances[j] = d;
				 */
				
				//cluster_distances[j] = compute_distance(mClusterCenters[j], loops[i].getAllFeaturesVectors(), mFeatureWeights, true);
				cluster_distances[j] = 0;
				cluster_distances[j] = compute_distance(mLibrary->getMedia(i)->getAllFeaturesVectors(), mClusterCenters[j], mFeatureWeights, false);
				
				//printf("distance cluster %d to object %d = %f\n", j, i,  cluster_distances[j]);
			}
			
			
			// pick the one with smallest distance
			int jmin;
			
			jmin = min_element(cluster_distances.begin(), cluster_distances.end()) - cluster_distances.begin();
			
			// update accumulator and counts
			
			cluster_counts[jmin]++;
			this->getMediaNode(i).setClusterId (jmin);
			for(f=0; f<feature_count; f++)
			{
				// XS again, what if all media don't have the same number of features ?
				int desc_count = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
				
				for(d=0; d<desc_count; d++)
				{
					cluster_accumulators[jmin][f][d] += mLibrary->getMedia(i)->getFeaturesVector(f)->getFeatureElement(d);
				}
			}
		}
		
		//printf("%fs, K-means it: %d\n", TiGetTime(), it);
		// get new centers from accumulators
		for(j=0; j<mClusterCount; j++)
		{
			if(cluster_counts[j] > 0)
			{
				for(f=0; f<feature_count; f++)
				{
					// XS again, what if all media don't have the same number of features ?
					int desc_count = mLibrary->getMedia(0)->getFeaturesVector(f)->getSize();
					
					for(d=0; d<desc_count; d++)
					{
						mClusterCenters[j][f][d] = cluster_accumulators[j][f][d] / (float)cluster_counts[j];
					}
				}
			}
			
			//printf("\tcluster %d count = %d\n", j, cluster_counts[j]); 
		}
	}
	
	// AM : TODO move this out of core (it's GUI related)
	if(animate)
	{
		updateNextPositions();
		//commitPositions();
		
		setState(AC_CHANGING);
	}
}



void ACMediaBrowser::setSelectedObject(int index)
{
	//assert(index >= -1 && index < objects.size());
	
	mSelectedLoop = index;
	
	pushNavigationState();
	
	updateNextPositions();
	setState(AC_CHANGING);
}


// AM : TODO move this out of core (it's GUI related)
void ACMediaBrowser::setNextPositionsPropeller(){
	std::cout << "ACMediaBrowser::setNextPositionsPropeller" <<std::endl;

	if (mLibrary->isEmpty() ) return;
	if (mSelectedLoop < 0 || mSelectedLoop >= getNumberOfMediaNodes()) return ;


	float r, theta;
	ACPoint p;
	p.x = p.y = p.z = 0.0;
	this->getMediaNode(mSelectedLoop).setNextPosition(p);
	
	TiRandomSeed(1234);
	
	// XS loop on MediaNodes.
	// each MediaNode has a MediaId by which we can access the Media
	
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		int ci = (*node).getClusterId();

		// SD TODO - test both approaches
		r=1;
		r = compute_distance(mLibrary->getMedia(mSelectedLoop)->getAllFeaturesVectors(), 
							 mLibrary->getMedia((*node).getMediaId())->getAllFeaturesVectors(), 
							 mFeatureWeights, false) * 10.0;
		r /= 100.0;
		theta = 2*M_PI * ci / (float)mClusterCount;
		
		double dt = 1;
		dt = compute_distance(mLibrary->getMedia((*node).getMediaId())->getAllFeaturesVectors(), mClusterCenters[ci], mFeatureWeights, false) / 2.0 * 10.0;
		dt /= 3.0;
		theta += dt;
		
		p.x = sin(theta)*r;
		p.y = cos(theta)*r;
		p.z = 0.0;
		
		printf("computed next position: theta:%f,r=%f,  (%f %f %f)\n", theta, r, p.x, p.y, p.z);
		
		(*node).setNextPosition(p);
	}
	
	setNeedsDisplay(true);
}




void ACMediaBrowser::commitPositions()
{
	int i;
	
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		(*node).commitPosition();
	}
}


void ACMediaBrowser::setState(ACBrowserState state)
{
	if(mState == AC_IDLE)
	{
		mRefTime = TiGetTime();
		mState = state;
		
		printf("state changing to %d", state);
	}
}

void ACMediaBrowser::updateState()
{
#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
	double t = TiGetTime(), frac;
	
	//frac = 2.0 * fabs(t - floor(t) - 0.5);
	
	if(mState == AC_CHANGING)
	{
		double andur = 1.0;
		frac = (t-mRefTime)/andur;
		
		mFrac = CUB_FRAC(frac);
		
		mNeedsDisplay = true;
		//frac = TI_CLAMP(frac, 0,1);
		
		//gRenderer.updateTransformsFromLibrary(gLibrary, CUB_FRAC(frac));
		//[self updateTransformsFromBrowser:CUB_FRAC(frac)];
		
		printf("frac = %f\n", mFrac);
		
		if(t-mRefTime > andur)
		{
			printf("transition to idle\n");
			mState = AC_IDLE;
			this->commitPositions();
		}
	}
	
}

#define PICKSCALE 10
int ACMediaBrowser::pickSource(float _x, float _y)
{
	int loop_id;
	
	loop_id = (int) (floor(_y / PICKSCALE) * floor(mViewWidth / PICKSCALE) + floor(_x / PICKSCALE));
	return loop_id;
}

void ACMediaBrowser::getSourcePosition(int loop_id, float* x, float* z)
{
	float ratio = loop_id / floor(mViewWidth / PICKSCALE);
	*z = floor(ratio) * PICKSCALE
	+ PICKSCALE / 2;
	*x = (ratio - floor(ratio)) * floor(mViewWidth / PICKSCALE) * PICKSCALE
	+ PICKSCALE / 2;
}

void ACMediaBrowser::setSourcePosition(float _x, float _y, float* x, float* z)
{
	*x = _x - mCenterOffsetX;
	*z = _y - mCenterOffsetZ;
}

/*
int ACMediaBrowser::toggleSourceActivity(float _x, float _y)
{
	int loop_id;
	float x, z;
	
	loop_id = pickSource(_x, _y);
	
	toggleSourceActivity(loop_id);
	setSourcePosition(_x, _y, &x, &z);
	
	return loop_id;
	
}
*/

// XS new 150310
// . toggleSourceActivity is in fact in ACMediaNode now
// . browser takes care of threads
int ACMediaBrowser::toggleSourceActivity(ACMediaNode &node, int _activity) {
	node.toggleActivity(_activity);
	setNeedsActivityUpdateLock(1);
	setNeedsActivityUpdateAddMedia(node.getMediaId()); // XS previously: loop_id
	setNeedsActivityUpdateLock(0);	
}

// XS deprecated
int ACMediaBrowser::toggleSourceActivity(int lid, int type)
{
	int loop_id;
	float x, y, z;
	
	loop_id = lid;
	//
	
	if ( (loop_id>=0) && (loop_id<mLibrary->getSize()) )
	{
		if (this->getMediaNode(loop_id).getActivity()==0) {
			this->getMediaNode(loop_id).setActivity(type);			
		}
		else if (this->getMediaNode(loop_id).getActivity() >= 1) {
			this->getMediaNode(loop_id).setActivity(0);
		}
		
		setNeedsActivityUpdateLock(1);
		setNeedsActivityUpdateAddMedia(loop_id);
		setNeedsActivityUpdateLock(0);
		// setNeedsActivityUpdate(1);
											   
		return 1;
	}
	else {
		return 0;
	}	
}

// XS TODO loopId is in fact a nodeId
void ACMediaBrowser::setClosestLoop(int _loop_id) {
	closest_loop = _loop_id; // XS set it here even if < 0 ?
	if (_loop_id<0) {
		return;
	}
	if (this->getMediaNode(_loop_id).getNavigationLevel() < getNavigationLevel()) {
		return;
	}
	
	if (auto_play) {
		// set inactive nodes from 0 to 2
		// set active modes from 2 to 0
		// nodes with activity = 1 are left as is
		for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
			if (((*node).getNavigationLevel() >= getNavigationLevel()) && ((*node).getActivity() != 1 )) {
				toggleSourceActivity(*node, 2);	
			}
			auto_play_toggle = 1;
		}
	}
	// switch to zero the nodes with activity =2
	else if (auto_play_toggle) {
		for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
			if ( (*node).getActivity() == 2 ) {
				toggleSourceActivity(*node);
			}
		}
		auto_play_toggle = 0;
	}
}

	
int ACMediaBrowser::muteAllSources()
{
	int loop_id;
	
	// XS TODO iterator
	for (loop_id=0;loop_id<mLibrary->getSize();loop_id++) {
		if (mLoopAttributes[loop_id].getActivity() >= 1) {
			// SD TODO - audio engine
			// audio_cycle->getAudioFeedback()->deleteSource(loop_id);
			mLoopAttributes[loop_id].setActivity(0);
			setNeedsActivityUpdateLock(1);
			setNeedsActivityUpdateAddMedia(loop_id);
			setNeedsActivityUpdateLock(0);
		}
	}
	
	setNeedsDisplay(true);
	// setNeedsActivityUpdate(1);
}

// Update audio engine sources
void ACMediaBrowser::setNeedsActivityUpdateLock(int i) {
	if (i) {
		pthread_mutex_lock(&activity_update_mutex);
	}
	else {
		pthread_mutex_unlock(&activity_update_mutex);
	}
}

void ACMediaBrowser::setNeedsActivityUpdateAddMedia(int loop_id) {
	mNeedsActivityUpdateMedia.push_back(loop_id);
	// mNeedsActivityUpdate = 1;
}

void ACMediaBrowser::setNeedsActivityUpdateRemoveMedia() {
	mNeedsActivityUpdateMedia.resize(0);
	// mNeedsActivityUpdate = 1;
}

vector<int>* ACMediaBrowser::getNeedsActivityUpdateMedia() {
	return &mNeedsActivityUpdateMedia;
}


// XS NEW 100310
ACMediaNode& ACMediaBrowser::getMediaNode(int i) {
// XS TODO check on bounds
// XS TODO mLoopAttributes is not necessarily a vector anymore --> tree
	return mLoopAttributes[i];
}

// CF prepareNodes ? (cf. OSG)
void ACMediaBrowser::initializeNodes(int _defaultNodeId){ // default = 0
	// makes an ACMediaNode for each Media in the library
	// if _defaultNodeId is set to 1, it will give a nodeID = mediaID
	// otherwize by default nodeID = 0;
	mLoopAttributes.clear(); // XS TODO if this is a vector of pointers it should be deleted properly
	if  (_defaultNodeId = 0){
		for (int i=0; i<mLibrary->getSize();i++){
			//ACMediaNode* mn = new ACMediaNode(0,mLibrary->getMedia(i)->getId());
			ACMediaNode mn(0,mLibrary->getMedia(i)->getId());
			mLoopAttributes.push_back(mn); // XS generalize
		}
	}
	else{
		for (int i=0; i<mLibrary->getSize();i++){
			int n= mLibrary->getMedia(i)->getId();
			//ACMediaNode* mn = new ACMediaNode(n,n);
			ACMediaNode mn(n,n);
			mLoopAttributes.push_back(mn); // XS generalize
		}
	}
}
