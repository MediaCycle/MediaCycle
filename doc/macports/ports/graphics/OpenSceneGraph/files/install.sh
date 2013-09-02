#!/bin/bash
#
# Adjust the OSG dependencies paths for OSX and MacPorts.
#
# Arguments:
# $1 destroot
# $2 prefix
#
# by Christian Frisson

# Adjust the libs ids
for i in $1$2/lib/*.dylib; 
	do echo "install_name_tool -id $2/lib/$i $i";
	install_name_tool -id $2/lib${i#$1$2/lib} $i;
done

# Adjust the libs dependencies paths
for i in $1$2/lib/*.dylib $1$2/lib/osgPlugins*/*.so $1$2/bin/*; 
	do osg=`otool -L $i | grep libosg | cut -d' ' -f1` ;
	for l in $osg; 
		do echo "install_name_tool -change $l $2/lib/libosg${l#*libosg} $i";
		install_name_tool -change $l $2/lib/libosg${l#*libosg} $i;
	done;
	ot=`otool -L $i | grep libOpenThreads | cut -d' ' -f1` ;
	for l in $ot; 
		do echo "install_name_tool -change $l $2/lib/libOpenThreads${l#*libOpenThreads} $i";
		install_name_tool -change $l $2/lib/libOpenThreads${l#*libOpenThreads} $i;
	done;
done


