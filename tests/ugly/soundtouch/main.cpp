/**
 * @brief BPM detection test app adapted from the SoundStretch main routine
 * @authors Olli Parviainen, Christian Frisson
 * @date 6/03/2013
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

//
////////////////////////////////////////////////////////////////////////////////
//
// License :
//
//  SoundTouch audio processing library
//  Copyright (c) Olli Parviainen
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SoundTouch.h"
#include "WavFile.h"
#include "BPMDetect.h"

using namespace soundtouch;
using namespace std;

// Processing chunk size
#define BUFF_SIZE           2048

#if _WIN32
    #include <io.h>
    #include <fcntl.h>

    // Macro for Win32 standard input/output stream support: Sets a file stream into binary mode
    #define SET_STREAM_TO_BIN_MODE(f) (_setmode(_fileno(f), _O_BINARY))
#else
    // Not needed for GNU environment... 
    #define SET_STREAM_TO_BIN_MODE(f) {}
#endif

// Detect BPM rate of inFile and adjust tempo setting accordingly if necessary
static void detectBPM(WavInFile *inFile)
{
    float bpmValue;
    int nChannels;
    BPMDetect bpm(inFile->getNumChannels(), inFile->getSampleRate());
    SAMPLETYPE sampleBuffer[BUFF_SIZE];

    // detect bpm rate
    printf( "Detecting BPM rate...");
    //fflush(stderr);

    nChannels = (int)inFile->getNumChannels();
    assert(BUFF_SIZE % nChannels == 0);

    // Process the 'inFile' in small blocks, repeat until whole file has 
    // been processed
    while (inFile->eof() == 0)
    {
        int num, samples;

        // Read sample data from input file
        num = inFile->read(sampleBuffer, BUFF_SIZE);

        // Enter the new samples to the bpm analyzer class
        samples = num / nChannels;
        bpm.inputSamples(sampleBuffer, samples);
    }

    // Now the whole song data has been analyzed. Read the resulting bpm.
    bpmValue = bpm.getBpm();
    fprintf(stderr, "Done!\n");

    // rewind the file after bpm detection
    inFile->rewind();

    if (bpmValue > 0)
    {
        printf("Detected BPM rate %.1f\n\n", bpmValue);
    }
    else
    {
        printf("Couldn't detect BPM rate.\n\n");
        return;
    }

    /*if (params->goalBPM > 0)
    {
        // adjust tempo to given bpm
        params->tempoDelta = (params->goalBPM / bpmValue - 1.0f) * 100.0f;
        fprintf(stderr, "The file will be converted to %.1f BPM\n\n", params->goalBPM);
    }*/
}

int main(int argc, char *argv[])
{
    WavInFile *inFile = 0;

    if(argc!=2)
        return 0;

    try 
    {
        inFile = new WavInFile(argv[1]);
        detectBPM(inFile);
        fprintf(stderr, "Done!\n");
    } 
    catch (const runtime_error &e) 
    {
        // An exception occurred during processing, display an error message
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}
