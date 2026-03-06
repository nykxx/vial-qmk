// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "keycode_utils.h"
//#include "bongo.h"

#define NUMBER 0
#define CURSOR 1
#define MACRO1 2
#define MACRO2 3
#define MACRO3 4

enum custom_keycodes {
  TOG_KEYVIEW = SAFE_RANGE,
};

#define TOG_KEYVIEW QK_KB_0
static bool toggle_view = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [NUMBER] = LAYOUT(
       /* Col0     Col1    Col2     Col3      */
/*Row0*/ TO(MACRO3), TO(CURSOR),    
/*Row1*/ KC_EQL,   KC_7,   KC_8,    KC_9,
/*Row2*/ KC_MINS,  KC_4,   KC_5,    KC_6,
/*Row3*/ S(KC_8),  KC_1,   KC_2,    KC_3,
/*Row4*/ KC_SLSH,  KC_0,   KC_DOT,  S(KC_EQL)
    ),
    [CURSOR] = LAYOUT( 
       /* Col0     Col1    Col2     Col3      */
/*Row0*/ TO(NUMBER), TO(MACRO1),      
/*Row1*/ KC_PGUP,  KC_NO,   KC_NO,    KC_NO,
/*Row2*/ KC_PGDN,  KC_NO,   KC_NO,    KC_NO,
/*Row3*/ KC_HOME,  KC_NO,   KC_UP,    KC_NO,
/*Row4*/ KC_END,   KC_LEFT, KC_DOWN,  KC_RGHT
    ),
    [MACRO1] = LAYOUT(
       /* Col0     Col1    Col2     Col3      */
/*Row0*/ TO(CURSOR), TO(MACRO2),   
/*Row1*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row2*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row3*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row4*/ KC_NO,  KC_NO,   KC_NO,   KC_NO
    ),
  [MACRO2] = LAYOUT(
       /* Col0     Col1    Col2     Col3      */
/*Row0*/ TO(MACRO1), TO(MACRO3),    
/*Row1*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row2*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row3*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row4*/ KC_NO,  KC_NO,   KC_NO,   KC_NO
    ),
   [MACRO3] = LAYOUT(
       /* Col0     Col1    Col2     Col3      */
/*Row0*/ TO(MACRO2), TO(NUMBER),    
/*Row1*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row2*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row3*/ KC_NO,  KC_NO,   KC_NO,   KC_NO,
/*Row4*/ KC_NO,  KC_NO,   KC_NO,   KC_NO
    )
};


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(TOG_KEYVIEW, TOG_KEYVIEW)  },
    [1] =   { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(TOG_KEYVIEW, TOG_KEYVIEW)  },
    [2] =   { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(TOG_KEYVIEW, TOG_KEYVIEW)  },
    [3] =   { ENCODER_CCW_CW(KC_WH_D, KC_WH_U), ENCODER_CCW_CW(TOG_KEYVIEW, TOG_KEYVIEW)  },
    [4] =   { ENCODER_CCW_CW(KC_MS_D, KC_MS_U), ENCODER_CCW_CW(TOG_KEYVIEW, TOG_KEYVIEW)  }
    //                  Encoder 1                                     Encoder 2
};
#endif



/**************************************************/
/* プロトタイプ                                      */
void count_type(void);
void oled_write_type_count(void);
void oled_write_layer_state(bool lower);
void oled_write_lockstate(void);
void oled_write_modstate(void);
void oled_write_wpm(void);
void oled_write_2digit(unsigned int digit);
void oled_write_uptime(void);
void render_keycodes_row(bool next);

/**************************************************/
/* 組込関数                                       */
void keyboard_post_init_user(void) {

}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;
}

// キーを押す・離すタイミングで呼ばれる関数
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      count_type();
    }

      switch (keycode) {
      case TOG_KEYVIEW:
        if (record->event.pressed) {
            toggle_view = !toggle_view;
        }
        return false;
  }
    return true;
}

bool oled_task_user(void) {
    oled_write_layer_state(toggle_view);
    render_keycodes_row(toggle_view);
    //oled_write_ln_P(PSTR("------------------"), false);
    //oled_write_lockstate();
    //oled_write_ln_P(PSTR("------------------"), false);
    //oled_write_modstate();
    //oled_write_uptime();

    //render_anim();
    return false;
}
/**************************************************/
/* My Func                                        */

void oled_write_layer_state(bool lower) {
    oled_write_P(PSTR("LAYER: "), false);
    // 現在のレイヤー番号を取得
    uint8_t layer = get_highest_layer(layer_state);
    // レイヤー名の表示
    switch(layer) {
        case NUMBER:
            oled_write_P(PSTR("NUMBER "), false);
            break;
        case CURSOR:
            oled_write_P(PSTR("CURSOR "), false);
            break;
        case MACRO1:
            oled_write_P(PSTR("MACRO1 "), false);
            break;
        case MACRO2:
            oled_write_P(PSTR("MACRO2 "), false);
            break; 
        case MACRO3:
            oled_write_P(PSTR("MACRO3 "), false);
            break;
        default:
            oled_write_P(PSTR("?"), false);
            break;
    }
    if (lower) {
        oled_write_ln_P(PSTR("(lower)"), false);
    } else {
        oled_write_ln_P(PSTR("(upper)"), false);
    }
}

void oled_write_lockstate(void) {
    led_t led_usb_state = host_keyboard_led_state();

    oled_write_P(PSTR("CPSLK"), led_usb_state.caps_lock);
    oled_write_P(PSTR("  "), false);
    oled_write_P(PSTR("NUMLK"), led_usb_state.num_lock);
    oled_write_P(PSTR("  "), false);
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
     oled_write_P(PSTR("CTRL"), ctrl);
     oled_write_P(PSTR("  "), false);
     oled_write_P(PSTR("SHFT"), shift);
     oled_write_P(PSTR("  "), false);
     oled_write_P(PSTR("ALT"), alt);
     oled_write_P(PSTR("  "), false);
     oled_write_ln_P(PSTR("GUI"), gui);
}

void oled_write_wpm(void) {
    static char wpm[5];
    oled_write_P(PSTR("WPM: "), false);
    itoa(get_current_wpm(), wpm, 10);
    oled_write_ln(wpm, false);
    oled_write_char('\n', false);
}

void render_keycodes_row(bool next) {
    uint8_t layer = get_highest_layer(layer_state);
    uint8_t offset = 0;
    if (next) {
      offset = 2;
    }
    oled_set_cursor(0, 2);  // 2行目の先頭にカーソルを移動

    // 1行目 (1,0), (1,1), (1,2), (1,3)
    for (int col = 0; col < 4; col++) {
        // 動的にキーコードを取得
        uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = 1 + offset, .col = col});
        
        // キーコード名を取得して表示
        oled_write(keycode_to_name(keycode), false);  
        oled_write_P(PSTR(" "), false);  // スペースで区切り
    }

    oled_set_cursor(0, 3);  // 4行目の先頭にカーソルを移動

    // 2行目 (2,0), (2,1), (2,2), (2,3)
    for (int col = 0; col < 4; col++) {
        // 動的にキーコードを取得
        uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = 2 + offset, .col = col});
        
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
  