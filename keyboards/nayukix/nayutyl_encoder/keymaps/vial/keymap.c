// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "keycode_utils.h"
//#include "bongo.h"

#define BASE 0
#define MARK 1
#define FN   2
#define NUM  3
#define EX   4

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE] = LAYOUT(
       /*                      LEFT HAND                        */     /*                    RIGHT HAND                           */
       /* Col0     Col1    Col2     Col3      Col4         Col5         Col6       Col7     Col8      Col9    Col10    Col11  */
/*Row0*/ KC_ESC,   KC_Q,    KC_L,    KC_U,     KC_COMM,    KC_DOT,      KC_F,      KC_W,    KC_R,     KC_Y,   KC_P,   KC_SLSH,
/*Row1*/ KC_LCTL,  KC_E,    KC_I,    KC_A,     KC_O,       KC_MINS,     KC_K,      KC_T,    KC_N,     KC_S,   KC_H,   KC_QUOT,
/*Row2*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,     KC_V,       KC_SCLN,     KC_G,      KC_D,    KC_M,     KC_J,   KC_B,   KC_EQL,
/*Row3*/                    KC_LGUI, KC_SPACE, KC_SPACE,                KC_ENTER,  KC_BSPC, TO(EX),
/*Row4*/                             KC_LSFT,  KC_LALT,                 KC_RALT,   KC_RSFT
    ),
    [MARK] = LAYOUT(      
        /*                      LEFT HAND                                                */          /*                               RIGHT HAND                                   */
       /* Col0      Col1           Col2           Col3            Col4            Col5               Col6            Col7             Col8          Col9          Col10        Col11  */
/*Row0*/ KC_GRAVE,  LSFT(KC_1),    LSFT(KC_2),    LSFT(KC_3),     LSFT(KC_4),     LSFT(KC_5),       LSFT(KC_6),     LSFT(KC_7),     LSFT(KC_8), LSFT(KC_9),     LSFT(KC_0),   LSFT(KC_SLSH),
/*Row1*/ KC_TAB,    KC_MINS,       KC_LBRC,       KC_RBRC,        LSFT(KC_COMM),  LSFT(KC_DOT),     KC_LEFT,        KC_LEFT,        KC_DOWN,    KC_UP,          KC_RGHT,      LSFT(KC_QUOT), 
/*Row2*/ _______,   LSFT(KC_MINS), LSFT(KC_LBRC), LSFT(KC_RBRC),  LSFT(KC_9),     LSFT(KC_0),       KC_GRAVE,       LSFT(KC_GRAVE), KC_BSLS,    LSFT(KC_BSLS),  KC_EQL,       LSFT(KC_EQL), 
/*Row3*/                                           _______,       _______,        LSFT(KC_SCLN),    _______,        KC_DEL,          _______,
/*Row4*/                                                          _______,        _______,          _______,        _______
    ),
    [FN] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3       Col4      Col5          Col6        Col7        Col8        Col9       Col10      Col11   */
/*Row0*/ KC_ESC,   KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,       KC_F6,      KC_F7,      KC_F8,      KC_F9,     KC_F10,     KC_F11,
/*Row1*/ KC_CAPS,  _______,   _______,  _______,   _______,   _______,     _______,    _______,    _______,    _______,    _______,   KC_F12, 
/*Row2*/ C(KC_S),  C(KC_Z),   C(KC_X),  C(KC_C),   C(KC_V),   G(KC_V),     _______,    _______,    _______,    _______,    _______,   LSG(KC_S), 
/*Row3*/                                _______,   _______,   _______,     _______,    _______,   _______,
/*Row4*/                                           _______,   _______,     _______,    _______
    ),
  [NUM] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                        */
       /* Col0     Col1       Col2       Col3      Col4       Col5          Col6       Col7      Col8     Col9    Col10    Col11  */
