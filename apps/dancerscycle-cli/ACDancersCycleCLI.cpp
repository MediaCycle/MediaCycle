/**
 * @brief Process to query Dancers! video through MediaCycle commandline interfaces (CLI)
 * @author Alexis Moinet, Christian Frisson
 * @date 15/07/2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#include <stdlib.h>

#include<ACDancersCycleCLI.h>

//#include "MediaCycle.h"

//#include <string>
//#include <cstring>
//#include <vector>
//#include <iostream>
//#include <iomanip>
//#include <sstream>

//#include <signal.h>
//#include <math.h>
//#include "Armadillo-utils.h"
////#include "fftsg_h.c"
//#include <time.h>

//#include <armadillo> // for sort(rand())
//#include <ctime> // for timing with clock()

//sleep()
#include <iostream>
#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#endif

using namespace std;
using namespace arma;

ACDancersCycleCLI::ACDancersCycleCLI(MediaCycle* _media_cycle)
    : ACAbstractProcessCLI(_media_cycle)
{
}

ACDancersCycleCLI::~ACDancersCycleCLI(){
}

static void dancers_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData); 
int processTcpMessageFromInstallation(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send); 
void saveLibraryAsXml(MediaCycle *mediacycle, string _path);
void readLibraryXml(MediaCycle *mediacycle, std::string filename);
std::string generateID(std::string filename);
void startOrRedraw(MediaCycle *that, int nbVideo, char**, int*);
void startOrRedrawRandom(MediaCycle *that, int nbVideo, char**, int*);
void itemClicked(MediaCycle *that, int idVideo, char**, int*);
void labelClicked(MediaCycle *mediacycle, int idLabel, char **, int*);
string fillOutputBuffer(ACMediaLibrary* media_library, ACMediaBrowser* media_browser, int nvid);

string dirpath = "./";
string xmlpath = dirpath+"dancers-all.xml";

void ACDancersCycleCLI::run(int argc, char *argv[]){

	string configFilename = dirpath+"config.txt";
	ifstream configIF(configFilename.c_str());
	cout << "MediaCycle - Dancers - v4" << endl;
	cout << "-------------------------" << endl;

	string libraryFilename, visPluginFilename;
	configIF >> libraryFilename;
	cout << "library : " << libraryFilename << endl;
	configIF >> visPluginFilename;
	cout << "plugin" << visPluginFilename << endl;
	configIF.close();
	
	cout<<"new MediaCycle"<<endl;
//CF this is now done by ACAbstractProcessCLI
//    MediaCycle* media_cycle;
//    media_cycle = new MediaCycle(MEDIA_TYPE_VIDEO);
//	if (media_cycle->addPluginLibrary (visPluginFilename) < 0){
//		cerr << "<MediaCycle-Dancers main> could not add Plugin: " << visPluginFilename << endl;
//		return;
//	}

    media_cycle->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_POSITIONS,"MediaCycle DANCERS!");

    if (media_cycle->importACLLibrary(libraryFilename) == 0) {
		cerr << "<MediaCycle-Dancers main> empty library: " << libraryFilename << endl;
        return;
	}
    media_cycle->libraryContentChanged();
    // saveLibraryAsXml(media_cycle, xmlpath);
	
    media_cycle->getBrowser()->randomizeNodePositions();
//	media_cycle->getBrowser()->setClusterNumber(1);
	// XSCF251003 added this
    //media_cycle->updateClusters(true);
    //media_cycle->setNeedsDisplay(true);
	
    if (media_cycle->startTcpServer(12345,5,dancers_tcp_callback) < 0){
		cerr << "<MediaCycle-Dancers main> could not start tcp server" << endl;
        return;
	}
//	//readLibraryXml(media_cycle, "/Users/dtardieu/Desktop/dancers-exemple.xml");
//	
	while(1) {
		sleep(30);
	}
//	
//	media_cycle->saveAsLibrary(mypath+"dancers-ex-2.acl");
    delete media_cycle;
}

static void dancers_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData) {
	MediaCycle *that = (MediaCycle*)userData;
	processTcpMessageFromInstallation(that, buffer, l, buffer_send, l_send);
}

// format of TCP messages:
/*
spec = speculoos
num = numédiart
IDvideo = celui du xml
IDmotClef = position dans le header du XML en commençant par 0

Message : start ou redraw
spec->num
0 nbVideo (nbVideo = 3 bits)
num->spec
0 nbVideo IDVideo posX posY
1 nbMotClef IDmotClef posX posY

Message click sur video (itemClicked)
spec->num : 
1 idvideo
num->spec
idem prec

Message click sur mot clef (largeTextClicked)
spec->num
2 idmotclef
num->spec
idem prec 
*/
// using streams to detect format errors.
int processTcpMessageFromInstallation(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send) {
	// XS timing
	clock_t t0=clock();
	// end XS
	
	std::string file_name;
	std::istringstream message_in(buffer);

	cout<< "buffer : " << buffer <<endl;
	string str_message_in;
	if (! (message_in >> str_message_in) ){
		if (! message_in.good()){ 
			cerr << "<processTcpMessageFromInstallation> : bad incoming buffer" << endl;
		}
		else {
			cerr << "<processTcpMessageFromInstallation> : bad problem reading incoming buffer" << endl;
		}
		cerr << "stream(buffer) = " << message_in << endl;
		return -1;
	}

	cout << "str_message_in = "<< str_message_in << endl;
	
	// l is used as a check of the message, but is not necessary
	if ( l == str_message_in.size()) {
		cout << "Processing TCP message of length " <<  l << endl;
	}
	else{
		cerr << "<processTcpMessageFromInstallation> : message lengths do not match" << endl;
		cerr << "l = " << l << "; str_message_in.size() = " << str_message_in.size() << endl;
	}
		
	std::istringstream ss ( str_message_in.substr(0,1) );
	int msgType ;
	if ( ! (ss >> msgType)) {
		if (! ss.good()){
			cerr << "<processTcpMessageFromInstallation> : bad streaming of message ID in incoming buffer" << endl;
		}
		else{
			cerr << "<processTcpMessageFromInstallation> : problem reading message ID from incoming buffer" << endl;
		}
		cerr << "corresponding buffer[0] = " << ss << endl;
		return -1;
	}
	
	cout << "type of message : " << msgType << endl;
    switch (msgType) {
		case 0:{ 
			// START / REDRAW
			// MESSAGE : 0 nbVideo
			unsigned int nbVideo;
			std::istringstream s_0 ( str_message_in.substr(1,3) );
			if ( !(s_0 >> nbVideo) ){
				if (! s_0.good()){
					cerr << "<processTcpMessageFromInstallation> : bad streaming of number of videos from incoming buffer" << endl;
				}
				else {
					cerr << "<processTcpMessageFromInstallation> : problem reading number of videos from incoming buffer" << endl;
				}
				cerr << "type : " << msgType << " ; corresponding buffer[1:3] = " << s_0 << endl;
				return -1;
			}
			
			cout << "nb videos : " << nbVideo << endl;
			
			// XS test
			startOrRedraw(that,nbVideo, buffer_send, l_send);
			
			std::istringstream s_out (*buffer_send);
			string str_buffer_send;
			if (! (s_out >> str_buffer_send) ) {
				if (! s_out.good()){
					cerr << "<processTcpMessageFromInstallation> : bad streaming of outgoing buffer" << endl;
				}
				else {
					cerr << "<processTcpMessageFromInstallation> : problem writing outgoing buffer" << endl;
				}
				cerr << "type : " << msgType << " ; corresponding buffer_send = " << s_out << endl;
				return -1;
			}
			
			cout << "(startOrRedraw) sent back this message : " << str_buffer_send << endl;
			// XS timing
			clock_t t1=clock();
			cout << "Execution time: " << (t1-t0)*1000/CLOCKS_PER_SEC << " ms." << endl;
			// end XS
			break;
		}
		case 1:{ 
			// ITEMCLICKED
			// MESSAGE : 1 idVideo
			string idVideoStr;
			unsigned int idVideo=10;
			std::istringstream s_1( str_message_in.substr(4,6) );
			if ( !(s_1 >> idVideoStr) ){
				if (! s_1.good()){
					cerr << "<processTcpMessageFromInstallation> : bad streaming of video ID from incoming buffer" << endl;
				}
				else {
					cerr << "<processTcpMessageFromInstallation> : problem reading video ID from incoming buffer" << endl;
				}
				cerr << "type : " << msgType << " ; corresponding buffer[1:3] = " << s_1 << endl;
				return -1;
			}
			string filename;
			int posSep;
			int posDot;
                        std::vector<long> ids = that->getLibrary()->getAllMediaIds();
                        for (int k=0; k<ids.size(); k++){
                                filename=that->getLibrary()->getMedia(ids[k])->getFileName();
				posSep = filename.find_last_of("/");
				posDot = filename.find_last_of(".");
				cout << "test id : " << filename.substr(posSep+1, posDot-posSep-10) << endl;
				if (!filename.compare(posSep+1, posDot-posSep-1, idVideoStr)){
                                        idVideo = ids[k];
					break;
				}	
			}
			itemClicked(that,idVideo, buffer_send, l_send);

			break;
		}
		case 2:{ 
			// LARGETEXTCLICKED
			// MESSAGE : 2 idText
			unsigned int idText;
			std::istringstream s_2( str_message_in.substr(1,3) );
			if ( !(s_2 >> idText) ){
				if (! s_2.good()){
					cerr << "<processTcpMessageFromInstallation> : bad streaming of text ID from incoming buffer" << endl;
				}
				else {
					cerr << "<processTcpMessageFromInstallation> : problem reading text ID from incoming buffer" << endl;
				}
				cerr << "type : " << msgType << " ; corresponding buffer[1:3] = " << s_2 << endl;
				return -1;
			}			
			cout << "id Text : " << idText << endl;
			
			labelClicked(that,idText, buffer_send, l_send);

			break;
		}	
		default:
			break;
    }
    return 0;
}

