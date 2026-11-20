// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"

#include "layer_define.h"
#include "my_combo.inc"
#include "my_tapdance.inc"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4                    Col5     Col6    Col7     Col8     Col9    */
/*Row0*/ KC_Q,     KC_L,    KC_U,    KC_COMM,  KC_DOT,                  KC_F,    KC_W,   KC_R,    KC_Y,    KC_P,
/*Row1*/ KC_E,     KC_I,    KC_A,    KC_O,     KC_MINS,                 KC_K,    KC_T,   KC_N,    KC_S,    KC_H,
/*Row2*/ KC_J,     KC_X,    KC_C,    KC_V,     KC_EQL,                  KC_G,    KC_D,   KC_M,    KC_Z,    KC_B,
/*Row3 */LALT_T(KC_F14),  LT(FN,KC_SPACE), LT(MARK,KC_SPACE),      LT(MARK,KC_ENTER),  LT(FN,KC_BSPC),      RALT_T(KC_F13)
    ),
    [MARK] = LAYOUT( 
       /* Col0       Col1         Col2          Col3        Col4          Col5        Col6          Col7        Col8         Col9   */
/*Row0*/ S(KC_1),    S(KC_2),     S(KC_3),     S(KC_4),     S(KC_5),      S(KC_6),   S(KC_7),      S(KC_8),    KC_SLSH,      KC_QUES,
/*Row1*/ KC_QUOT,    KC_LBRC,     KC_RBRC,     KC_LABK,     KC_RABK,      KC_COLN,   KC_LEFT,      KC_DOWN,    KC_UP,        KC_RGHT,
/*Row2*/ KC_UNDS,    S(KC_LBRC),  S(KC_RBRC),  KC_LPRN,     KC_RPRN,      KC_GRAVE,  S(KC_GRAVE),  KC_BSLS,    S(KC_BSLS),   KC_DQUO,
/*Row3*/ KC_F16,                               KC_F15,      KC_SPACE,     KC_BSPC,   KC_DEL,                                 KC_SCLN
    ),
    [FN] = LAYOUT(
       /* Col0      Col1       Col2       Col3       Col4           Col5          Col6         Col7        Col8        Col9       */
/*Row0*/ KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,            KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,   
/*Row1*/  _______,   _______,  _______,   _______,   _______,         _______,    _______,    _______,    KC_F11,     KC_F12,
/*Row2*/  C(KC_Z),   C(KC_X),  C(KC_C),   C(KC_V),   G(KC_V),         KC_HOME,    KC_PGDN,    KC_PGUP,    KC_END,   LSG(KC_S),
/*Row3*/ _______,                       _______,   _______,           _______,    _______,                           _______
    ),
    [NUM] = LAYOUT(
       /* Col0     Col1       Col2       Col3      Col4             Col5        Col6         Col7        Col8        Col9   */
/*Row0*/ KC_NUM,   KC_P7,    KC_P8,     KC_P9,    XXXXXXX,         XXXXXXX,    KC_P7,      KC_P8,       KC_P9,       KC_EQL,   
/*Row1*/ XXXXXXX,  KC_P4,    KC_P5,     KC_P6,    KC_LPRN,         KC_PPLS,    KC_P4,      KC_P5,       KC_P6,       KC_PAST,
/*Row2*/ XXXXXXX,  KC_P1,    KC_P2,     KC_P3,    KC_RPRN,         KC_PMNS,    KC_P1,      KC_P2,       KC_P3,       KC_PSLS,
/*Row3*/ TO(BASE),                      KC_P0,    KC_PDOT,         KC_P0,      KC_PDOT,                              XXXXXXX
    ),
    [EX1] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4            Col5          Col6         Col7        Col8        Col9    */
/*Row0*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row1*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row3*/ _______,                       _______,   _______,         _______,     _______,                            _______
    ),
    [EX2] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4            Col5          Col6         Col7        Col8        Col9    */
/*Row0*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row1*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,         _______,     _______,    _______,    _______,    _______, 
/*Row3*/ _______,                       _______,   _______,         _______,     _______,                            _______
    ),
   [LIGHT] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4            Col5          Col6         Col7        Col8        Col9    */
/*Row0*/ TO(BASE), XXXXXXX,   XXXXXXX,  XXXXXXX,   RM_NEXT,         RM_HUEU,     RM_SATU,    RM_VALU,    RM_SPDU,    XXXXXXX, 
/*Row1*/ XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX,   RM_PREV,         RM_HUED,     RM_SATD,    RM_VALD,    RM_SPDD,    XXXXXXX, 
/*Row2*/ XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX,   RM_TOGG,         XXXXXXX,     XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX, 
/*Row3*/ XXXXXXX,                       XXXXXXX,   XXXXXXX,         XXXXXXX,     XXXXXXX,                            XXXXXXX
    ),
};


