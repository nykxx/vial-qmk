// Copyright 2025 nayukix
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define USB_VBUS_PIN GP20
// #define SPLIT_USB_DETECT
// #define SPLIT_USB_TIMEOUT 10000  //Default 2000
// #define SPLIT_USB_TIMEOUT_POLL 10 //Default 10

/* define RP2040 boot用 */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
