# OpenWrt OpenPrinting CUPS Feed

本仓库提供一个最小化的 OpenWrt package feed，用于直接从 [OpenPrinting/cups](https://github.com/OpenPrinting/cups) 官方源码编译 CUPS。

## CI 构建

本仓库配置了 GitHub Actions：

1. **Build CUPS**：使用 [OpenWrt gh-action-sdk](https://github.com/openwrt/gh-action-sdk) 自动编译 CUPS 包。推送到 `main`/`master` 分支或提交 PR 时会触发构建，编译产物（`.ipk`）会作为 Artifact 提供下载。支持架构：`x86_64`、`aarch64`、`mips_24kc`。

2. **Check for CUPS Updates**：每天 UTC 0:00 定时检查 [OpenPrinting/cups](https://github.com/OpenPrinting/cups) 是否有新版本。若有新发布，会自动更新 `packages/print/cups/Makefile` 并直接推送到 master，无需手动合并。

## 使用方式

1. 将仓库克隆到 OpenWrt 源码树旁，例如：

   ```
   git clone https://github.com/<your-account>/OpenWrt_OpenPrinting_cups.git
   cd openwrt
   ```

2. 在 OpenWrt 源码目录的 `feeds.conf`（若不存在则编辑 `feeds.conf.default`）中追加：

   ```
   src-link openprinting_cups ../OpenWrt_OpenPrinting_cups
   ```

3. 更新并安装 feed：

   ```bash
   ./scripts/feeds update openprinting_cups
   ./scripts/feeds install -a -p openprinting_cups
   ```

4. 执行 `make menuconfig`，在 `Network -> Printing` 中选择 `cups`（建议 `<M>` 只编译 ipk）。

5. 运行 `make package/cups/{clean,compile} V=s` 或构建包含 CUPS 的完整固件。

## 注意事项

- `packages/print/cups/Makefile` 中的 `PKG_HASH` 为占位值，编译前请根据所选版本的源码 tarball 更新。
- CUPS 依赖 `libusb-1.0`, `libjpeg`, `libpng`, `zlib`, `libopenssl`, `libpthread`, `libstdcpp` 等库，请在 menuconfig 中一并勾选。
- 在资源有限的路由器上运行 CUPS 可能压力较大，如仅需 USB 打印透传，可考虑 `p910nd` 等轻量方案。

## 目录结构

```
OpenWrt_OpenPrinting_cups/
└── packages/
    └── print/
        └── cups/
            ├── files/
            │   └── cupsd.init
            └── Makefile
```

根据需求可继续扩展（例如拆分 libcups、cups-client 等子包）。欢迎 PR 与 Issue。 

