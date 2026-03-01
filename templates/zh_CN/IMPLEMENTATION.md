# zh_CN Web 模板实现方案研究

## 1. 上游 CUPS 2.4.16 中 ja / ru / fr 等多语言如何实现

### 1.1 locale（决定“安装哪些语言”）

- **位置**：`locale/cups_<lang>.po`
- **用途**：gettext 消息（命令行/程序内字符串）；同时被 **configure** 用来生成 `LANGUAGES`。
- **config-scripts/cups-defaults.m4**：
  ```m4
  LANGUAGES="$(ls -1 locale/cups_*.po 2>/dev/null | sed -e '1,$s/locale\/cups_//' -e '1,$s/\.po//' | tr '\n' ' ')"
  ```
- 上游 v2.4.16 的 locale 目录包含：ca, cs, da, de, en, es, fr, it, ja, pt_BR, ru, **zh_CN** 等。

### 1.2 templates（Web 界面翻译）

- **位置**：`templates/<lang>/*.tmpl`（每个语言一个子目录，与 locale 的 lang 名一致）。
- **用途**：CGI 根据 `Accept-Language` 选择对应目录下的 `.tmpl` 渲染 Web 界面。
- **templates/Makefile** 的 install-data：
  ```makefile
  for lang in $(LANGUAGES); do \
    if test -d $$lang; then \
      $(INSTALL_DIR) -m 755 $(DATADIR)/templates/$$lang; \
      for file in $(FILES); do \
        $(INSTALL_DATA) $$lang/$$file $(DATADIR)/templates/$$lang ... || true; \
      done \
    fi \
  done
  ```
- 上游 v2.4.16 的 templates 下**语言子目录**只有：**da, de, es, fr, ja, pt_BR, ru**。  
- **没有 templates/zh_CN/**，即上游未提供 zh_CN 的 Web 模板。

### 1.3 小结：ja / ru / fr 的“完整链路”

| 语言 | locale/cups_XX.po | templates/XX/ | 结果 |
|------|-------------------|---------------|------|
| ja   | 有                | 有            | LANGUAGES 含 ja，make install 会安装 templates/ja/ |
| ru   | 有                | 有            | 同上 |
| fr   | 有                | 有            | 同上 |
| zh_CN| 有                | **无**        | LANGUAGES 含 zh_CN，但 `test -d zh_CN` 为假，**不会安装** templates/zh_CN/ |

因此：**有 .po 只保证“语言被列入 LANGUAGES”；真正把 Web 界面装进系统的是“存在 templates/<lang>/ 且 make install 时执行安装”。**

---

## 2. 我们当前的做法与缺口

### 2.1 已有

- 仓库内维护 **templates/zh_CN/*.tmpl**（66 个中文模板）。
- CI 在 **Prepare 之后、Compile 之前** 把 `templates/zh_CN/*.tmpl` 复制到 `$CUPS_SRC/templates/zh_CN/`。
- 上游 tarball 自带 **locale/cups_zh_CN.po**，故 configure 后 **LANGUAGES 已包含 zh_CN**。

### 2.2 理论预期

- 复制后，`$CUPS_SRC/templates/zh_CN/` 存在且含所有 FILES 中的 .tmpl。
- `make install` 时会对 `lang=zh_CN` 执行 `if test -d zh_CN` 为真，应安装到 `$(DATADIR)/templates/zh_CN/`。
- 若如此，ipk 里应包含 `/usr/share/cups/templates/zh_CN/`，Web 应能显示中文。

### 2.3 若“编译完成后没有中文”的可能原因

1. **复制时机**  
   - 若 configure 在 Prepare 阶段就执行并生成 Makedefs，而复制在 Prepare 之后，则复制晚于 configure 没问题；但若 compile 时再次从干净源码解压或未使用我们复制过的目录，则 zh_CN 不会被安装。
2. **FILES 与 zh_CN 内文件名不一致**  
   - templates/Makefile 的 FILES 列表与 `templates/zh_CN/` 内文件名必须一致；缺失或拼写不同会用 `|| true` 静默跳过，导致部分或全部未安装。
3. **安装目标与打包目标不一致**  
   - CUPS 的 `make install` 使用 `BUILDROOT="$(PKG_INSTALL_DIR)"`，安装到 `$(PKG_INSTALL_DIR)/usr/share/cups/templates/zh_CN/`；若打包时只拷贝了部分目录，可能漏掉 zh_CN。

---

## 3. 推荐方案（与 ja/ru/fr 一致 + 兜底）

### 与 ja/ru/fr 相同路径

- **保证**：在 **configure 之后、make install 之前**，`$CUPS_SRC/templates/zh_CN/` 已存在且包含与 `templates/Makefile` 中 FILES 一致的 .tmpl。
- **核对**：上游 `templates/Makefile` 的 FILES 与仓库 `templates/zh_CN/` 的 .tmpl 文件名一致，缺的补上。
- **构建顺序**：Prepare（解压）→ 复制 templates/zh_CN 到 $CUPS_SRC/templates/zh_CN → Compile（configure + make all install）。与 ja/ru/fr 一致，由 CUPS 的 install-data 安装 zh_CN。
- **CI 复查**：复制后在构建日志中执行 `ls -la $CUPS_SRC/templates/zh_CN/`，并与 `templates/ja` 的 .tmpl 数量对比，确认文件结构正确。

---

## 4. 总结

- **ja / ru / fr**：上游同时提供 `locale/cups_<lang>.po` 和 `templates/<lang>/*.tmpl`，configure 把 <lang> 放进 LANGUAGES，make install 根据 LANGUAGES 安装对应 templates 子目录。
- **zh_CN**：上游有 `locale/cups_zh_CN.po`（LANGUAGES 含 zh_CN），但**没有** `templates/zh_CN/`，因此默认不会安装 zh_CN Web 模板。
- 我们仅通过 CI 在 Compile 前复制 templates/zh_CN 到 $CUPS_SRC，由 CUPS make install 按 LANGUAGES 安装，使 zh_CN 与 ja/ru/fr 一样出现在 `/usr/share/cups/templates/zh_CN/`。
- CI 中复制后执行 `ls -la` 与 ja 数量对比，用于复查文件结构。若仍无中文，按上文“可能原因”逐项检查即可。
