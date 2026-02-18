#!/bin/bash
# Linux flash script for Raspberry Pi Pico / Pico 2 (UF2)
set -euo pipefail
cd "$(dirname "$0")"

UF2_FILE="build/monster_book.uf2"

# Check if UF2 file exists
if [ ! -f "$UF2_FILE" ]; then
  echo "Error: $UF2_FILE not found. Run ./build-linux.sh first."
  exit 1
fi

echo "Looking for Pico in BOOTSEL mode..."

# 1) Find the block device by label first (preferred), fallback to RP2350 USB VID:PID if needed.
# Common labels: RPI-RP2 (often), sometimes RP2350
PICO_DEV=""
PICO_DEV="$(lsblk -rno PATH,LABEL | awk '$2=="RPI-RP2" || $2=="RP2350" {print $1; exit}')"

if [ -z "$PICO_DEV" ]; then
  # Fallback: try to find a USB removable vfat partition (common for BOOTSEL)
  # This is a best-effort fallback; label-based detection is most reliable.
  PICO_DEV="$(lsblk -rno PATH,TRAN,FSTYPE | awk '$2=="usb" && $3=="vfat" {print $1; exit}')"
fi

if [ -z "$PICO_DEV" ]; then
  echo "Error: Pico block device not found."
  echo "Make sure you hold BOOTSEL while plugging it in."
  echo "Tip: run 'lsusb' and 'lsblk' to verify it appears."
  exit 1
fi

# 2) Check if it's already mounted
PICO_MNT="$(lsblk -rno MOUNTPOINT "$PICO_DEV" | head -n1 | tr -d '[:space:]')"

# 3) If not mounted, mount it (try udisksctl first; fallback to sudo mount)
if [ -z "$PICO_MNT" ]; then
  echo "Pico detected at $PICO_DEV but not mounted. Mounting..."

  if command -v udisksctl >/dev/null 2>&1; then
    # udisksctl chooses a nice mountpoint under /run/media/$USER/...
    udisksctl mount -b "$PICO_DEV" >/dev/null
    PICO_MNT="$(lsblk -rno MOUNTPOINT "$PICO_DEV" | head -n1 | tr -d '[:space:]')"
  fi

  if [ -z "$PICO_MNT" ]; then
    # Manual mount fallback
    MNT_BASE="/mnt/pico"
    sudo mkdir -p "$MNT_BASE"

    # If already mounted elsewhere, this will fail; that's OK because we'll re-check after.
    sudo mount "$PICO_DEV" "$MNT_BASE" 2>/dev/null || true
    PICO_MNT="$(lsblk -rno MOUNTPOINT "$PICO_DEV" | head -n1 | tr -d '[:space:]')"

    # If still empty, assume our fallback mountpoint
    if [ -z "$PICO_MNT" ] && mountpoint -q "$MNT_BASE"; then
      PICO_MNT="$MNT_BASE"
    fi
  fi
fi

if [ -z "$PICO_MNT" ]; then
  echo "Error: Pico was detected ($PICO_DEV) but could not be mounted."
  echo "Try: udisksctl mount -b $PICO_DEV  (or check permissions / dmesg)"
  exit 1
fi

echo "Mounted Pico at: $PICO_MNT"
echo "Flashing UF2: $UF2_FILE"

# 4) Copy UF2 and ensure it hits disk
cp -f "$UF2_FILE" "$PICO_MNT/"
sync

echo "Done! The Pico should reboot automatically."

# 5) Optional: unmount after flashing (usually safe and tidy)
# If you don't want auto-unmount, set AUTO_UNMOUNT=0 when running the script.
AUTO_UNMOUNT="${AUTO_UNMOUNT:-1}"

if [ "$AUTO_UNMOUNT" = "1" ]; then
  echo "Unmounting..."
  if command -v udisksctl >/dev/null 2>&1; then
    udisksctl unmount -b "$PICO_DEV" >/dev/null || true
  else
    sudo umount "$PICO_MNT" || true
  fi
  echo "Unmount complete."
fi