char* get_error_message(){
	string sbuffer_send_error="-1";
	char* buffer_send_error = new char [sbuffer_send_error.size()+1]; // extra byte needed for the trailing '\0' 
	strcpy (buffer_send_error, sbuffer_send_error.c_str());
	return buffer_send_error;
}

// common to all types of messages:
string fillOutputBuffer(ACMediaLibrary* media_library, ACMediaBrowser* media_browser, int nvid){
        int n_medias = media_browser->getNumberOfMediaNodes();
	int n_labels = media_browser->getNumberOfDisplayedLabels();
	string sbuffer_send;

	// === 1) videos
	string sepu="_";
	ostringstream onvid ;
	onvid.fill('0'); // fill with zeros (otherwise will leave blanks)
	onvid << setw(3) << nvid << sepu;
	// === start filling sbuffer_send
	sbuffer_send = onvid.str(); // could add "0" in front for message type (not done here because not useful for web application)
	
	// loop on all videos to see which ones to send out
        int chk_medias=0;
// XS TODO iterators ?
// like	for (ACMediaNodes::iterator node = mMediaNodes.begin(); node != mMediaNodes.end(); ++node){

	ACPoint p;
        std::vector<long> ids = media_library->getAllMediaIds();
        for (int i=0; i< ids.size(); i++){
            if (media_browser->getMediaNode(ids[i])->isDisplayed()){
                        chk_medias++;
                        p = media_browser->getMediaNode(ids[i])->getNextPosition();
			int posx = (int) p.x;
			int posy = (int) p.y;
			ostringstream oss ;
			oss.fill('0'); // fill with zeros (otherwise will leave blanks)
                        oss << setw(6) << generateID(media_library->getMedia(ids[i])->getFileName()) << setw(3) << posx << setw(3)<< posy; // fixed format
			sbuffer_send += oss.str(); // concatenates all videos in one string
			
			// XS test
			cout << oss.str() << endl;
		}		
	}
        if (chk_medias != media_browser->getNumberOfDisplayedNodes()) {
                cerr << "<startOrRedraw> consistency check failed: problem with number of displayed videos: " << chk_medias << " differs from "<< media_browser->getNumberOfDisplayedNodes() << endl;
		sbuffer_send = "-1";	
		return sbuffer_send;
	}
	
	// === 2) labels (keywords)
	string sep="-";
	ostringstream onlab ;
	onlab.fill('0'); // fill with zeros (otherwise will leave blanks)
	// NB: SEPU FIRST
	onlab << sepu << setw(3) << media_browser->getNumberOfDisplayedLabels();
	sbuffer_send += onlab.str();
	
	// loop on all labels to see which ones to send out
	const vector<ACLabel> label_attributes = media_browser->getLabelAttributes();
	int chk_labels=0;
	for (int i=0; i< n_labels; i++){
		if (label_attributes[i].isDisplayed){
			chk_labels++;
			int posx = (int) label_attributes[i].pos.x;
			int posy = (int) label_attributes[i].pos.y;
			ostringstream oss ;
			oss.fill('0'); // fill with zeros (otherwise will leave blanks)
			oss << sep << label_attributes[i].text << sep << setw(3) << posx << setw(3)<< posy; // fixed format
			sbuffer_send += oss.str(); // concatenates all videos in one string
			
			// XS test
			cout << oss.str() << endl;
			
		}		
	}
	if (chk_labels != media_browser->getNumberOfDisplayedLabels()) {
		cerr << "<startOrRedraw> consistency check failed: problem with number of displayed labels: " << chk_labels << " differs from "<< media_browser->getNumberOfDisplayedLabels() << endl;
		sbuffer_send = "-1";	
		return sbuffer_send;
	}
	return sbuffer_send;
}

