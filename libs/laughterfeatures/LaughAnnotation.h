/**
 * @brief LaughAnnotation.h is the interface file for the LaughAnnotation class.
 * @author J. Anderson Mills III
 * @date 2009-05-26
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

#ifndef LAUGHANNOTATION_H
#define LAUGHANNOTATION_H

// include/using-----------------------------------------------------
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_statistics_ulong.h>
#include <gsl/gsl_statistics_double.h>

using namespace std;

#include "AnnotationLabel.h"
#include "LaughAnnotationFile.h"

#ifdef DEBUG
#include "TextFile.h"
#endif
// constants---------------------------------------------------------

// INTERFACE=========================================================

class LaughAnnotation 
{
   public:
      LaughAnnotation();                    // costructor
      // default copy constructor
      // default destructor
      void acquireFromFile( const string & );  // get the Annotation from a file 
      void show( void );                   // show the Annotation on the screen
      void showStats( void );              // show the Annotation statistics
   protected:
      vector< AnnotationLabel > labels;
      void calculateStats( void );
      // Some notes about semantics.  - First laughter.
      // An episode is from when one starts laughing until one stops.
      // A bout is a full exhalation of laughter through and including an
      //    inhale (or inspiration).
      // A burst is a short unit of sound determined by a rise, continuation,
      //    and fall of energy.
      // - Second annotation
      // A frame is the time which is given one label.  Hopefully this
      //   corresponds to bursts, but may not.
      // A section is the time from the beginning to the end of the same label
      //   and may include several frames.
      double episodeStartTime;
      double episodeEndTime;
      double totalTimeLaugh;
      double episodeLength;
      double percTimeLaugh;
      double meanTimeLaugh;
      double stdDevTimeLaugh;
      unsigned long int nFramesLaugh;
      unsigned long int nSectionsLaugh;
      unsigned long int nFramesInspiration;
      unsigned long int nSectionsInspiration;
      unsigned long int nFramesSilence;
      unsigned long int nSectionsSilence;
      unsigned long int nFramesTrash;
      unsigned long int nSectionsTrash;
      double laughSectionsPerSecond;
      double meanFramesLaughPerSectionsLaugh;
      double stdDevFramesLaughPerSectionsLaugh;
      double framesPerSecondInSectionsLaugh;
      vector< unsigned long int > nFramesLaughInSectionsLaugh;
      vector< double > timesOfSectionsLaugh;
   private:
};
// ------------------------------------------------------------------
#endif
