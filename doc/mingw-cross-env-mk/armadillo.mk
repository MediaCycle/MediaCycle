# This file is part of mingw-cross-env.
# See doc/index.html for further information.

# armadillo
PKG             := armadillo
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 2.2.3
$(PKG)_CHECKSUM := 32b25242be6c3c42a801dd64270a06c94ae56648
$(PKG)_SUBDIR   := armadillo-$($(PKG)_VERSION)
$(PKG)_FILE     := armadillo-$($(PKG)_VERSION).tar.gz
$(PKG)_WEBSITE  := http://www.armadillo.sf.net
$(PKG)_URL      := http://$(SOURCEFORGE_MIRROR)/project/arma/$($(PKG)_FILE)
$(PKG)_DEPS     := boost cblas lapack

define $(PKG)_UPDATE
    wget -q -O- 'http://sourceforge.net/projects/arma/files/arma/' | \
    $(SED) -n 's,.*/\([0-9][^"]*\)/".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && cmake . \
        -DCMAKE_TOOLCHAIN_FILE='$(CMAKE_TOOLCHAIN_FILE)' \
        -DCMAKE_CXX_FLAGS=-D__STDC_CONSTANT_MACROS \
        -DCMAKE_HAVE_PTHREAD_H=OFF \
        -DDYNAMIC_OPENTHREADS=OFF \
        -DDYNAMIC_armadillo=OFF \
        -DBUILD_OSG_APPLICATIONS=OFF \
        -D_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS_EXITCODE=1
    $(MAKE) -C '$(1)' -j '$(JOBS)' install VERBOSE=1
endef
