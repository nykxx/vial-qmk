/* Copyright 2020 Josef Adamcik
 * Modification for VIA support and RGB underglow by Jens Bonk-Wiltfang
 * Modification for Vial support by Drew Petersen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// clang-format off

#pragma once

// Vial Support
#define VIAL_KEYBOARD_UID {0x87, 0x09, 0x6D, 0x6B, 0xF9, 0x10, 0xE4, 0x07}

// The four corners
#define VIAL_UNLOCK_COMBO_ROWS { 0, 5, 3, 8 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 0, 0, 0 }

/* define RP2040 PC起動時認識用（SPLIT_USB_DETECTは標準で有効となっているはずだが念のため） */
#define SPLIT_USB_DETECT
#define SPLIT_USB_TIMEOUT 10000 //Default 2000
#define SPLIT_USB_TIMEOUT_POLL 10 //Default 10

/* define RP2040 boot用 */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.

//#define SPLIT_TRANSPORT_MIRROR 		// マスターのマトリックス接続状況
#define SPLIT_LAYER_STATE_ENABLE	// レイヤー状態
#define SPLIT_LED_STATE_ENABLE		// CapsLockなどのLED点灯状態
#define SPLIT_MODS_ENABLE			// シフトキーなどのMODキーの状態
//#define SPLIT_WPM_ENABLE			// キーボードWPMの状態
#define SPLIT_OLED_ENABLE			// OLEDのオン、オフの状態