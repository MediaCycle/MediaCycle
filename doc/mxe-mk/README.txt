These are additional makefiles and patches for the MXE (M cross environment) project (http://mxe.cc) that allows to cross-compile dependencies for win32 from unix hosts (OSX and ubuntu for instance).

Howto:
* Install the requirements of MXE for your platform (see http://mxe.cc/#requirements)
* Download and extract MXE (using: git clone -b stable https://github.com/mxe/mxe.git, see http://mxe.cc/#download for updates)
* Edit index.html, look for List of Packages, and add alphabetically each of the following package definitions if more up to date versions aren't already listed:
    <tr>
        <td id="armadillo-package">armadillo</td>
        <td id="armadillo-version">3.2.4</td>
        <td id="armadillo-website"><a href="http://armadillo.sf.net/">Armadillo C++ linear algebra library</a></td>
    </tr>

    <tr>
        <td id="argtable-package">argtable</td>
        <td id="argtable-version">2.13</td>
        <td id="argtable-website"><a href="http://argtable.sf.net/">ANSI C command line parser</a></td>
    </tr>

    <tr>
        <td id="boost-package">boost</td>
        <td id="boost-version">1.50.0</td>
        <td id="boost-website"><a href="http://www.boost.org/">Boost C++ Library</a></td>
    </tr>

    <tr>
        <td id="dlfcn-win32-package">dlfcn-win32</td>
        <td id="dlfcn-win32-version">r19</td>
        <td id="dlfcn-win32-website"><a href="http://code.google.com/p/dlfcn-win32/">dlfcn library for Windows</a></td>
    </tr>

    <tr>
        <td id="gcc-package">gcc</td>
        <td id="gcc-version">4.7.1</td>
        <td id="gcc-website"><a href="http://gcc.gnu.org/">GCC</a></td>
    </tr>

    <tr>
        <td id="lapack-package">lapack</td>
        <td id="lapack-version">3.4.1</td>
        <td id="lapack-website"><a href="http://www.netlib.org/lapack/">lapack</a></td>
    </tr>

    <tr>
        <td id="mpg123-package">mpg123</td>
        <td id="mpg123-version">1.14.4</td>
        <td id="mpg123-website"><a href="http://www.mpg123.org/">mpg123</a></td>
    </tr>

    <tr>
        <td id="opencv-package">opencv</td>
        <td id="opencv-version">2.4.2</td>
        <td id="opencv-website"><a href="http://opencv.willowgarage.com">OpenCV</a></td>
    </tr>

    <tr>
        <td id="qwt-package">qwt</td>
        <td id="qwt-version">6.0.1</td>
        <td id="qwt-website"><a href="http://qwt.sourceforge.net/">Qwt</a></td>
    </tr>
* For each package added into index.html, copy the associated files contained in the current folder into the src folder of MXE.
* Open a terminal in the MXE root folder (containing doc, src, tools…), install the dependencies with the command:
make gcc ...

TODO:
* add clucene

Changelog:
* (27/07/2012, mc changeset 838): debugged opencv 2.4.2
* (27/07/2012, mc changeset 838): added mpg123 1.14.4 and argtable 2.13 and dlfcn r19
* (26/07/2012, mc changeset 838): updated and debugged lapack 3.4.1 and armadillo 3.2.4!
* (25/07/2012, mc changeset 838): added opencv 2.4.2
* (23/07/2012, mc changeset 838): updated boost to 1.50.0 and armadillo to 3.2.4, both should survive gcc 4.7.x
* (16/05/2012, mc changeset 838): added qwt 6.0.1, armadillo to 3.2.2 and gcc to 4.7.1 (armadillo fails with 4.6.x, keywords: Mat_bones.hpp copy_binfo) - mingw-cross-env was renamed MXE and no more provides releases but updates thru git
* (22/10/2011, mc changeset 662): added OpenCV 2.3.1a and armadillo 2.2.3 makefiles/patches for mingw-cross-env 2011-06-07 – Release 2.21 updated with mingw-cross-env mercurial changeset 2000 (notably bringing ffmpeg 0.8.5)