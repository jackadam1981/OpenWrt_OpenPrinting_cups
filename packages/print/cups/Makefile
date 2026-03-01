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

# feed 内 zh_CN 模板路径：Makefile 在 packages/print/cups/，上三级为 feed 根
CUPS_FEED_ROOT := $(dir $(lastword $(MAKEFILE_LIST)))../../..
CUPS_ZH_CN_TEMPLATES := $(CUPS_FEED_ROOT)/templates/zh_CN
# SDK 下 feed 安装路径（CI 与本地 src-link openprinting_cups 时）
CUPS_ZH_CN_FEEDS := $(TOPDIR)/feeds/openprinting_cups/templates/zh_CN
# 本地 CUPS 源码（有则用本地源码构建，含 locale/cups_zh_CN.po 时 LANGUAGES 会含 zh_CN）
CUPS_LOCAL_SRC := $(CUPS_FEED_ROOT)/cups
CUPS_LOCAL_SRC_FEEDS := $(TOPDIR)/feeds/openprinting_cups/cups

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

# 在 prepare 阶段：有本地 cups 源码则用本地（含 zh_CN po 与可覆盖的 templates），否则下载并覆盖 zh_CN
define Build/Prepare
	@LOCAL=""; \
	if [ -f "$(CUPS_LOCAL_SRC_FEEDS)/config-scripts/cups-defaults.m4" ] || [ -f "$(CUPS_LOCAL_SRC)/config-scripts/cups-defaults.m4" ]; then \
		if [ -f "$(CUPS_LOCAL_SRC_FEEDS)/config-scripts/cups-defaults.m4" ]; then LOCAL="$(CUPS_LOCAL_SRC_FEEDS)"; \
		else LOCAL="$(CUPS_LOCAL_SRC)"; fi; \
	fi; \
	if [ -n "$$LOCAL" ]; then \
		echo "Using local CUPS source: $$LOCAL"; \
		rm -rf $(PKG_BUILD_DIR); mkdir -p $(dir $(PKG_BUILD_DIR)); cp -a "$$LOCAL/." "$(PKG_BUILD_DIR)/"; \
		ZH_CN=""; [ -d "$(CUPS_ZH_CN_FEEDS)" ] && ZH_CN="$(CUPS_ZH_CN_FEEDS)"; [ -z "$$ZH_CN" ] && [ -d "$(CUPS_ZH_CN_TEMPLATES)" ] && ZH_CN="$(CUPS_ZH_CN_TEMPLATES)"; \
		if [ -n "$$ZH_CN" ]; then mkdir -p "$(PKG_BUILD_DIR)/templates/zh_CN"; cp $$ZH_CN/*.tmpl "$(PKG_BUILD_DIR)/templates/zh_CN/"; echo "Overlaid zh_CN: $$(ls $(PKG_BUILD_DIR)/templates/zh_CN/*.tmpl 2>/dev/null | wc -l) files"; fi; \
	else \
		$(call Build/Prepare/Default); \
		ZH_CN=""; [ -d "$(CUPS_ZH_CN_FEEDS)" ] && ZH_CN="$(CUPS_ZH_CN_FEEDS)"; [ -z "$$ZH_CN" ] && [ -d "$(CUPS_ZH_CN_TEMPLATES)" ] && ZH_CN="$(CUPS_ZH_CN_TEMPLATES)"; \
		if [ -n "$$ZH_CN" ]; then mkdir -p $(PKG_BUILD_DIR)/templates/zh_CN; cp $$ZH_CN/*.tmpl $(PKG_BUILD_DIR)/templates/zh_CN/; echo "Copied zh_CN: $$(ls $(PKG_BUILD_DIR)/templates/zh_CN/*.tmpl 2>/dev/null | wc -l) files"; fi; \
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
	# LANGUAGES 不含 zh_CN 时 make install 不会安装 zh_CN，在此补装
	@ZH_SRC=""; \
	echo "zh_CN lookup: CUPS_ZH_CN_FEEDS=$(CUPS_ZH_CN_FEEDS)"; \
	echo "zh_CN lookup: CUPS_ZH_CN_TEMPLATES=$(CUPS_ZH_CN_TEMPLATES)"; \
	if [ -d "$(CUPS_ZH_CN_FEEDS)" ]; then ZH_SRC="$(CUPS_ZH_CN_FEEDS)"; \
	elif [ -d "$(CUPS_ZH_CN_TEMPLATES)" ]; then ZH_SRC="$(CUPS_ZH_CN_TEMPLATES)"; fi; \
	if [ -n "$$ZH_SRC" ]; then \
		$(INSTALL_DIR) $(1)/usr/share/cups/templates/zh_CN; \
		$(CP) $$ZH_SRC/*.tmpl $(1)/usr/share/cups/templates/zh_CN/; \
		echo "Installed zh_CN templates from $$ZH_SRC ($$(ls $$ZH_SRC/*.tmpl 2>/dev/null | wc -l) files)"; \
	else \
		echo "zh_CN skip: neither path exists"; \
	fi

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

