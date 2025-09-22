#include "keycode_utils.h"
#include <stdio.h>

const char* keycode_to_name(uint16_t keycode) {

    static char unknown_keycode[5];  // 未定義キー用のバッファ（4文字 + 終端）

    switch (keycode) {
        // アルファベットキー
        case KC_A: return "A   ";
        case KC_B: return "B   ";
        case KC_C: return "C   ";
        case KC_D: return "D   ";
        case KC_E: return "E   ";
        case KC_F: return "F   ";
        case KC_G: return "G   ";
        case KC_H: return "H   ";
        case KC_I: return "I   ";
        case KC_J: return "J   ";
        case KC_K: return "K   ";
        case KC_L: return "L   ";
        case KC_M: return "M   ";
        case KC_N: return "N   ";
        case KC_O: return "O   ";
        case KC_P: return "P   ";
        case KC_Q: return "Q   ";
        case KC_R: return "R   ";
        case KC_S: return "S   ";
        case KC_T: return "T   ";
        case KC_U: return "U   ";
        case KC_V: return "V   ";
        case KC_W: return "W   ";
        case KC_X: return "X   ";
        case KC_Y: return "Y   ";
        case KC_Z: return "Z   ";

        // 数字キー
        case KC_1: return "1   ";
        case KC_2: return "2   ";
        case KC_3: return "3   ";
        case KC_4: return "4   ";
        case KC_5: return "5   ";
        case KC_6: return "6   ";
        case KC_7: return "7   ";
        case KC_8: return "8   ";
        case KC_9: return "9   ";
        case KC_0: return "0   ";
        
        // 記号キーなど
        case KC_ENTER: return "ENT ";
        case KC_ESC:   return "ESC ";
        case KC_BSPC: return "BSPC";
        case KC_TAB:    return "TAB ";
        case KC_SPACE:  return "SPC ";
        case KC_MINUS: return "-   ";
        case KC_EQUAL: return "=   ";
        case KC_LBRC: return "[   ";
        case KC_RBRC: return "]   ";
        case KC_BSLS: return "\\   ";
        case KC_NONUS_HASH: return "#   ";
        case KC_SCLN: return ";   ";
        case KC_QUOTE: return "'   ";
        case KC_GRAVE: return "`   ";
        case KC_COMMA: return ",   ";
        case KC_DOT: return ".   ";
        case KC_SLASH: return "/   ";
        
        // ファンクションキー
        case KC_F1: return "F1  ";
        case KC_F2: return "F2  ";
        case KC_F3: return "F3  ";
        case KC_F4: return "F4  ";
        case KC_F5: return "F5  ";
        case KC_F6: return "F6  ";
        case KC_F7: return "F7  ";
        case KC_F8: return "F8  ";
        case KC_F9: return "F9  ";
        case KC_F10: return "F10 ";
        case KC_F11: return "F11 ";
        case KC_F12: return "F12 ";
        case KC_F13: return "F13 ";
        case KC_F14: return "F14 ";
        case KC_F15: return "F15 ";
        case KC_F16: return "F16 ";
        case KC_F17: return "F17 ";
        case KC_F18: return "F18 ";
        case KC_F19: return "F19 ";
        case KC_F20: return "F20 ";
        case KC_F21: return "F21 ";
        case KC_F22: return "F22 ";
        case KC_F23: return "F23 ";
        case KC_F24: return "F24 ";
        
        // 装飾キー
        case KC_LCTL: return "LCTL";
        case KC_LSFT: return "LSFT";
        case KC_LALT: return "LALT";
        case KC_LGUI: return "LGUI";
        case KC_RCTL: return "RCTL";
        case KC_RSFT: return "RSFT";
        case KC_RALT: return "RALT";
        case KC_RGUI: return "RGUI";
    	
    	// アローキー
        case 0x004F: return "RGHT"; // 右矢印
        case 0x0050: return "LEFT"; // 左矢印
        case 0x0052: return "UP  "; // 上矢印
        case 0x0051: return "DOWN"; // 下矢印
        
        // その他特殊キー
        case 0x0001: return "same"; // ▽
        case 0x004C: return "DEL";  // Delete
        case 0x011B: return "CUT "; // Ctrl+x
        case 0x0106: return "COPY"; // Ctrl+c
        case 0x0119: return "PSTE"; // Ctrl+v
        case 0x0819: return "PSTE"; // Win+v
        
        // 未定義キー
        default:
            snprintf(unknown_keycode, sizeof(unknown_keycode), "%04X", keycode);
            return unknown_keycode;
    }
}