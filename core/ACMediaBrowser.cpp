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

double getTime()
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
        //		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f], obj2[f]);
        //		dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
        float temp=obj1[f]->getFeaturesVector().distance(obj2[f]->getFeaturesVector());
        dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
	//	delete E;
    }
    dis = sqrt(dis);

    return dis;
}

// this one is mostly used
// e.g., compute_distance(mLibrary->getMedia(i)->getAllPreProcFeaturesVectors(), mClusterCenters[j], mFeatureWeights, false);

static double compute_distance(vector<ACMediaFeatures*> &obj1, const vector<FeaturesVector> &obj2, const vector<float> &weights, bool inverse_features)
{
    int s1=obj1.size();
    int s2=obj2.size();
    int s3=weights.size();

    assert(obj1.size() == obj2.size()) ;
    assert(obj1.size() == weights.size());
    int feature_count = obj1.size();

    double dis = 0.0;

    for (int f=0; f<feature_count; f++) {
        //ACEuclideanDistance* E = new ACEuclideanDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
        //FeaturesVector tmp  = obj1[f]->getFeaturesVector();
	//	ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
	//	dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
	//	delete E;
        float temp=obj1[f]->getFeaturesVector().distance(obj2[f]);
        dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
    }
    dis = sqrt(dis);

    return dis;
}

ACMediaBrowser::ACMediaBrowser() {
    mViewWidth = 820;
    mViewHeight = 365;
    mCenterOffsetX = mViewWidth / 2;
    mCenterOffsetZ = mViewHeight / 2;

    mUserLog = 0;
    this->clean();

    // XS TODO 1 this assumes cluster mode !
    // XS TODO 2 define const and sync it with GUI:
    mClusterCount = 5;

    // SD 2010 OCT
    /*
 mRefTime = getTime();
 mFrac = 0.0;
 */

    mClustersMethodPlugin = 0;
    mNeighborsMethodPlugin = 0;
    mClustersPosPlugin = 0;
    mNeighborsPosPlugin = 0;
    mNoMethodPosPlugin = 0;

    mUserLog = new ACUserLog();

    pthread_mutexattr_init(&activity_update_mutex_attr);
    pthread_mutex_init(&activity_update_mutex, &activity_update_mutex_attr);
    pthread_mutexattr_destroy(&activity_update_mutex_attr);

    pthread_mutexattr_init(&navigation_update_mutex_attr);
    pthread_mutex_init(&navigation_update_mutex, &navigation_update_mutex_attr);
    pthread_mutexattr_destroy(&navigation_update_mutex_attr);


    this->resetPointers();
}

ACMediaBrowser::~ACMediaBrowser() {
    // XS TODO delete mLoopAttributes if vector of pointers <*>
    pthread_mutex_destroy(&activity_update_mutex);
    pthread_mutex_destroy(&navigation_update_mutex);
    if (mUserLog) delete mUserLog;
}

void ACMediaBrowser::clean(){
    prevLibrarySize = 0;
    auto_play = 0;
    auto_play_toggle = 0;

    mState = AC_IDLE;
    mLayout = AC_LAYOUT_TYPE_NONE;
    mMode = AC_MODE_CLUSTERS; // XS why not NONE ?
    mModeChanged = false;
    mClustersMethodPlugin = 0;
    mClustersPosPlugin = 0;
    mNeighborsMethodPlugin = 0;
    mNeighborsPosPlugin = 0;

    mLabelAttributes.clear(); // XS leave it like this or also make a tree ?
    nbDisplayedLabels = 0;

    mLoopAttributes.clear(); // XS TODO make this a tree ;delete mLoopAttributes if vector of pointers <*>
    nbDisplayedLoops = 0;

    mFeatureWeights.clear();

    this->resetNavigation();
    this->resetCamera();

    if (mUserLog) {
        //mUserLog->dump();
        mUserLog->clean();
    }
    this->resetPointers();
}

int ACMediaBrowser::getLabelSize() {
    return mLabelAttributes.size();
}

void ACMediaBrowser::displayAllLabels(bool isDisplayed){
    for(ACLabels::iterator mLabelAttribute=mLabelAttributes.begin();mLabelAttribute!=mLabelAttributes.end();mLabelAttribute++)
        (*mLabelAttribute).isDisplayed = isDisplayed;
}

void ACMediaBrowser::addLabel(string text, ACPoint pos) {
    ACLabel tmpLbl;
    tmpLbl.text = text;
    tmpLbl.pos = pos;
    mLabelAttributes.push_back(tmpLbl);
    return;
}

void ACMediaBrowser::addLabel(string text) {
    ACLabel tmpLbl;
    tmpLbl.text = text;
    tmpLbl.isDisplayed = false;
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
    mLabelAttributes[i].pos = pos;
}

void ACMediaBrowser::setLabel(int i, string text) {
    // TODO : XS 260310 removed this ugly if : if you want to append, don't set
    // (was introduced again to make the dancers app work)
    if (mLabelAttributes.size()<=i) {
        mLabelAttributes.resize(i+1);
    }
    mLabelAttributes[i].text = text;
    mLabelAttributes[i].isDisplayed = false;
}

string ACMediaBrowser::getLabelText(int i) {
    return mLabelAttributes[i].text;
}

ACPoint ACMediaBrowser::getLabelPos(int i) {
    return mLabelAttributes[i].pos;
}

