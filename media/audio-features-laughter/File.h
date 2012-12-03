/**
 * @brief File.h is the interface file for the base File class.
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

#ifndef FILE_H
#define FILE_H

// include/using-----------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>  //JU: for exit
using namespace std;

// constants---------------------------------------------------------

// interface---------------------------------------------------------

class File {
   public:
      File();                          // constructors
      File( const string & );
      // default copy constructor
      // default destructor
      void setName( const string & );  // setting the filename
      string getName() const { return fileName; }  // get the filename
      void openFileIn( void );         // input file
      void openFileOut( void );        // output file
      void close( void );              // close the file
      fstream &getStream();            // return the fstream
   protected:
      fstream file;
      string fileName;
   private:

};

// ------------------------------------------------------------------
#endif
