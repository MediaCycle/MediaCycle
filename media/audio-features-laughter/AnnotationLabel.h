/**
 * @brief AnnotationLabel.h is the interface file for the AnnotationLabel class.
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

#ifndef ANNOTATIONLABEL_H
#define ANNOTATIONLABEL_H

// include/using-----------------------------------------------------
#include <iostream>
using namespace std; 

// constants---------------------------------------------------------

// INTERFACE=========================================================

class AnnotationLabel 
{
   public:
      AnnotationLabel();
      AnnotationLabel( const float &, const float &, const char & );
      // default copy constructor
      // default destructor
      float getStartTime( void ) { return startTime; }
      float getEndTime( void ) { return endTime; }
      char getLabel( void ) { return label; }
      void setStartTime( const float s ) { startTime = s; }
      void setEndTime( const float e ) { endTime = e; }
      void setLabel( const char l ) { label = l; }
      void show( void );
   protected:
      float startTime;
      float endTime;
      char label;
   private:
};
// ------------------------------------------------------------------
#endif
