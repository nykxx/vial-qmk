// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4             Col5               Col6             Col7              Col8       Col9     Col10    Col11 */
/*Row0*/ KC_ESC,   KC_Q,    KC_L,    KC_U,     KC_COMM,        KC_DOT,            KC_F,            KC_W,             KC_R,      KC_Y,    KC_P,    KC_SLSH,   
/*Row1*/ KC_LCTL,  KC_E,    KC_I,    KC_A,     KC_O,           KC_MINS,           KC_K,            KC_T,             KC_N,      KC_S,    KC_H,    KC_QUOT,
/*Row2*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,     KC_V,           KC_SCLN,           KC_G,            KC_D,             KC_M,      KC_J,    KC_B,    KC_EQL,   
/*Row3*/ MO(2),                      KC_LALT , LT(3,KC_SPACE), LT(1,KC_SPACE),    LT(1,KC_ENTER),  LT(3, KC_BSPC),   KC_RALT,                     MO(2)
    ),
    [1] = LAYOUT( 
       /* Col0         Col1          Col2             Col3             Col4             Col5                 Col6            Col7           Col8          Col9            Col10           Col11      */
/*Row0*/ KC_GRAVE,     LSFT(KC_1),   LSFT(KC_2),      LSFT(KC_3),      LSFT(KC_4),      LSFT(KC_5),          LSFT(KC_6),     LSFT(KC_7),    LSFT(KC_8),   LSFT(KC_9),     LSFT(KC_0),    LSFT(KC_SLSH), 
/*Row1*/ KC_TAB,       _______,      KC_LBRC,         KC_RBRC,         LSFT(KC_COMM),   LSFT(KC_MINS),       _______,        _______,       _______,      _______,        _______,       LSFT(KC_QUOT),   
/*Row2*/ _______,      _______,      LSFT(KC_LBRC),   LSFT(KC_RBRC),    _______,        LSFT(KC_SCLN),       _______,        _______,       KC_BSLS,      LSFT(KC_BSLS),  _______,       LSFT(KC_EQL),   
/*Row3*/ _______,                                     _______,          _______,        _______,             _______,        KC_DEL,        _______,                                     _______
    ),
    [2] = LAYOUT(
       /* Col0     Col1       Col2       Col3       Col4      Col5          Col6         Col7        Col8        Col9       Col10       Col11   */
/*Row0*/ _______,  KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,       KC_F6,       KC_F7,      KC_F8,      KC_F9,      KC_0,       KC_F11,   
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    KC_F12,    
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row3*/ _______,                       _______,   _______,   _______,     _______,     _______,    _______,                            _______ 
    ),
  [3] = LAYOUT(
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7       Col8        Col9       Col10       Col11  */
/*Row0*/ _______,  KC_1,      KC_2,      KC_3,     KC_4,      KC_5,        KC_6,       KC_7,       KC_8,       KC_9,      KC_0,      _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,    KC_4,       KC_5,       KC_6,      _______,   _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,    KC_1,       KC_2,       KC_3,      _______,   _______,
/*Row3*/ _______,                       _______,   _______,   _______,     _______,    _______,    _______,                          _______
    ), 
   [4] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4      Col5          Col6         Col7        Col8        Col9        Col10        Col11 */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,
/*Row3*/ _______,                       _______,   _______,   _______,     _______,     _______,    _______,                            _______
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

/**************************************************/
/* プロトタイプ                                      */


/**************************************************/
/* 組込関数                                       */
void keyboard_post_init_user(void) {
    // すべてのLEDに同じ設定をする場合はこちらの関数
    //rgblight_sethsv(127, 255, 100);

}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 
    int is_layer = get_highest_layer(layer_state|default_layer_state);  
    HSV hsv = {0, 255, rgblight_get_val()};
    if (is_layer == 1) {
        hsv.h = 191; //PURPLE
    } else if (is_layer == 2)  {
        hsv.h = 85; //GREEN
    } else if (is_layer == 3)  {
        hsv.h = 43; //YELLOW
    } else if (is_layer == 4)  {
        hsv.h = 225; 
    } else if (is_layer == 5)  {
        hsv.h = 0;
    } else {
        hsv.h = 128; //CYAN
    }

    RGB rgb = hsv_to_rgb(hsv);
    for (uint8_t i = led_min; i <= led_max; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
        else if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER)) {
            rgb_matrix_set_color(i, 255, 125, 255);
        }
        else {
            // do nothing
        }
    }
    
    return false;
}

/**************************************************/
/* My Func                                        */


// EOF
  