bool ACMediaBrowser::isLabelDisplayed(int i){
    return mLabelAttributes[i].isDisplayed;
}

void ACMediaBrowser::removeAllLabels(){
    mLabelAttributes.clear();
}

// goes to the previous navigation state
// for the moment, it only makes sense in the AC_MODE_CLUSTERS
void ACMediaBrowser::goBack()
{
#ifdef VERBOSE
    printf("backward\n");
#endif // VERBOSE

    if (getMode() == AC_MODE_CLUSTERS){
        if (mBackwardNavigationStates.size() > 0) {
            mForwardNavigationStates.push_back(this->getCurrentNavigationState());
            setCurrentNavigationState(mBackwardNavigationStates.back());
            mBackwardNavigationStates.pop_back();
            this->updateDisplay(true);
        }
    }
}

// goes to the next navigation state
// for the moment, it only makes sense in the AC_MODE_CLUSTERS
void ACMediaBrowser::goForward()
{
#ifdef VERBOSE
    printf("forward\n");
#endif // VERBOSE

    if (getMode() == AC_MODE_CLUSTERS){
        if (mForwardNavigationStates.size() > 0) {
            mBackwardNavigationStates.push_back(this->getCurrentNavigationState());
            setCurrentNavigationState(mForwardNavigationStates.back());
            mForwardNavigationStates.pop_back();
            this->updateDisplay(true);
        }
    }
}

// e.g., user does 'a' + left-click
// so before zooming into cluster, we must keep track of the previous state
// if there was a list of forward states, we will overwrite it (no branching)
void ACMediaBrowser::storeNavigationState(){
    mForwardNavigationStates.clear();
    mBackwardNavigationStates.push_back(getCurrentNavigationState());

    // all items that are at higher navigation states should go back to current navigation state
    int l=this->getNavigationLevel();
    for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
        if ((*node).getNavigationLevel() > l){
            (*node).setNavigationLevel(l);
        }
    }
}

