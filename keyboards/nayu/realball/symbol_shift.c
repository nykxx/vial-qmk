// SPDX-License-Identifier: GPL-2.0-or-later
//
// Shift 付きキーコード（KC_EXLM = LSFT(KC_1) など、以下「記号キー」）の Shift の扱いを変える。
//
// QMK では、記号キーを押すと Shift を「weak mod」として一時的に付け、そのキーを離すと
// weak mod を外す。記号キーを2つ重ねて押し、先に1つ目を離すと、2つ目を押したままでも
// Shift が外れる。Wayland + fcitx5 の環境では、HID のキー押下にはすべて Shift が付いていたのに、
// 記号ではなく数字が入力されることがあった（2026-11-20 の実機確認）。
//
// そこで、押している記号キーが最後の1つまで離されるまで weak Shift を保つ。押下は QMK の
// 通常処理を通す。また、記号でないキーを押すと QMK が先に weak mod を消すので、
// 「!a」が「!A」になることはない。
#include "quantum.h"

#ifndef REALBALL_SYMBOL_SHIFT_HOLD
#    define REALBALL_SYMBOL_SHIFT_HOLD 1
#endif

#if REALBALL_SYMBOL_SHIFT_HOLD
_Static_assert(MATRIX_ROWS * MATRIX_COLS <= 64, "symbol key bitmap too small");
// 記号キーを押している最中のマトリクス位置。ビット番号は row * MATRIX_COLS + col で、
// 左右の手を合わせた48か所すべてを表せる。
static uint64_t symbol_keys;

// Shift だけを修飾した基本キーコード（LSFT(kc) / RSFT(kc)）なら true。
// Ctrl+Shift などの組み合わせは対象外で、QMK 標準どおりに扱う。
static bool is_symbol_keycode(uint16_t keycode) {
    if (!IS_QK_MODS(keycode)) return false;
    uint8_t mods = QK_MODS_GET_MODS(keycode);
    return (mods == MOD_LSFT || mods == MOD_RSFT) && IS_BASIC_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode));
}
#endif

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;
#if REALBALL_SYMBOL_SHIFT_HOLD
    // 物理キー以外のイベント（コンボなど）や記号キー以外は、QMK 標準の処理に任せる。
    keypos_t pos = record->event.key;
    if (record->event.type != KEY_EVENT || pos.row >= MATRIX_ROWS || pos.col >= MATRIX_COLS || !is_symbol_keycode(keycode)) return true;
    uint64_t bit = (uint64_t)1 << (pos.row * MATRIX_COLS + pos.col);
    if (record->event.pressed) {
        symbol_keys |= bit;
        return true; // QMK が weak Shift を付け、通常どおりキーを登録する。
    }
    // 解放: QMK 標準の解放処理（weak Shift を外す）は使わず、基本キーだけを離す。
    symbol_keys &= ~bit;
    unregister_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
    // 最後の記号キーを離したときだけ weak Shift を外してレポートを送る。
    if (!symbol_keys) {
        del_weak_mods(MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
        send_keyboard_report();
    }
    return false; // QMK 側の解放処理を行わない。
#else
    return true;
#endif
}
