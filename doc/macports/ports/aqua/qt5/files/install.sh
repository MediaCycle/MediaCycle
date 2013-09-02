#!/bin/bash
#
# Install the compiled Qt framework from the dmg installer, for OSX and MacPorts.
#
# Arguments:
# $1 destroot
# $2 prefix
#
# by Christian Frisson

# Adjust the frameworks ids
for i in $1$2/Library/Frameworks/Qt*framework/Versions/*/Qt*; 
	do echo $i; 
	install_name_tool -id $2/${i#$1$2} $i;
done

# Adjust the frameworks dependencies paths
for i in $1$2/Library/Frameworks/Qt*framework/Versions/*/Qt*; 
	do echo $i; 
	list=`otool -L $i | grep -v $2 | grep Qt | grep framework | cut -d' ' -f1` ;
	for l in $list; 
		do install_name_tool -change $l $2/Library/Frameworks/Qt${l#*lib/Qt} $i;
	done;
done

# Adjust the dynamic libraries ids
for i in $(find $1$2 -name "*.dylib"); 
	do echo $i; install_name_tool -id $2${i#$1$2} $i;
done

# Adjust the dynamic libraries dependencies paths
for i in $(find $1$2 -name "*.dylib") $(find $1$2/bin -type f ! -name "*.*"); 
	do echo $i; 
	list=`otool -L $i | grep -v $2 | grep Qt | grep framework | cut -d' ' -f1` ;
	for l in $list; 
		do install_name_tool -change $l $2/Library/Frameworks/Qt${l#*lib/Qt} $i;
	done;
done