/*Row0*/ _______,  KC_1,      KC_2,     KC_3,      KC_4,      KC_5,         KC_6,      KC_7,     KC_8,    KC_9,   KC_0   ,  _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,      _______,   KC_4,     KC_5,    KC_6,   _______,  _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,      _______,   KC_1,     KC_2,    KC_3,   _______,  _______, 
/*Row3*/                                 _______,  _______,   _______,      _______,  _______,  _______,
/*Row4*/                                           _______,   _______,      _______,  _______
    ),
   [EX] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7        Col8         Col9       Col10        Col11  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row3*/                                 _______,  _______,   _______,     _______,     _______,    TO(BASE),
/*Row4*/                                           _______,   _______,     _______,     _______
    ),
};


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [MARK] = { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [FN] =   { ENCODER_CCW_CW(KC_DOWN, KC_UP), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [NUM] =  { ENCODER_CCW_CW(KC_WH_D, KC_WH_U), ENCODER_CCW_CW(KC_WH_L, KC_WH_R) },
    [EX] =   { ENCODER_CCW_CW(KC_MS_D, KC_MS_U), ENCODER_CCW_CW(KC_MS_L, KC_MS_R) }
    //                  Encoder 1                                     Encoder 2
};
#endif

/**************************************************/
/* プロトタイプ                                      */
void count_type(void);
void oled_write_type_count(void);
void oled_write_layer_state(void);
void oled_write_lockstate(void);
void oled_write_modstate(void);
void oled_write_wpm(void);
void oled_write_2digit(unsigned int digit);
void oled_write_uptime(void);
void render_keycodes_row(void);

/**************************************************/
/* 組込関数                                       */
void keyboard_post_init_user(void) {

}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

// キーを押す・離すタイミングで呼ばれる関数
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      count_type();
    }
    return true;
}

bool oled_task_user(void) {
    oled_write_layer_state();
    oled_write_ln_P(PSTR("_____"), false);
    oled_write_lockstate();
    oled_write_ln_P(PSTR("-----"), false);
    oled_write_modstate();
    //oled_write_uptime();

    //render_anim();
    return false;
}
/**************************************************/
/* My Func                                        */

void oled_write_layer_state(void) {
    // 現在のレイヤー番号を取得
    uint8_t layer = get_highest_layer(layer_state);
    // レイヤー名の表示
    switch(layer) {
        case BASE:
            oled_write_ln_P(PSTR("BASE"), false);
            break;
        case MARK:
            oled_write_ln_P(PSTR("MARK"), false);
            break;
        case FN:
            oled_write_ln_P(PSTR("FN"), false);
            break;
        case NUM:
            oled_write_ln_P(PSTR("NUM"), false);
            break; 
        case EX:
            oled_write_ln_P(PSTR("EX"), false);
            break;
        default:
            oled_write_P(PSTR("?"), false);
            break;
    }
}

void oled_write_lockstate(void) {
    led_t led_usb_state = host_keyboard_led_state();

    oled_write_ln_P(PSTR("CPSLK"), led_usb_state.caps_lock);

    oled_write_ln_P(PSTR("NUMLK"), led_usb_state.num_lock);

    oled_write_ln_P(PSTR("SCLK"), led_usb_state.scroll_lock);
}

void oled_write_modstate(void) {
    uint8_t mods = get_mods();
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    bool gui = false;
    if (mods & MOD_MASK_CTRL) {
      ctrl = true;
    }
    if (mods & MOD_MASK_SHIFT) {
      shift = true;
    }
    if (mods & MOD_MASK_ALT) {
      alt = true;
    }
    if (mods & MOD_MASK_GUI) {
      gui = true;
    }
     oled_write_ln_P(PSTR("CTRL"), ctrl);
     oled_write_ln_P(PSTR("SHFT"), shift);
     oled_write_ln_P(PSTR("ALT"), alt);
     oled_write_ln_P(PSTR("GUI"), gui);
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
  