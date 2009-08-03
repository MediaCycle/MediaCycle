/*
 * This part of code has been copied from OSC source code
 */
#include "DynamicLibrary.h"

using namespace std;

//The dlopen calls were not adding to OS X until 10.3
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#if !defined(MAC_OS_X_VERSION_10_3) || (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3)
#define APPLE_PRE_10_3
#endif
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#include <windows.h>
#include <winbase.h>
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
#include <mach-o/dyld.h>
#else // all other unix
#include <unistd.h>
#ifdef __hpux__
// Although HP-UX has dlopen() it is broken! We therefore need to stick
// to shl_load()/shl_unload()/shl_findsym()
#include <dl.h>
#include <errno.h>
#else
#include <dlfcn.h>
#endif
#endif

DynamicLibrary::DynamicLibrary(const std::string& name, HANDLE handle)  //constructor
{
    _name = name;
    _handle = handle;
    cout<<"Opened DynamicLibrary "<<_name<<std::endl;
}

DynamicLibrary::~DynamicLibrary()
{
    if (_handle)
    {
        cout<<"Closing DynamicLibrary "<<_name<<std::endl;
#if defined(WIN32) && !defined(__CYGWIN__)
        FreeLibrary((HMODULE)_handle);
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
        NSUnLinkModule(static_cast<NSModule>(_handle), FALSE);
#elif defined(__hpux__)
        // fortunately, shl_t is a pointer
        shl_unload (static_cast<shl_t>(_handle));
#else // other unix
        dlclose(_handle);
#endif
    }
}

DynamicLibrary* DynamicLibrary::loadLibrary(const std::string& libraryName) //!= Loadlibrary (see getLibraryHandle() )
{
    HANDLE handle = NULL;
    /*
    std::string fullLibraryName = osgDB::findLibraryFile(libraryName);
    if (!fullLibraryName.empty()) handle = getLibraryHandle( fullLibraryName );    // try the lib we have found
    else handle = getLibraryHandle( libraryName );    // havn't found a lib ourselves, see if the OS can find it simply from the library name.
    */
    handle = getLibraryHandle( libraryName );
    if (handle) return new DynamicLibrary(libraryName,handle);
    // else no lib found so report errors.
    else { cout << "DynamicLibrary::failed loading \""<<libraryName<<"\""<<std::endl; }

    return NULL;
}

DynamicLibrary::HANDLE DynamicLibrary::getLibraryHandle( const std::string& libraryName)
{
    HANDLE handle = NULL;

#if defined(WIN32) && !defined(__CYGWIN__)
    handle = LoadLibrary( libraryName.c_str() );
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
    NSObjectFileImage image;
    // NSModule os_handle = NULL;
    if (NSCreateObjectFileImageFromFile(libraryName.c_str(), &image) == NSObjectFileImageSuccess) {
        // os_handle = NSLinkModule(image, libraryName.c_str(), TRUE);
        handle = NSLinkModule(image, libraryName.c_str(), TRUE);
        NSDestroyObjectFileImage(image);
    }
#elif defined(__hpux__)
    // BIND_FIRST is neccessary for some reason
    handle = shl_load ( libraryName.c_str(), BIND_DEFERRED|BIND_FIRST|BIND_VERBOSE, 0);
    return handle;
#else // other unix
    // dlopen will not work with files in the current directory unless
    // they are prefaced with './'  (DB - Nov 5, 2003).
    std::string localLibraryName;
    /*if( libraryName == osgDB::getSimpleFileName( libraryName ) )
        localLibraryName = "./" + libraryName;
    else
        localLibraryName = libraryName;*/

    localLibraryName = libraryName;

    handle = dlopen( localLibraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if( handle == NULL )
    {
        if (fileExists(localLibraryName))
        {
            cout << "Warning: dynamic library '" << libraryName << "' exists, but an error occurred while trying to open it:" << std::endl;
            cout << dlerror() << std::endl;
        }
        else
        {
            cout << "Warning: dynamic library '" << libraryName << "' does not exist (or isn't readable):" << std::endl;
            cout << dlerror() << std::endl;
        }
    }
#endif
    return handle;
}

DynamicLibrary::PROC_ADDRESS DynamicLibrary::getProcAddress(const std::string& procName)
{
    if (_handle==NULL) return NULL;
#if defined(WIN32) && !defined(__CYGWIN__)
    return (DynamicLibrary::PROC_ADDRESS)GetProcAddress( (HMODULE)_handle,
                                                         procName.c_str() );
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
    std::string temp("_");
    NSSymbol symbol;
    temp += procName;   // Mac OS X prepends an underscore on function names
    symbol = NSLookupSymbolInModule(static_cast<NSModule>(_handle), temp.c_str());
    return NSAddressOfSymbol(symbol);
#elif defined(__hpux__)
    void* result = NULL;
    if (shl_findsym (reinterpret_cast<shl_t*>(&_handle), procName.c_str(), TYPE_PROCEDURE, result) == 0)
    {
        return result;
    }
    else
    {
        cout << "DynamicLibrary::failed looking up " << procName << std::endl;
        cout << "DynamicLibrary::error " << strerror(errno) << std::endl;
        return NULL;
    }
#else // other unix
    void* sym = dlsym( _handle,  procName.c_str() );
    if (!sym) {
        cout << "DynamicLibrary::failed looking up " << procName << std::endl;
        cout << "DynamicLibrary::error " << dlerror() << std::endl;
    }
    return sym;
#endif
}

bool fileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }

  return(blnReturn);
}