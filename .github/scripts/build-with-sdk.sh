#!/bin/bash
# 使用 downloads.openwrt.org 的 SDK 构建 CUPS 包（产出 .ipk）
set -e

OPENWRT_VERSION="${OPENWRT_VERSION:-24.10.0}"
BASE_URL="https://downloads.openwrt.org/releases/${OPENWRT_VERSION}/targets"
TARGET="$1"
SUBTARGET="$2"

if [ -z "$TARGET" ] || [ -z "$SUBTARGET" ]; then
  echo "Usage: $0 <target> <subtarget>  # e.g. x86 64, ramips mt7621"
  exit 1
fi

TARGET_PATH="${TARGET}/${SUBTARGET}"
SDK_PATTERN="openwrt-sdk-${OPENWRT_VERSION}-${TARGET}-${SUBTARGET}"
FEED_DIR="${FEED_DIR:-.}"

echo "==> Target: $TARGET_PATH"

# 从目录页解析 SDK 文件名（兼容 gcc 版本变化）
echo "==> Fetching SDK URL..."
INDEX=$(curl -sL "${BASE_URL}/${TARGET_PATH}/")
SDK_FILE=$(echo "$INDEX" | grep -oE "openwrt-sdk-${OPENWRT_VERSION}-${TARGET}-${SUBTARGET}_[^\"]+\.tar\.zst" | head -1)

if [ -z "$SDK_FILE" ]; then
  echo "ERROR: Could not find SDK for ${TARGET_PATH}"
  exit 1
fi

SDK_URL="${BASE_URL}/${TARGET_PATH}/${SDK_FILE}"
echo "==> SDK: $SDK_FILE"

# 下载并解压
echo "==> Downloading SDK..."
curl -sL -o sdk.tar.zst "$SDK_URL"

echo "==> Extracting SDK..."
tar -I zstd -xf sdk.tar.zst
rm sdk.tar.zst

SDK_DIR=$(find . -maxdepth 1 -type d -name "openwrt-sdk-*" | head -1)
cd "$SDK_DIR"

# 确保 feeds.conf 存在（复制 default 若需要）
[ -f feeds.conf ] || cp feeds.conf.default feeds.conf

# 添加 feed（当前仓库作为 openprinting_cups feed）
echo "==> Configuring feed..."
echo "src-link openprinting_cups $FEED_DIR" >> feeds.conf
cat feeds.conf

# 更新并安装
echo "==> Updating feeds..."
./scripts/feeds update -a

echo "==> Installing cups..."
./scripts/feeds install -p openprinting_cups -f cups

# defconfig（SDK 通常已包含）
make defconfig 2>/dev/null || true

# 构建
echo "==> Building package/cups/compile..."
make package/cups/compile -j$(nproc) V=s

echo "==> Build complete. Output in bin/"
