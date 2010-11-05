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
using namespace std;

static double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    
    gettimeofday(&tv, &tz);
    
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
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

// this one is mostly used
// e.g., compute_distance(mLibrary->getMedia(i)->getAllFeaturesVectors(), mClusterCenters[j], mFeatureWeights, false);

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
	
	// XS TODO put the following in a separate function, since it's called by libraryContentChanged too !
	
	mCameraPosition[0] = 0.0;
	mCameraPosition[1] = 0.0;
	mCameraZoom = 1.0;
	mCameraAngle = 0.0;
	
	mClickedNode = -1;
	mClickedLabel = -1;
	mClosestNode = -1;
	mReferenceNode = 0; //CF ugly, could be -1
	mLastSelectedNode = -1;

	mClusterCount = 5;
	mNavigationLevel = 0;
	
	mState = AC_IDLE;
	mLayout = AC_LAYOUT_TYPE_NONE;
	mMode = AC_MODE_CLUSTERS;
	
	// SD 2010 OCT
	/*
	mRefTime = getTime();
	mFrac = 0.0;
	*/
	
	mousex = 0.0;
	mousey = 0.0;
	
	auto_play = 0;
	auto_play_toggle = 0;
	
	mLabelAttributes.resize(0); // XS leave it like this or also make a tree ?
	nbDisplayedLabels = 0;
	
//	mLoopAttributes.resize(0); // XS TODO make this a tree
	nbDisplayedLoops = 20;
	
	mClustersMethodPlugin = NULL;
	mNeighborsMethodPlugin = NULL;
	mClustersPosPlugin = NULL;
	mNeighborsPosPlugin = NULL;
	mVisPlugin = NULL;
	
	mUserLog = new ACUserLog();
	
	pthread_mutexattr_init(&activity_update_mutex_attr);
	pthread_mutex_init(&activity_update_mutex, &activity_update_mutex_attr);
	pthread_mutexattr_destroy(&activity_update_mutex_attr);
	
	prevLibrarySize = 0;
}

ACMediaBrowser::~ACMediaBrowser() {
	// XS TODO delete mLoopAttributes if vector of pointers <*>
	pthread_mutex_destroy(&activity_update_mutex);
	delete mUserLog;
}

int ACMediaBrowser::getLabelSize() {
	return mLabelAttributes.size();
}

void ACMediaBrowser::addLabel(string text, ACPoint pos) {
	ACLabelAttribute tmpLbl;
	tmpLbl.text = text;
	tmpLbl.size = 1.0;
	tmpLbl.pos = pos;
	mLabelAttributes.push_back(tmpLbl);
	return;
}

void ACMediaBrowser::setLabel(int i, string text, ACPoint pos) {
	// TODO : XS 260310 removed this ugly if : if you want to append, don't set
	// (was introduced again to make the dancers app work)
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

void ACMediaBrowser::goBack()
{
#ifdef VERBOSE
	printf("backward\n");
#endif // VERBOSE
	
	if (getMode() == AC_MODE_CLUSTERS){
		if (mBackwardNavigationStates.size() > 0) { // XS: was 1
			// XS way
			mForwardNavigationStates.push_back(this->getCurrentNavigationState());
			setCurrentNavigationState(mBackwardNavigationStates.back());
			mBackwardNavigationStates.pop_back();
			
//			mForwardNavigationStates.push_back(mBackwardNavigationStates.back());
//			mBackwardNavigationStates.pop_back();	
//			setCurrentNavigationState(mBackwardNavigationStates.back());
			// XSCF 250310 added this
			this->updateDisplay(true);
		}
	}	
}

void ACMediaBrowser::goForward()
{
#ifdef VERBOSE
	printf("forward\n");
#endif // VERBOSE
	
	if (getMode() == AC_MODE_CLUSTERS){
		if (mForwardNavigationStates.size() > 0) {
			// XS way
			mBackwardNavigationStates.push_back(this->getCurrentNavigationState());
			setCurrentNavigationState(mForwardNavigationStates.back());
			mForwardNavigationStates.pop_back();
			
//			mBackwardNavigationStates.push_back(mForwardNavigationStates.back());
//			mForwardNavigationStates.pop_back();		
//			setCurrentNavigationState(mBackwardNavigationStates.back());
			// XSCF 250310 added this
			this->updateDisplay(true);
		}
	}	
}

void ACMediaBrowser::storeNavigationState(){
	// e.g., user does 'a' + left-click
	// so we zoom into cluster, but must keep track of the previous state
	// if there was a list of forward states, we will overwrite it (no branching)
	// previously = (sort of) pushNavigationState, which was ambiguous
	mForwardNavigationStates.clear();
	mBackwardNavigationStates.push_back(getCurrentNavigationState());
	
	// XS all items that are at higher navigation states should go back to current navigation state
	// XS might be sort of heavy. 
	int l=this->getNavigationLevel();
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		if ((*node).getNavigationLevel() > l){
			(*node).setNavigationLevel(l);
		}
	}
	
	// XS  TODO check this : do we need
	// this->updateCluster() ou updateDisplay() ?
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

#ifdef VERBOSE
	std::cout << "mFeatureWeights = " << mFeatureWeights[0] << std::endl;
	std::cout << "mFeatureWeights = " << mFeatureWeights[1] << std::endl;
	std::cout << "mFeatureWeights = " << mFeatureWeights[2] << std::endl;
#endif // VERBOSE

	// XSCF 250310 removed this
	// updateClusters(true); 
	// setNeedsDisplay(true);
}

