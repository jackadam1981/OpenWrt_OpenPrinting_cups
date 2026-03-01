---
name: translate-cups-zh-cn
description: Translates cups_zh_CN.po for OpenPrinting (CUPS) print server. Uses printing and typesetting terminology. Use when translating cups_zh_CN.po, editing zh_CN locale, or working with CUPS Chinese localization.
---

# CUPS 简体中文翻译规范

## 适用文件

- `cups_zh_CN.po`：OpenPrinting / CUPS 打印服务器的简体中文翻译文件
- 目标用户：中国大陆简体中文用户

## 领域术语

翻译时参考**打印、排版、装订**等专业术语：

| 英文 | 推荐译法 | 说明 |
|------|----------|------|
| document | 文档 | 待打印内容 |
| printer / print server | 打印机 / 打印服务器 | |
| queue | 队列 | 打印任务队列 |
| cover / cover-sheet | 封面 | 打印封面 |
| duplex | 双面 | 双面打印 |
| simplex | 单面 | 单面打印 |
| collate | 分套 | 多份文档按套排列 |
| binding / baling | 装订 / 打捆 | |
| media / media-size | 介质 / 介质尺寸 | 纸张规格 |
| spool | 打印队列 | 不用"假脱机" |
| filter | 过滤器 | 文件格式转换 |
| job accounting | 作业统计 | 打印作业统计，不用"计费" |
| PPD | PPD | 打印机描述文件，保留不译 |

## 复合字符串翻译规则

msgid 常含 `-` 或 `.` 分隔，**按整句语义翻译**，不要只译后半段：

### 1. 点号分隔 (如 `xxx.yyy`)

- **错误**：只译 `.` 后面 → "document-state-reasons.aborted-by-system" → "系统中止"
- **正确**：整句语义 → "文档状态原因：系统中止"
- 点前为类别，点后为具体项，翻译时要体现完整含义

### 2. 连字符分隔 (如 `xxx-yyy-zzz`)

- **错误**：逐词直译或只译部分
- **正确**：理解为完整短语，用自然中文表达

### 3. 示例对照

| msgid | ❌ 不推荐 | ✅ 推荐 |
|-------|-----------|---------|
| cover-type.print-back | 只打背面 | 封面类型：仅背面封面 |
| document-state-reasons.queued | 排队 | 文档状态原因：已排队 |
| covering-name.pre-cut | 预切 | 封面类型：预切封面 |
| baling-when.after-sets | 每套之后 | 打捆时机：每套完成后 |

## 翻译原则

1. **口语化**：用自然、易懂的中文，少用生硬直译
2. **术语统一**：同类术语在文件中保持一致
3. **保留占位符**：`%s`、`%d`、`\n` 等原样保留
4. **保留专有名词**：CUPS、PPD、IPP、OAuth、TLS 等不译
5. **尺寸格式**：如 `3.00x4.00"` → `3.00×4.00 英寸`（用 × 不用 x）

## PO 格式注意

- `msgid` 为源字符串，不可修改
- `msgstr` 为翻译结果
- 多行字符串用 `\n` 表示换行
- 引号、反斜杠需正确转义

## 参考

- CUPS 官网：https://openprinting.org/cups/
- 上游仓库：https://github.com/OpenPrinting/cups
