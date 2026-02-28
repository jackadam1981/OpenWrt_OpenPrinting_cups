# CUPS 7 包结构说明

本文档说明参考项目（如 [TheMMcOfficial/lede-cups](https://github.com/TheMMcOfficial/lede-cups)、[lixingcong/cups-openprinting-openwrt](https://github.com/lixingcong/cups-openprinting-openwrt)）采用的 CUPS 分包规则。

## 7 包结构概览

| 包名 | 依赖 | 安装内容 | 面向用户 |
|------|------|----------|----------|
| **libcups** | zlib, libpthread, libpng, libjpeg | libcups.so | 被其他包依赖的基础库 |
| **libcupsimage** | libcups | libcupsimage.so | 图像处理库 |
| **cups** | libcups, libusb, libstdcpp | cupsd, cupsctl, backend/cgi/daemon/driver/monitor, 配置, 文档, init | 服务端最小运行环境 |
| **cups-bsd** | libcups | lpr, lpq, lprm, lpc | 只用 BSD 命令的用户 |
| **cups-client** | libcups, libcupsimage | lp, cancel, cupstestppd, ipptool, lpoptions, lpstat, cupsaccept, cupsfilter, lpadmin, lpinfo, lpmove | 使用 System V 命令和管理的用户 |
| **cups-filters** | libcupsimage | commandtops, gziptoany, pstops, rastertoepson, rastertohp, rastertolabel, rastertopwg | 需要这些过滤器的打印流程 |
| **cups-ppdc** | libcups, zlib, libpng, libjpeg, libstdcpp | ppdc, ppdhtml, ppdi, ppdmerge, ppdpo | 开发/维护 PPD 的用户 |

## 各包详细说明

### 1. libcups

- **类型**：库
- **依赖**：zlib, libpthread, libpng, libjpeg
- **安装**：libcups.so（CUPS 核心库）
- **说明**：提供与 CUPS 服务器通信的 API，所有 CUPS 相关包的基础依赖

### 2. libcupsimage

- **类型**：库
- **依赖**：libcups
- **安装**：libcupsimage.so
- **说明**：CUPS 图像处理库，用于将图片转为打印所需格式

### 3. cups

- **类型**：服务
- **依赖**：libcups, libusb-1.0, libstdcpp
- **安装**：
  - cupsd（守护进程）
  - cupsctl（控制工具）
  - backend（ipp, usb, lpd, snmp, socket 等）
  - cgi-bin（Web 管理界面）
  - daemon（cups-deviced, cups-driverd, cups-exec, cups-lpd）
  - driver（驱动）
  - monitor
  - 配置（/etc/cups/*）
  - 文档（/usr/share/doc/cups/*）
  - init 脚本
- **说明**：打印服务器核心，最小可运行环境

### 4. cups-bsd

- **类型**：客户端工具
- **依赖**：libcups
- **安装**：lpr, lpq, lprm（usr/bin）, lpc（usr/sbin）
- **说明**：Berkeley Unix 风格命令，适合习惯 `lpr file.pdf` 等老式用法的用户

### 5. cups-client

- **类型**：客户端工具
- **依赖**：libcups, libcupsimage
- **安装**：
  - usr/bin：lp, cancel, cupstestppd, ipptool, lpoptions, lpstat
  - usr/sbin：cupsaccept, cupsfilter, lpadmin, lpinfo, lpmove
  - 符号链接：cupsenable, cupsdisable, cupsreject → cupsaccept
  - ipptool 资源
- **说明**：System V 风格命令，用于提交任务、管理队列和打印机

### 6. cups-filters

- **类型**：过滤器
- **依赖**：libcupsimage
- **安装**：usr/lib/cups/filter/ 下的 commandtops, gziptoany, pstops, rastertoepson, rastertohp, rastertolabel, rastertopwg 等
- **说明**：将打印数据转换为打印机可识别的格式，实际打印必需

### 7. cups-ppdc

- **类型**：开发工具
- **依赖**：libcups, zlib, libpng, libjpeg, libstdcpp
- **安装**：ppdc, ppdhtml, ppdi, ppdmerge, ppdpo
- **说明**：PPD 编译器及工具，供开发者或需要自定义 PPD 的用户使用

## 依赖关系

```
libcups
  ├── libcupsimage
  ├── cups
  ├── cups-bsd
  ├── cups-client
  ├── cups-filters
  └── cups-ppdc
```

## 常见安装组合

| 需求 | 安装组合 |
|------|----------|
| 最小打印服务器 | libcups + libcupsimage + cups + cups-filters |
| + BSD 命令 | 上述 + cups-bsd |
| + System V 管理 | 上述 + cups-client |
| 全功能 | 上述 + cups-ppdc |

## 4 包结构（本项目采用）

四个包互相独立，可任意安装其一或组合，无交叉依赖。

| 包名 | 依赖 | 安装内容 | 面向用户 |
|------|------|----------|----------|
| **cups-mini** | zlib, libjpeg-turbo, libpng, libopenssl, libusb-1.0, libstdcpp | cupsd, cupsctl, cupsfilter, libcups, libcupsimage, backend/filter/cgi/daemon/driver/monitor, 配置, 文档, init | 最小打印服务器（Web 管理） |
| **cups-bsd** | 同上 | mini 全部 + lpr, lpq, lprm, lpc | 服务器 + BSD 命令，可独立安装 |
| **cups-client** | 同上 | mini 全部 + lp, cancel, lpadmin, cupsaccept 等 | 服务器 + System V 命令，可独立安装 |
| **cups-full** | 同上 | mini + bsd + client 全部内容 | 全功能包，可独立安装 |

## 4 包依赖关系

四个包无交叉依赖，仅依赖系统库。

## 4 包安装组合

| 需求 | 安装 |
|------|------|
| 最小打印服务器（Web 管理） | `opkg install cups-mini` |
| 服务器 + BSD 命令 | `opkg install cups-bsd`（已含 mini） |
| 服务器 + System V 命令 | `opkg install cups-client`（已含 mini） |
| 全功能 | `opkg install cups-full` |

## 与单包 / 7 包方案对比

| 方案 | 优点 | 缺点 |
|------|------|------|
| **4 包** | 按需安装、结构清晰、维护适中 | 略多于单包 |
| **7 包** | 按需安装、节省空间、依赖清晰 | Makefile 复杂、维护成本高 |
| **单包** | 安装简单、一次到位、维护简单 | 体积较大、无法按需精简 |
