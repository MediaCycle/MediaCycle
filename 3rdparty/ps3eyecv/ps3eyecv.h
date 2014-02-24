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

#include "ps3eyethread.h"
#include "ps3eyemutex.h"
#include "ps3eye.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class ps3eyecv : public ps3eyethread {
public:

    ps3eyecv(int _width = 640, int _height = 480, int _framerate = 60);
    ~ps3eyecv();
    virtual void thread();
    bool getVideo(cv::Mat& output);

private:
    std::vector<uint8_t> m_buffer_rgb;
    cv::Mat rgbMat;
    cv::Mat ownMat;
    ps3eyemutex m_rgb_mutex;
    bool m_new_rgb_frame;

    ps3eye::PS3EYECam::PS3EYERef eye;
    pthread_t threadUpdate;
    uint8_t * videoFrame;
    int camFrameCount;
    int camFpsLastSampleFrame;
    float camFpsLastSampleTime;
    float camFps;
    bool streaming;
};
