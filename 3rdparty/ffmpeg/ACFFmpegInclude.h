/*
 *  ACFFmpegInclude.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/05/2011
 *
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

#ifndef ACFFmpegInclude_H
#define ACFFmpegInclude_H

extern "C" {

// ACFFmpegToOpenCV.h needs this under Ubuntu 11.04 Natty
#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif

#ifdef __cplusplus
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif
#include <errno.h>    // for error codes defined in avformat.h
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
}

#endif
