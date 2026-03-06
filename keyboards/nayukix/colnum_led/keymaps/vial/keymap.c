// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "keycode_utils.h"
//#include "bongo.h"
//#include "jeep.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4            Col5         Col6       Col7       Col8       Col9     Col10    Col11    Col12    Col13  */
/*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,     KC_4,          KC_5,        KC_LBRC,   KC_RBRC,   KC_6,      KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
/*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,     KC_COMM,       KC_DOT,      KC_LALT,   KC_RALT,   KC_F,      KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
/*Row2*/ KC_LCTL,  KC_E,    KC_I,    KC_A,     KC_O,          KC_MINS,     KC_LEFT,   KC_RGHT,   KC_K,      KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
/*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,     KC_V,          KC_SCLN,     KC_DOWN,   KC_UP,     KC_G,      KC_D,    KC_M,    KC_J,    KC_B,    KC_EQL,
/*Row4*/ KC_LSFT,  MO(2),   KC_LGUI, KC_SPACE, KC_SPACE,                   TO(3),     TO(4),     KC_ENTER,  KC_BSPC, KC_RGUI, MO(2),   KC_RCTL
    ),
    [1] = LAYOUT( 
       /* Col0            Col1          Col2         Col3         Col4            Col5           Col6            Col7           Col8          Col9          Col10        Col11        Col12        Col13  */
/*Row0*/ KC_GRAVE,  LSFT(KC_1),   LSFT(KC_2),  LSFT(KC_3),      LSFT(KC_4),     LSFT(KC_5),    LSFT(KC_LBRC),  LSFT(KC_RBRC), LSFT(KC_6),   LSFT(KC_7),   LSFT(KC_8), LSFT(KC_9),  LSFT(KC_0),  LSFT(KC_SLSH),
/*Row1*/ _______,         _______,      _______,    _______,    LSFT(KC_COMM),  LSFT(KC_DOT),  _______,        _______,       _______,      _______,      _______,     _______,    _______,     LSFT(KC_BSLS),
/*Row2*/ _______,         _______,      _______,    _______,    _______,        LSFT(KC_MINS), _______,        _______,       _______,      _______,      _______,     _______,    _______,     LSFT(KC_QUOT),
/*Row3*/ _______,         _______,      _______,    _______,    _______,        LSFT(KC_SCLN), _______,        _______,       _______,      _______,      _______,     _______,    _______,     LSFT(KC_EQL),
/*Row4*/ _______,         _______,      _______,    _______,    _______,                       _______,        _______,       _______,      KC_DEL,       _______,     _______,    _______
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
/*Row2*/ KC_CAPS,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row4*/ _______,  _______,   _______,  _______,   _______,                TO(0),       TO(0),      _______,    _______,    _______,    _______,    _______
    ),
   [4] = LAYOUT(
       /* Col0     Col1         Col2     Col3      Col4      Col5          Col6         Col7        Col8        Col9        Col10        Col11      Col12      Col13  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,    _______,    _______,
/*Row4*/ _______,  _______,   _______,  _______,   _______,                TO(0),       TO(0),      _______,    _______,    _______,    _______,    _______
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
void count_type(void);
void oled_write_type_count(void);
void oled_write_layer_state(void);
void oled_write_lockstate(void);
void oled_write_wpm(void);
void oled_write_2digit(unsigned int digit);
void oled_write_uptime(void);
void render_keycodes_row(void);


static const char PROGMEM jeep[]= {
0xff, 0xff, 0x7f, 0x7f, 0x3f, 0xbf, 0xbf, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0xbf, 
0xbf, 0xbf, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0xff, 0xff, 0x7f, 0x7f, 
0x7f, 0x3f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x3f, 
0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x3f, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x3f, 0xff, 0x7f, 0x3f, 
0x3f, 0x3f, 0x3f, 0xff, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x3f, 0xff, 0x7f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 
0xbf, 0xbf, 0x3f, 0x7f, 0xff, 0xff, 0xbf, 0x3f, 0x3f, 0x3f, 0xbf, 0xff, 0xff, 0xff, 0xbf, 0x3f, 
0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf7, 0x07, 0x07, 0x03, 0xff, 0xff, 0xff, 
0x03, 0x00, 0x00, 0x7e, 0xff, 0xff, 0xff, 0xfc, 0xf8, 0xfd, 0xff, 0x83, 0x00, 0x00, 0xfe, 0xff, 
0xff, 0xff, 0xfe, 0x00, 0x00, 0x83, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x3c, 0x00, 0x00, 0xfe, 0xff, 0xff, 
0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xfe, 0xfe, 0xfe, 0xef, 0x01, 0x00, 0x00, 0xff, 
0xff, 0xff, 0xf6, 0xf0, 0xf0, 0xff, 0xff, 0xfe, 0xf0, 0x80, 0x01, 0x0f, 0x7f, 0x9f, 0xf1, 0xfe, 
0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 
0x07, 0x06, 0x04, 0x00, 0x01, 0x01, 0x01, 0x01, 0x05, 0x07, 0x07, 0x07, 0x06, 0x04, 0x00, 0x03, 
0x03, 0x03, 0x00, 0x04, 0x06, 0x07, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 
0x00, 0x00, 0x00, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x04, 0x00, 0x00, 
0x01, 0x01, 0x05, 0x06, 0x00, 0x00, 0x00, 0x03, 0x07, 0x03, 0x00, 0x00, 0x00, 0x01, 0x07, 0x07, 
0x03, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 
0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x06, 0x04, 0x04, 0x00, 
0x01, 0x01, 0x01, 0x05, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x00, 0x04, 0x07, 0x07, 0x07, 
0x07, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03
};
/**************************************************/
/* 組込関数                                       */
void keyboard_post_init_user(void) {
    // すべてのLEDに同じ設定をする場合はこちらの関数
    //rgblight_sethsv(127, 255, 100);

}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

// キーを押す・離すタイミングで呼ばれる関数
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      count_type();
    }
    return true;
}

