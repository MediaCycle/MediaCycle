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

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    
    gettimeofday(&tv, &tz);
    
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

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

ACMediaBrowser::ACMediaBrowser() {
	
	mViewWidth = 820;
	mViewHeight = 365;
	mCenterOffsetX = mViewWidth / 2;
	mCenterOffsetZ = mViewHeight / 2;
	
	mCameraPosition[0] = 0.0;
	mCameraPosition[1] = 0.0;
	mCameraZoom = 1.0;
	mCameraAngle = 0.0;
	
	mClickedNode = -1;
	mClickedLabel = -1;
	mClosestNode = -1;

	mClusterCount = 5;
	mNavigationLevel = 0;
	
	mState = AC_IDLE;
	mLayout = AC_LAYOUT_TYPE_NONE;
	mMode = AC_MODE_CLUSTERS;
	
	mRefTime = getTime();
	mFrac = 0.0;
	
	mousex = 0.0;
	mousey = 0.0;
	
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
	
	pthread_mutexattr_init(&activity_update_mutex_attr);
	pthread_mutex_init(&activity_update_mutex, &activity_update_mutex_attr);
	pthread_mutexattr_destroy(&activity_update_mutex_attr);	
}

ACMediaBrowser::~ACMediaBrowser() {
	// XS TODO delete mLoopAttributes if vector of pointers <*>
	pthread_mutex_destroy(&activity_update_mutex);
	delete mUserLog;
}

int ACMediaBrowser::getLabelSize() {
	return mLabelAttributes.size();
}

