# SPDX-License-Identifier: Apache-2.0

include $(TOPDIR)/rules.mk

PKG_NAME:=cups
PKG_VERSION:=2.4.16
PKG_RELEASE:=1

PKG_HASH:=0339587204b4f9428dd0592eb301dec0bf9ea6ea8dce5d9690d56be585aba92d

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-source.tar.gz
PKG_SOURCE_URL:=https://github.com/OpenPrinting/cups/releases/download/v$(PKG_VERSION)/
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE.txt
PKG_MAINTAINER:=OpenPrinting Community <printing-discuss@lists.linuxfoundation.org>

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=bison/host flex/host

include $(INCLUDE_DIR)/package.mk

define Package/cups
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Printing
  TITLE:=Common Unix Printing System (CUPS)
  URL:=https://openprinting.org/cups/
  DEPENDS:=+zlib +libjpeg-turbo +libpng +libopenssl +libusb-1.0
endef

define Package/cups/description
 CUPS (Common Unix Printing System) 是 OpenPrinting 主导的跨平台打印服务框架。
 它提供 IPP/IPP Everywhere 兼容的打印服务器、调度与驱动管理能力。
endef

CONFIGURE_ARGS += \
    --disable-dbus \
    --disable-launchd \
    --disable-systemd \
    --with-cups-user=lp \
    --with-cups-group=lp \
    --with-system-groups=lpadmin \
    --without-java \
    --without-rcdir \
    --with-docdir=/usr/share/cups/doc

CONFIGURE_VARS += \
    ac_cv_func_gnutls_handle_handshake_timeout=no \
    ac_cv_cups_use_networking=yes

TARGET_CFLAGS += -fPIC
TARGET_LDFLAGS += -Wl,--as-needed -Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/Configure
	$(call Build/Configure/Default, \
		--with-pdftops=none \
		--without-perl \
		--without-python \
		--without-php \
		--enable-libusb \
		--disable-acl \
		--disable-dnssd \
		--disable-avahi \
		--disable-ldap \
		--disable-pam \
		--disable-slp \
		--disable-tiff, \
		UNAME="Linux" \
		LIBS="$(TARGET_LDFLAGS) -lz -lpng -ljpeg -lssl -lcrypto -lusb-1.0" \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BUILDROOT="$(PKG_INSTALL_DIR)" \
		STRIP="/bin/true" \
		all install
endef

define Package/cups/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/cupsd $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/usr/bin
	for bin in cancel cupsaccept cupsdisable cupsenable cupsfilter \
		cupsreject lp lpadmin lpinfo lpmove lpoptions lpr lprm; do \
		[ -f $(PKG_INSTALL_DIR)/usr/bin/$$bin ] && \
			$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/$$bin $(1)/usr/bin/; \
	done

	$(INSTALL_DIR) $(1)/etc/cups
	$(CP) $(PKG_INSTALL_DIR)/etc/cups/* $(1)/etc/cups/

	$(INSTALL_DIR) $(1)/usr/lib/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cups/* $(1)/usr/lib/cups/

	$(INSTALL_DIR) $(1)/usr/share/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cups/* $(1)/usr/share/cups/

	for d in cache certs logs spool; do \
		$(INSTALL_DIR) $(1)/var/run/cups/$$d; \
	done

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/cupsd.init $(1)/etc/init.d/cupsd
endef

define Package/cups/conffiles
/etc/cups/cups-files.conf
/etc/cups/cupsd.conf
endef

$(eval $(call BuildPackage,cups))

