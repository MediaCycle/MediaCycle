/**
 * @brief File.cpp is the implementation file for the base File class.
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
#include "File.h"

// implementation----------------------------------------------------

File::File() {}

// ------------------------------------------------------------------
// Constructor which sets the name of the file.
File::File( const string &nameValue ) {
   setName( nameValue );
}

// ------------------------------------------------------------------
// Setting the name of the file.
void File::setName( const string & nameValue) {
   fileName = nameValue;
}
// ------------------------------------------------------------------
// Open the file for input to program.
void File::openFileIn( void ) {
   file.open( fileName.c_str(), ios::in );
   if ( !file ) {
      cerr << "Error: Cannot input filename: " << fileName << endl;
      exit( -1 ); 
   }
}
// ------------------------------------------------------------------
// Open a file for output from program.
void File::openFileOut( void ) {
   file.open( fileName.c_str(), ios::out );
   if ( !file ) {
      cerr << fileName << " could not be opened for output." << endl;
      exit( -1 ); 
   }
}
// ------------------------------------------------------------------
// Close the file.
void File::close( void ) {
   file.close();
}
// ------------------------------------------------------------------
// Return the stream for input or output.
fstream &File::getStream()
{
   return file;
}
// ------------------------------------------------------------------