// this one is really used in the installation
// ex: for 50 videos and 3 keywords:
//     049_000000395082000001537313000002296262000003258158000004418017000005125012000006137074000007725067000008624351000009628293000010164241000011716215000012292320000013127106000014641262000015007337000016359310000017682187000018292364000019172221000020727340000021322181000022202148000023399290000024516337000025547320000026562295000027385060000028679246000029306072000030513305000031462088000032737272000033028157000034131043000035750036000036529002000037538363000038153225000039162223000040605340000041459202000042470296000043639279000044635290000045000002000046436056000047311139000048323257_003-descA2-090031-descB0-029080-descC1-059005
// 
// 001052050015
// dancerxxxyyy

void startOrRedraw(MediaCycle *mediacycle, int nbVideo, char **buffer_send, int* l_send){
	ACMediaLibrary* media_library = mediacycle->getLibrary();
	
	if (media_library->getSize() == 0) {
		cerr << "<startOrRedrawRandom> : empty media library" << endl;
		*buffer_send = get_error_message();	
		return;
	}
	
	ACMediaBrowser* media_browser;
	media_browser = mediacycle->getBrowser();
	
        int n_medias = media_browser->getLibrary()->getSize();
//	int n_labels = media_browser->getNumberOfLabels();

        if (nbVideo > n_medias) {
		cerr << "<startOrRedrawRandom> : you are asking for too many videos" << endl;
		*buffer_send = get_error_message();	
		return;
	}

	// tell the browser nothing specific was clicked.
	media_browser->setClickedNode(-1);
	media_browser->setClickedLabel(-1);
        media_browser->setNumberOfDisplayedNodes(nbVideo);
	media_browser->updateNextPositions();
        if (nbVideo != media_browser->getNumberOfDisplayedNodes()){
		cerr << "<startOrRedraw> browser returned wrong number of videos" << endl;
		*buffer_send = get_error_message();	
		return;
	}
	
	// XS below this is common to all message types -- put it in subroutine
	string sbuffer_send = fillOutputBuffer(media_library, media_browser,nbVideo);
	*buffer_send = new char [sbuffer_send.size()+1]; // extra byte needed for the trailing '\0' 
	strcpy (*buffer_send, sbuffer_send.c_str());
	*l_send = sbuffer_send.length();
	
}

