These are additional makefiles and patches for the MXE (M cross environment) project (http://mxe.cc) that allows to cross-compile dependencies for win32 from unix hosts (OSX and ubuntu for instance).

Howto:

* Install the requirements of MXE for your platform (see http://mxe.cc/#requirements)

* Download and extract MXE (using: git clone -b master https://github.com/mxe/mxe.git, see http://mxe.cc/#download for updates)

* Edit index.html, look for List of Packages, and add alphabetically each of the following package definitions if more up to date versions aren't already listed:

    <tr>
        <td id="argtable-package">argtable</td>
        <td id="argtable-version">2.13</td>
        <td id="argtable-website"><a href="http://argtable.sf.net/">ANSI C command line parser</a></td>
    </tr>

    <tr>
        <td id="dlfcn-win32-package">dlfcn-win32</td>
        <td id="dlfcn-win32-version">r19</td>
        <td id="dlfcn-win32-website"><a href="http://code.google.com/p/dlfcn-win32/">dlfcn library for Windows</a></td>
    </tr>

    <tr>
        <td id="glib-package">glib</td>
        <td id="glib-version">2.34.3</td>
        <td id="glib-website"><a href="http://www.gtk.org/">GLib</a></td>
    </tr>

    <tr>
        <td id="mpg123-package">mpg123</td>
        <td id="mpg123-version">1.14.4</td>
        <td id="mpg123-website"><a href="http://www.mpg123.org/">mpg123</a></td>
    </tr>

    <tr>
        <td id="openal-package">openal</td>
        <td id="openal-version">1.15.1</td>
        <td id="openal-website"><a href="http://kcat.strangesoft.net/openal.html">openal</a></td>
    </tr>

    <tr>
        <td id="opencv-package">opencv</td>
        <td id="opencv-version">2.4.4</td>
        <td id="opencv-website"><a href="http://opencv.org/">OpenCV</a></td>
    </tr>

    <tr>
        <td id="openscenegraph-package">openscenegraph</td>
        <td id="openscenegraph-version">800b103</td>
        <td id="openscenegraph-website"><a href="http://www.openscenegraph.org/">OpenSceneGraph</a></td>
    </tr>

    <tr>
        <td id="podofo-package">podofo</td>
        <td id="podofo-version">0.9.2</td>
        <td id="podofo-website"><a href="http://podofo.sourceforge.net/">PoDoFo</a></td>
    </tr>

* For each package added into index.html, copy the associated files contained in the current folder into the src folder of MXE.

* Open a terminal in the MXE root folder (containing doc, src, tools...), install the dependencies with the command:
make gcc argtable armadillo boost curl dlfcn-win32 ffmpeg liblo libsndfile mpg123 openal opencv openscenegraph podofo pthreads qt qwt tinyxml JOBS=4 -j 4

* Open a terminal in a MediaCycle build directory:
cmake MC_SRC -DCMAKE_TOOLCHAIN_FILE=MXE_ROOT/usr/i686-pc-mingw32/share/cmake/mxe-conf.cmake
(adapt the MediaCycle source directory MC_SRC and the mxd root directory MXE_ROOT accordingly)

TODO:
* add clucene

Changelog:
* (14/04/2013, mc changeset 1128): added podofo 0.9.2, fixed openscenegraph with pkg config files for plugins
* (11/04/2013, mc changeset 1127): removed makefiles from armadillo, boost, gcc, lapack, qwt (part of mxe-master in sufficient versions); fixed opencv (now part of mxe-master), glib, openal
* (09/09/2012, mc changeset 907): updated armadillo to 3.4.0, submitted to MacPorts <https://trac.macports.org/ticket/36062> (already accepted, seems it's gonna be available from the next MacPorts version, current is 2.1.2) and to MXE <https://github.com/mxe/mxe/pull/77>
* (27/07/2012, mc changeset 838): debugged opencv 2.4.2
* (27/07/2012, mc changeset 838): added mpg123 1.14.4 and argtable 2.13 and dlfcn r19
* (26/07/2012, mc changeset 838): updated and debugged lapack 3.4.1 and armadillo 3.2.4!
* (25/07/2012, mc changeset 838): added opencv 2.4.2
* (23/07/2012, mc changeset 838): updated boost to 1.50.0 and armadillo to 3.2.4, both should survive gcc 4.7.x
* (16/05/2012, mc changeset 838): added qwt 6.0.1, armadillo to 3.2.2 and gcc to 4.7.1 (armadillo fails with 4.6.x, keywords: Mat_bones.hpp copy_binfo) - mingw-cross-env was renamed MXE and no more provides releases but updates thru git
* (22/10/2011, mc changeset 662): added OpenCV 2.3.1a and armadillo 2.2.3 makefiles/patches for mingw-cross-env 2011-06-07 Ð Release 2.21 updated with mingw-cross-env mercurial changeset 2000 (notably bringing ffmpeg 0.8.5)