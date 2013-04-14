#!/bin/bash
echo "#include <osgDB/Registry>" > osgdb_$1.cpp
echo "#include <osgPlugins/$1>" >> osgdb_$1.cpp
#echo "USE_OSGPLUGIN($1)" >> osgdb_$1.cpp
echo "int main(){return 1;}" >> osgdb_$1.cpp
echo "i686-pc-mingw32-g++ -o osgdb_$1_test.exe osgdb_$1.cpp `/Volumes/data/CrossTalk/mxe-master/usr/bin/i686-pc-mingw32-pkg-config --cflags openscenegraph-osgPlugin-$1` `/Volumes/data/CrossTalk/mxe-master/usr/bin/i686-pc-mingw32-pkg-config --libs openscenegraph-osgPlugin-$1` -lglu32 -lopengl32"
/Volumes/data/CrossTalk/mxe-master/usr/bin/i686-pc-mingw32-g++ -o osgdb_$1_test.exe osgdb_$1.cpp `/Volumes/data/CrossTalk/mxe-master/usr/bin/i686-pc-mingw32-pkg-config --cflags openscenegraph-osgPlugin-$1` `/Volumes/data/CrossTalk/mxe-master/usr/bin/i686-pc-mingw32-pkg-config --libs openscenegraph-osgPlugin-$1` -lglu32 -lopengl32