// this one is a test version of the previous one
void startOrRedrawRandom(MediaCycle *mediacycle, int nbVideo, char **buffer_send, int* l_send){
	ACMediaLibrary* media_library = mediacycle->getLibrary();
        int n_medias = media_library->getSize();
	
        if (n_medias==0) {
		cerr << "<startOrRedrawRandom> : empty media library" << endl;
		return;
	}
        if (nbVideo > n_medias) {
		cerr << "<startOrRedrawRandom> : you are asking for too many videos" << endl;
		return;
	}
	
	ACMediaBrowser* media_browser;
	media_browser = mediacycle->getBrowser();
	// tell the browser nothing specific was clicked.
	media_browser->setClickedNode(-1);
	media_browser->setClickedLabel(-1);

	// using armadillo to get nbVideo random ids among all videos:
	#ifdef ARMADILLO_HAVE_RANDU
                colvec q = arma::randu<colvec>(n_medias);
	#else
                colvec q = arma::rand<colvec>(n_medias);
	#endif
	ucolvec indices = sort_index(q);
	ucolvec trunc_indices = indices.rows(0,nbVideo-1);
	//cout << trunc_indices << endl;
	if (media_browser->getNumberOfMediaNodes() < trunc_indices.n_rows ){
		cerr << "<startOrRedrawRandom> : not enough loop attributes" << endl;
		return;
	}
	
	// === 1) videos
	string sepu="_";
	ostringstream onvid ;
	onvid.fill('0'); // fill with zeros (otherwise will leave blanks)
	onvid << setw(3) << nbVideo << sepu;
	// === start filling sbuffer_send

	string sbuffer_send = onvid.str(); // could add "0" in front for message type (not done here because not useful for web application)
	for (unsigned int i=0; i <trunc_indices.n_rows ;i++){
		int l = trunc_indices[i];
		// positions should in range [0:999], which is a fraction of the screen size
		// XS for the moment they depend on mViewWidth, mViewHeight in ACMediaBrowser
		
                ACPoint pos = media_browser->getMediaNode(l)->getCurrentPosition();
		//int posx = (int) media_browser->getMediaNode(l).getCurrentPositionX();
		//int posy = (int) media_browser->getMediaNode(l).getCurrentPositionY();
		ostringstream oss ;
		oss.fill('0'); // fill with zeros (otherwise will leave blanks)
		oss << setw(6) << i << setw(3) << (int) pos.x << setw(3)<< (int) pos.y; // fixed format
		sbuffer_send += oss.str(); // concatenates all videos in one string

		// XS test
		cout << oss.str() << endl;
		
		// does it send out information each time buffer is updated ?
		//int full_id = generateID(filename); // <- this way we need to get the filename from media[i]->getFileName() ... it's kind of annoying
	}
	// === 2) keywords
	string sep="-";
	// dummy
	ostringstream onkw ;
	onkw.fill('0'); // fill with zeros (otherwise will leave blanks)
	// SEPU FIRST
	onkw << sepu << setw(3) << "3";// media_browser->getNumberOfDisplayedLabels();
	sbuffer_send += onkw.str();
	vector<string> v;
	v.push_back("A");
	v.push_back("B");
	v.push_back("C");
	for (int i=0; i<3; i++){
		int dumx = int(ACRandom()*100);
		int dumy = int(ACRandom()*100);
		int hlm = int(ACRandom()*3.9);
		ostringstream oss ;
		oss.fill('0');
		oss << sep << "desc" << v[i] << hlm << sep  << setw(3) << dumx  << setw(3) << dumy ;
		sbuffer_send += oss.str();

		// XS test
		cout << oss.str() << endl;

	}
	// === finished filling sbuffer_send
	
	// XS is this the proper way to do it ?
	*buffer_send = new char [sbuffer_send.size()+1]; // extra byte needed for the trailing '\0' 
	strcpy (*buffer_send, sbuffer_send.c_str());
	*l_send = sbuffer_send.length();
	
	
// try sending dummy packet back
//	string sbuffer_send;
//	sbuffer_send = "001052050015";
//	*buffer_send = (char*) (sbuffer_send).c_str();
//	*l_send = sbuffer_send.length();
}