float ACMediaBrowser::getWeight(int i){
	if (i < int(mFeatureWeights.size()) && i >= 0){
		return mFeatureWeights[i];
	}
	else{
		std::cerr << "getWeight : Index of weight out of bounds : " << i << std::endl;
		exit(1);
	}
}

void ACMediaBrowser::setClusterNumber(int n)
{
	if (n>=0)
		mClusterCount = n;
	else
		std::cerr << "<ACMediaBrowser::setClusterNumber> : n has to be > 0" << std::endl;
	// XSCF 250310 removed this
//	updateClusters(true);
//	setNeedsDisplay(true);
}

void ACMediaBrowser::setClickedNode(int inode){
	if (inode < -1 || inode >= this->getNumberOfMediaNodes())
		cerr << "<ACMediaBrowser::setClickedNode> : index " << inode << " out of bounds (nb node = " << this->getNumberOfMediaNodes() << ")"<< endl;
	else{
		mClickedNode = inode;
		if (inode > -1)
		{
			mLastSelectedNode = inode;
			if (mMode == AC_MODE_NEIGHBORS)
				mUserLog->clickNode(inode, 0);//CF put some machine time in here!
		}
	}
}

// returns true if the node is selected, false if not selected
bool ACMediaBrowser::toggleNode(int node){
	
	for (set<int>::const_iterator iter = mSelectedNodes.begin();iter != mSelectedNodes.end();++iter){
		if ((*iter)==node) {
			mSelectedNodes.erase(*iter);
			this->getMediaNode(node).setSelection(false);
			return false;
		}	
	}
		
	mSelectedNodes.insert(node);
	this->getMediaNode(node).setSelection(true);
	mLastSelectedNode = node;
	
	vector<ACMedia*> tmpSegments;
	tmpSegments =  mLibrary->getMedia(this->getMediaNode(node).getMediaId())->getAllSegments();
	for (vector<ACMedia*>::const_iterator iter = tmpSegments.begin(); iter != tmpSegments.end() ; iter++){
		mSelectedNodes.insert((*iter)->getId());
		this->getMediaNode((*iter)->getId()).setSelection(true);		
	}
	return true;
}	

void ACMediaBrowser::dumpSelectedNodes(){
	std::cout << "Selected Nodes: ";
	for (set<int>::const_iterator iter = mSelectedNodes.begin();iter != mSelectedNodes.end();++iter)
		std::cout << *iter << " ";
	std::cout << std::endl;
}	

void ACMediaBrowser::unselectNodes(){
	for (set<int>::const_iterator iter = mSelectedNodes.begin();iter != mSelectedNodes.end();++iter){
		this->getMediaNode(*iter).setSelection(false);
		//mSelectedNodes.erase(*iter);
	}
	mSelectedNodes.clear();
}	

void ACMediaBrowser::setClickedLabel(int ilabel){
	if (ilabel < -1 || ilabel >= this->getNumberOfLabels())
		cerr << "<ACMediaBrowser::setClickedLabel> : index " << ilabel << "out of bounds" << endl;
	else
		mClickedLabel = ilabel;
}

