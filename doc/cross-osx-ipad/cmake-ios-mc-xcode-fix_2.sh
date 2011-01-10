#!/bin/bash
sed -e 's/\/Developer\/Platforms\/iPhoneSimulator.platform\/Developer\/SDKs\/iPhoneSimulator4.2.sdk/iphoneos/g' -e 's/\/opt\/ios\/simulator\/4.2\/lib\/\$(CONFIGURATION)/\/opt\/ios\/simulator\/4.2\/lib/g' -e 's/\/Developer\/Platforms\/iPhoneOS.platform\/Developer\/SDKs\/iPhoneOS4.2.sdk/iphoneos/g' -e 's/\/opt\/ios\/device\/4.2\/lib\/\$(CONFIGURATION)/\/opt\/ios\/device\/4.2\/lib/g' MediaCycle.xcodeproj/project.pbxproj > tmp.pbxproj
mv tmp.pbxproj MediaCycle.xcodeproj/project.pbxproj