void ACMediaBrowser::setLabel(int i, string text, ACPoint pos) {
	// XS 260310 removed this ugly if : if you want to append, don't set
//	if (mLabelAttributes.size()<=i) {
//		mLabelAttributes.resize(i+1);
//	}
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

void ACMediaBrowser::goBack()
{
	printf("backward\n");
	
	if (mBackwardNavigationStates.size() > 1) {
		mForwardNavigationStates.push_back(mBackwardNavigationStates.back());
		mBackwardNavigationStates.pop_back();
		
		setCurrentNavigationState(mBackwardNavigationStates.back());
		// XSCF 250310 added this
		this->updateNeighborhoods();
		this->updateClusters(true);

	}
}

void ACMediaBrowser::goForward()
{
	printf("forward\n");
	
	if (mForwardNavigationStates.size() > 0) {
		mBackwardNavigationStates.push_back(mForwardNavigationStates.back());
		mForwardNavigationStates.pop_back();
		
		setCurrentNavigationState(mBackwardNavigationStates.back());
		// XSCF 250310 added this
		this->updateNeighborhoods();
		this->updateClusters(true);

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
	std::cout << "mFeatureWeights = " << mFeatureWeights[0] << std::endl;
		std::cout << "mFeatureWeights = " << mFeatureWeights[1] << std::endl;
		std::cout << "mFeatureWeights = " << mFeatureWeights[2] << std::endl;
	// XSCF 250310 removed this
	// updateClusters(true); 
	// setNeedsDisplay(true);
}

float ACMediaBrowser::getWeight(int i){
	if (i < mFeatureWeights.size()){
		return mFeatureWeights[i];
	}
	else{
		std::cerr << "getWeight : Index of weight out of bound" << std::endl;
		exit(1);
	}
}

void ACMediaBrowser::setClusterNumber(int n)
{
	mClusterCount = n;
	// XSCF 250310 removed this
//	updateClusters(true);
//	setNeedsDisplay(true);
}

void ACMediaBrowser::setClickedNode(int inode){
	if (inode < -1 || inode >= this->getNumberOfMediaNodes())
		cerr << "<ACMediaBrowser::setClickedNode> : index " << inode << " out of bounds (nb node = " << this->getNumberOfMediaNodes() << ")"<< endl;
	else{
		mClickedNode = inode;
		mUserLog->clickNode(inode, 0);//CF put some machine time in here!
	}
}

void ACMediaBrowser::setClickedLabel(int ilabel){
	if (ilabel < -1 || ilabel >= this->getNumberOfLabels())
		cerr << "<ACMediaBrowser::setClickedLabel> : index " << ilabel << "out of bounds" << endl;
	else
		mClickedLabel = ilabel;
}

// XS 250310 was: setNodePosition
void ACMediaBrowser::setNodeNextPosition(int node_id, float x, float y, float z){
	if (node_id>=0 && node_id < this->getNumberOfMediaNodes()) {
		this->getMediaNode(node_id).setNextPosition(x,y,z);
	}	
	else {
		cerr << "ACMediaBrowser::setNodeNextPosition : wrong node ID:" << node_id << endl;
	}
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
	
	// XS TODO: which if goes first ? do we still want to reset if we have a wrong loop_index ?
	if (mNavigationLevel==0)
		resetLoopNavigationLevels();
	
	if(!(loopIndex >= 0 && loopIndex < n))  return;
	
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
	state.mReferenceNode = mReferenceNode;
	state.mNavigationLevel = mNavigationLevel;
	state.mFeatureWeights = mFeatureWeights;
	
	return state;
}


void ACMediaBrowser::setCurrentNavigationState(ACNavigationState state)
{
	mReferenceNode = state.mReferenceNode;
	mNavigationLevel = state.mNavigationLevel;
	mFeatureWeights = state.mFeatureWeights;
	
	// XSCF 250310 removed this
//	updateNeighborhoods();
//	updateClusters(true);
}

ACBrowserLayout ACMediaBrowser::getLayout()
{
	return mLayout;
}

void ACMediaBrowser::setLayout(ACBrowserLayout _layout)
{
	mLayout = _layout;
}

ACBrowserMode ACMediaBrowser::getMode()
{
	return mMode;
}

void ACMediaBrowser::setMode(ACBrowserMode _mode)
{
	mMode = _mode;
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

void ACMediaBrowser::randomizeNodePositions(){
	if(mLibrary == NULL) return;
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		(*node).setCurrentPosition (ACRandom() * mViewWidth, 
									ACRandom() * mViewHeight, 
									0);
		(*node).setNextPosition((*node).getCurrentPositionX() + ACRandom() * mViewWidth / 100.0, 
								(*node).getCurrentPositionY() + ACRandom() * mViewHeight / 100.0, 
								0);
	}	
}

void ACMediaBrowser::libraryContentChanged() {
	// update initial positions 
	// previously: resize other vector structures dependent on loop count.
	
	// XS 150310 TODO: check this one
	//if (this->getMode() == AC_MODE_CLUSTERS)
		initializeNodes(1); // media_ID = node_ID

	if(mLibrary == NULL) return;
	else if(mLibrary->isEmpty()) {
		
		// Reset the browser settings when cleaning the library
		mCameraPosition[0] = 0.0;
		mCameraPosition[1] = 0.0;
		mCameraZoom = 1.0;
		mCameraAngle = 0.0;
		
		//CF we need more than setCameraRecenter()
		mClickedNode = -1;
		mClickedLabel = -1;
		mClosestNode = -1;
		//mClusterCount = 5; //CF might be previously set by apps
		mNavigationLevel = 0;
		
		resetLoopNavigationLevels();
		
		setNeedsDisplay(true);
		return;
	}
	
	// XS TODO randomiwe positions only at the beginning...
	if (mVisPlugin==NULL && mPosPlugin==NULL) {	
		for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
			(*node).setCurrentPosition (ACRandom(), 
										ACRandom(), 
										ACRandom() / 10.0);
			
			(*node).setNextPosition ((*node).getCurrentPositionX() + ACRandom() / 100.0,
									 (*node).getCurrentPositionY() + ACRandom() / 100.0, 
									 (*node).getCurrentPositionZ() + ACRandom() / 100.0);		
			(*node).setDisplayed (true);
		}	
	}
	
	//mFrac = 0.0f; //CF
	//this->updateState(); //CF
	//setNeedsDisplay(true);//CF
	
	// XS what if all media don't have the same number of features as the first one ?
	int fc = mLibrary->getMedia(0)->getNumberOfFeaturesVectors();
	mFeatureWeights.resize(fc);
	
	// XS if (config_file)...
	printf("setting all feature weights to 1.0 (count=%d)\n", (int) mFeatureWeights.size());
	for(int i=0; i<fc; i++) {
		mFeatureWeights[i] = 0.0;//SD temporary hack before config filing
	}
	mFeatureWeights[0] = 1.0;//SD temporary hack before config filing

	// XS 250310 cleaned these 4:
//	updateNeighborhoods();
//	updateClusters(false);
//	updateNextPositions();		
//	setNeedsDisplay(true);
	// into:
	updateDisplay(true);
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

/*
void ACMediaBrowser::setFeatureWeights(vector<float> &weights)
{
	//assert(weights.size() == objects.back().size());
// XS todo check sizes without assert	
	mFeatureWeights = weights;
}
*/

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



// SD TODO - Different clustering algorithms should have their own classes
// SD TODO - DIfferent dimensionality reduction too
// This function make the kmeans and set some varaibles : 
// mClusterCenters
// mLoopAttributes -> ACMediaNode
void ACMediaBrowser::updateClusters(bool animate){
	if (mVisPlugin==NULL && mNeighborsPlugin==NULL)
		updateClustersKMeans(animate);
	else{
		if (mNeighborsPlugin==NULL) {
			initClusterCenters();
			//std::cout << "UpdateClusters : Plugin" << std::endl;
			mVisPlugin->updateClusters(this);
			//XS TODO check this
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
		updateNextPositionsPropeller();
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
	//	setProximityGrid(); // XS change to something like: mGridPlugin->updateNextPositions(this);

}

void ACMediaBrowser::updateNextPositions2dim(){
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
void ACMediaBrowser::updateClustersKMeans(bool animate) {
	int i,j,d,f;
	
	if(mLibrary == NULL) {
		cerr << "<ACMediaBrowser::updateClustersKMeans> : Media Library NULL" << endl;
		return;
	}
	else if(mLibrary->isEmpty()) {
		cerr << "<ACMediaBrowser::updateClustersKMeans> : empty Media Library " << endl;
		return;
	}

	int object_count = mLibrary->getSize();
	
	// XS note: problem if all media don't have the same number of features
	//          but we suppose it is not going to happen
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
	
	// SD TOTO 
	srand(15);
	mClusterCenters.resize(mClusterCount);
	cluster_counts.resize(mClusterCount);
	cluster_accumulators.resize(mClusterCount);
	cluster_distances.resize(mClusterCount);
	
	for(i=0; i<mClusterCount; i++)
	{
		mClusterCenters[i].resize(feature_count);
		cluster_accumulators[i].resize(feature_count);
		
		// initialize cluster center with a randomly chosen object
		int r = rand() % object_count;
		int l = 100;
		
		// TODO SD - Avoid selecting the same twice
		while(l--)
		{
			if(this->getMediaNode(r).getNavigationLevel() >= mNavigationLevel) break;
			else r = rand() % object_count;
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



void ACMediaBrowser::setReferenceNode(int index)
{
	// XS TODO (index >= -1 && index < objects.size());
	
	mReferenceNode = index;

	// XSCF 250310 commented this 
//	pushNavigationState();
//	updateNextPositions();
//	setState(AC_CHANGING);
}

// AM : TODO move this out of core (it's GUI related)
// CF : it should be a core positions component (as opposed to plugin loaded at runtime)
void ACMediaBrowser::updateNextPositionsPropeller(){
	std::cout << "ACMediaBrowser::updateNextPositionsPropeller" <<std::endl;

	if (mLibrary->isEmpty() ) return;
	if (mReferenceNode < 0 || mReferenceNode >= getNumberOfMediaNodes()) return ;


	float r, theta;
	ACPoint p;
	p.x = p.y = p.z = 0.0;
	this->getMediaNode(mReferenceNode).setNextPosition(p);
	
	// srand(1234);
	
	// XS loop on MediaNodes.
	// each MediaNode has a MediaId by which we can access the Media
	
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		int ci = (*node).getClusterId();

		// SD TODO - test both approaches
		r=1;
		r = compute_distance(mLibrary->getMedia(mReferenceNode)->getAllFeaturesVectors(), 
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
		
		//printf("computed next position: theta:%f,r=%f,  (%f %f %f)\n", theta, r, p.x, p.y, p.z);//CF free the console
		
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
		mRefTime = getTime(); //CF instead of TiGetTime();
		mState = state;
		
		printf("state changing to %d", state);
	}
}

void ACMediaBrowser::updateState()
{
	if(mState == AC_CHANGING)
	{
		#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
		double t = getTime();
		//printf("time: %f", t);
		double frac;
		
		//frac = 2.0 * fabs(t - floor(t) - 0.5);
		
		double andur = 1.0;
		frac = (t-mRefTime)/andur;
		
		frac = CUB_FRAC(frac); //CF was mFrac = 
		
		mNeedsDisplay = true;
		
		#define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
		
		mFrac = TI_CLAMP(frac, 0,1);
		
		//gRenderer.updateTransformsFromLibrary(gLibrary, CUB_FRAC(frac));
		//[self updateTransformsFromBrowser:CUB_FRAC(frac)];
		//frac = CUB_FRAC(frac);
		
		printf("frac = %f\n", mFrac);
		//this->commitPositions();//CF
		
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
	std::cout << "Toggle Acitivity of media : " << node.getMediaId() << " to " << _activity << std::endl;
	int mt;
	mt = mLibrary->getMedia(node.getMediaId())->getType();
	if (mt == MEDIA_TYPE_AUDIO) {
		setNeedsActivityUpdateLock(1);
		setNeedsActivityUpdateAddMedia(node.getMediaId()); // XS previously: loop_id
		setNeedsActivityUpdateLock(0);	
	}
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
		
		int mt;
		mt = mLibrary->getMedia(loop_id)->getType();
		if (mt == MEDIA_TYPE_AUDIO) {
			setNeedsActivityUpdateLock(1);
			setNeedsActivityUpdateAddMedia(loop_id);
			setNeedsActivityUpdateLock(0);
		}
		// setNeedsActivityUpdate(1);
											   
		return 1;
	}
	else {
		return 0;
	}	
}

void ACMediaBrowser::setClosestNode(int _node_id) {
	mClosestNode = _node_id; 
	// XS: if _node_id < 0 should we still assign it to closest_node ?
	// note : MediaCycle::pickedObjectCallback will look for closest loop if < 0
	if (_node_id<0) {
		return;
	}
	if (this->getMediaNode(_node_id).getNavigationLevel() < getNavigationLevel()) {
		return;
	}
	
	if (auto_play) {
		// set active modes from 2 to 0
		// set closest from 0 to 2
		// nodes with activity = 1 are left as is
		for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
			if ((*node).getNavigationLevel() >= getNavigationLevel()) {
				if ( ((*node).getNodeId()==mClosestNode) && ((*node).getActivity() == 0) ) {
					toggleSourceActivity(*node, 2);	
				}
				else if ( ((*node).getNodeId()!=mClosestNode) && ((*node).getActivity() == 2) ) {
					toggleSourceActivity(*node);	
				}
			}
		}
		auto_play_toggle = 1;
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
	// XS TODO iterator
	for (int node_id=0;node_id<mLibrary->getSize();node_id++) {
		if (mLoopAttributes[node_id].getActivity() >= 1) {
			// SD TODO - audio engine
			// audio_cycle->getAudioFeedback()->deleteSource(node_id);
			mLoopAttributes[node_id].setActivity(0);
			setNeedsActivityUpdateLock(1);
			setNeedsActivityUpdateAddMedia(node_id);
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

void ACMediaBrowser::setNeedsActivityUpdateAddMedia(int node_id) {
	mNeedsActivityUpdateMedia.push_back(node_id);
	// mNeedsActivityUpdate = 1;
}

void ACMediaBrowser::setNeedsActivityUpdateRemoveMedia() {
	mNeedsActivityUpdateMedia.resize(0);
	// mNeedsActivityUpdate = 1;
}

vector<int>* ACMediaBrowser::getNeedsActivityUpdateMedia() {
	return &mNeedsActivityUpdateMedia;
}

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
	if  (_defaultNodeId == 0){
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
			//std::cout << "Media Id : " << n << std::endl;//CF free the console
			ACMediaNode mn(n,n);
			mLoopAttributes.push_back(mn); // XS generalize
		}
	}
}

// XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
void ACMediaBrowser::updateDisplay(bool animate, bool neighborhoods){
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			//XS TODO check this
			if(animate) {
				setState(AC_CHANGING);
			}
			if (mVisPlugin != NULL){
				mVisPlugin->updateNextPositions(this);
			}
			else {
				updateClustersKMeans(animate); // = neighborhood
				updateNextPositionsPropeller(); // = positions
			}
			break;
		case AC_MODE_NEIGHBORS:
			
			/*
			if (mPosPlugin != NULL){
				mPosPlugin->updateNextPositions(this);
				//XS TODO check this
				if(animate) {
					setState(AC_CHANGING);
				}
			}
			*/
			if (neighborhoods) {
				
			if(animate) {
				setState(AC_CHANGING);
			}
			
			if (mNeighborsPlugin != NULL){
				mNeighborsPlugin->updateNeighborhoods(this);
			}
			else {
				cout << "No neighboorhood plugin set" << endl; // XS default ?
			}
			
			if (mPosPlugin != NULL){
				mPosPlugin->updateNextPositions(this);
			}
			}
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	this->setNeedsDisplay(true);
	
	// TODO: SD/XS check this
//	if (mGridPlugin != NULL){
//		mGridPlugin->setProximityGrid();
//	}
}	