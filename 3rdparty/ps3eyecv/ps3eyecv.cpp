/**
 * @brief OpenCV wrapper around the Sony PS3 EYE Camera Driver using libusb
 * @author Christian Frisson
 * @date 21/11/2013
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

#include "ps3eyecv.h"
#include <iostream>
#include <vector>
#include <cmath>

static const int ITUR_BT_601_CY = 1220542;
static const int ITUR_BT_601_CUB = 2116026;
static const int ITUR_BT_601_CUG = -409993;
static const int ITUR_BT_601_CVG = -852492;
static const int ITUR_BT_601_CVR = 1673527;
static const int ITUR_BT_601_SHIFT = 20;

static void yuv422_to_rgba(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height)
{
    const int bIdx = 2;
    const int uIdx = 0;
    const int yIdx = 0;
    
    const int uidx = 1 - yIdx + uIdx * 2;
    const int vidx = (2 + uidx) % 4;
    int j, i;
    
#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)
    
    for (j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 4) * j; // 4 channels
        
        for (i = 0; i < 2 * width; i += 4, row += 8)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;
            
            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;
            
            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            row[3]      = (0xff);
            
            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[6-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[5]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[4+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            row[7]      = (0xff);
        }
    }
}

using namespace ps3eye;

ps3eyecv::ps3eyecv(int _width, int _height, int _framerate)
    :  ps3eyethread(),m_new_rgb_frame(false),streaming(false),
      rgbMat(cv::Size(_width,_height),CV_8UC4,cv::Scalar(0))
{
    // list out the devices
    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );

    if(devices.size())
    {
        eye = devices.at(0);
        bool res = eye->init(_width, _height, _framerate);
        eye->start();
        videoFrame 	= new uint8_t[eye->getWidth()*eye->getHeight()*4];
        this->start();
    }
}

ps3eyecv::~ps3eyecv(){
    streaming = false;
    //waitExit();
    if(eye) eye->stop();
    delete[] videoFrame;
}

void ps3eyecv::thread()
{
    streaming = true;
    while( streaming )
    {

        bool res = ps3eye::PS3EYECam::updateDevices();
        if(!res)
        {
            break;
        }

        if(eye)
        {
            bool isNewFrame = eye->isNewFrame();
            if(isNewFrame)
            {
                m_rgb_mutex.lock();
                yuv422_to_rgba(eye->getLastFramePointer(), eye->getRowBytes(), videoFrame, eye->getWidth(),eye->getHeight());
                rgbMat.data = videoFrame;
                m_new_rgb_frame = true;
                m_rgb_mutex.unlock();
            }

            /*camFrameCount += isNewFrame ? 1: 0;
                 float timeNow = ofGetElapsedTimeMillis();
                 if( timeNow > camFpsLastSampleTime + 1000 ) {
                 uint32_t framesPassed = camFrameCount - camFpsLastSampleFrame;
                 camFps = (float)(framesPassed / ((timeNow - camFpsLastSampleTime)*0.001f));

                 camFpsLastSampleTime = timeNow;
                 camFpsLastSampleFrame = camFrameCount;
                 }*/
        }
    }
}

bool ps3eyecv::getVideo(cv::Mat& output) {
    m_rgb_mutex.lock();
    if(m_new_rgb_frame) {
        cv::cvtColor(rgbMat, output, cv::COLOR_RGB2GRAY /*cv::COLOR_RGB2BGR*/);
        m_new_rgb_frame = false;
        m_rgb_mutex.unlock();
        return true;
    } else {
        m_rgb_mutex.unlock();
        return false;
    }
}