/**************************************************/
/* プロトタイプ                                   */
void set_my_combo(void);
void set_my_tapdance(void);
/**************************************************/
/* 組込関数                                       */
void keyboard_post_init_user(void) {
    // vialはEEPROMに保存する都合でQMKと同様の方法ではコンボやタップダンスをコードで事前に定義できない
    // 以下の関数でキーボードの初期化時にEEPROMの設定がなければ注入する
    set_my_combo();
    set_my_tapdance();
}


// キーを押す・離すタイミングで呼ばれる関数
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {

    }
    return true;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 
    int is_layer = get_highest_layer(layer_state|default_layer_state);
    HSV hsv = {0, 255, rgblight_get_val()};
    if (is_layer == MARK) {
        hsv.h = 75; 
    } else if (is_layer == FN)  {
        hsv.h = 240; // PINK
    } else if (is_layer == NUM) {
        hsv.h = 10; // ORANGE
    } else if (is_layer == EX1)  {
        hsv.h = 65; 
    } else if (is_layer == EX2)  {
        hsv.h = 160; 
    } else if (is_layer == LIGHT) {
        hsv.h = 0;  // RED
    } else {
        hsv.h = 128; // CYAN 
    }

    RGB rgb = hsv_to_rgb(hsv);
    for (uint8_t i = led_min; i <= led_max; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
        else if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER)) {
            //rgb_matrix_set_color(i, 255, 125, 255);
        }
        else {
            // do nothing
        }
    }
    
    return false;
}

/**************************************************/
/* My Func                                        */
void set_my_combo(void) {
    // eepromから1個目のcomboをgetして、0でなければVialで設定済みとして処理しない
    vial_combo_entry_t combo = {0};
    if (dynamic_keymap_get_combo(0, &combo) < 0) {
        return;
    }

    const vial_combo_entry_t init_combo = {0};
    if (memcmp(&init_combo, &combo, sizeof(vial_combo_entry_t)) != 0) {
        return;
    }


    // vial.cで定義された変数を持ってきて上書きする
    extern combo_t key_combos[VIAL_COMBO_ENTRIES];
    extern uint16_t key_combos_keys[VIAL_COMBO_ENTRIES][5];

    for (uint8_t i = 0; i < local_combo_size; i++) {
        vial_combo_entry_t entry = {0};
        // local_key_combos.keysを、eeprom用(Vial GUI用)のentry.inputと, QMK用のkey_combos_keyに代入
        // 終端のCOMBO_ENDの場合はbreak
        for (uint8_t j = 0; j < ARRAY_SIZE(entry.input); j++) {
            entry.input[j] = local_key_combos[i].keys[j];
            key_combos_keys[i][j] = local_key_combos[i].keys[j];
            if (local_key_combos[i].keys[j] == COMBO_END) {
                break;
            }
        }

        // local_key_combos.keycodeを、eeprom用(Vial GUI用)のentry.outputと, QMK用のkey_combos.keycodeに代入
        entry.output = local_key_combos[i].keycode;
        key_combos[i].keycode = local_key_combos[i].keycode;

        // set combo to eeprom
        if (dynamic_keymap_set_combo(i, &entry) < 0) {
            break;
        }
    }
}

void set_my_tapdance(void) {
    vial_tap_dance_entry_t tapdance = {0};
    uprintf("on_tap = %d\non_hold = %d\nterm = %d\n", tapdance.on_tap,tapdance.on_hold,tapdance.custom_tapping_term);
    if (dynamic_keymap_get_tap_dance(0, &tapdance) != 0) {
        // 0以外は異常値
        uprintf("tapdance set skipped. return at 1\n");
        return;
    }
    uprintf("on_tap = %d\non_hold = %d\nterm = %d\n", tapdance.on_tap,tapdance.on_hold,tapdance.custom_tapping_term);
    
    // dynamic_keymap_getの時にtapdance構造体に値が入っていれば
    // EEPROMに保存した値があるということでreturn
    // 0番のon_tapだけ見るってので通常は十分と思うが、0番以外だけ設定するようなことがあると機能しない
    if (tapdance.on_tap != 0) {
        uprintf("tapdance set skipped. return at 2\n");
        return;
    }

    for (int i = 0; i < local_tapdance_size; i++) {
      dynamic_keymap_set_tap_dance(i, &td_entries[i]);
    }
}
// EOF
  