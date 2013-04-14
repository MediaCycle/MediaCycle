# This file is part of MXE.
# See index.html for further information.

# armadillo
PKG             := podofo
$(PKG)_IGNORE   :=
$(PKG)_CHECKSUM := 8a6e27e17e0ed9f12e1a999cff66eae8eb97a4bc
$(PKG)_SUBDIR   := $(PKG)-$($(PKG)_VERSION)
$(PKG)_FILE     := $(PKG)-$($(PKG)_VERSION).tar.gz
$(PKG)_URL      := http://sourceforge.net/projects/podofo/files/podofo/0.9/$($(PKG)_FILE)
$(PKG)_DEPS     := gcc cppunit freetype jpeg libpng openssl tiff

define $(PKG)_UPDATE
    $(WGET) -q -O- 'http://podofo.sourceforge.net/download.html' | \
    $(SED) -n 's,.*/podofo-\([0-9.]*\)[.]tar.*".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && cmake . -DCMAKE_TOOLCHAIN_FILE='$(CMAKE_TOOLCHAIN_FILE)'
    $(MAKE) -C '$(1)' -j '$(JOBS)' install VERBOSE=1
endef