// XS 250310 was: setNodePosition
void ACMediaBrowser::setNodeNextPosition(int node_id, ACPoint p){
	double t = getTime();
	if (node_id>=0 && node_id < this->getNumberOfMediaNodes()) {
		this->getMediaNode(node_id).setNextPosition(p,t);
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
	if (nd < 0 || nd > this->getLibrary()->getSize())
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

void ACMediaBrowser::resetLoopNavigationLevels(int l) {
	// default : l=0
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		(*node).setNavigationLevel (l);
	}	
}

void ACMediaBrowser::incrementLoopNavigationLevels(int loopIndex) {
	int n=getNumberOfMediaNodes(),clusterIndex;
	
	// XS TODO: why this "if" ?
//	if (mNavigationLevel==0)
//		resetLoopNavigationLevels();
	
	// XS TODO: which "if" goes first ? do we still want to reset if we have a wrong loop_index ?
	if(!(loopIndex >= 0 && loopIndex < n))  return;
	
	clusterIndex = this->getMediaNode(loopIndex).getClusterId();	
	if(clusterIndex < 0 || clusterIndex >= mClusterCount) return;
	
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		if ((*node).getClusterId() == clusterIndex){
			//XS TODO vérifier que ça n'incrémente pas 2x celle sur laquelle on a cliqué
			(*node).increaseNavigationLevel();
		}
	}

// I just don't get why this is here: 
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
	return 1;
}


int ACMediaBrowser::setSourceCursor(int lid, int frame_pos) {
	this->getMediaNode(lid).setCursor(frame_pos);
}

void ACMediaBrowser::randomizeNodePositions(){
	if(mLibrary == NULL) return;
	double t = getTime();
	ACPoint p;
	for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
		p.x = ACRandom() * mViewWidth;
		p.y = ACRandom() * mViewHeight;
		p.z = 0;
		p.x = p.x + ACRandom() * mViewWidth / 100.0;
		p.y = p.y + ACRandom() * mViewHeight / 100.0;
		p.z = 0;
		(*node).setNextPosition(p,t);
	}	
}