void itemClicked(MediaCycle *mediacycle, int idVideo, char **buffer_send, int* l_send){
	ACMediaLibrary* media_library = mediacycle->getLibrary();	
	if (media_library->getSize()==0) {
		cerr << "<itemClicked> : empty media library" << endl;
		return;
	}
	
	ACMediaBrowser* media_browser;
	media_browser = mediacycle->getBrowser();
        int n_medias = media_browser->getNumberOfMediaNodes();
	// XS should also be = media_library->getSize()
	
        if (idVideo < 0 || idVideo > n_medias) {
		cerr << "<itemClicked> : video ID out of bounds" << endl;
		return;
	}
	
	media_browser->setClickedNode(idVideo);
	media_browser->setClickedLabel(-1);
	media_browser->updateNextPositions();
        int nbVideo = media_browser->getNumberOfDisplayedNodes();
	
	// XS below this is common to all message types -- put it in subroutine
	string sbuffer_send = fillOutputBuffer(media_library, media_browser, nbVideo);
	*buffer_send = new char [sbuffer_send.size()+1]; // extra byte needed for the trailing '\0' 
	strcpy (*buffer_send, sbuffer_send.c_str());
	*l_send = sbuffer_send.length();
	
}

void labelClicked(MediaCycle *mediacycle, int idLabel, char **buffer_send, int* l_send){
	ACMediaLibrary* media_library = mediacycle->getLibrary();	
	if (media_library->getSize() == 0) {
		cerr << "<itemClicked> : empty media library" << endl;
		return;
	}
	
	ACMediaBrowser* media_browser;
	media_browser = mediacycle->getBrowser();
	int n_labels = media_browser->getNumberOfLabels();
	if (idLabel < 0 || idLabel > n_labels) {
		cerr << "<labelClicked> : label ID out of bounds" << endl;
		return;
	}
	
	media_browser->setClickedNode(-1);
	media_browser->setClickedLabel(idLabel);
	media_browser->updateNextPositions();
        int nbVideo = media_browser->getNumberOfDisplayedNodes();
	
	// XS below this is common to all message types -- put it in subroutine
	string sbuffer_send = fillOutputBuffer(media_library, media_browser, nbVideo);
	*buffer_send = new char [sbuffer_send.size()+1]; // extra byte needed for the trailing '\0' 
	strcpy (*buffer_send, sbuffer_send.c_str());
	*l_send = sbuffer_send.length();
}

