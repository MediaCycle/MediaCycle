
// J. Anderson) Mills III
// 2009-05-26
//
// LaughAnnotationFile.h
// is the header file for the LaughAnnotationFile class.

#ifndef LAUGHANNOTATIONFILE_H
#define LAUGHANNOTATIONFILE_H

// include/using-----------------------------------------------------
#include <string>
#include "File.h"

// constants---------------------------------------------------------

// INTERFACE=========================================================

class LaughAnnotationFile : public File    // inherit from File class
{
   public:
      LaughAnnotationFile() {};
      LaughAnnotationFile( const string &name ) : File( name ) {};
      string getNextLine();
      // default copy constructor
      // default destructor
   protected:
   private:
};

// ------------------------------------------------------------------
#endif
