/* 
 * File:   DynamicLibrary.h
 * Author: Julien Dubois
 *
 * @date 27 juillet 2009
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

#ifndef _DYNAMICLIBRARY_H
#define	_DYNAMICLIBRARY_H

/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <sys/stat.h>

bool fileExists(std::string strFilename);

class DynamicLibrary
{
    public:

        typedef void*   HANDLE;
        typedef void*   PROC_ADDRESS;
        
        /** returns a pointer to a DynamicLibrary object on successfully
          * opening of library returns NULL on failure.
          */
        static DynamicLibrary* loadLibrary(const std::string& libraryName);

        /** return name of library stripped of path.*/
	const std::string& getName() const     { return _name; };

        /** return name of library including full path to it.*/
	const std::string& getFullName() const { return _fullName; };

        /** return handle to .dso/.dll dynamic library itself.*/
	HANDLE             getHandle() const   { return _handle; };

        /** return address of function located in library.*/
        PROC_ADDRESS       getProcAddress(const std::string& procName);

   //protected:

        /** get handle to library file */
        static HANDLE getLibraryHandle( const std::string& libraryName);
        
        /** disallow default constructor.*/
	DynamicLibrary() { _handle = NULL; };
        /** disallow copy constructor.*/
	DynamicLibrary(const DynamicLibrary&) { };
        /** disallow copy operator.*/
	DynamicLibrary& operator = (const DynamicLibrary&) { return *this; };

        /** Disallow public construction so that users have to go
          * through loadLibrary() above which returns NULL on
          * failure, a valid DynamicLibrary object on success.
          */
	DynamicLibrary(const std::string& name) {_name = name; _handle = NULL;};
        DynamicLibrary(const std::string& name,HANDLE handle);
        ~DynamicLibrary();

    protected:

        HANDLE          _handle;
        std::string     _name;
        std::string     _fullName;

};

class DynamicLibraryFactory {
    static DynamicLibrary* loadLibrary(const std::string& libraryName);
    static DynamicLibrary::HANDLE getLibraryHandle( const std::string& libraryName);
};
#endif	/* _DYNAMICLIBRARY_H */

