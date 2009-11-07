/*
 *  ACVideoPlugin.cpp
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

#include "ACVideoPlugin.h"

#include <vector>
#include <string>

ACVideoPlugin::ACVideoPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_VIDEO;
    this->mPluginType = PLUGIN_TYPE_FEATURES;
    this->mName = "Video";
    this->mDescription = "Video plugin";
    this->mId = "";
	
    //local vars
}

ACVideoPlugin::~ACVideoPlugin() {
}

// the following two HAVE to be defined to make the class non-abstract
int ACVideoPlugin::initialize(){
}

std::vector<ACMediaFeatures*> ACVideoPlugin::calculate(){
}

//uses ACVideoAnalysis and converts the results into ACMediaFeatures
std::vector<ACMediaFeatures*>  ACVideoPlugin::calculate(std::string aFileName) {
	ACVideoAnalysis* video = new ACVideoAnalysis(aFileName);
	std::vector<ACMediaFeatures*> allVideoFeatures;

	cout << "calculateMeanOfTrajectory" << endl;
	ACMediaFeatures* videoMeanTrajectory = this->calculateMeanOfTrajectory(video);
	if (videoMeanTrajectory != NULL){
		allVideoFeatures.push_back(videoMeanTrajectory);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL mean feature" << endl;
	}

	cout << "calculateStdOfTrajectory" << endl;
	ACMediaFeatures* videoStdTrajectory = this->calculateStdOfTrajectory(video);
	if (videoStdTrajectory != NULL){
		allVideoFeatures.push_back(videoStdTrajectory);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL std feature" << endl;
	}

	cout << "calculateContractionIndex" << endl;
	ACMediaFeatures* videoContractionIndex = this->calculateContractionIndex(video);
	if (videoContractionIndex != NULL){
		allVideoFeatures.push_back(videoContractionIndex);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL mean ci feature" << endl;
	}

	cout << "calculateMaxOfTrajectory" << endl;
	ACMediaFeatures* videoMaxTrajectory = this->calculateMaxOfTrajectory(video);
	if (videoMaxTrajectory != NULL){
		allVideoFeatures.push_back(videoMaxTrajectory);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL max feature" << endl;
	}

	cout << "calculateMeanSpeedOfTrajectory" << endl;
	ACMediaFeatures* videoMeanSpeedTrajectory = this->calculateMeanSpeedOfTrajectory(video);
	if (videoMeanSpeedTrajectory != NULL){
		allVideoFeatures.push_back(videoMeanSpeedTrajectory);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL speed feature" << endl;
	}
	
	cout << "calculateMeanBoundingBoxRatio" << endl;
	ACMediaFeatures* videoMeanBoundingBoxRatio = this->calculateMeanBoundingBoxRatio(video);
	if (videoMeanBoundingBoxRatio != NULL){
		allVideoFeatures.push_back(videoMeanBoundingBoxRatio);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL mean bounding box ratio feature" << endl;
	}

	cout << "calculateMeanPixelSpeed" << endl;
	ACMediaFeatures* videoMeanPixelSpeed = this->calculateMeanPixelSpeed(video);
	if (videoMeanPixelSpeed != NULL){
		allVideoFeatures.push_back(videoMeanPixelSpeed);
	}
	else{
		cerr << "<ACVideoPlugin::calculate> : NULL mean bounding box ratio feature" << endl;
	}
	
	delete video;
	return allVideoFeatures;
}

ACMediaFeatures* ACVideoPlugin::calculateMeanOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);

	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean(); // will do "new" and set name

	// XS TEST
	// trajectory_mtf->dump("/Users/xavier/Desktop/traj.txt");
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoPlugin::calculateStdOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);

	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->std();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoPlugin::calculateMaxOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getMergedBlobsTrajectory(), "trajectory");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->max();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoPlugin::calculateContractionIndex(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	video->computeContractionIndices();

	ACMediaTimedFeatures* ci_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getContractionIndices(), "contraction index");
	ACMediaFeatures* contractionIndex = ci_mtf->mean();
	delete ci_mtf;
	return contractionIndex;
}

ACMediaFeatures* ACVideoPlugin::calculateMeanSpeedOfTrajectory(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	video->computeMergedBlobsSpeeds(0);
	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getMergedBlobsSpeeds(), "speed");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean();
	delete trajectory_mtf;
	return trajectory_mf;
}

//ACMediaFeatures* ACVideoPlugin::calculateMostOccupiedCell(ACVideoAnalysis* video){
//	if (!video->areBlobsComputed()) video->computeBlobsUL();
//	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
//	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getMergedBlobsTrajectory(), "trajectory");
//	mat hist_m = hist3(trajectory_mtf->getValue(), 10, 10);
//	
//	// XS TODO -- this is not correct
//	// hist_m.print();
//	ACMediaFeatures* trajectory_mf = trajectory_mtf->max(); // DELETE THIS
//	//max_indice(hist_m);	
//	delete trajectory_mtf;
//	return trajectory_mf;
//}

ACMediaFeatures* ACVideoPlugin::calculateMeanBoundingBoxRatio(ACVideoAnalysis* video){
	if (!video->areBlobsComputed()) video->computeBlobsUL();
	if (!video->isTrajectoryComputed()) video->computeMergedBlobsTrajectory(0);
	
	video->computeBoundingBoxRatios();
	ACMediaTimedFeatures *trajectory_mtf = new ACMediaTimedFeatures(video->getTimeStamps(), video->getBoundingBoxRatios(), "bounding box ratios");
	ACMediaFeatures* trajectory_mf = trajectory_mtf->mean();
	delete trajectory_mtf;
	return trajectory_mf;
}

ACMediaFeatures* ACVideoPlugin::calculateMeanPixelSpeed(ACVideoAnalysis* video){
	video->computePixelSpeed();
	vector<float> t = video->getTimeStamps();
	vector<float> s = video->getPixelSpeeds();
	
	ACMediaTimedFeatures* ps_mtf = new ACMediaTimedFeatures(t,s, "pixel speed");
	ACMediaFeatures* pixel_speed = ps_mtf->mean();
	delete ps_mtf;
	return pixel_speed;
}