void saveLibraryAsXml(MediaCycle* mediacycle, string _path) {
	ACMediaLibrary* media_library;
	media_library = mediacycle->getLibrary();
	ACMedia* local_media;
        int n_medias = media_library->getSize();
	int featureSize;
	float featureValue;
	std::string featureName;
	std::string ID;
	FILE *library_file = fopen(_path.c_str(),"w");
	fprintf(library_file, "%s\n", "<?xml version=\"1.0\"\?>");
	fprintf(library_file, "%s\n", "<dancers>");
	
	/// HEADER ///
	fprintf(library_file, "%s\n", "<head>");
	
	fprintf(library_file, "<feature size=\"6\" >ID</feature>\n");
	
        for (int i=0; i < media_library->getFirstMedia()->getNumberOfFeaturesVectors(); i++){
                featureSize = media_library->getFirstMedia()->getFeaturesVector(i)->getSize();
                featureName = media_library->getFirstMedia()->getFeaturesVector(i)->getName();
		
		if (featureSize > 1){
			std::cout << "Warning : Multidimensional feature, won't be exported" << std::endl;
		}
		else{
			fprintf(library_file, "<feature size=\"1\">");
                        fprintf(library_file, "%s",  media_library->getFirstMedia()->getFeaturesVector(i)->getName().c_str());
			fprintf(library_file, "</feature>\n");
		}
	}
	fprintf(library_file, "%s\n", "</head>");
	
	/// ITEMS //
	fprintf(library_file, "%s\n", "<items>");
        std::vector<long> ids = media_library->getAllMediaIds();
        for(int i=0; i<ids.size(); i++) {
            fprintf(library_file, "<v duration=\"%.1lf\">",  media_library->getMedia(ids[i])->getDuration());
                local_media = media_library->getMedia(ids[i]);
		if (i>80){
			std::cout << "filename : " <<  ID << std::endl;
		}
		// printing ID
		ID = generateID(local_media->getFileName());
		fprintf(library_file, "%s", ID.c_str());
                for (int j=0; j < media_library->getMedia(ids[i])->getNumberOfFeaturesVectors(); j++){
                        featureSize = media_library->getMedia(ids[i])->getFeaturesVector(j)->getSize();
                        featureName = media_library->getMedia(ids[i])->getFeaturesVector(j)->getName();
			
			if (featureSize == 1){
                                featureValue = media_library->getMedia(ids[i])->getFeaturesVector(j)->getDiscretizedFeature();
				fprintf(library_file, "%d", (int) featureValue);
				if (i>80){
					std::cout << featureName << " = " << featureValue << std::endl;
				}
			}
			else{
				std::cout << "Warning : Multidimensional feature, won't be exported" << std::endl;
			}
		}
		fprintf(library_file, "</v>\n");
	}
	fprintf(library_file, "%s\n", "</items>");  
	fprintf(library_file, "%s\n", "</dancers>");
	fclose(library_file);
}