// XS TODO change this name into something non-passive.
void ACMediaBrowser::libraryContentChanged() {
	// update initial positions 
	// previously: resize other vector structures dependent on loop count.
	
	int librarySize;
	librarySize = mLibrary->getSize();
	
	// XS 150310 TODO: check this one
	initializeNodes(mMode);

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
		mLastSelectedNode = -1;
		//mClusterCount = 5; //CF might be previously set by apps
		mNavigationLevel = 0;
		
		resetLoopNavigationLevels();
		
		setNeedsDisplay(true);
		return;
	}
	
	// XS TODO randomize positions only at the beginning...
	if ( mMode == AC_MODE_CLUSTERS && (librarySize>prevLibrarySize) ) {//(mVisPlugin==NULL && mPosPlugin==NULL) {	
	double t = getTime();
	ACPoint p;
	for (ACMediaNodes::iterator node = mLoopAttributes.begin()+prevLibrarySize; node != mLoopAttributes.end(); ++node){
			/*
			(*node).setCurrentPosition (ACRandom(), 
										ACRandom(), 
										ACRandom() / 10.0);
			(*node).setNextPosition ((*node).getCurrentPositionX() + ACRandom() / 100.0,
									 (*node).getCurrentPositionY() + ACRandom() / 100.0, 
									 (*node).getCurrentPositionZ() + ACRandom() / 100.0);
			 */
		p.x = 0;
		p.y = 0;
		p.z = 0;
		    (*node).setNextPosition (p, t);
			(*node).setCurrentPosition (p);
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
	
	prevLibrarySize = mLibrary->getSize();
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

// SD TODO - DIfferent dimensionality reduction too
// This function make the kmeans and set some varaibles : 
// mClusterCenters
// mLoopAttributes -> ACMediaNode
//CF do we need an extra level of tests along the browsing mode (render inactive during AC_MODE_NEIGHBORS?)
void ACMediaBrowser::updateClusters(bool animate){
	if (mClustersMethodPlugin==NULL && mVisPlugin==NULL){//CF no plugin set, factory settings
		std::cout << "updateNextPositions : Cluster KMeans (default)" << std::endl;
		updateClustersKMeans(animate);
	}	
	else{
		initClusterCenters();
		if (mClustersMethodPlugin) { //CF priority on the Clusters Plugin
			mClustersMethodPlugin->updateClusters(this);
		}
		else if (mVisPlugin) {
			if ( mVisPlugin->getPluginType() == PLUGIN_TYPE_CLUSTERS_PIPELINE || mVisPlugin->getPluginType() == PLUGIN_TYPE_ALLMODES_PIPELINE)
				mVisPlugin->updateClusters(this);
			else
				animate=false;//CF trick to end ACMediaBrowser::updateClusters
		}
		// SD 2010 OCT - removed
		/*
		if(animate) {
			updateNextPositions();
			//commitPositions();
			setState(AC_CHANGING);
		}
		 */
	}
}	

//CF do we need an extra level of tests along the browsing mode (render inactive during AC_MODE_CLUSTERS?)
void ACMediaBrowser::updateNeighborhoods(){
	if (mNeighborsMethodPlugin==NULL && mVisPlugin==NULL)
		std::cout << "No neighboorhood method plugin set" << std::endl; // CF: waiting for a factory one!
	else{
		if (mNeighborsMethodPlugin){
			std::cout << "UpdateNeighborhoods : Plugin" << std::endl;
			mNeighborsMethodPlugin->updateNeighborhoods(this);
		}
		else if (mVisPlugin){
			if ( mVisPlugin->getPluginType() == PLUGIN_TYPE_NEIGHBORS_PIPELINE || mVisPlugin->getPluginType() == PLUGIN_TYPE_ALLMODES_PIPELINE)
				mVisPlugin->updateNeighborhoods(this);
		}
	}
}

//CF do we need an extra level of tests along the browsing mode and plugin types?
void ACMediaBrowser::updateNextPositions(){
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			if (mClustersPosPlugin==NULL && mVisPlugin==NULL) {
				std::cout << "updateNextPositions : Cluster Propeller (default)" << std::endl;
				updateNextPositionsPropeller();
			}	
			else{
				if (mClustersPosPlugin){
					std::cout << "updateNextPositions : Cluster Positions Plugin" << std::endl;
					mClustersPosPlugin->updateNextPositions(this);	
				}	
				else{	
					std::cout << "updateNextPositions : Visualisation Plugin" << std::endl;
					mVisPlugin->updateNextPositions(this);
				}	
			}
			break;
		case AC_MODE_NEIGHBORS:
			if (mNeighborsPosPlugin==NULL && mVisPlugin==NULL)
				std::cout << "No neighboorhood positions plugin set" << std::endl; // CF: waiting for a factory one!
			else{
				if (mNeighborsPosPlugin){
					std::cout << "updateNextPositions : Neighbors Positions Plugin" << std::endl;
					mNeighborsPosPlugin->updateNextPositions(this);	
				}	
				else{	
					std::cout << "updateNextPositions : Visualisation Plugin" << std::endl;
					mVisPlugin->updateNextPositions(this);
				}	
			}
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	//	setProximityGrid(); // XS change to something like: mGridPlugin->updateNextPositions(this);
}


// XS 150310
// . removed assert size
// . included ACMediaNode 
// XS 230810 removed inv_weight (not used)
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
		
	vector< int > 			cluster_counts;
	vector<vector<vector <float> > >cluster_accumulators; // cluster, feature, desc
	vector< float > 		cluster_distances; // for computation
	
	// picking random object as initial cluster center
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
		// SD OCT 2010 - for gradual appearance of media to be more stable
		r = i % object_count;
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
	for (unsigned int fw=0; fw < mFeatureWeights.size(); fw++)
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
			// note: the following "if" skips to next i if true.
			if(this->getMediaNode(i).getNavigationLevel() < mNavigationLevel) continue;
			
			// compute distance between this object and every cluster
			for(j=0; j<mClusterCount; j++)
			{
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
		// SD 2010 OCT
		/*
		updateNextPositions();
		//commitPositions();
		
		setState(AC_CHANGING);
		 */
	}
}



void ACMediaBrowser::setReferenceNode(int index)
{
	// XS TODO (index >= -1 && index < objects.size());
	
	mReferenceNode = index;
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
	double t = getTime();
	this->getMediaNode(mReferenceNode).setNextPosition(p, t);
	
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
		double t = getTime();
		(*node).setNextPosition(p, t);
	}
	
	// printf("PROPELER \n");
	
	setNeedsDisplay(true);
}