bool oled_task_user(void) {
    oled_write_raw_P(jeep, sizeof(jeep));
    //render_jeep();
    oled_write_P(PSTR("\n"), false);
    oled_write_layer_state();
    oled_write_lockstate();
    oled_write_uptime();
    oled_write_type_count();
    oled_write_wpm();

    oled_write_P(PSTR("\n"), false);
    //render_anim();
    return false;
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

void oled_write_layer_state(void) {
    // 現在のレイヤー番号を取得
    uint8_t layer = get_highest_layer(layer_state);

    // 「Layer:」の文字を表示
    oled_write_P(PSTR("Layer: "), false);

    // レイヤー番号を文字列として表示
    char layer_str[4]; // レイヤー番号を格納する文字列 (2桁まで対応)
    snprintf(layer_str, sizeof(layer_str), "%d", layer);
    oled_write(layer_str, false);  // 改行付きで表示

    switch(layer) {
        case 0:
        oled_write_ln_P(PSTR("(BASE)"), false);
        break;
    case 1:
        oled_write_ln_P(PSTR("(SHIFT)"), false);
        break;
    case 2:
        oled_write_ln_P(PSTR("(FN)"), false);
        break;
    case 3:
        oled_write_ln_P(PSTR("(WHEEL,CAPS)"), false);
        break; 
    case 4:
        oled_write_ln_P(PSTR("(RGB)"), false);
        break;
    default:
        oled_write_P(PSTR("(?)"), false);
        break;
    }

    oled_write_P(PSTR("\n"), false);
}

void oled_write_lockstate(void) {
    led_t led_usb_state = host_keyboard_led_state();
    oled_write_P(PSTR("CPSLK"), led_usb_state.caps_lock);
    oled_write_P(PSTR("  "), false);
    oled_write_P(PSTR("NUMLK"), led_usb_state.num_lock);
    oled_write_P(PSTR("  "), false);
    oled_write_ln_P(PSTR("SCLK"), led_usb_state.scroll_lock);
    oled_write_P(PSTR("\n"), false);
}

void oled_write_wpm(void) {
    static char wpm[5];
    oled_write_P(PSTR("WPM: "), false);
    itoa(get_current_wpm(), wpm, 10);
    oled_write_ln(wpm, false);
    oled_write_char('\n', false);
}

void render_keycodes_row(void) {
    uint8_t layer = get_highest_layer(layer_state);

    oled_set_cursor(0, 2);  // 2行目の先頭にカーソルを移動

    // 1行目 (1,0), (1,1), (1,2), (1,3)
    for (int col = 0; col < 4; col++) {
        // 動的にキーコードを取得
        uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = 1, .col = col});
        
        // キーコード名を取得して表示
        oled_write(keycode_to_name(keycode), false);  
        oled_write_P(PSTR(" "), false);  // スペースで区切り
    }

    oled_set_cursor(0, 3);  // 4行目の先頭にカーソルを移動

    // 2行目 (2,0), (2,1), (2,2), (2,3)
    for (int col = 0; col < 4; col++) {
        // 動的にキーコードを取得
        uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = 2, .col = col});
        
        // キーコード名を取得して表示
        oled_write(keycode_to_name(keycode), false);
        oled_write_P(PSTR(" "), false);  // スペースで区切り
    }
}

/**************************************************/
/* 打鍵数のカウント                                 */
static unsigned int type_count = 0;
void count_type(void) {
    type_count++;
}

void oled_write_type_count(void) {
    static char type_count_str[7];
    oled_write_P(PSTR("Type count: "), false);
    itoa(type_count, type_count_str, 10);
    oled_write_ln(type_count_str, false);
    oled_write_char('\n', false);
}

/**************************************************/
/* 起動時間                                       */
// write given digit, with leading zero if digit < 10
void oled_write_2digit(unsigned int digit) {
    static char buf[6];
    itoa(digit, buf, 10);
    if (digit < 10) {
        oled_write_char('0', false);
    }
    oled_write(buf, false);
}

void oled_write_uptime(void) {
    static uint32_t uptime_s;
    uptime_s = timer_read32() / 1000;
    oled_write_P(PSTR("Uptime "), false);
    // hour
    oled_write_2digit((uptime_s / 3600) % 60);
    oled_write_char(':', false);
    // minutes
    oled_write_2digit((uptime_s / 60) % 60);
    oled_write_char(':', false);
    // seconds
    oled_write_2digit(uptime_s % 60);
    oled_write_char('\n', false);
    oled_write_char('\n', false);
}
// EOF
  