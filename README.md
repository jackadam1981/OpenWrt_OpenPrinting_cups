# OpenWrt OpenPrinting CUPS Feed

本仓库提供 OpenWrt package feed，用于从 [OpenPrinting/cups](https://github.com/OpenPrinting/cups) 官方源码编译 CUPS。

## 4 包结构

四个包互相独立，可任意安装其一或组合，无交叉依赖。

| 包名 | 依赖 | 安装内容 | 面向用户 |
|------|------|----------|----------|
| **cups-mini** | zlib, libjpeg-turbo, libpng, libopenssl, libusb-1.0, libstdcpp | cupsd, cupsctl, cupsfilter, libcups, libcupsimage, backend/filter/cgi/daemon/driver/monitor, 配置, 文档, init | 最小打印服务器（Web 管理） |
| **cups-bsd** | 同上 | mini 全部 + lpr, lpq, lprm, lpc | 服务器 + BSD 命令，可独立安装 |
| **cups-client** | 同上 | mini 全部 + lp, cancel, lpadmin, cupsaccept 等 | 服务器 + System V 命令，可独立安装 |
| **cups-full** | 同上 | mini + bsd + client 全部内容 | 全功能包，可独立安装 |

### 安装组合

| 需求 | 安装 |
|------|------|
| 最小打印服务器（Web 管理） | `opkg install cups-mini` |
| 服务器 + BSD 命令 | `opkg install cups-bsd`（已含 mini） |
| 服务器 + System V 命令 | `opkg install cups-client`（已含 mini） |
| 全功能 | `opkg install cups-full` |

## CI 构建

GitHub Actions 从 [downloads.openwrt.org](https://downloads.openwrt.org) 下载 OpenWrt 24.10 SDK 并编译，产出 `.ipk`。推送到 `main`/`master` 或提交 PR 时触发，产物作为 Artifact 提供下载。

### 支持设备示例

| 设备型号 | 芯片 | OpenWrt target/subtarget | Artifact |
|----------|------|--------------------------|----------|
| x86/64 设备 | - | x86/64 | cups-x86_64-packages |
| **D-Link DIR-505** | Atheros AR1311 | ath79/generic | cups-mips_24kc_ath79-packages |
| **TP-Link TL-WR720N** | Atheros AR9331 | ath79/tiny | cups-mips_24kc_ath79_tiny-packages |
| **VoCore** | Ralink RT5350F | ramips/rt305x | cups-mipsel_24kc_rt305x-packages |
| **Mi WiFi Mini** | MediaTek MT7620A | ramips/mt7620 | cups-mipsel_24kc_mt7620-packages |
| **新路由3 (Newifi D2)** | MediaTek MT7621 | ramips/mt7621 | cups-mipsel_24kc-packages |
| **CMCC RAX3000M** | MediaTek MT7981B | mediatek/filogic | cups-aarch64_cortex-a53_filogic-packages |

其他设备可查 [OpenWrt 设备页](https://openwrt.org/toh/start)，选择对应 target/subtarget 的 artifact。

### 版本检查

每天 UTC 0:00 定时检查 [OpenPrinting/cups](https://github.com/OpenPrinting/cups) 是否有新版本，若有则自动更新 Makefile 并推送。

## 使用方式

### 从 CI Artifact 安装

1. 打开 GitHub Actions 对应构建的 Artifact，下载与设备 target/subtarget 匹配的包。
2. 解压后使用 `opkg install cups-mini_*.ipk`（或 cups-bsd/cups-client/cups-full）。

### 作为 Feed 本地构建

1. 将仓库克隆到 OpenWrt 源码树旁：

   ```
   git clone https://github.com/<your-account>/OpenWrt_OpenPrinting_cups.git
   cd openwrt
   ```

2. 在 `feeds.conf`（或 `feeds.conf.default`）中追加：

   ```
   src-link openprinting_cups ../OpenWrt_OpenPrinting_cups
   ```

3. 更新并安装 feed：

   ```bash
   ./scripts/feeds update openprinting_cups
   ./scripts/feeds install -a -p openprinting_cups
   ```

4. `make menuconfig`，在 Network -> Printing 中选择 cups。
5. 运行 `make package/cups/{clean,compile} V=s` 或构建完整固件。

## Web 管理 (http://IP:631)

- 管理入口：**http://设备IP:631/admin**（需用 root 及 root 密码登录）。
- **同时出现「请求的条目过大」且没有登录框**：多为升级后保留了旧 conffile。升级包已带自动修复（postinst），修复后执行 **`/etc/init.d/cupsd restart`** 即可。若未生效，请手动修改 `/etc/cups/cupsd.conf`：1）在 `MaxLogSize` 下一行添加 `MaxRequestSize 0`；2）把所有 `Allow 10.0.0.0/8`、`Allow 172.16.0.0/12`、`Allow 192.168.0.0/16` 改为 **Allow from** 同网段（如 `Allow from 192.168.0.0/16`），保存后 `/etc/init.d/cupsd restart`。
- **仅不出现登录框**：确认 `/etc/group` 中组名为 **lpadmin**（小写），并为 root 设置密码：`passwd root`。若仍不出现，尝试**无痕/隐私模式**或**清除浏览器缓存**后重新访问。

## 注意事项

- CUPS 依赖 `libusb-1.0`, `libjpeg-turbo`, `libpng`, `zlib`, `libopenssl`, `libstdcpp`，请确保已选。
- 资源有限的路由器上 CUPS 可能压力较大，若仅需 USB 打印透传可考虑 `p910nd`。

## 目录结构

```
OpenWrt_OpenPrinting_cups/
├── packages/print/cups/
│   ├── files/cupsd.init
│   └── Makefile
├── .github/
│   ├── scripts/build-with-sdk.sh
│   └── workflows/
└── README.md
```
