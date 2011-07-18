/*
 *  ACVideoDancersPlugin.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 22/09/09
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

#include "ACVideoDancersPlugin.h"

#include <vector>
#include <string>

// to check if file exist or do directory manipulations
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
using std::cout;
using std::endl;
using std::cerr;

ACVideoDancersPlugin::ACVideoDancersPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    //this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Video";
    this->mDescription = "Video plugin";
    this->mId = "";
	this->mDescriptorsList.push_back("(Front) Mean Contraction Index");
	this->mDescriptorsList.push_back("(Front) Mean Bounding Box Ratio");
	this->mDescriptorsList.push_back("(Front) Mean Blobs Pixel Speed (Energy)");
	this->mDescriptorsList.push_back("(Top) Mean Position (x,y) ");
	this->mDescriptorsList.push_back("(Top) Std Position (x,y) ");
	this->mDescriptorsList.push_back("(Top) Max Position (x,y) ");
	this->mDescriptorsList.push_back("(Top) Mean Speed (x,y) ");
}

ACVideoDancersPlugin::~ACVideoDancersPlugin() {
}

std::vector<ACMediaFeatures*>  ACVideoDancersPlugin::_calculate(std::string aFileName, bool _save_timed_feat) {
	std::vector<ACMediaFeatures*> allVideoFeatures;
	std::vector<ACMediaFeatures*> topVideoFeatures;
	string topFilename = changeLastFolder(aFileName,"Top");

	allVideoFeatures = calculateFront(aFileName);
	topVideoFeatures = calculateTop(topFilename);
	allVideoFeatures.insert( allVideoFeatures.end(), topVideoFeatures.begin(), topVideoFeatures.end() );
	return allVideoFeatures;
}

// this is the method called from outside
// --- FILE ORGANISATION IS VERY SPECIFIC TO THE DANCERS PROJECT ---
// you have to give the filename of the FRONT video
// and the corresponding top video must be in a TOP directory, as such:
// path_to_the_dancers/FRONT/001022.mov
// path_to_the_dancers/TOP/001022.mov
// so from the FRONT file it will go ../TOP
std::vector<ACMediaFeatures*> ACVideoDancersPlugin::calculate(ACMediaData* video_data, ACMedia* theMedia, bool _save_timed_feat) {
	string file_name_front = video_data -> getFileName();
	vector<ACMediaFeatures*> tmp = this->_calculate(file_name_front);
	return tmp;
}


//uses ACVideoAnalysis and converts the results into ACMediaFeatures
std::vector<ACMediaFeatures*>  ACVideoDancersPlugin::calculateTop(std::string aFileName) {
	ACVideoAnalysis* video = new ACVideoAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allVideoFeatures;

	cout << "calculateMeanOfTrajectory" << endl;
	ACMediaFeatures* videoMeanTrajectory = this->calculateMeanOfTrajectory(video);
	if (videoMeanTrajectory != 0){
		allVideoFeatures.push_back(videoMeanTrajectory);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no mean feature" << endl;
	}

	cout << "calculateStdOfTrajectory" << endl;
	ACMediaFeatures* videoStdTrajectory = this->calculateStdOfTrajectory(video);
	if (videoStdTrajectory != 0){
		allVideoFeatures.push_back(videoStdTrajectory);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no std feature" << endl;
	}

	cout << "calculateMaxOfTrajectory" << endl;
	ACMediaFeatures* videoMaxTrajectory = this->calculateMaxOfTrajectory(video);
	if (videoMaxTrajectory != 0){
		allVideoFeatures.push_back(videoMaxTrajectory);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no max feature" << endl;
	}

	cout << "calculateMeanSpeedOfTrajectory" << endl;
	ACMediaFeatures* videoMeanSpeedTrajectory = this->calculateMeanSpeedOfTrajectory(video);
	if (videoMeanSpeedTrajectory != 0){
		allVideoFeatures.push_back(videoMeanSpeedTrajectory);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no speed feature" << endl;
	}
		
	delete video;
	return allVideoFeatures;
}

//uses ACVideoAnalysis and converts the results into ACMediaFeatures
std::vector<ACMediaFeatures*>  ACVideoDancersPlugin::calculateFront(std::string aFileName) {
	ACVideoAnalysis* video = new ACVideoAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allVideoFeatures;

	cout << "calculateContractionIndex" << endl;
	ACMediaFeatures* videoContractionIndex = this->calculateContractionIndex(video);
	if (videoContractionIndex != 0){
		allVideoFeatures.push_back(videoContractionIndex);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no mean ci feature" << endl;
	}

	cout << "calculateMeanBoundingBoxRatio" << endl;
	ACMediaFeatures* videoMeanBoundingBoxRatio = this->calculateMeanBoundingBoxRatio(video);
	if (videoMeanBoundingBoxRatio != 0){
		allVideoFeatures.push_back(videoMeanBoundingBoxRatio);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no mean bounding box ratio feature" << endl;
	}

	cout << "calculateMeanBlobPixelsSpeed" << endl;
	ACMediaFeatures* videoMeanPixelSpeed = this->calculateMeanBlobPixelsSpeed(video);
	if (videoMeanPixelSpeed != 0){
		allVideoFeatures.push_back(videoMeanPixelSpeed);
	}
	else{
		cerr << "<ACVideoDancersPlugin::calculate> : no mean bounding box ratio feature" << endl;
	}
		
	delete video;
	return allVideoFeatures;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateMeanOfTrajectory(ACVideoAnalysis* video){
	//if (!video->areBlobsComputed()) video->computeBlobsUL();
	//if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	// TODO make this more general ?
	video->computeBlobs();
	video->computeMergedBlobsTrajectory(0);
	//
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getNormalizedMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean(); // will do "new" and set name

	// XS TEST
	// trajectory_mtf->dump("Test/traj.txt");
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateStdOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobs();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);

	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getNormalizedMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->std();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateMaxOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobs();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getNormalizedMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->max();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateContractionIndex(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobs();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);

	video->computeContractionIndices();

	ACMediaTimedFeature* ci_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getContractionIndices(), "contraction index");
	ACMediaFeatures* contractionIndex = ci_mtf->mean();
	delete ci_mtf;
	return contractionIndex;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateMeanSpeedOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobs();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	video->computeMergedBlobsSpeeds(0);
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getNormalizedMergedBlobsSpeeds(), "speed");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean();
	delete trajectory_mtf;
	return trajectory_mf;
}


//ACMediaFeatures* ACVideoDancersPlugin::calculateMostOccupiedCell(ACVideoAnalysis* video){
//	if (!video->areBlobsComputed()) video->computeBlobsUL();
//	if (!video->isTrajectoryComputed()) video->computeNormalizedMergedBlobsTrajectory(0);
//	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getNormalizedMergedBlobsTrajectory(), "trajectory");
//	mat hist_m = hist3(trajectory_mtf->getValue(), 10, 10);
//	
//	// XS TODO -- this is not correct
//	// hist_m.print();
//	ACMediaFeatures* trajectory_mf = trajectory_mtf->max(); // DELETE THIS
//	//max_indice(hist_m);	
//	delete trajectory_mtf;
//	return trajectory_mf;
//}

ACMediaFeatures* ACVideoDancersPlugin::calculateMeanBoundingBoxRatio(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	video->computeBoundingBoxRatios();
	ACMediaTimedFeature *trajectory_mtf = new ACMediaTimedFeature(video->getBlobsTimeStamps(), video->getBoundingBoxRatios(), "bounding box ratios");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoDancersPlugin::calculateMeanBlobPixelsSpeed(ACVideoAnalysis* video){
	video->computeBlobPixelSpeed();
	vector<float> t = video->getBlobsTimeStamps();
	vector<float> s = video->getBlobPixelsSpeeds();
	
	ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature(t,s, "pixel speed");
	ACMediaFeatures* pixel_speed = ps_mtf->mean();
	delete ps_mtf;
	return pixel_speed;
}

string ACVideoDancersPlugin::changeLastFolder(string path, string folder)
{
    int index = 0;
    int tmp = 0;
    string sep = "";
    string dir = extractDirectory(path);
    
    tmp = dir.substr(0,dir.size()-2).find_last_of('\\');
    int tmp2 = 0;
    tmp2 = dir.substr(0,dir.size()-2).find_last_of('/');
    if (tmp > tmp2)
    {
        index = tmp;
        sep = "\\";
    }
    else
    {
        index = tmp2;
        sep = "/";
    }
    return dir.substr(0,index+1) + folder + sep + extractFilename(path);
}

string ACVideoDancersPlugin::extractDirectory(string path)
{
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\');
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(0, index + 1);
}

string ACVideoDancersPlugin::extractFilename(string path)
{
    int index = 0;
    int tmp = 0;
    tmp = path.find_last_of('\\' );
    int tmp2 = 0;
    tmp2 = path.find_last_of('/');
    if (tmp > tmp2)
        index = tmp;
    else
        index = tmp2;
    return path.substr(index + 1);
}
