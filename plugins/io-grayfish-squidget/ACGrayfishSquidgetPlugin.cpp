/**
 * @brief Plugin for using the Grayfish Squidget jog wheel for intra-media browsing (ACM TEI 2014)
 * @author Christian Frisson
 * @date 12/02/2014
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACGrayfishSquidgetPlugin.h"
#include <MediaCycle.h>

#include <ps3eyecv.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <deque>

// Headers needed for sleeping.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace cv;
using namespace std;
using namespace ps3eye;

ACGrayfishSquidgetPlugin::ACGrayfishSquidgetPlugin()
    : QObject(), ACPluginQt(), ACClientPlugin()
{
    this->mName = "Grayfish Squidget jog wheel for intra-media browsing";
    this->mDescription ="Plugin for using the Grayfish Squidget jog wheel for intra-media browsing (ACM TEI 2014)";
    this->mMediaType = MEDIA_TYPE_ALL;

    this->active = false;

    this->init_device();
}

bool ACGrayfishSquidgetPlugin::init_device(){

    //
    std::cout << "Grayfish Squidget device init" << std::endl;
    return this->StartInternalThread();
}


ACGrayfishSquidgetPlugin::~ACGrayfishSquidgetPlugin(){

    this->active = false;

    this->WaitForInternalThreadToExit();

#ifdef WIN32
    system("pause");
#endif
}

void ACGrayfishSquidgetPlugin::device_loop(){
    std::cout << "Grayfish Squidget device loop" << std::endl;

    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );
    if(devices.size()==0)
        return;

    /*int _width = 640;
    int _height = 480;
    int _framerate = 60;*/

    int _width = 320;
    int _height = 240;
    int _framerate = 125;

    int _slice = 5;
    int _bits = 8;
    float _slices = 9;

    int _span = (int)(pow(2.0f,_bits));

    Mat _rgbMat(Size(_width,_height),CV_8UC4,Scalar(0));

    ps3eyecv device = ps3eyecv(_width,_height,_framerate);

    int value = 0;
    std::clock_t time = 0;
    int counter = 0;
    std::deque<float> buffer;
    int bufsize = 32;
    buffer.assign(bufsize,0.0f);
    float speed = 0;
    int direction = 0;
    int d_diff = 0;

    int turns = 0;

    while (active) {
        device.getVideo(_rgbMat);
        //cv::imshow("rgb", _rgbMat);
        cv::Mat slice(_rgbMat, cv::Rect(0,(int)(0.5f*_height)-_slice,_width,2*_slice));
        //cv::imshow("slice",slice);

        std::vector<int> bin(_slices,0);
        std::vector<int> gray(_slices,0);

        for(int s=0;s<_slices;s++){
            std::stringstream _name;
            _name << "slice " << s;
            cv::Mat sliced(_rgbMat, cv::Rect(s*_width/(float)_slices,(int)(0.5f*_height)-_slice,_width/(float)_slices,2*_slice));
            //cv::imshow(_name.str(),sliced);

            //computes mean over roi
            cv::Scalar avgPixelIntensity = cv::mean( sliced );

            //prints out only .val[0] since image was grayscale
            int _value = (avgPixelIntensity.val[0] > 40) ? 1 : 0;
            gray[s]=_value;
        }

        /*for(int s=0;s<_bits;s++){
            std::cout << gray[s] << " ";
        }*/

        //std::cout << " > ";

        /*Bn = Gn // MS bits always the same */
        bin[_slices-2] = gray[_slices-2];
        int _value = bin[_slices-2];
        //std::cout << bin[_slices-2] << " ";

        for(int s=_slices-2;s>=0;s--){
            bin[s] = bin[s+1] ^ gray[s];
            //std::cout << bin[s] << " ";
            _value += bin[s]*pow(2.0f,s);
        }
        /*
        Bn-1 = Bn ^ Gn-1 // Exclusive 'OR' the bits together to construct the 7 bit binary value
        ...
        B0 = B1 ^ G0
*/
        //std::cout << " > ";

        if( (_value-value) !=0 && value!=0){
            int _direction = (_value<value ? 1 : -1);
            float _d_diff = ((direction != _direction && direction !=0) ? ((_value-value+_span+5)%_span)-5 : _value-value);
            direction = _direction;
            if( d_diff*_d_diff!=-1){
                std::clock_t _time = clock();
                float _t_diff = (_time - time);
                speed += _d_diff / _t_diff;
                speed -= buffer.front();
                buffer.pop_front();
                buffer.push_back( _d_diff / _t_diff);

                speed = _d_diff / _t_diff;

                std::cout << _value;
                std::cout << " " << speed/(float)bufsize*1000;
                std::cout << std::endl;

                time = _time;

                if ( fabs(_value-value) >= _span - 1)
                    turns+=direction;

                if(this->getTimeline() && this->getTimeline()->getTrack(0)){
                    float pos_x = this->getTimeline()->getTrack(0)->getSelectionPosX() + 0.1f*_d_diff/(float)_span /*(float)value/(float)_span*/;
                    if(pos_x < 0.0f) pos_x = 1.0f + pos_x;
                    if(pos_x > 1.0f) pos_x = pos_x - 1.0f;
                    this->getTimeline()->getTrack(0)->setManualSelection(true);
                    this->getTimeline()->getTrack(0)->moveSelection( pos_x , 0.0f);
                }

            }
            else{
                if(speed !=0){
                    speed =0;
                    buffer.clear();
                    buffer.assign(bufsize,0.0f);
                }
            }
            d_diff = _d_diff;
        }
        else{
            std::clock_t _time = clock() - time;
            //speed -= buffer.front();

            buffer.push_back(0);
            buffer.pop_front();
        }
        std::clock_t _time = clock() - time;
        if(speed !=0 && (fabs(speed) < 0.000001 || _time > 1000000 )){
            speed = 0;
            std::cout << _value;
            std::cout << " " << speed/(float)bufsize*1000;
            std::cout << std::endl;

            if(this->getTimeline() && this->getTimeline()->getTrack(0)){
                 this->getTimeline()->getTrack(0)->setManualSelection(false);
            }

        }
        value = _value;
        //usleep(8000);

        /*if(this->getTimeline() && this->getTimeline()->getTrack(0) && this->getTimeline()->getTrack(0)->getSharedThumbnailStream()){
            osg::ref_ptr<osg::ImageStream> stream = this->getTimeline()->getTrack(0)->getSharedThumbnailStream();
            float current_time = stream->getCurrentTime();
            float _length = stream->getLength();
            stream->seek(_length/(float)_span*(float)value);

            /*float destination = current_time + speed/(float)bufsize/ 0.002;
            if(destination > _length)
                destination = 0;
            if(destination < 0)
                destination += _length;
            stream->seek(destination );*/
        //}
        usleep(8000); // 8ms @ 125fps
    }

}

std::vector<ACInputActionQt*> ACGrayfishSquidgetPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    return actions;
}