void ACMediaBrowser::commitPositions()
{	
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
		
		printf("state changing to %d \n", state);
	}
}

// SD 2010 OCT
void ACMediaBrowser::updateState()
{
	mNeedsDisplay = true;
	
	return;
	
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
		
		//printf("frac = %f\n", mFrac);
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
	std::cout << "Toggle Activity of media : " << node.getMediaId() << " to " << _activity << std::endl;
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
		//if (mLoopAttributes[node_id].getActivity() >= 1) {
			// SD TODO - audio engine
			// audio_cycle->getAudioFeedback()->deleteSource(node_id);
			mLoopAttributes[node_id].setActivity(0);
			setNeedsActivityUpdateLock(1);
			setNeedsActivityUpdateAddMedia(node_id);
			setNeedsActivityUpdateLock(0);
		//}
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
void ACMediaBrowser::initializeNodes(ACBrowserMode _mode) { // default = AC_MODE_CLUSTERS
	// makes an ACMediaNode for each Media in the library
	
	int librarySize;
	//prevLibrarySize;
	
	librarySize = mLibrary->getSize();
	if (librarySize<prevLibrarySize) {
		mLoopAttributes.resize(librarySize); // XS TODO if this is a vector of pointers it should be deleted properly	
	}
	switch ( _mode ){
		case AC_MODE_CLUSTERS:
			for (int i=prevLibrarySize; i<librarySize;i++){
				int n= mLibrary->getMedia(i)->getId();
				//ACMediaNode* mn = new ACMediaNode(n,n);
				//std::cout << "Media Id : " << n << std::endl;//CF free the console
				// nodeID = mediaID
				ACMediaNode mn(n,n);
				mLoopAttributes.push_back(mn); // XS generalize
			}
			break;
		case AC_MODE_NEIGHBORS:
			for (int i=prevLibrarySize; i<librarySize;i++){
				//ACMediaNode* mn = new ACMediaNode(0,mLibrary->getMedia(i)->getId());
				// nodeID = 0;
				ACMediaNode mn(0,mLibrary->getMedia(i)->getId());
				mLoopAttributes.push_back(mn); // XS generalize
			}
			break;
		default:
			cerr << "unknown browser mode: " << _mode << endl;
			break;
	}
}

// XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
// SD 2010 OCT - removed severa lines of codes, as was duplicate with updateClusters and updateNextPositions
void ACMediaBrowser::updateDisplay(bool animate) {
	
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			
			updateClusters(animate);
			
			updateNextPositions();
			
			break;
		case AC_MODE_NEIGHBORS:
		
			updateNeighborhoods();
			
			updateNextPositions();
			
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

//CF to debug with all scenarios! Plugins and OSG (tree nodes should not be colored) might need some tweaking...
void ACMediaBrowser::switchMode(ACBrowserMode _mode){
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			switch ( _mode ){
				case AC_MODE_CLUSTERS:
					break;
				case AC_MODE_NEIGHBORS:
					if ( getLibrary()->getSize() > 0 ) {
						//CF do we have to clean the navigation states?
						//CF do we have to reset the referent node? mUserLog->addRootNode( mReferenceNode , 0); //CF change click
						//(2nd arg)!, use LastClickedNode instead of ReferenceNode?
						//CF 1) bring the nodes to the center
						double t = getTime();
						ACPoint p;
						for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){	
							//(*node).setDisplayed (false);
							p.x = 0;
							p.y = 0;
							p.z = 0;
							(*node).setNextPosition(p, t);
						}
						this->updateDisplay(true);
						/*
						this->setState(AC_CHANGING);
						this->commitPositions();
						this->setNeedsDisplay(true);
						*/
						
						//CF 2) hide all nodes, change mode and make the reference node appear
						for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){	
							(*node).setDisplayed (false);
						}	
						this->setMode(_mode);
						this->updateDisplay(true);
						
						//CF 3) expand the first branch at the reference node
						mUserLog->clickNode(0,0);//CF check if the ref node is correct everytime this way (1 arg), change clicktime (2nd arg)
						this->updateDisplay(true);
					}	
					else
						this->setMode(_mode);
					break;
				default:
					cerr << "unknown browser mode: " << mMode << endl;
					break;
			}	
			break;
		case AC_MODE_NEIGHBORS:
			switch ( _mode ){
				case AC_MODE_CLUSTERS:
					//CF 1) Move all nodes to the center
					double t = getTime();
					ACPoint p;
					for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){	
						p.x = 0;
						p.y = 0;
						p.z = 0;
						(*node).setNextPosition(p, t);
					}
					this->setState(AC_CHANGING);
					this->setNeedsDisplay(true);
					
					//CF 2) Recreate the user log, links should thus disappear
					delete mUserLog;
					mUserLog = new ACUserLog();
					
					//CF 3) Change the mode and display all the nodes
					this->setMode(_mode);
					for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){	
						(*node).setDisplayed (true);
						//(*node).setNextPosition(0.0, 0.0, 0.0);
					}
					this->updateDisplay(true);
					break;
				case AC_MODE_NEIGHBORS:
					break;
				default:
					cerr << "unknown browser mode: " << mMode << endl;
					break;
			}
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;	
	}	
}

