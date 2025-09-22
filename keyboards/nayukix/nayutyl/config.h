// Copyright 2025 nayukix
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define USB_VBUS_PIN GP10
// #define SPLIT_USB_DETECT
// #define SPLIT_USB_TIMEOUT 3500  //Default 2000
// #define SPLIT_USB_TIMEOUT_POLL 25 //Default 10

/* define RP2040 boot用 */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

// #define ENCODER_A_PINS { GP12, GP16 }
// #define ENCODER_B_PINS { GP13, GP17 }
// #define ENCODER_RESOLUTION 2

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
