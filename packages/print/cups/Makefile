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

define Package/cups/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Printing
  URL:=https://openprinting.org/cups/
endef

define Package/cups-mini
$(call Package/cups/Default)
  TITLE:=CUPS - Print Server (mini)
  DEPENDS:=+zlib +libjpeg-turbo +libpng +libopenssl +libusb-1.0 +libstdcpp
endef

define Package/cups-mini/description
 CUPS 打印服务器核心（mini），含 cupsd、库、backend、filter、cgi-bin。
 支持 Web 管理（http://IP:631）。适合资源有限的设备。
endef

define Package/cups-bsd
$(call Package/cups/Default)
  TITLE:=CUPS - mini + BSD Commands
  DEPENDS:=+zlib +libjpeg-turbo +libpng +libopenssl +libusb-1.0 +libstdcpp
endef

define Package/cups-bsd/description
 含 mini 全部内容 + BSD 风格命令（lpr、lpq、lprm、lpc）。可独立安装。
endef

define Package/cups-client
$(call Package/cups/Default)
  TITLE:=CUPS - mini + System V Client Commands
  DEPENDS:=+zlib +libjpeg-turbo +libpng +libopenssl +libusb-1.0 +libstdcpp
endef

define Package/cups-client/description
 含 mini 全部内容 + System V 风格客户端（lp、cancel、lpadmin 等）。可独立安装。
endef

define Package/cups-full
$(call Package/cups/Default)
  TITLE:=CUPS - Full (mini + bsd + client)
  DEPENDS:=+zlib +libjpeg-turbo +libpng +libopenssl +libusb-1.0 +libstdcpp
endef

define Package/cups-full/description
 全功能包：自包含 mini + bsd + client 全部内容，可独立安装，无交叉依赖。
endef

CONFIGURE_ARGS += \
    --libdir=/usr/lib \
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

# 在 prepare 阶段复制 zh_CN Web 模板，确保 compile 时（含 make install）源码树中已存在
# 若在 CI 中复制，compile 可能重跑 prepare 并覆盖；放在 Build/Prepare 中可保证每次 prepare 后都有 zh_CN
define Build/Prepare
	$(call Build/Prepare/Default)
	@if [ -d "$(CURDIR)/../../templates/zh_CN" ]; then \
		mkdir -p $(PKG_BUILD_DIR)/templates/zh_CN && \
		cp $(CURDIR)/../../templates/zh_CN/*.tmpl $(PKG_BUILD_DIR)/templates/zh_CN/ 2>/dev/null || true; \
		echo "Copied zh_CN templates: $$(ls $(PKG_BUILD_DIR)/templates/zh_CN/*.tmpl 2>/dev/null | wc -l) files"; \
	fi
endef
TARGET_LDFLAGS += -Wl,--as-needed -Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
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

define Package/cups-mini/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{cupsctl,cupsd,cupsfilter} $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/etc/cups
	$(CP) $(PKG_INSTALL_DIR)/etc/cups/* $(1)/etc/cups/
	# Listen on all interfaces, allow LAN access for web UI
	sed -i 's/Listen localhost:631/Listen *:631/' $(1)/etc/cups/cupsd.conf
	# Remove IdleExitTimeout if unsupported by this build (avoids "Unknown directive" error)
	sed -i '/^IdleExitTimeout/d' $(1)/etc/cups/cupsd.conf
	# Add Allow from private IP ranges (Location uses Order allow,deny; Policy uses Order deny,allow)
	sed -i '/  Order allow,deny/a\  Allow from 192.168.0.0\/16' $(1)/etc/cups/cupsd.conf
	sed -i '/  Order allow,deny/a\  Allow from 172.16.0.0\/12' $(1)/etc/cups/cupsd.conf
	sed -i '/  Order allow,deny/a\  Allow from 10.0.0.0\/8' $(1)/etc/cups/cupsd.conf

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcups.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcupsimage.so* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cups/* $(1)/usr/lib/cups/

	$(INSTALL_DIR) $(1)/usr/share/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cups/* $(1)/usr/share/cups/

	$(INSTALL_DIR) $(1)/usr/share/locale
	-if [ -d $(PKG_INSTALL_DIR)/usr/share/locale ]; then $(CP) -r $(PKG_INSTALL_DIR)/usr/share/locale/* $(1)/usr/share/locale/; fi

	for d in cache certs logs spool; do \
		$(INSTALL_DIR) $(1)/var/run/cups/$$d; \
	done

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/cupsd.init $(1)/etc/init.d/cupsd
endef

define Package/cups-mini/conffiles
/etc/cups/cups-files.conf
/etc/cups/cupsd.conf
endef

define Package/cups-bsd/install
	$(call Package/cups-mini/install,$(1))

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{lpr,lpq,lprm} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/lpc $(1)/usr/sbin/
endef

define Package/cups-bsd/conffiles
/etc/cups/cups-files.conf
/etc/cups/cupsd.conf
endef
define Package/cups-client/install
	$(call Package/cups-mini/install,$(1))

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{cancel,cupstestppd,lp,lpoptions,lpstat} $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{cupsaccept,cupsfilter,lpadmin,lpinfo,lpmove} $(1)/usr/sbin/
	(cd $(1)/usr/sbin && ln -sf cupsaccept cupsenable && ln -sf cupsaccept cupsdisable && ln -sf cupsaccept cupsreject)
endef

define Package/cups-client/conffiles
/etc/cups/cups-files.conf
/etc/cups/cupsd.conf
endef
define Package/cups-full/install
	$(call Package/cups-mini/install,$(1))
	# + bsd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{lpr,lpq,lprm} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/lpc $(1)/usr/sbin/
	# + client
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{cancel,cupstestppd,lp,lpoptions,lpstat} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{cupsaccept,cupsfilter,lpadmin,lpinfo,lpmove} $(1)/usr/sbin/
	(cd $(1)/usr/sbin && ln -sf cupsaccept cupsenable && ln -sf cupsaccept cupsdisable && ln -sf cupsaccept cupsreject)
endef

define Package/cups-full/conffiles
/etc/cups/cups-files.conf
/etc/cups/cupsd.conf
endef
$(eval $(call BuildPackage,cups-mini))
$(eval $(call BuildPackage,cups-bsd))
$(eval $(call BuildPackage,cups-client))
$(eval $(call BuildPackage,cups-full))

