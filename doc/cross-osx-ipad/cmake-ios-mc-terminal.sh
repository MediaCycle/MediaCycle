#!/bin/sh
# adapted by Christian Frisson
# from http://github.com/niw/iphone_opencv_test/blob/master/opencv_cmake.sh
# info http://niw.at/articles/2009/03/14/using-opencv-on-iphone/en

if [ "$1" = "-h" -o "$1" = "--help" -o -z "$1" ]; then
	echo "Usage: $0 {Device,Simulator} {MediaCycle Source Dir} [MediaCycle Install Dir] [iOS SDK Version = 3.2]"
	exit
fi

if ! type 'cmake' > /dev/null 2>&1; then
	echo "cmake is not found, please install cmake command which is required to build MediaCycle stmh."
	exit 1;
fi

TARGET_SDK=`echo "$1"|tr '[:upper:]' '[:lower:]'`
if [ "$TARGET_SDK" = "device" ]; then
	TARGET_SDK_NAME="iPhoneOS"
	IOS_DEV_VALUE="ON"
	IOS_SIM_VALUE="OFF"
elif [ "$TARGET_SDK" = "simulator" ]; then
	TARGET_SDK_NAME="iPhoneSimulator"
	IOS_DEV_VALUE="OFF"
	IOS_SIM_VALUE="ON"
else
	echo "Please select Device or Simulator."
	exit 1
fi

if [ -z "$4" ]; then
	SDK_VERSION="3.2"
else
	SDK_VERSION="$4"
fi

IPHONEOS_VERSION_MIN="3.2"
DEVELOPER_ROOT="/Developer/Platforms/${TARGET_SDK_NAME}.platform/Developer"
SDK_ROOT="${DEVELOPER_ROOT}/SDKs/${TARGET_SDK_NAME}${SDK_VERSION}.sdk"

if [ ! -d "$SDK_ROOT" ]; then
	echo "iOS SDK Version ${SDK_VERSION} is not found, please select iOS version you have."
	exit 1
fi

if [ -z "$2" ]; then
	echo "Please assign path to MediaCycle source directory which includes CMakeLists.txt."
	exit 1
else
	MediaCycle_ROOT="$2"
fi

if [ ! -f "${MediaCycle_ROOT}/CMakeLists.txt" ]; then
	echo "No CMakeLists.txt in ${MediaCycle_ROOT}, please select MediaCycle source directory."
	exit 1
fi

if [ -z "$3" ]; then
	INSTALL_PREFIX="`pwd`/../MediaCycle_${TARGET_SDK}"
else
	INSTALL_PREFIX="$3"
fi

#BUILD_PATH="`pwd`/build_${TARGET_SDK}"
#if [ -d "${BUILD_PATH}" ]; then
#	echo "${BUILD_PATH} is found, please remove it prior to run this command."
#	exit 1
#else
#	mkdir -p "${BUILD_PATH}"
#fi
#cd "${BUILD_PATH}"

echo "Starting cmake..."
echo "Target SDK            = $TARGET_SDK_NAME"
echo "iOS SDK Version       = $SDK_VERSION"
echo "iOS Deployment Target = $IPHONEOS_VERSION_MIN"
echo "MediaCycle Root           = $MediaCycle_ROOT"
echo "MediaCycle Install Prefix = $INSTALL_PREFIX"
echo ""

if [ "$TARGET_SDK" = "device" ]; then
	FLAGS="-miphoneos-version-min=${IPHONEOS_VERSION_MIN}"
	ARCH="armv6"
	#CMAKE_OPTIONS="-D ENABLE_SSE=OFF -D ENABLE_SSE2=OFF"
else
	FLAGS="-mno-thumb -arch i386 -pipe -no-cpp-precomp -fobjc-abi-version=2 -fobjc-legacy-dispatch"
	ARCH="i386"
	#CMAKE_OPTIONS='-D CMAKE_OSX_DEPLOYMENT_TARGET="10.6"'
fi

env \
	CFLAGS="${FLAGS}" \
	CXXFLAGS="${FLAGS}" \
	LDFLAGS="${FLAGS}" \
cmake \
	-D CMAKE_BUILD_TYPE=Release \
	-D USE_QT4=OFF \
	-D USE_COCOA=OFF \
	-D USE_OPENAL=ON \
	-D USE_OSC=OFF \
	-D USE_AUDIOFEAT=OFF \
	-D USE_LAUGHFEAT=OFF \
	-D USE_PSYCLONE=OFF \
	-D BUILD_APPS=ON \
	-D BUILD_TESTS=OFF \
	-D BUILD_PLUGINS=OFF \
	-D USE_AUDIOGARDEN=OFF \
	-D USE_SDIF=OFF \
	-D IOS_DEV="${IOS_DEV_VALUE}" \
	-D IOS_SIM="${IOS_SIM_VALUE}" \
	-D CMAKE_OSX_SYSROOT="${SDK_ROOT}" \
	-D CMAKE_OSX_ARCHITECTURES="${ARCH}" \
	-D CMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
	-D CMAKE_FIND_ROOT_PATH="${INSTALL_PREFIX}" \
	-D CMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
	-D CMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
	-D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
	-D CMAKE_VERBOSE_MAKEFILE=TRUE \
	-D CMAKE_C_COMPILER="${DEVELOPER_ROOT}/usr/bin/gcc-4.2" \
	-D CMAKE_CXX_COMPILER="${DEVELOPER_ROOT}/usr/bin/g++-4.2" \
	${CMAKE_OPTIONS} \
	"${MediaCycle_ROOT}" \
	&& echo "" \
	&& echo "Done! next step is running make (with -j option if you want to build using multi cores)."
