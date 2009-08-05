/* 
 * File:   ACOpenCVInclude.h
 * Author: Alexis Moinet
 *
 * @date 15 juillet 2009
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

#ifndef ACOPENCVINCLUDE_H
#define	ACOPENCVINCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef __APPLE__
    #ifdef MAC_FRAMEWORKS
    #include <OpenCV/cv.h>
    #include <OpenCV/cxcore.h>
    #include <OpenCV/highgui.h> // for cvloadimage
    #else
    // -lcv -lhighgui -lcxcore
    // I path: /opt/local/include/opencv or /usr/local/include/opencv
    // L path: /opt/local/lib or /usr/local/lib
    // as of 05/03/2009, MacPorts (/opt/*) only provides version 1.0
    // I downloaded 1.1 from SourceForge
    #include <OpenCV/cv.h>
    #include <OpenCV/cxcore.h>
    #include <OpenCV/highgui.h> // for cvloadimage
    #endif
#else
    #include <opencv/cv.h>
    #include <opencv/cxcore.h>
    #include <opencv/highgui.h>
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* ACOPENCVINCLUDE_H */
