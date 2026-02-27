#!/bin/bash
# 检查 OpenPrinting/cups 是否有新版本，若有则更新 Makefile 并输出 updated=true
set -e

MAKEFILE="packages/print/cups/Makefile"

# 获取当前 Makefile 中的版本
current_version=$(grep -E '^PKG_VERSION:=' "$MAKEFILE" | cut -d= -f2)
echo "Current version: $current_version"

# 获取 OpenPrinting/cups 最新发布版本
api_resp=$(curl -sL "https://api.github.com/repos/OpenPrinting/cups/releases/latest")
new_tag=$(echo "$api_resp" | jq -r '.tag_name // empty')
if [ -z "$new_tag" ]; then
  echo "Failed to fetch latest release"
  exit 1
fi
# 去掉 v 前缀
new_version="${new_tag#v}"
echo "Latest upstream version: $new_version"

# 版本比较：若新版本 <= 当前版本则退出
if [ "$current_version" = "$new_version" ]; then
  echo "Already at latest version"
  echo "updated=false" >> "$GITHUB_OUTPUT"
  exit 0
fi

# 若当前版本大于上游（如使用预发布），则不更新
if [ "$(printf '%s\n' "$current_version" "$new_version" | sort -V | head -n1)" = "$new_version" ] && [ "$current_version" != "$new_version" ]; then
  echo "No update needed (current >= latest)"
  echo "updated=false" >> "$GITHUB_OUTPUT"
  exit 0
fi

# 下载 source tarball 并计算 SHA256
tarball_url="https://github.com/OpenPrinting/cups/releases/download/v${new_version}/cups-${new_version}-source.tar.gz"
echo "Downloading $tarball_url to compute hash..."
new_hash=$(curl -sL "$tarball_url" | sha256sum | cut -d' ' -f1)
echo "New SHA256: $new_hash"

# 更新 Makefile
sed -i "s/^PKG_VERSION:=.*/PKG_VERSION:=${new_version}/" "$MAKEFILE"
sed -i "s/^PKG_HASH:=.*/PKG_HASH:=${new_hash}/" "$MAKEFILE"
# 移除 TODO 注释（已更新为真实 hash）
sed -i '/^# TODO: 更新为对应版本 tarball 的真实 SHA256$/d' "$MAKEFILE"

echo "Makefile updated successfully"
echo "updated=true" >> "$GITHUB_OUTPUT"
echo "version=$new_version" >> "$GITHUB_OUTPUT"
echo "old_version=$current_version" >> "$GITHUB_OUTPUT"