bool ACMediaBrowser::changeClustersMethodPlugin(ACPlugin* acpl)
{
	bool success = false;
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			if (acpl) {
				this->setClustersMethodPlugin(acpl);
			}	
			else
				mClustersMethodPlugin = NULL;
			if (getLibrary()->getSize() > 0)
				this->updateDisplay(true);
			success = true;
			break;
		case AC_MODE_NEIGHBORS:
			if (acpl) {
				this->setClustersMethodPlugin(acpl);
			}	
			else
				mClustersMethodPlugin = NULL;
			success = true;
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	return success;	
}

bool ACMediaBrowser::changeNeighborsMethodPlugin(ACPlugin* acpl)
{
	bool success = false;
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			this->setNeighborsMethodPlugin(acpl);
			success = true;
			break;
		case AC_MODE_NEIGHBORS:	
			//CF 1) Move all nodes to the center
			double t = getTime();
			ACPoint p;
			for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){	
				p.x = 0;
				p.y = 0;
				p.z = 0;
				(*node).setNextPosition(p, t);
			}
			this->setState(AC_CHANGING);
			this->setNeedsDisplay(true);
			
			//CF 2) Recreate the user log
			delete mUserLog;
			mUserLog = new ACUserLog();
			
			//CF 3) Change the plugin and update the display, it will make the reference node appear:
			this->setNeighborsMethodPlugin(acpl);
			if (mNeighborsPosPlugin != NULL && getLibrary()->getSize() > 0)
			{	
				this->updateDisplay(true);
			
			//CF 4) Expand the first branch at the reference node
				mUserLog->clickNode(0,0);//CF check if the ref node is correct everytime this way (1 arg), change clicktime (2nd arg)
				this->updateDisplay(true);
			}	
			success = true;
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	return success;
}

bool ACMediaBrowser::changeClustersPositionsPlugin(ACPlugin* acpl)
{
	bool success = false;
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			if (acpl) {
				this->setClustersPositionsPlugin(acpl);
			}	
			else
				mClustersPosPlugin = NULL;
			if (getLibrary()->getSize() > 0){
				setState(AC_CHANGING);
				if (acpl) 
					mClustersPosPlugin->updateNextPositions(this);
				else
					this->updateNextPositionsPropeller();
			}	
			success = true;
			break;
		case AC_MODE_NEIGHBORS:	
			if (acpl)
				this->setClustersPositionsPlugin(acpl);
			else
				mClustersPosPlugin = NULL;
			success = true;
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	return success;	
}

bool ACMediaBrowser::changeNeighborsPositionsPlugin(ACPlugin* acpl)
{
	bool success = false;
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			this->setNeighborsPositionsPlugin(acpl);
			success = true;
			break;
		case AC_MODE_NEIGHBORS:	
			this->setNeighborsPositionsPlugin(acpl);
			if (mNeighborsMethodPlugin != NULL && getLibrary()->getSize() > 0){
				setState(AC_CHANGING);
				mNeighborsPosPlugin->updateNextPositions(this);
			}
			success = true;
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	return success;	
}

/*
bool ACMediaBrowser::changeVisualisationPlugin(ACPlugin* acpl)
{
	bool success = false;
	switch ( mMode ){
		case AC_MODE_CLUSTERS:
			success = true;
			break;
		case AC_MODE_NEIGHBORS:	
			success = true;
			break;
		default:
			cerr << "unknown browser mode: " << mMode << endl;
			break;
	}
	return success;
}
*/
