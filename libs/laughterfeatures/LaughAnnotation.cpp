/**
 * @brief LaughAnnotation.cpp is the implementation file for the LaughAnnotation class.
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

// include/using-----------------------------------------------------
#include "LaughAnnotation.h"

// IMPLEMENTATION====================================================

LaughAnnotation::LaughAnnotation() 
{
   nFramesLaugh = 0;
   nSectionsLaugh = 0;
   nFramesInspiration = 0;
   nSectionsInspiration = 0;
   nFramesSilence = 0;
   nSectionsSilence = 0;
   nFramesTrash = 0;
   nSectionsTrash = 0;
   statsCalculated = false;
   annotationSorted = false;
}

// ------------------------------------------------------------------
//get the Annotation line by line from the file
void LaughAnnotation::acquireFromFile( const string & fileName )
{
   string currentLine;

   cout << "acquiring annotation from " << fileName << endl;

   // If labels is not empty, then something's weird.
   if ( !labels.empty() )
   {
      cout << "When acquiring a LaughAnnotation from a file, the vector"
              " is expected to begin empty." << endl;
   }

   LaughAnnotationFile lAFile( fileName );
   lAFile.openFileIn();
   
   // as long as there lines to read, keep reading.
   while ( "" != ( currentLine = lAFile.getNextLine() ) )
   { 
      double startTime, endTime;
      char label;
      //cout << currentLine << endl;

      // the lines must have the float, float, character format
      sscanf( currentLine.c_str(), "%lf %lf %c", 
              &startTime, &endTime, &label );

      //store a new AnnotationLabel in the labels vector
      AnnotationLabel aLabel( startTime, endTime, label );
      labels.push_back( aLabel );
   }

   // must be tidy and close up files after reading
   lAFile.close();

   // Now sort the laughAnnotation according to startTimes.
   sortAnnotation();

   // Now go calculate the stats 
   calculateStats();
}

// ------------------------------------------------------------------
// sort the Annotation by startTimes

void LaughAnnotation::sortAnnotation( void )
{
   // Why would I want to do this if it is already sorted?
   if ( !annotationSorted )
   {
     
      // If labels is empty, then something's weird.
      if ( labels.empty() )
         cout << "When sorting a LaughAnnotation, the vector"
                 " is expected to not be empty." << endl;

      // keep repeating until the annotation is sorted  (BubbleSort)
      while( !annotationSorted )
      {
         // reset the "I'm done" flag for this run through the annotation labels
         bool swappedAtLeastOnePair = false;

         // run through the entire vector (minus one)
         for( vector< AnnotationLabel >::iterator iAL = labels.begin();
              iAL < ( labels.end() - 1 ); iAL++ )
         {
            // looking for neighbors out of order to swap
            if ( ( *( iAL + 1 ) ).getStartTime() < ( *iAL ).getStartTime() ) 
            {
               // swap iAL and iAL+1
               AnnotationLabel dummyAL = *iAL;
               *iAL = *( iAL + 1 );
               *( iAL + 1 ) = dummyAL;
               // set the "I'm done" flag to "I'm not done."
               swappedAtLeastOnePair = true;
            } 
         }

         // I'm really done if the "I'm done" flag says so.
         annotationSorted = !swappedAtLeastOnePair;
      }

   }
   // annotationSorted will now be true;
}

// ------------------------------------------------------------------
// calculate the statistics based on the labels

void LaughAnnotation::calculateStats( void )
{
   // If labels is empty, then something's weird.
   if ( labels.empty() )
      cout << "When calculating the stats for a LaughAnnotation, the vector"
              " is expected to not be empty." << endl;

   // Initialize some variables.
   episodeStartTime = 0.0;
   episodeEndTime = 0.0;
   double timeOfSectionLaugh = 0.0;
   bool firstLFound = false;
   bool lastSectionWasLaugh = false;
   bool frameExtendsSection = false;
   char label = 0; 
   unsigned long int nFramesLaughInThisSection = 0;

#ifdef DEBUG
   cout << "DEBUG mode" << endl;
#endif
   // Iterate over the annotation
   for( vector< AnnotationLabel >::iterator iAL = labels.begin();
        iAL < labels.end(); iAL++ )
   {
      double startTime = (*iAL).getStartTime();
      double endTime = (*iAL).getEndTime();;
      char lastLabel = label;   // store the last label for logic
      label = (*iAL).getLabel();;

      // collect information to be able to calculate statistics

      // Does this frame extend the current section?
      frameExtendsSection = ( label == lastLabel );

      // The last section was a laugh if ...
      if ( ( frameExtendsSection && lastSectionWasLaugh ) 
         || ( !frameExtendsSection && ( 'l' == lastLabel ) ) )
            lastSectionWasLaugh = true;
      else
         lastSectionWasLaugh = false;

      switch( label )
      {
         // if this is an inspiration section
         case 'i':
            // increment the inspiration counts.
            ++nFramesInspiration;
            if ( !frameExtendsSection )
               ++nSectionsInspiration;

            // episode end time is the end time of the last laugh 
            // or inspiration section following a laugh
            if ( lastSectionWasLaugh )
               episodeEndTime = endTime;

            break; 

         // if this is a laugh section
         case 'l':
            // increment the laugh counts
            ++nFramesLaugh;
            ++nFramesLaughInThisSection;
            double timeThisLaugh = endTime - startTime;
            totalTimeLaugh += timeThisLaugh;
            timeOfSectionLaugh += timeThisLaugh;
            if ( !frameExtendsSection )
               ++nSectionsLaugh;

            // episode end time is the end time of the last laugh 
            // or inspiration following a laugh
            episodeEndTime = endTime;

            // beginning of the first laugh is the episode start
            if (!firstLFound) 
            {
               firstLFound = true;
               episodeStartTime = startTime;
            }

            break;
      
         // if this is a silence section
         case 's':
            // increment the silence counts
            ++nFramesSilence;
            if ( !frameExtendsSection )
               ++nSectionsSilence;

            break;

         // if this is a trash section
         case 't': 
            // increment the trash counts
            ++nFramesTrash;
            if ( !frameExtendsSection )
               ++nSectionsTrash;
            
            break;

         default:
            cout << "An unknown label appeared = " << label << endl;
      }

      // If this is the first frame after a laugh, or the last frame and a
      // laugh, write some values
      if  ( ( !frameExtendsSection && lastSectionWasLaugh )
          || ( ( ( labels.end() - 1 ) == iAL ) && ( 'l' == label ) ) )
      {
         nFramesLaughInSectionsLaugh.push_back( nFramesLaughInThisSection );
         timesOfSectionsLaugh.push_back( timeOfSectionLaugh );
         nFramesLaughInThisSection = 0;
         timeOfSectionLaugh = 0.0;
      }

   } // Finish iterating through the annotation

   // Now that the label stats are known, calculate some other values.
   episodeLength = episodeEndTime - episodeStartTime;
   percentTimeLaugh = totalTimeLaugh/episodeLength;
   laughSectionsPerSecond = nSectionsLaugh / episodeLength;

   // necessary because sometimes there is only one (or no!) laughter sections
   if( 0 < nSectionsLaugh )
   {
      meanTimeLaugh = totalTimeLaugh/nSectionsLaugh;
      meanFramesLaughPerSectionsLaugh = (float)nFramesLaugh / nSectionsLaugh;
      framesPerSecondInSectionsLaugh = nFramesLaugh / totalTimeLaugh;
   }
   else
   {
      meanTimeLaugh = 0.0;
      meanFramesLaughPerSectionsLaugh = 0.0;
      framesPerSecondInSectionsLaugh = 0.0;
   }
   if( 1 < nSectionsLaugh )
   {
      double dummyArray[ timesOfSectionsLaugh.size() ];
      copy( timesOfSectionsLaugh.begin(), timesOfSectionsLaugh.end(), dummyArray );
      stdDevTimeLaugh = gsl_stats_sd( dummyArray, 1, timesOfSectionsLaugh.size() );
      unsigned long int dummyArray2[ nFramesLaughInSectionsLaugh.size() ];
      copy( nFramesLaughInSectionsLaugh.begin(), 
         nFramesLaughInSectionsLaugh.end(), dummyArray2 );
      stdDevFramesLaughPerSectionsLaugh = gsl_stats_ulong_sd( dummyArray2, 1, 
         nFramesLaughInSectionsLaugh.size() );
   }
   else 
   {
      stdDevTimeLaugh = 0.0;
      stdDevFramesLaughPerSectionsLaugh = 0.0;
   }

   statsCalculated = true;

}
// ------------------------------------------------------------------
// show the current annotation on the screen

void LaughAnnotation::show( void )
{
   // Show all the AnnotationLabels in the labels vector
  for ( vector< AnnotationLabel >::iterator it = labels.begin(); 
        it != labels.end(); ++it )
  {
     // Using an interator forces the use of the pointer precedence
     (*it).show();
  }

   // If this is a debug build, output bout info to a file.
#ifdef DEBUG
   TextFile debugDataFile( "boutStats.dat" );
   debugDataFile.openFileOut();

  for ( int i = 0; i < nFramesLaughInSectionsLaugh.size(); i++ )
  {
     cout << " bout = " << i << " - ";
     cout << " bursts in bouts = " << nFramesLaughInSectionsLaugh[ i ]<< " - ";
     cout << " bout length = " << timesOfSectionsLaugh[ i ] << endl;
     stringstream tmpLine;
     tmpLine << i << " " << nFramesLaughInSectionsLaugh[ i ]
        << " " << timesOfSectionsLaugh[ i ];
     debugDataFile.writeLine( tmpLine.str() );
  }

   debugDataFile.close();
#endif

} 

// ------------------------------------------------------------------
// show the current annotation on the screen

void LaughAnnotation::showStats( void )
{
  //cout << "total labels = " << labels.size() + 1 << endl;
  cout << "episode length = " << episodeLength << endl;
  cout << "number of bouts = " << nSectionsLaugh << endl;
  cout << "total time in bouts = " << totalTimeLaugh << endl;
  cout << "% of time in bouts = " << percentTimeLaugh << endl;
  cout << "mean of bout length = " << meanTimeLaugh << endl;
  cout << "standard deviation of bout length = " << stdDevTimeLaugh << endl;
  cout << "bouts/second = " << laughSectionsPerSecond << endl;
  cout << "number of laugh bursts = " << nFramesLaugh << endl;
  cout << "mean of laugh bursts in a bout = " 
       << meanFramesLaughPerSectionsLaugh << endl;
  cout << "standard deviation of laugh bursts in a bout= " 
       << stdDevFramesLaughPerSectionsLaugh << endl;
  cout << "bursts/second inside bouts = " 
       << framesPerSecondInSectionsLaugh << endl;
  cout << "seconds/burst inside bouts = " 
       << 1.0/framesPerSecondInSectionsLaugh << endl;
}

// ------------------------------------------------------------------
// put the current annotation into a features vector

FeaturesVector LaughAnnotation::statsAsVector( void )
{
   vector< float > outputStats;

   if ( statsCalculated ) 
   {
      outputStats.push_back( episodeLength );
      outputStats.push_back( nSectionsLaugh );
      outputStats.push_back( totalTimeLaugh );
      outputStats.push_back( percentTimeLaugh );
      outputStats.push_back( meanTimeLaugh );
      outputStats.push_back( stdDevTimeLaugh );
      outputStats.push_back( laughSectionsPerSecond );
      outputStats.push_back( nFramesLaugh );
      outputStats.push_back( meanFramesLaughPerSectionsLaugh );
      outputStats.push_back( stdDevFramesLaughPerSectionsLaugh );
      outputStats.push_back( framesPerSecondInSectionsLaugh );
   }  // statsCalculated    

   return outputStats;
}

// ------------------------------------------------------------------
