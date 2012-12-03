
// J. Anderson) Mills III
// 2009-06-12
//
// TextFile.h
// is the header file for the TextFile class.

#ifndef TEXTFILE_H
#define TEXTFILE_H

// include/using-----------------------------------------------------
#include <string>
#include "File.h"

// constants---------------------------------------------------------

// INTERFACE=========================================================

class TextFile : public File    // inherit from File class
{
   public:
      TextFile() {};
      TextFile( const string &name ) : File( name ) {};
      string getNextLine();
      int writeLine( const string &line );
      // default copy constructor
      // default destructor
   protected:
   private:
};

// ------------------------------------------------------------------
#endif