// zoom into cluster
void ACMediaBrowser::forwardNextLevel() {
    int loop = this->getClickedNode();
    if (loop >= 0) {
        if (this->getMode() == AC_MODE_CLUSTERS) {
            // store first otherwise we store the next state
            this->storeNavigationState();
            this->incrementLoopNavigationLevels(loop);
        }
        // in neighbors mode, the node is already unwrapped with forward down and node clicked
        this->setReferenceNode(loop);
        this->updateDisplay(true);
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

#ifdef VERBOSE
    for (unsigned int i=0; i<mFeatureWeights.size(); i++){
        std::cout << "mFeatureWeights["<<i<<"] = " << mFeatureWeights[i] << std::endl;
    }
#endif // VERBOSE
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
    if (n>0)
        mClusterCount = n;
    else
        std::cerr << "<ACMediaBrowser::setClusterNumber> : n has to be > 0" << std::endl;
}

void ACMediaBrowser::setClickedNode(int inode,int p_id){
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
    if (node_id>=0 ){//&& node_id < this->getNumberOfMediaNodes()) {
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

// XS TODO getsize; this should be the same as mLibrary->getSize(), but this way it is more similar to getNumberOfLabels
// CF not true in non-explatory mode (one loop can be displayed more than once at a time)
int ACMediaBrowser::getNumberOfMediaNodes(){
    long _n = -1;
    switch (mMode){
    case AC_MODE_CLUSTERS:
        _n = mLoopAttributes.size();
        break;
    case AC_MODE_NEIGHBORS:
        _n = mUserLog->getSize();
        break;
    default:
        cerr << "unknown browser mode: " << mMode << endl;
        break;
    }
    //std::cout << "mLoopAttributes.size() " << mLoopAttributes.size() << " mUserLog->getSize() " << mUserLog->getSize() << std::endl;
    return _n;

} // XS TODO getsize; this should be the same as mLibrary->getSize(), but this way it is more similar to getNumberOfLabels // CF not true in non-explatory mode (one loop can be displayed more than once at a time)

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

    // XS I just don't get why this is here:
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

void ACMediaBrowser::hoverWithPointerIndex(float mxx, float myy, int p_index)
{
    ACPoint p;
    p.x = mxx; p.y = myy; p.z = 0.0;
    if (this->getPointerFromIndex(p_index)) {
        this->getPointerFromIndex(p_index)->setCurrentPosition(p);
    }
    else
        std::cerr << "ACMediaBrowser::hoverWithPointerIndex: wrong pointer index " << p_index << std::endl;
}

void ACMediaBrowser::hoverWithPointerId(float mxx, float myy, int p_id)
{
    ACPoint p;
    p.x = mxx; p.y = myy; p.z = 0.0;
    if (this->getPointerFromId(p_id)) {
        this->getPointerFromId(p_id)->setCurrentPosition(p);
    }
    else
    {
        if (p_id==-1)
            this->addPointer(p_id,AC_POINTER_MOUSE);
        else
            this->addPointer(p_id);
        if (this->getPointerFromId(p_id)){
            this->getPointerFromId(p_id)->setCurrentPosition(p);
        }
        else
            std::cerr << "ACMediaBrowser::hoverWithPointerId: wrong pointer id " << p_id << std::endl;
    }
}

void ACMediaBrowser::setSourceCursor(int lid, int frame_pos) {
    this->getMediaNode(lid).setCursor(frame_pos);
}

void ACMediaBrowser::setCurrentFrame(int lid, int frame_pos) {
    this->getMediaNode(lid).setCurrentFrame(frame_pos);
}

void ACMediaBrowser::randomizeNodePositions(){
    if(mLibrary == 0) return;
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
// update initial positions
// previously: resize other vector structures dependent on loop count.
void ACMediaBrowser::libraryContentChanged(int needsCluster) {
    if(mLibrary == 0) return; // put this first otherwize getsize does not work !

    int librarySize = mLibrary->getSize();

    // XS 150310 TODO: check this one
    initializeNodes(mMode);

    if(mLibrary->isEmpty()) {
        this->resetCamera();
        this->resetNavigation();
        this->setNeedsDisplay(true);
        return;
    }

    // XS TODO randomize positions only at the beginning...
    if ( mMode == AC_MODE_CLUSTERS && (librarySize>prevLibrarySize) ) {//(mNoMethodPosPlugin==0 && mPosPlugin==0) {
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

    //if (mFeatureWeights.size()==0)
    if (needsCluster)
        this->initializeFeatureWeights(); //TR NEM modification

    updateDisplay(true, needsCluster);

    prevLibrarySize = mLibrary->getSize();
}

// sets first feature weight to 1, others to 0
// can be changed afterwards, e.g. when reading XML file with config and descriptors
// assumes all media have the same number of features (as the first one)
void ACMediaBrowser::initializeFeatureWeights(){

    //int fc = mLibrary->getMedia(0)->getNumberOfPreProcFeaturesVectors();
    ACMedia *mediaTemp=mLibrary->getMedia(0);
    if (mediaTemp){
        int fc = mLibrary->getMedia(0)->getNumberOfPreProcFeaturesVectors();
        if (mFeatureWeights.size()!=fc){
            mFeatureWeights.resize(fc);
            printf("setting all feature weights to 1.0 (count=%d)\n", (int) mFeatureWeights.size());
            for(int i=0; i<fc; i++) {
                mFeatureWeights[i] = 0.0;
            }
            mFeatureWeights[0] = 1.0;
        }
    }
    else {
        int fc=1;
        mFeatureWeights.resize(fc);
        mFeatureWeights[0] = 1.0;

    }
    cout<<"featureWeight:"<<mFeatureWeights.size()<<endl;

}

// SD - Brute Force KNN
// SD TODO - Different kNN algorithms should have their own classes
int ACMediaBrowser::getKNN(int id, vector<int> &ids, int k) {

    int i, j;
    int el;
    int min_pos;
    double min_distance, max_distance;
    int kcount;

    if(mLibrary == 0) return -1;

    // XS TODO simplify this
    vector<ACMedia*> loops = mLibrary->getAllMedia();
    int object_count = loops.size(); if(object_count == 0) return -1;
    int feature_count = loops.back()->getNumberOfPreProcFeaturesVectors();
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
        if(loops[el]->getType() == mLibrary->getMediaType() && loops[i]->getType() == mLibrary->getMediaType()){//CF multimedia compatibility
            distances[i] = compute_distance(loops[el]->getAllPreProcFeaturesVectors(), loops[i]->getAllPreProcFeaturesVectors(), mFeatureWeights, false);
            if (distances[i]>max_distance) {
                max_distance = distances[i];
            }
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

    if (mLibrary == 0) return -1;

    // XS TODO simplify this (same as above)
    vector<ACMedia*> loops = mLibrary->getAllMedia();
    //assert(loops.size() == mLoopAttributes.size());
    int object_count = loops.size();
    if (object_count == 0) return -1;
    int feature_count = loops.back()->getNumberOfPreProcFeaturesVectors();
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
        if(aMedia->getType() == mLibrary->getMediaType() && loops[i]->getType() == mLibrary->getMediaType()){//CF multimedia compatibility
            distances[i] = compute_distance(aMedia->getAllPreProcFeaturesVectors(), loops[i]->getAllPreProcFeaturesVectors(), mFeatureWeights, false);
            if (distances[i] > max_distance) {
                max_distance = distances[i];
            }
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

// get k first items sorted on feature number f (and its component dim)
// revert = true = lest k items
int ACMediaBrowser::getKSortedOnFeature(int k, int f, int dim, bool revert) {
    ACMediaLibrary *m_lib = this->getLibrary();
    if (m_lib->getSize() == 0) {
        cerr << "<ACMediaBrowser::getKSortedOnFeature> : empty library" << endl;
        return 0;
    }
    if (k >= m_lib->getSize() || k <=0) {
        cerr << "<ACMediaBrowser::getKSortedOnFeature> : wrong k : " << k << endl;
        return 0;
    }

    int nfeatv = m_lib->getMedia(0)->getNumberOfFeaturesVectors();
    if (f >= nfeatv) {
        cerr << "<ACMediaBrowser::getKSortedOnFeature> : wrong feature index : " << f << endl;
        return 0;
    }
    int dimfeatv = m_lib->getMedia(0)->getFeaturesVector(f)->getSize();
    if (dim >= dimfeatv) {
        cerr << "<ACMediaBrowser::getKSortedOnFeature> : wrong feature dimension : " << f << endl;
        return 0;
    }

    std::vector<std::pair<float, int> > feature_to_sort;
    for (int i = 0; i < m_lib->getSize(); i++) {
        feature_to_sort.push_back(std::pair<float, int> (m_lib->getMedia(i)->getFeaturesVector(k)->getFeatureElement(dim), m_lib->getMedia(i)->getId()));
    }

    sort(feature_to_sort.begin(), feature_to_sort.end());
    std::vector<std::pair<float, int> >::const_iterator itr;
    // XS TODO if revert...
    for (int i = 0; i<k; i++) {
        cout << feature_to_sort[i].first << "\t" <<  feature_to_sort[i].second << endl;
    }
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

void ACMediaBrowser::setClusterCenter(int clusterIdx, vector<FeaturesVector > clusterCenter){
    // XS TODO check bounds
    this->mClusterCenters[clusterIdx] = clusterCenter;
}

void ACMediaBrowser::initClusterCenters(){
    vector<ACMedia*> loops = mLibrary->getAllMedia();
    if (loops.size()==0)
        return;
    int feature_count = loops.back()->getNumberOfPreProcFeaturesVectors();
    int desc_count;
    mClusterCenters.resize(mClusterCount);
    for(int j=0; j<mClusterCount; j++){
        mClusterCenters[j].resize(feature_count);
        for(int f=0; f<feature_count; f++){
            desc_count = loops.back()->getPreProcFeaturesVector(f)->getSize();
            mClusterCenters[j][f].resize(desc_count);

            mClusterCenters[j][f].init();

        }
    }
}

// SD TODO - DIfferent dimensionality reduction too
// This function make the kmeans and set some varaibles :
// mClusterCenters
// mLoopAttributes -> ACMediaNode
//CF do we need an extra level of tests along the browsing mode (render inactive during AC_MODE_NEIGHBORS?)
void ACMediaBrowser::updateClusters(bool animate, int needsCluster) {
    setNeedsNavigationUpdateLock(1);
    this->removeAllLabels();
    if (mClustersMethodPlugin==0 && mNoMethodPosPlugin==0){//CF no plugin set, factory settings
        std::cerr << "updateClusters : no clustering plugin set" << std::endl;
    }
    else{//TR TODO cancel the clustering if needCluster ==0
        if (needsCluster)
            initClusterCenters();
        if (mClustersMethodPlugin) { //CF priority on the Clusters Plugin
            mClustersMethodPlugin->updateClusters(this,needsCluster);
        }
        else if (mNoMethodPosPlugin) {
            if ( mNoMethodPosPlugin->implementsPluginType(PLUGIN_TYPE_CLUSTERS_PIPELINE) || mNoMethodPosPlugin->implementsPluginType(PLUGIN_TYPE_ALLMODES_PIPELINE))
            {}	//mNoMethodPosPlugin->updateClusters(this);
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
    // Until we design cross-media browsing, mediadocuments should assign their cluster ID to their children media
    if(this->getLibrary()->getMediaType() == MEDIA_TYPE_MIXED){
        vector<ACMedia*> medias = this->getLibrary()->getAllMedia();
        for (int i=0; i < medias.size(); i++){
            if (medias[i]->getParentId() > -1){
                int locIdCluster=this->getMediaNode(medias[i]->getParentId()).getClusterId();
                this->getMediaNode(medias[i]->getId()).setClusterId(locIdCluster);
            }
        }
    }
    setNeedsNavigationUpdateLock(0);

}

//CF do we need an extra level of tests along the browsing mode (render inactive during AC_MODE_CLUSTERS?)
void ACMediaBrowser::updateNeighborhoods(){
    setNeedsNavigationUpdateLock(1);
    this->removeAllLabels();
    if (mNeighborsMethodPlugin==0 && mNoMethodPosPlugin==0)
        std::cerr << "updateNeighborhoods : No neighboorhood method plugin set" << std::endl; // CF: waiting for a factory one!
    else{
        if (mNeighborsMethodPlugin){
            std::cout << "UpdateNeighborhoods : Plugin" << std::endl;
            mNeighborsMethodPlugin->updateNeighborhoods(this);
        }
        else if (mNoMethodPosPlugin){
            if ( mNoMethodPosPlugin->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_PIPELINE) || mNoMethodPosPlugin->implementsPluginType(PLUGIN_TYPE_ALLMODES_PIPELINE))
            {}//	mNoMethodPosPlugin->updateNeighborhoods(this);
        }
    }
    setNeedsNavigationUpdateLock(0);
}

//CF do we need an extra level of tests along the browsing mode and plugin types?
void ACMediaBrowser::updateNextPositions() {
    setNeedsNavigationUpdateLock(1);
    this->displayAllLabels(false);
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        if (mClustersPosPlugin==0 && mNoMethodPosPlugin==0) {
            std::cerr << "updateNextPositions : no clustering positions plugin set" << std::endl;
        }
        else{
            if (mClustersPosPlugin){
                std::cout << "updateNextPositions : Cluster Positions Plugin" << std::endl;
                mClustersPosPlugin->updateNextPositions(this);
                commitPositions();
            }
            else{
                std::cout << "updateNextPositions : Visualisation Plugin" << std::endl;
                mNoMethodPosPlugin->updateNextPositions(this);
                commitPositions();
            }
        }
        break;
    case AC_MODE_NEIGHBORS:
        if (mNeighborsPosPlugin==0 && mNoMethodPosPlugin==0)
            std::cerr << "updateNextPositions : no neighboorhood positions plugin set" << std::endl; // CF: waiting for a factory one!
        else{
            if (mNeighborsPosPlugin){
                std::cout << "updateNextPositions : Neighbors Positions Plugin" << std::endl;
                mNeighborsPosPlugin->updateNextPositions(this);
                commitPositions();
            }
            else{
                std::cout << "updateNextPositions : Visualisation Plugin" << std::endl;
                mNoMethodPosPlugin->updateNextPositions(this);
                commitPositions();
            }
        }
        break;
    default:
        cerr << "unknown browser mode: " << mMode << endl;
        break;
    }
    setNeedsNavigationUpdateLock(0);
    //	setProximityGrid(); // XS change to something like: mGridPlugin->updateNextPositions(this);
}

void ACMediaBrowser::setReferenceNode(int index)
{
    // XS TODO (index >= -1 && index < objects.size());

    mReferenceNode = index;
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

// . toggleSourceActivity is in fact in ACMediaNode now
// . browser takes care of threads
// XS TODO return value makes no sense
int ACMediaBrowser::toggleSourceActivity(ACMediaNode &node, int _activity) {
    node.toggleActivity(_activity);
#ifdef USE_DEBUG // use debug message levels instead
    //std::cout << "Toggle Activity of media : " << node.getMediaId() << " to " << _activity << std::endl;
#endif
    int mt;
    mt = mLibrary->getMedia(node.getMediaId())->getType();
    if (mt == MEDIA_TYPE_AUDIO) {
        setNeedsActivityUpdateLock(1);
        setNeedsActivityUpdateAddMedia(node.getMediaId()); // XS previously: loop_id
        setNeedsActivityUpdateLock(0);
    }
    return 1;
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


void ACMediaBrowser::setClosestNode(int _node_id, int p_index) {
    std::vector<int> hoveredNodes;
    hoveredNodes.resize(getNumberOfPointers());
    //std::cout << "hoverNodes ndeId=" << _node_id << " p_index=" << p_index << " : ";
    for (int ps = 0; ps < getNumberOfPointers(); ps++){
        hoveredNodes[ps]=getClosestNode(ps);
        //std::cout << hoveredNodes[ps] << " ";
    }
    //std::cout << std::endl;

    //int prev_node_id = getClosestNode(p_index);

    //mClosestNode = _node_id;//CF to deprecate

    ACPointer* p = 0;
    p = this->getPointerFromIndex(p_index);
    if(p)
        p->setClosestNode(_node_id);
    else
        std::cerr << "ACMediaBrowser::setClosestNode: couldn't access pointer with index " << p_index << std::endl;

    // XS: if _node_id < 0 should we still assign it to closest_node ?
    // note : MediaCycle::pickedObjectCallback will look for closest loop if < 0
    if (_node_id<0) {
        return;
    }
    if (this->getMediaNode(_node_id).getNavigationLevel() < getNavigationLevel()) {
        return;
    }

    if (auto_play) {
        // set active nodes from 2 to 0 (CF: 0?)
        // set closest from 0 to 2
        // nodes with activity = 1 are left as is
        for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
            if ((*node).getNavigationLevel() >= getNavigationLevel()) {
                if ( ((*node).getNodeId()==_node_id) && ((*node).getActivity() == 0) ) {
                    // set closest from 0 to 2
                    toggleSourceActivity(*node, 2);
                }
                /*else if ( ((*node).getNodeId()==prev_node_id) && ((*node).getActivity() == 2) ) {
     toggleSourceActivity(*node, 0);
    }*/
                else if ( ((*node).getNodeId()!=_node_id) && ((*node).getActivity() == 2) ) {
                    // set active nodes from 2 to 0
                    int donottoggle = 0;
                    for (int ps = 0; ps < getNumberOfPointers(); ps++){
                        if ( ((*node).getNodeId()==hoveredNodes[ps]) ){
                            donottoggle = 1;
                        }
                    }
                    if ( (!donottoggle)	&& ((*node).getActivity() == 2)  ) {
                        toggleSourceActivity(*node);
                    }
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

// XS TODO iterator + return value makes no sense
int ACMediaBrowser::muteAllSources()
{
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
    return 1;
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
void ACMediaBrowser::setNeedsNavigationUpdateLock(int i) {
    if (i) {
        pthread_mutex_lock(&navigation_update_mutex);
    }
    else {
        pthread_mutex_unlock(&navigation_update_mutex);
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
    /*switch (mMode){
    case AC_MODE_CLUSTERS:
    {*/
        if(i>=0 && i<mLoopAttributes.size())
            return mLoopAttributes[i];
        else
            std::cerr << "ACMediaBrowser::getMediaNode: index " << i << " outside bounds (clusters mode)" << std::endl;
    /*}
    break;
    case AC_MODE_NEIGHBORS:
    {
        if(this->mUserLog){
            if(i>=0) //&& i<mUserLog->getSize())
                return mUserLog->getNodeFromId(i);
            else
                std::cerr << "ACMediaBrowser::getMediaNode: index " << i << " outside bounds (neighbors mode)" << std::endl;
        }
        else
            std::cerr << "ACMediaBrowser::getMediaNode: no user log available" << std::endl;
    }
    break;
    default:
        cerr << "unknown browser mode: " << mMode << endl;
        break;
    }*/
}

ACMediaNode& ACMediaBrowser::getNodeFromMedia(ACMedia* _media) {
    //if(_media->getId() >-1)
    //    return this->getMediaNode(_media->getId()); // mLoopAttributes[_media->getId()];
    int i = _media->getId();

    /*     switch (mMode){
        case AC_MODE_CLUSTERS:
            {*/
    if(i>=0 && i<mLoopAttributes.size())
        return mLoopAttributes[i];
    else
        std::cerr << "ACMediaBrowser::getNodeFromMedia: index " << i << " outside bounds (clusters mode)" << std::endl;
    /*}
            break;
        case AC_MODE_NEIGHBORS:
            {
                if(this->mUserLog){
                    if(i>=0 && i<mUserLog->getSize())
                        return mUserLog->getNodeFromMediaId(i);
                    else
                        std::cerr << "ACMediaBrowser::getNodeFromMedia: index " << i << " outside bounds (neighbors mode)" << std::endl;
                }
                else
                    std::cerr << "ACMediaBrowser::getNodeFromMedia: no user log available" << std::endl;
            }
            break;
        default:
            cerr << "unknown browser mode: " << mMode << endl;
            break;
    }*/
}

int ACMediaBrowser::getNumberOfPointers() {
    return mPointers.size();
}

ACPointerType ACMediaBrowser::getPointerTypeFromIndex(int _index) {
    return this->getPointerFromIndex(_index)->getType();
}

ACPointerType ACMediaBrowser::getPointerTypeFromId(int _id) {	
    return this->getPointerFromId(_id)->getType();
}	

ACPointer* ACMediaBrowser::getPointerFromId(int _id) {


    ACPointers::iterator p_attr_it = mPointers.find(_id);


    if (p_attr_it ==  mPointers.end()){ //new pointer
        /*if (_id==-1)
   this->addPointer(_id,AC_POINTER_MOUSE);
  else
   this->addPointer(_id);
  p_attr_it = mPointers.find(_id);
  if (p_attr_it ==  mPointers.end()) {
   std::cerr << "ACMediaBrowser::getPointerFromId: couldn't get pointer with id " << _id << std::endl;
   return 0;
  }
  else
   return p_attr_it->second;*/
        return 0;//TR Modification. Get function doesn't create pointer.
    }
    else {// existing pointer
        return p_attr_it->second;
    }
}

ACPointer* ACMediaBrowser::getPointerFromIndex(int _index) {
    ACPointers::iterator p_attr_it = mPointers.begin();
    for(int i=0; i<_index;i++)
        p_attr_it++;

    if (p_attr_it != mPointers.end())
        return p_attr_it->second;
    else{
        std::cerr << "ACMediaBrowser::getPointerFromIndex: wrong pointer index " << _index << "/" << mPointers.size() << std::endl;
        return 0;
    }
}

void ACMediaBrowser::resetPointers() {
    ACPointers::iterator it;
    for (it=mPointers.begin();it!=mPointers.end();it++)
        if (it->second!=0)
            delete (it->second);

    mPointers.clear();
    mPointersActiveNumber = 0;
    //CF this initializes the default mouse as first pointer by default for now, might be changed to none
    /*mPointers.insert(mPointers.begin(),ACPointers::value_type(0,new ACPointer()));// MC GUI applications require at least 1 mouse pointer
 mPointers.begin()->second->setText("Mouse");
 mPointers.begin()->second->setType(AC_POINTER_MOUSE);
 mPointersActiveNumber = 1;*/
    //std::cout << "ACMediaBrowser::resetPointers" << std::endl;
}

void ACMediaBrowser::addPointer(int _id,ACPointerType _pointerType) {
    ACPointers::iterator p_iter = mPointers.find(_id);
    if (p_iter ==  mPointers.end()){ //new pointer
        // create the pointer
        std::stringstream id_ss;
        id_ss << _id;

        // temporarily, if new pointer, others might have died
        //this->resetPointers();

        // add it to the pointer list (at the end of the process since the pointer size is checked in the OSG view)
        mPointers.insert(mPointers.end(),ACPointers::value_type(_id,new ACPointer(id_ss.str(),_pointerType)));
        //std::cout << "ACMediaBrowser::addPointer" << _id << "/" << mPointers.size() << std::endl;
    }
    else
        std::cout << "ACMediaBrowser::addPointer: pointer of id " << _id << " already created" << std::endl;
}

void ACMediaBrowser::removePointer(int _id) {
    ACPointers::iterator p_iter = mPointers.find(_id);
    if (p_iter !=  mPointers.end()){ //existing*/
        if (mLibrary->getSize()>0 && p_iter->second){
            //CF we need first to desactivate the closest node of the pointer to be removed, if in audiohover mode
            /*ACMediaNode closest = getMediaNode(p_iter->second->getClosestNode());
   if (closest.getActivity() == 2)
    toggleSourceActivity(closest);*///TR NEM
        }
        if (p_iter->second)
            delete p_iter->second;
        mPointers.erase(p_iter);
    }
    this->mNeedsDisplay=true;
}

void ACMediaBrowser::addMousePointer(){
    this->addPointer(-1, AC_POINTER_MOUSE);
}

void ACMediaBrowser::removeMousePointer(){
    this->removePointer(-1);
}		

// CF prepareNodes ? (cf. OSG)
// makes an ACMediaNode for each Media in the library
// - AC_MODE_CLUSTERS : nodeID = mediaID if the whole Library is used in the Browser
// - AC_MODE_NEIGHBORS : nodeID = 0 initially, then only the neighbors will receive a nodeID

void ACMediaBrowser::initializeNodes(ACBrowserMode _mode) { // default = AC_MODE_CLUSTERS
    int librarySize;

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

ACUserLog* ACMediaBrowser::getUserLog()
{
    return mUserLog;
}

long int ACMediaBrowser::addNode(long int _parentId, long int _mediaId, int _clickTime)
{
    if(this->mMode == AC_MODE_NEIGHBORS){
        if(this->mUserLog){
            long int nodeId = -1;
            nodeId = this->mUserLog->addNode(_parentId, _mediaId, _clickTime);
            /*int librarySize = mLibrary->getSize();
            if( _mediaId > librarySize){
                std::cerr << "ACMediaBrowser::addNode Trying to add neighbor node beyond the library size" << std::endl;
            }*/
            /*if (nodeId == mLoopAttributes.size()){
                ACMediaNode mn(0,_mediaId, _clickTime);
                mLoopAttributes.push_back(mn);
            }*/
            /*else
                std::cerr << "ACMediaBrowser::addNode error: couldn't create related media node." << std::endl;*/
        }
        else
            std::cerr << "ACMediaBrowser::addNode error: no user log." << std::endl;
    }
    else
        std::cerr << "Currently nodes can be added only in neighbors mode." << std::endl;
    return -1;
}

bool ACMediaBrowser::removeNode(long int _id){
    if(this->mMode == AC_MODE_NEIGHBORS){
        if(this->mUserLog){
            return this->mUserLog->removeNode(_id);
        }
    }
    return false;
}

bool ACMediaBrowser::removeChildrenNodes(long int _id){
    if(this->mMode == AC_MODE_NEIGHBORS){
        if(this->mUserLog){
            return this->mUserLog->removeChildrenNodes(_id);
        }
    }
    return false;
}

// XS 260310 new way to manage update of clusters, positions, neighborhoods, ...
// SD 2010 OCT - removed severa lines of codes, as was duplicate with updateClusters and updateNextPositions
void ACMediaBrowser::updateDisplay(bool animate, int needsCluster) {
    this->removeAllLabels();
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
    {
        updateClusters(animate, needsCluster);
        updateNextPositions();
    }
    break;
    case AC_MODE_NEIGHBORS:
    {
        updateNeighborhoods();
        updateNextPositions();
    }
    break;
    default:
        cerr << "unknown browser mode: " << mMode << endl;
        break;
    }
    this->setNeedsDisplay(true);

    // TODO: SD/XS check this
    //	if (mGridPlugin != 0){
    //		mGridPlugin->setProximityGrid();
    //	}
}

//CF to debug with all scenarios! Plugins and OSG (tree nodes should not be colored) might need some tweaking...
void ACMediaBrowser::switchMode(ACBrowserMode _mode){
    if (mMode == _mode) return;

    double t = getTime();

    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        switch ( _mode ){
        case AC_MODE_NEIGHBORS:
            if ( getLibrary()->getSize() > 0 ) {
                //CF do we have to clean the navigation states?
                //CF do we have to reset the referent node? mUserLog->addRootNode( mReferenceNode , 0); //CF change click
                //(2nd arg)!, use LastClickedNode instead of ReferenceNode?

                //CF 1) Bring the nodes to the center
                ACPoint p;
                p.x = 0;
                p.y = 0;
                p.z = 0;
                for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
                    (*node).setCurrentPosition(p);
                    (*node).setNextPosition(p,t);
                }
                this->updateDisplay(true);//this->updateNextPositions();

                //CF 2) Hide all nodes
                for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
                    (*node).setDisplayed(false);
                }
                this->updateDisplay(true);//this->updateNextPositions();

                //CF 3) Recreate the user log
                delete mUserLog;
                mUserLog = 0;
                mUserLog = new ACUserLog();

                //CF 4) Notify the browsing mode change and make the reference node appear
                mUserLog->addRootNode(this->getReferenceNode(), 0); // 0
                this->setMode(_mode);
                this->setModeChanged(true);
                this->updateDisplay(true);
                //this->setModeChanged(false);// CF done by the browser renderer until we implement signal/slots

                //CF 5) Expand the first branch at the reference node
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
        {
            //CF 1) Move all nodes to the center
            //mUserLog->wrapToOrigin();
            //this->updateDisplay(true);//this->updateNextPositions();

            //CF 2) Notify the browsing mode change
            this->setMode(_mode);
            //this->setModeChanged(true);
            //this->updateDisplay(true);

            //CF 3) Recreate the user log, links should thus disappear
            delete mUserLog;
            mUserLog = 0;
            mUserLog = new ACUserLog();

            //CF 4) Display all the nodes from the center
            ACPoint p;
            p.x = 0;
            p.y = 0;
            p.z = 0;
            for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
                (*node).setDisplayed (true);
                (*node).setCurrentPosition(p);
                (*node).setNextPosition(p,t);
            }
            //this->updateNextPositions();

            //CF 5) Display all the nodes clustered
            this->updateDisplay(true);
        }
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
    this->removeAllLabels();
    bool success = false;
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        if (acpl) {
            this->setClustersMethodPlugin(acpl);
        }
        else
            mClustersMethodPlugin = 0;
        if (getLibrary()->getSize() > 0)
            this->updateDisplay(true);
        success = true;
        break;
    case AC_MODE_NEIGHBORS:
        if (acpl) {
            this->setClustersMethodPlugin(acpl);
        }
        else
            mClustersMethodPlugin = 0;
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
    this->removeAllLabels();
    bool success = false;
    double t = getTime();

    ACPoint p;
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        this->setNeighborsMethodPlugin(acpl);
        success = true;
        break;
    case AC_MODE_NEIGHBORS:
        //CF 1) Move all nodes to the center
        for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
            p.x = 0;
            p.y = 0;
            p.z = 0;
            (*node).setNextPosition(p, t);
            (*node).setNextPosition(p,t);
        }
        this->setNeedsDisplay(true);

        //CF 2) Recreate the user log
        delete mUserLog;
        mUserLog = new ACUserLog();

        mUserLog->addRootNode(this->getReferenceNode(), 0); // 0

        //CF 3) Change the plugin and update the display, it will make the reference node appear:
        this->setNeighborsMethodPlugin(acpl);
        if (mNeighborsPosPlugin != 0 && getLibrary()->getSize() > 0)
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
    this->displayAllLabels(false);
    bool success = false;
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        if (acpl) {
            this->setClustersPositionsPlugin(acpl);
        }
        else
            mClustersPosPlugin = 0;
        if (getLibrary()->getSize() > 0){
            setState(AC_CHANGING);
            if (acpl){
                mClustersPosPlugin->updateNextPositions(this);
                success = true;
            }
            else{
                std::cerr << "ACMediaBrowser::changeClustersPositionsPlugin: no plugin set" << std::endl;
                success = false;
            }
        }
        break;
    case AC_MODE_NEIGHBORS:
        if (acpl)
            this->setClustersPositionsPlugin(acpl);
        else
            mClustersPosPlugin = 0;
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
    this->displayAllLabels(false);
    bool success = false;
    switch ( mMode ){
    case AC_MODE_CLUSTERS:
        this->setNeighborsPositionsPlugin(acpl);
        success = true;
        break;
    case AC_MODE_NEIGHBORS:
        this->setNeighborsPositionsPlugin(acpl);
        setState(AC_CHANGING);
        /*if (mNeighborsMethodPlugin != 0 && getLibrary()->getSize() > 0){
    mNeighborsPosPlugin->updateNextPositions(this);
   }*/
        this->updateNextPositions();
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

std::string ACMediaBrowser::getActivePluginName(ACPluginType PluginType){
    std::string name("");
    switch ( PluginType ){
    case PLUGIN_TYPE_CLUSTERS_METHOD:
        if(mClustersMethodPlugin)
            name = mClustersMethodPlugin->getName();
        break;
    case PLUGIN_TYPE_CLUSTERS_POSITIONS:
        if(mClustersPosPlugin)
            name = mClustersPosPlugin->getName();
        break;
    case PLUGIN_TYPE_NEIGHBORS_METHOD:
        if(mNeighborsMethodPlugin)
            name = mNeighborsMethodPlugin->getName();
        break;
    case PLUGIN_TYPE_NEIGHBORS_POSITIONS:
        if(mNeighborsPosPlugin)
            name = mNeighborsPosPlugin->getName();
        break;
    default:
        cerr << "plugin type not used for the browser" << endl;
        break;
    }
    return name;
}

// -- private methods
void ACMediaBrowser::resetNavigation() {
    this->resetLoopNavigationLevels(0);
    mClickedNode = -1;
    mClickedLabel = -1;
    mClosestNode = -1;
    mSelectedNodes.clear();
    mLastSelectedNode = -1;
    mNavigationLevel = 0;
    mReferenceNode = 0; //CF ugly, could be -1
    mBackwardNavigationStates.clear();
    mForwardNavigationStates.clear();
    mClusterCenters.clear();
}

void ACMediaBrowser::resetLoopNavigationLevels(int l) {
    // default : l=0
    for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
        (*node).setNavigationLevel (l);
    }
}

void ACMediaBrowser::resetCamera() {
    //CF we need more than setCameraRecenter()
    mCameraPosition[0] = 0.0;
    mCameraPosition[1] = 0.0;
    mCameraZoom = 1.0;
    mCameraAngle = 0.0;
}

void ACMediaBrowser::commitPositions()
{
    for (ACMediaNodes::iterator node = mLoopAttributes.begin(); node != mLoopAttributes.end(); ++node){
        (*node).commitPosition();
        if(mLibrary->getMediaType() == MEDIA_TYPE_MIXED)
            std::cout << "Node " << (int)((*node).getMediaId()) << " of media type " << ACMediaFactory::getInstance().getNormalCaseStringFromMediaType( mLibrary->getMedia((*node).getMediaId())->getType() ) << " at position " <<  (*node).getCurrentPosition().x << " " << (*node).getCurrentPosition().y << " "<< (*node).getCurrentPosition().z << std::endl;
    }
}


