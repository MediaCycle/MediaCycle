/*
 *  ACHuMomentsVideoPlugin.cpp
 *  MediaCycle
 *
 *  @author Sidi Mahmoudi and Xavier Siebert and Christian Frisson
 *  @date 05/01/11
 *  @copyright (c) 2011 – UMONS - Numediart
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
#if defined(USE_STARPU)
#include "ACHuMomentsVideoPlugin.h"

#include <stdio.h>
#include <stdint.h>
#include <starpu.h>
#include <pthread.h>
#include <sys/time.h>

#include <ACOpenCVInclude.h>

#include <iostream>
using namespace std;

void cpu_codelet(void *descr[], void *_args);

// The old version (05/01/2011) featured the following, uncommented:
/*
starpu_codelet cl =
{
    #if defined(USE_STARPU_CPU)
        .where = STARPU_CPU,
        .cpu_func = cpu_codelet,
    #endif 
    #endif
*/ 

ACHuMomentsVideoPlugin::ACHuMomentsVideoPlugin() : ACTimedFeaturesPlugin(){
	//vars herited from ACPlugin
	this->mMediaType = MEDIA_TYPE_VIDEO;
	this->mName = "Hu Moments (StarPU)";
	this->mDescription = "Hu_Moments_StarPU";
	this->mId = "";
	this->mDescriptorsList.push_back("Hu Moments (StarPU)");
	this->mtf_file_name = "";
	//starpu_init(NULL);
}

ACHuMomentsVideoPlugin::~ACHuMomentsVideoPlugin() {
    starpu_shutdown();
}

void cpu_codelet(void *descr[], void *_args)
{
	int l1= 512;
	int h1= 512;

	float *res = (float *)STARPU_VECTOR_GET_PTR(descr[0]);
	cv::Mat resultat1=cv::Mat(l1,h1,IPL_DEPTH_8U);
	int y,p;
	for(y = 0; y < (l1*h1); y++)
	{
		resultat1.data[y] = res[y];
	}

	cvSmooth(resultat1, resultat1, CV_GAUSSIAN, 3, 7, 3, 0);
	cvCanny(resultat1, resultat1, 20, 20 * 3, 3);
	
	uchar* ddata;
	ddata = (uchar*) resultat1.data;

	for (p = 0; p < (l1*h1); p++)
	{
		res[p] = (float) ddata[p];
	}
}

//std::vector<ACMediaFeatures*>  ACHuMomentsVideoPlugin::calculate(std::string aFileName, bool _save_timed_feat) {
//	return this->_calculate(aFileName,_save_timed_feat);
//}

std::vector<ACMediaFeatures*> ACHuMomentsVideoPlugin::calculate(ACMediaData* _data, ACMedia* theMedia, bool _save_timed_feat) {
    //starpu_init(NULL);
	std::string aFileName=_data->getFileName()
	std::vector<FeaturesVector> desc;
	ACMediaTimedFeature* descmtf;
	vector<ACMediaFeatures*> result;
	vector<float>time_stamps;
	
	cv::VideoCapture capture;
	cv::Mat img;
	cv::Mat resultat;
	cv::Mat frame;
	cv::Mat frame1;

	capture.open(aFileName.c_str());
	int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	resultat = cv::Mat(width, height, IPL_DEPTH_8U);
	frame1 = cv::Mat(512, 512, IPL_DEPTH_8U);
	frame = cv::Mat(width, height, IPL_DEPTH_8U);

	int p, n, j;
	const int area = width*height;
	float * data_in1 = (float*) malloc(area * sizeof (float));
	uchar* ddata;
	int nframe = (int) capture.get(cv::CAP_PROP_FRAME_COUNT);

	CvMoments myRawmoments;
	CvHuMoments myHumoments;

	// The newer version (14/04/2011) had the following uncommented
	//starpu_init(NULL);
	starpu_data_handle A_handle[nframe];

	starpu_codelet cl;
	memset (&cl, 0, sizeof(cl));

	#if defined(USE_STARPU_CPU)
	cl.where |= STARPU_CPU;
	cl.cpu_func = cpu_codelet,
	#endif
		cl.nbuffers = 1;
	/// end of new version
	
	for (j = 0; j < nframe; ++j) {
		capture.set(cv::CAP_PROP_POS_FRAMES, j);
		cvGrabFrame(capture);
		img = cvRetrieveFrame(capture, 0);
		cvCvtColor(img, frame, cv::COLOR_BGR2GRAY);
		cvResize(frame, frame1, cv::INTER_CUBIC);
		ddata = (uchar*) frame1.data;

		for (p = 0; p < area; p++) {
			data_in1[p] = (float) ddata[p];
		}

		starpu_data_handle A_handle;
		starpu_vector_data_register(&A_handle, 0 /* home node */,(uintptr_t) data_in1, area, sizeof (float));
		struct starpu_task *task = starpu_task_create();
		task->cl = &cl;
		// On remplit les 3 buffers par les handles enregistrés

		task->buffers[0].handle = A_handle;
		task->buffers[0].mode = STARPU_RW;

		starpu_task_submit(task);
//		starpu_data_sync_with_mem(A_handle, STARPU_R);//CF this won't compile, to debug!

		// ou starpu_data_acquire

		int x = 0;
		//for (x = 0; x < (l * h); x++) {//old version
		for (x = 0; x < (area); x++) {//new version	
			resultat.data[x] = data_in1[x];
		}
		cvMoments(resultat, &myRawmoments, 0);
		cvGetHuMoments(&myRawmoments, &myHumoments);

		FeaturesVector descfv;
		
		descfv.push_back((float)myHumoments.hu1);
		descfv.push_back((float)myHumoments.hu2);
		descfv.push_back((float)myHumoments.hu3);
		descfv.push_back((float)myHumoments.hu4);
		descfv.push_back((float)myHumoments.hu5);
		descfv.push_back((float)myHumoments.hu6);
		descfv.push_back((float)myHumoments.hu7);

		#ifdef USE_DEBUG
			std::cout << "ACHuMomentsVideoPlugin: Frame " << j << "/" << nframe << " Hu Moments:";
			for (int hm = 0;hm<descfv.size();hm++)
				std::cout << " " << descfv[hm];
			std::cout << std::endl;
		#endif		

		desc.push_back(descfv);
		time_stamps.push_back(j);
		descfv.clear();
	}

	starpu_task_wait_for_all();
	cvReleaseImage(&frame);
	cvReleaseImage(&frame1);
	cvReleaseCapture(&capture);

	descmtf = new ACMediaTimedFeature(time_stamps,desc,"Hu Moments (StarPU)");
	ACMediaFeatures* tmp = descmtf->mean();
	result.push_back(tmp);
	bool _binary=false;
	theMedia->addTimedFileNames(this->saveTimedFeatures(descmtf, aFileName, _save_timed_feat,_binary)); // by default : binary
	delete descmtf;
        
	return result;
	//starpu_shutdown();
}
#endif