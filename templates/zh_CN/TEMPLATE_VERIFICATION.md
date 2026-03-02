# CUPS Web UI Templates 调用流程验证

## 1. 调用链概览

```
浏览器 HTTP 请求 (Accept-Language: zh-CN)
    ↓
cups/scheduler/client.c: 解析 Accept-Language，设置 con->language
    ↓
cups/scheduler/client.c pipe_command(): 构造 env，设置 LANG=zh_CN.UTF8
    ↓
cupsd 启动 CGI (cgi-bin/*.cgi)，传入 env（含 LANG、CUPS_DATADIR）
    ↓
cups/cgi-bin/template.c cgiCopyTemplateLang(tmpl):
    - getenv("LANG") → zh_CN.UTF8
    - 转为 locale = "/zh_CN"（去掉 charset）
    - cgiGetTemplateDir() → getenv("CUPS_DATADIR")||CUPS_DATADIR + "/templates"
    - 查找顺序：
      1. {datadir}/templates/zh_CN/{tmpl}  ← 我们的 zh_CN 模板
      2. {datadir}/templates/zh/{tmpl}     （locale[3]='\0' 截断为 2 字符）
      3. {datadir}/templates/{tmpl}        （默认英文，无语言子目录）
```

## 2. 关键路径与配置

| 组件 | 源码位置 | 说明 |
|------|----------|------|
| 模板根目录 | `cgi-bin/template.c` cgiGetTemplateDir() | `CUPS_DATADIR` 环境变量 或 编译时 `CUPS_DATADIR`（默认 `/usr/share/cups`） |
| LANG 来源 | `scheduler/client.c` pipe_command() | 来自 `con->language`（由 Accept-Language 解析） |
| con->language | `scheduler/client.c` 865-901 | 解析 `Accept-Language` 头，取第一个语言；无则用 DefaultLocale |
| DataDir | `scheduler/conf.c` | cups-files.conf 的 DataDir，默认 `CUPS_DATADIR` |
| CUPS_DATADIR 传入 CGI | `scheduler/env.c` cupsdUpdateEnv() | `set_if_undefined("CUPS_DATADIR", DataDir)`，cupsdLoadEnv 会复制到 CGI 的 env |

## 3. 手动覆盖后仍无汉化的可能原因

### 3.1 浏览器未发送 Accept-Language: zh-CN

- 若请求没有 `Accept-Language`，则 `con->language = cupsLangGet(DefaultLocale)`，通常是 `C` 或 `en`
- 此时 LANG=C 或 LANG=en，CGI 只会在 `templates/` 或 `templates/en/` 下查找，不会用 `templates/zh_CN/`

**验证**：在浏览器开发者工具 → Network → 选中访问 631 的请求 → 查看 Request Headers 是否包含 `Accept-Language: zh-CN,zh;q=0.9` 或类似值。

### 3.2 模板安装路径错误

- 正确路径：`/usr/share/cups/templates/zh_CN/*.tmpl`
- OpenWrt 安装后通常为：`/usr/share/cups/templates/`（DataDir 由 cups-files.conf 或默认值决定）

**验证**：
```bash
ls -la /usr/share/cups/templates/zh_CN/
# 应能看到 header.tmpl, admin.tmpl 等
```

### 3.3 LANG 与 locale 格式

- LANG 需为 `zh_CN.UTF8` 或 `zh_CN.UTF-8`（下划线）
- `zh-CN`（连字符）在 cupsLangGet 中会转换为 `zh_CN`
- 若 LANG=C 或 LANG=en，则不会使用 zh_CN 子目录

### 3.4 CUPS_DATADIR 与 DataDir

- CGI 通过 `getenv("CUPS_DATADIR")` 获取模板根目录
- 若 cupsd 未设置该变量，则使用编译时 `CUPS_DATADIR`（如 `/usr/share/cups`）
- OpenWrt 上 DataDir 通常为 `/usr/share/cups`，应与安装路径一致

## 4. 调试建议

在设备上临时启用 CGI 调试输出（若 cups 支持），或查看 cupsd 错误日志：

```bash
# 确认模板是否存在
ls /usr/share/cups/templates/zh_CN/

# 浏览器设置：将语言偏好设为中文（zh-CN）优先
# Chrome: 设置 → 语言 → 中文置顶
# Firefox: 设置 → 语言 → 选择 中文 [zh-CN]
```

## 5. 结论

Templates 的调用依赖：

1. **LANG**：由 cupsd 根据 HTTP `Accept-Language` 设置并传给 CGI
2. **CUPS_DATADIR**：由 cupsd 从 DataDir 配置传入
3. **目录结构**：`{DataDir}/templates/zh_CN/*.tmpl` 必须存在

若手动复制了 zh_CN 模板到正确路径但仍无汉化，优先排查：**浏览器是否发送了 Accept-Language: zh-CN**，以及 cupsd 是否将对应的 LANG 传递给了 CGI。
