#!/bin/sh
# adapted by Christian Frisson
# from http://github.com/niw/iphone_opencv_test/blob/master/opencv_cmake.sh
# info http://niw.at/articles/2009/03/14/using-opencv-on-iphone/en
# tested with Armadillo v. 0.9.80

if [ "$1" = "-h" -o "$1" = "--help" -o -z "$1" ]; then
	echo "Usage: $0 {Device,Simulator} {Armadillo Source Dir} [Armadillo Install Dir] [iOS SDK Version = 3.2]"
	exit
fi

if ! type 'cmake' > /dev/null 2>&1; then
	echo "cmake is not found, please install cmake command which is required to build Armadillo."
	exit 1;
fi

TARGET_SDK=`echo "$1"|tr '[:upper:]' '[:lower:]'`
if [ "$TARGET_SDK" = "device" ]; then
	TARGET_SDK_NAME="iPhoneOS"
elif [ "$TARGET_SDK" = "simulator" ]; then
	TARGET_SDK_NAME="iPhoneSimulator"
else
	echo "Please select Device or Simulator."
	exit 1
fi

if [ -z "$4" ]; then
	SDK_VERSION="3.2"
else
	SDK_VERSION="$4"
fi

IPHONEOS_VERSION_MIN="3.0"
DEVELOPER_ROOT="/Developer/Platforms/${TARGET_SDK_NAME}.platform/Developer"
SDK_ROOT="${DEVELOPER_ROOT}/SDKs/${TARGET_SDK_NAME}${SDK_VERSION}.sdk"

if [ ! -d "$SDK_ROOT" ]; then
	echo "iOS SDK Version ${SDK_VERSION} is not found, please select iOS version you have."
	exit 1
fi

if [ -z "$2" ]; then
	echo "Please assign path to Armadillo source directory which includes CMakeLists.txt."
	exit 1
else
	Armadillo_ROOT="$2"
fi

if [ ! -f "${Armadillo_ROOT}/CMakeLists.txt" ]; then
	echo "No CMakeLists.txt in ${Armadillo_ROOT}, please select Armadillo source directory."
	exit 1
fi

if [ -z "$3" ]; then
	INSTALL_PREFIX="`pwd`/../Armadillo_${TARGET_SDK}"
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
echo "Armadillo Root           = $Armadillo_ROOT"
echo "Armadillo Install Prefix = $INSTALL_PREFIX"
echo ""

if [ "$TARGET_SDK" = "device" ]; then
	FLAGS="-miphoneos-version-min=${IPHONEOS_VERSION_MIN}"
	ARCH="armv6;armv7"
	CMAKE_OPTIONS="-D ENABLE_SSE=OFF -D ENABLE_SSE2=OFF"
else
	FLAGS=""
	ARCH="i386"
	CMAKE_OPTIONS='-D CMAKE_OSX_DEPLOYMENT_TARGET="10.6"'
fi

env \
	CFLAGS="${FLAGS}" \
	CXXFLAGS="${FLAGS}" \
	LDFLAGS="${FLAGS}" \
cmake \
	-D CMAKE_BUILD_TYPE=Release \
	-D CMAKE_OSX_SYSROOT="${SDK_ROOT}" \
	-D CMAKE_OSX_ARCHITECTURES="${ARCH}" \
	-D CMAKE_C_COMPILER="${DEVELOPER_ROOT}/usr/bin/gcc" \
	-D CMAKE_CXX_COMPILER="${DEVELOPER_ROOT}/usr/bin/g++" \
	-D CMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
	-D CMAKE_FIND_ROOT_PATH="${INSTALL_PREFIX}" \
	-D CMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
	-D CMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
	-D CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
	${CMAKE_OPTIONS} \
	"${Armadillo_ROOT}" \
	&& echo "" \
	&& echo "Done! next step is running make (with -j option if you want to build using multi cores)."
