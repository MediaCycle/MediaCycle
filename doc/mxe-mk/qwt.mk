# This file is part of MXE.
# See index.html for further information.

PKG             := qwt
$(PKG)_IGNORE   :=
$(PKG)_CHECKSUM := 7ea84ee47339809c671a456b5363d941c45aea92
$(PKG)_SUBDIR   := $(PKG)-$($(PKG)_VERSION)
$(PKG)_FILE     := $(PKG)-$($(PKG)_VERSION).zip
$(PKG)_URL      := http://$(SOURCEFORGE_MIRROR)/project/$(PKG)/$(PKG)/$($(PKG)_VERSION)/$($(PKG)_FILE)
$(PKG)_DEPS     := gcc qt zlib

define $(PKG)_UPDATE
    wget -q -O- 'http://sourceforge.net/projects/$(PKG)/files/$(PKG)/' | \
    $(SED) -n 's,.*/\([0-9][^"]*\)/".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && '$(PREFIX)/bin/$(TARGET)-qmake'
    $(MAKE)  -C '$(1)' -j '$(JOBS)'
    $(INSTALL) -d '$(PREFIX)/$(TARGET)/lib'
    $(INSTALL) -m644 '$(1)/lib/libqwt.a' '$(PREFIX)/$(TARGET)/lib/'
    $(INSTALL) -d '$(PREFIX)/$(TARGET)/include'
    $(INSTALL) -d '$(PREFIX)/$(TARGET)/include/qwt'
    $(INSTALL) -m644 '$(1)/src'/*.h  '$(PREFIX)/$(TARGET)/include/qwt/'
endef

