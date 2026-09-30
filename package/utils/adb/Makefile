include $(TOPDIR)/rules.mk
#Based on adb package from AUR https://aur.archlinux.org/packages/adb/ , reused Makefile

PKG_NAME:=adb
PKG_SOURCE_VERSION:=6fe92d1a3fb17545d82d020a3c995f32e6b71f9d
PKG_VERSION:=5.0.2~$(call version_abbrev,$(PKG_SOURCE_VERSION))
PKG_RELEASE:=6
PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_adb CONFIG_PACKAGE_adbd

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://android.googlesource.com/platform/system/core
PKG_MIRROR_HASH:=2ff96b4342cd05f475083207a4927635548c6693771c12a24cfa99f175fdb10a

PKG_MAINTAINER:=Henryk Heisig <hyniu@o2.pl>
PKG_CPE_ID:=cpe:/a:google:android_debug_bridge

include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_BIG_ENDIAN),y)
TARGET_CFLAGS+= -DHAVE_BIG_ENDIAN=1
endif
TARGET_CFLAGS+= -D_GNU_SOURCE

define Package/adb
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Android Debug Bridge CLI tool
  URL:=http://tools.android.com/
  DEPENDS:=+zlib +libmbedtls +libpthread
endef

define Package/adb/description
 Android Debug Bridge (adb) is a versatile command line tool that lets you communicate with an emulator instance or connected Android-powered device.
endef

define Package/adbd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Android Debug Bridge USB daemon for Galileo
  URL:=https://android.googlesource.com/platform/system/core
  DEPENDS:=@TARGET_x86_galileo +libmbedtls +libpthread
endef

define Package/adbd/description
 USB shell and file transfer service for Intel Galileo. Runs as root over
 FunctionFS; TCP transport is disabled. Optional host-key authentication
 uses /etc/adbd/adb_keys.
endef

define Package/adbd/conffiles
/etc/config/adbd
/etc/adbd/adb_keys
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/* $(PKG_BUILD_DIR)/adb/
endef

define Build/Compile/adb
	$(MAKE) -C $(PKG_BUILD_DIR)/adb/ \
		$(TARGET_CONFIGURE_OPTS) \
		TARGET=Linux \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Build/Compile/adbd
	$(MAKE) -C $(PKG_BUILD_DIR)/adb/ -f Makefile.adbd \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Build/Compile
	$(if $(CONFIG_PACKAGE_adb),$(call Build/Compile/adb))
	$(if $(CONFIG_PACKAGE_adbd),$(call Build/Compile/adbd))
endef

define Package/adb/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adb/adb $(1)/usr/bin/
endef

define Package/adbd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/usr/libexec $(1)/etc/init.d \
		$(1)/etc/config $(1)/etc/adbd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adb/adbd $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/adbd.init $(1)/etc/init.d/adbd
	$(INSTALL_BIN) ./files/adbd-gadget $(1)/usr/libexec/
	$(INSTALL_CONF) ./files/adbd.config $(1)/etc/config/adbd
	touch $(1)/etc/adbd/adb_keys
endef

$(eval $(call BuildPackage,adb))
$(eval $(call BuildPackage,adbd))