void readLibraryXml(MediaCycle* mediacycle, std::string filename){
	std::vector<string> descNames;
	std::vector<int> descDims;
	TiXmlDocument doc( filename.c_str() );
	doc.LoadFile();
	TiXmlNode* node = 0;
	TiXmlNode* dancers = 0;
	TiXmlNode* items = 0;
	TiXmlElement* head = 0;
	TiXmlElement* feature=0;
	
	int dim;
	dancers = doc.FirstChild("dancers");
	assert(dancers);
	head = dancers->FirstChild("head")->ToElement();
	assert(head);
	for( node = head->FirstChild("feature"); node; node = node->NextSibling() ){
		feature = node->ToElement();
		feature->QueryIntAttribute("size", &dim);
		descDims.push_back(dim);
		descNames.push_back(feature->FirstChild()->Value());
	}
	
	items = dancers->FirstChild("items");
	string tmpFeature;
	for( node = items->FirstChild("v"); node; node = node->NextSibling() ){
		int currIdx = 0;
		ACMedia* local_media = ACMediaFactory::getInstance().create(MEDIA_TYPE_VIDEO);
		tmpFeature = node->FirstChild()->Value();
		for (int i=0; i < descDims.size(); i++) {
			ACMediaFeatures* mediaFeatures = new ACMediaFeatures();
			mediaFeatures->resize(1);
			mediaFeatures->setComputed();
			mediaFeatures->setName(descNames[i]);
			std::cout << tmpFeature.substr(currIdx, descDims[i]).c_str() << std::endl;
			mediaFeatures->setFeatureElement(0, atof(tmpFeature.substr(currIdx, descDims[i]).c_str()));
			currIdx = currIdx+descDims[i];
			local_media->addFeaturesVector(mediaFeatures);
		}
		mediacycle->getLibrary()->addMedia(local_media);
	}
}

// std::string generateID(std::string filename){
// 	const int nbCities=2;
// 	const std::string cityNames[nbCities] = {"Bru", "Par"};
// 	const std::string cityID[nbCities] = {"00", "01"};
// 	std::string IDs, numDancer, numTry, city;
// 	int posCity;
// 	int posSep = filename.find_last_of("/\\");
// 	int posDot = filename.find_last_of(".");
// 	city = filename.substr(posSep+1, 3);
// 	numDancer = filename.substr(posSep+5, 3);
// 	numTry = filename[posSep+9];
// 	for (int i=0; i<nbCities; i++){
// 		if (!city.compare(cityNames[i]))
// 			posCity = i;
// 	}
// 	IDs.assign(cityID[posCity]);
// 	IDs += numDancer;
// 	IDs += numTry;
// 	return IDs;
// }

std::string generateID(std::string filename){
	std::string IDs;
	int posSep = filename.find_last_of("/\\");
	IDs = filename.substr(posSep+1, 6);
	return IDs;
}

// sample XML file:
/*
<?xml version="1.0"?>
<dancers>
<head>
<feature size="6"> ID </feature>
<feature size="1"> Quantity_of_motion </feature>
<feature size="1"> Speed </feature>
</head>
<items>
<v>00101109</v>
<v>00102150</v>
<v>00103255</v>
<v>00212195</v>
<v>00214290</v>
</items>
</dancers>
*/
