#!/bin/bash
sed -e 's/Debug\//$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)\//g' -e 's/Release\//$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)\//g' -e 's/RelWithDebInfo\//Release$(EFFECTIVE_PLATFORM_NAME)\//g' -e 's/MinSizeRel\//Release$(EFFECTIVE_PLATFORM_NAME)\//g' MediaCycle.xcodeproj/project.pbxproj > tmp.pbxproj
mv tmp.pbxproj MediaCycle.xcodeproj/project.pbxproj
