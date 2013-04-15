# This file is part of MXE.
# See index.html for further information.

# clucene
PKG             := clucene
$(PKG)_IGNORE   :=
$(PKG)_CHECKSUM := 76d6788e747e78abb5abf8eaad78d3342da5f2a4
$(PKG)_SUBDIR   := clucene-core-$($(PKG)_VERSION)
$(PKG)_FILE     := clucene-core-$($(PKG)_VERSION).tar.gz
$(PKG)_URL      := http://$(SOURCEFORGE_MIRROR)/project/clucene/clucene-core-unstable/2.3/$($(PKG)_FILE)
$(PKG)_DEPS     := gcc boost zlib

define $(PKG)_UPDATE
    wget -q -O- 'http://sourceforge.net/projects/clucene/files/' | \
    $(SED) -n 's,.*/\([0-9][^"]*\)/".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && \
    cmake . -DCMAKE_TOOLCHAIN_FILE='$(CMAKE_TOOLCHAIN_FILE)' \
    -D_CL_HAVE_TRY_BLOCKS_EXITCODE=TRUE \
    -D_CL_HAVE_NAMESPACES_EXITCODE=1 \
    -D_CL_HAVE_NO_SNWPRINTF_BUG_EXITCODE=1 \
    -DLUCENE_STATIC_CONSTANT_SYNTAX_EXITCODE=1 \
    -D_CL_HAVE_NO_SNPRINTF_BUG=1 \
    -D_CL_HAVE_GCC_ATOMIC_FUNCTIONS=0 \
    -DBUILD_STATIC_LIBRARIES=ON \
    -DDISABLE_MULTITHREADING=ON
    $(MAKE) -C '$(1)' -j '$(JOBS)' install VERBOSE=1
endef


