// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4            Col5         Col6       Col7       Col8       Col9     Col10    Col11    Col12    Col13  */
/*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,     KC_4,          KC_5,        KC_LBRC,   KC_UP,     KC_6,      KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
/*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,     KC_COMM,       KC_DOT,      KC_LALT,   KC_RALT,   KC_F,      KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
/*Row2*/ KC_LCTL,  KC_E,    KC_I,    KC_A,     KC_O,          KC_MINS,     KC_LEFT,   KC_RGHT,   KC_K,      KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
/*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,     KC_V,          KC_SCLN,     KC_DOWN,   KC_RBRC,   KC_G,      KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
/*Row4*/ KC_LSFT,  MO(2),   KC_LGUI, KC_SPACE, KC_SPACE,                   DF(3),     DF(4),     KC_ENTER,  KC_BSPC, KC_RGUI, MO(2),   KC_RCTL
    ),
    [1] = LAYOUT( 
       /* Col0            Col1          Col2         Col3         Col4            Col5           Col6            Col7           Col8          Col9          Col10        Col11        Col12        Col13  */
/*Row0*/ LSFT(KC_GRAVE),  LSFT(KC_1),   LSFT(KC_2),  LSFT(KC_3),  LSFT(KC_4),     LSFT(KC_5),    LSFT(KC_LBRC),  _______,       LSFT(KC_6),   LSFT(KC_7),   LSFT(KC_8), LSFT(KC_9),  LSFT(KC_0),  LSFT(KC_SLSH),
/*Row1*/ _______,         _______,      _______,    _______,      LSFT(KC_COMM),  LSFT(KC_DOT),  _______,        _______,       _______,      _______,      _______,     _______,    _______,     LSFT(KC_BSLS),
/*Row2*/ _______,         _______,      _______,    _______,      LSFT(KC_MINS),  _______,       _______,        _______,       _______,      _______,      _______,     _______,    _______,     LSFT(KC_QUOT),
/*Row3*/ _______,         _______,      _______,    _______,      _______,        LSFT(KC_SCLN), _______,        LSFT(KC_RBRC), _______,      _______,      _______,     _______,    _______,     _______,
/*Row4*/ _______,         _______,      _______,    _______,      _______,                       _______,        _______,       _______,      KC_DEL,       _______,     _______,    _______
    ),
    [2] = LAYOUT(
       /* Col0     Col1       Col2       Col3       Col4      Col5          Col6         Col7        Col8        Col9       Col10       Col11      Col12      Col13  */
/*Row0*/ _______,  KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,       _______,     _______,    KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,    KC_F11,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,   KC_F12,
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,   _______,
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,   _______,
/*Row4*/ _______,  _______,   _______,  _______,   _______,                _______,     _______,    _______,    _______,    _______,    _______,    _______
    ),
  [3] = LAYOUT(
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7        Col8         Col9       Col10        Col11      Col12      Col13  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row4*/ _______,  _______,   _______,  _______,   _______,                DF(0),       DF(0),      _______,    _______,    _______,    _______,    _______
    ),
   [4] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4      Col5          Col6         Col7        Col8        Col9        Col10        Col11      Col12      Col13  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row4*/ _______,  _______,   _______,  _______,   _______,                DF(0),       DF(0),      _______,    _______,    _______,    _______,    _______
    ),
};


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT)  },
    [1] =   { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT)  },
    [2] =   { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT)  },
    [3] =   { ENCODER_CCW_CW(KC_WH_D, KC_WH_U), ENCODER_CCW_CW(KC_WH_L, KC_WH_R)  },
    [4] =   { ENCODER_CCW_CW(KC_MS_D, KC_MS_U), ENCODER_CCW_CW(KC_MS_L, KC_MS_R)  }
    //                  Encoder 1                                     Encoder 2
};
#endif

void keyboard_post_init_user(void) {
    // すべてのLEDに同じ設定をする場合はこちらの関数
    rgblight_sethsv(127, 255, 100);

}