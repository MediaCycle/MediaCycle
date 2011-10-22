# This file is part of mingw-cross-env.
# See doc/index.html for further information.

# OpenCV
PKG             := opencv
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 2.3.1
$(PKG)_REVISION  := 2.3.1a
$(PKG)_CHECKSUM := 9784e6824c5f677fac82ae83a2f366743adb3299
$(PKG)_SUBDIR   := OpenCV-$($(PKG)_VERSION)
$(PKG)_FILE     := OpenCV-$($(PKG)_REVISION).tar.bz2
$(PKG)_WEBSITE  := http://opencv.willowgarage.com/wiki/
$(PKG)_URL      := http://$(SOURCEFORGE_MIRROR)/project/opencvlibrary/opencv-unix/$($(PKG)_VERSION)/$($(PKG)_FILE)
$(PKG)_DEPS     := ffmpeg
#bzip2 faad2 ffmpeg lame liboil ogg theora vorbis x264 zlib

define $(PKG)_UPDATE
    wget -q -O- 'http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/' | \
    $(SED) -n 's,.*/\([0-9][^"]*\)/".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && cmake . \
        -DCMAKE_TOOLCHAIN_FILE='$(CMAKE_TOOLCHAIN_FILE)' \
        -DCMAKE_CXX_FLAGS=-D__STDC_CONSTANT_MACROS \
        -DCMAKE_HAVE_PTHREAD_H=OFF \
        -DBUILD_SHARED_LIBS=OFF \
        -DBUILD_EXAMPLES=ON \
        -DINSTALL_C_EXAMPLES=ON \
        -DWITH_OPENEXR=OFF \
        -DWITH_FFMPEG=ON
    $(MAKE) -C '$(1)' -j '$(JOBS)' install VERBOSE=1
endef
