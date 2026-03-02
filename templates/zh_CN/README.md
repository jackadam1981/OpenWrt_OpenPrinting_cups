# CUPS Web 界面简体中文模板 (zh_CN)

本目录为 CUPS Web 管理界面（http://IP:631）的简体中文模板，供 CGI（admin.cgi、printers.cgi、jobs.cgi 等）在浏览器语言为 zh_CN 时使用。

**当前状态**：模板已从本地 `cups/templates/` 复制而来，内容仍为英文，需逐文件将界面文案翻译为简体中文。翻译时请保留所有 `{变量名}`、`{条件? ...}` 等模板语法不变，只改可见的英文句子和按钮文字。

## 与 locale/cups_zh_CN.po 的区别

- **cups_zh_CN.po**：命令行与程序内部消息（gettext），编译为 `.mo` 安装到 `/usr/share/locale/zh_CN/LC_MESSAGES/`。
- **本目录**：Web 界面 HTML 模板，需部署到设备上的 `/usr/share/cups/templates/zh_CN/`（或由构建脚本在编译时复制到 CUPS 源码的 `templates/zh_CN/`）。

## 构建集成

若需在 OpenWrt 构建时自动将本目录复制到 CUPS 源码中，可在 `build.yml` 的 “Replace zh_CN locale” 步骤后增加复制 `templates/zh_CN/*.tmpl` 到 `$CUPS_SRC/templates/zh_CN/` 的步骤。
