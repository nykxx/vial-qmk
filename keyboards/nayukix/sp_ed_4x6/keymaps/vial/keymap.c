// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "keycode_utils.h"
#include "translate_ansi_to_jis.h"

#define BASE   0
#define MARK   1
#define FN     2
#define NUM    3
#define EX_1   4
#define EX_2   5
#define EX_3   6

enum custom_keycodes {
  TOG_JIS = SAFE_RANGE,
  TOG_TAKASHI
};

enum takashi_mode_define {
  NO_TAKASHI = 0,
  BLACK_TAKASHI,
  WHITE_TAKASHI
};
#define TOG_JIS QK_KB_0
#define TOG_TAKASHI QK_KB_1

static bool is_jis_mode = true;
static int takashi_mode = BLACK_TAKASHI;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE] = LAYOUT(
       /*                      LEFT HAND                        */          /*                    RIGHT HAND                           */
       /*    Col0     Col1    Col2     Col3      Col4         Col5           Col6       Col7     Col8      Col9    Col10    Col11  */
/* Row0  */ KC_ESC,   KC_1,    KC_2,    KC_3,     KC_4,       KC_5,          KC_6,      KC_7,    KC_8,     KC_9,   KC_0,   KC_SLSH,
/* Row1  */ KC_TAB,   KC_Q,    KC_L,    KC_U,     KC_COMM,    KC_DOT,        KC_F,      KC_W,    KC_R,     KC_Y,   KC_P,   KC_BSLS,
/* Row2  */ KC_LCTL,  KC_E,    KC_I,    KC_A,     KC_O,       KC_MINS,       KC_K,      KC_T,    KC_N,     KC_S,   KC_H,   KC_QUOT,
/* Row3  */ KC_LSFT,  KC_Z,    KC_X,    KC_C,     KC_V,       KC_SCLN,       KC_G,      KC_D,    KC_M,     KC_J,   KC_B,   KC_EQL,
/* Row4  */                             KC_LGUI,  KC_LALT,    KC_SPACE,      KC_ENTER,  KC_BSPC, MO(FN),
/*Encoder*/ _______,  _______
    ),
    [MARK] = LAYOUT(      
        /*                      LEFT HAND                                                */          /*                             RIGHT HAND                                   */
       /* Col0      Col1           Col2           Col3            Col4            Col5               Col6          Col7             Col8          Col9          Col10        Col11  */
/*Row0*/ KC_GRAVE,  LSFT(KC_1),    LSFT(KC_2),    LSFT(KC_3),     LSFT(KC_4),     LSFT(KC_5),       LSFT(KC_6),   LSFT(KC_7),     LSFT(KC_8), LSFT(KC_9),     LSFT(KC_0),   LSFT(KC_SLSH),
/*Row1*/ KC_TAB,    KC_MINS,       KC_LBRC,       KC_RBRC,        LSFT(KC_COMM),  LSFT(KC_DOT),     KC_LEFT,      KC_LEFT,        KC_DOWN,    KC_UP,          KC_RGHT,      LSFT(KC_QUOT), 
/*Row2*/ _______,   LSFT(KC_MINS), LSFT(KC_LBRC), LSFT(KC_RBRC),  LSFT(KC_9),     LSFT(KC_0),       KC_GRAVE,     LSFT(KC_GRAVE), KC_BSLS,    LSFT(KC_BSLS),  KC_EQL,       LSFT(KC_EQL), 
/*Row3*/ _______,   _______,       _______,       _______,        _______,        LSFT(KC_SCLN),    _______,      _______,        _______,    _______,        _______,      _______,
/*Row4*/                                          _______,        _______,        _______,          _______,      _______,        _______,
/*Encoder*/   _______,  _______
    ),
    [FN] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3       Col4      Col5          Col6        Col7        Col8        Col9       Col10      Col11   */
/*Row0*/ KC_ESC,   KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,       KC_F6,      KC_F7,      KC_F8,      KC_F9,     KC_F10,     KC_F11,
/*Row1*/ KC_TAB,  _______,   _______,   _______,   _______,   _______,     _______,    _______,    _______,    _______,   _______,    KC_F12, 
/*Row2*/ KC_CAPS, _______,   _______,   _______,   _______,   _______,     _______,    _______,    _______,    _______,   _______,    _______,
/*Row3*/ C(KC_S),  C(KC_Z),   C(KC_X),  C(KC_C),   C(KC_V),   G(KC_V),     _______,    _______,    _______,    _______,   _______,    LSG(KC_S), 
/*Row4*/                                _______,   _______,   _______,     _______,    _______,    _______, 
/*Encoder*/  _______,  _______
    ),
  [NUM] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                        */
       /* Col0     Col1       Col2       Col3      Col4       Col5          Col6       Col7      Col8      Col9      Col10     Col11  */
/*Row0*/ _______,  KC_1,      KC_2,     KC_3,      KC_4,      KC_5,         KC_6,      KC_7,     KC_8,     KC_9,     KC_0   ,  _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,      _______,   KC_4,     KC_5,     KC_6,     _______,  _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,      _______,   KC_1,     KC_2,     KC_3,     _______,  _______, 
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,      _______,   KC_0,     _______,  _______,  _______,  _______,
/*Row4*/                                _______,   _______,   _______,      _______,   _______,  _______,
/*Encoder*/  _______,  _______
    ),
   [EX_1] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7        Col8         Col9       Col10        Col11  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row4*/                                 _______,  _______,   _______,     _______,     _______,    _______,
/*Encoder*/   _______,     _______
    ),
   [EX_2] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7        Col8         Col9       Col10        Col11  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row4*/                                 _______,  _______,   _______,     _______,     _______,    _______,
/*Encoder*/   _______,     _______
    ),
   [EX_3] = LAYOUT(
       /*                      LEFT HAND                            */     /*                    RIGHT HAND                                    */
       /* Col0     Col1       Col2       Col3      Col4      Col5          Col6         Col7        Col8         Col9       Col10        Col11  */
/*Row0*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______,
/*Row1*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row2*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row3*/ _______,  _______,   _______,  _______,   _______,   _______,     _______,     _______,    _______,    _______,    _______,    _______, 
/*Row4*/                                 _______,  _______,   _______,     _______,     _______,    _______,
/*Encoder*/   _______,     _______
    ),
};


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [MARK] = { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [FN] =   { ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT) },
    [NUM] =  { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(KC_WH_L, KC_WH_R) },
    [EX_1] =   { ENCODER_CCW_CW(KC_MS_U, KC_MS_D), ENCODER_CCW_CW(KC_MS_L, KC_MS_R) },
    [EX_2] =   { ENCODER_CCW_CW(KC_MS_U, KC_MS_D), ENCODER_CCW_CW(KC_MS_L, KC_MS_R) },
    [EX_3] =   { ENCODER_CCW_CW(KC_MS_U, KC_MS_D), ENCODER_CCW_CW(KC_MS_L, KC_MS_R) },
    //                  Encoder 1                                     Encoder 2
};
#endif
// 'takashi56', 128x32px, white back
static const char PROGMEM takashi56_black[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xe0, 
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x60, 0x70, 0x70, 0x70, 0x70, 0x30, 0x30, 0x10, 
	0x10, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc0, 
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x40, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0xc1, 0xf9, 0xff, 0xff, 0x7f, 0x1f, 0x03, 0xc0, 0xe0, 0xf8, 0xfc, 0x1e, 0xfe, 0xfe, 0xfe, 0x84, 
	0x00, 0x00, 0xf0, 0xfe, 0xff, 0xff, 0xf7, 0xf8, 0x38, 0x1c, 0x1c, 0x0e, 0x06, 0x06, 0x03, 0x01, 
	0x00, 0x80, 0xc0, 0xf0, 0xfc, 0xbe, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x38, 0x7c, 0xfc, 
	0xfe, 0xee, 0xc7, 0xc7, 0xc3, 0x83, 0x0f, 0x0f, 0x07, 0xc0, 0xf8, 0xff, 0xff, 0xc7, 0xe0, 0xe0, 
	0xf0, 0xfe, 0x7e, 0x1e, 0x00, 0xf0, 0xfc, 0xfd, 0x3d, 0x01, 0x00, 0x00, 0xe0, 0xfc, 0xff, 0xff, 
	0xff, 0x73, 0x73, 0x71, 0x71, 0xf1, 0xf1, 0xe1, 0x81, 0x01, 0x80, 0xe0, 0xf0, 0xf8, 0x7c, 0x7e, 
	0x7f, 0x77, 0x73, 0x71, 0xf0, 0xe0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfe, 
	0x7f, 0x3f, 0x07, 0x61, 0x78, 0x7e, 0x3f, 0x0f, 0x07, 0x03, 0x03, 0xfb, 0x7f, 0x7f, 0x0f, 0x00, 
	0x78, 0x7f, 0x3f, 0x0f, 0x03, 0x03, 0x07, 0x0f, 0x1f, 0x3e, 0x78, 0x70, 0xc0, 0xc0, 0x72, 0x7e, 
	0x3f, 0x1f, 0x07, 0x03, 0x03, 0x03, 0x7f, 0x7f, 0x1f, 0x00, 0x00, 0x1c, 0x38, 0x70, 0x70, 0x70, 
	0x78, 0x38, 0x3d, 0x1f, 0x1f, 0x0f, 0x07, 0x70, 0x3e, 0x3f, 0x0f, 0x01, 0x01, 0x00, 0x3c, 0x1f, 
	0x0f, 0x03, 0x00, 0xf8, 0x7f, 0x3f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0xe0, 0xc0, 0xc0, 0xc0, 0xe0, 
	0xe0, 0xf0, 0xf0, 0x78, 0x7c, 0x3f, 0x1f, 0x0f, 0x03, 0x1e, 0x7f, 0xff, 0xff, 0xe1, 0xe0, 0xe0, 
	0xe0, 0xf0, 0x70, 0x7c, 0x3f, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x01, 
	0x10, 0x30, 0x30, 0x30, 0x38, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x1c, 
	0x1c, 0x1c, 0x1c, 0x1c, 0x1e, 0x1e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x06, 
	0x06, 0x06, 0x06, 0x06, 0x06, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x02, 0x02, 0x02, 
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'takashi56', 128x32px, black back
static const char PROGMEM takashi56_white[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 
	0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x9f, 0x8f, 0x8f, 0x8f, 0x8f, 0xcf, 0xcf, 0xef, 
	0xef, 0xff, 0xff, 0xff, 0x3f, 0x1f, 0x3f, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x3f, 
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xbf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 
	0x3e, 0x06, 0x00, 0x00, 0x80, 0xe0, 0xfc, 0x3f, 0x1f, 0x07, 0x03, 0xe1, 0x01, 0x01, 0x01, 0x7b, 
	0xff, 0xff, 0x0f, 0x01, 0x00, 0x00, 0x08, 0x07, 0xc7, 0xe3, 0xe3, 0xf1, 0xf9, 0xf9, 0xfc, 0xfe, 
	0xff, 0x7f, 0x3f, 0x0f, 0x03, 0x41, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc7, 0x83, 0x03, 
	0x01, 0x11, 0x38, 0x38, 0x3c, 0x7c, 0xf0, 0xf0, 0xf8, 0x3f, 0x07, 0x00, 0x00, 0x38, 0x1f, 0x1f, 
	0x0f, 0x01, 0x81, 0xe1, 0xff, 0x0f, 0x03, 0x02, 0xc2, 0xfe, 0xff, 0xff, 0x1f, 0x03, 0x00, 0x00, 
	0x00, 0x8c, 0x8c, 0x8e, 0x8e, 0x0e, 0x0e, 0x1e, 0x7e, 0xfe, 0x7f, 0x1f, 0x0f, 0x07, 0x83, 0x81, 
	0x80, 0x88, 0x8c, 0x8e, 0x0f, 0x1f, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x01, 
	0x80, 0xc0, 0xf8, 0x9e, 0x87, 0x81, 0xc0, 0xf0, 0xf8, 0xfc, 0xfc, 0x04, 0x80, 0x80, 0xf0, 0xff, 
	0x87, 0x80, 0xc0, 0xf0, 0xfc, 0xfc, 0xf8, 0xf0, 0xe0, 0xc1, 0x87, 0x8f, 0x3f, 0x3f, 0x8d, 0x81, 
	0xc0, 0xe0, 0xf8, 0xfc, 0xfc, 0xfc, 0x80, 0x80, 0xe0, 0xff, 0xff, 0xe3, 0xc7, 0x8f, 0x8f, 0x8f, 
	0x87, 0xc7, 0xc2, 0xe0, 0xe0, 0xf0, 0xf8, 0x8f, 0xc1, 0xc0, 0xf0, 0xfe, 0xfe, 0xff, 0xc3, 0xe0, 
	0xf0, 0xfc, 0xff, 0x07, 0x80, 0xc0, 0xf0, 0xfe, 0xff, 0xff, 0xff, 0x1f, 0x3f, 0x3f, 0x3f, 0x1f, 
	0x1f, 0x0f, 0x0f, 0x87, 0x83, 0xc0, 0xe0, 0xf0, 0xfc, 0xe1, 0x80, 0x00, 0x00, 0x1e, 0x1f, 0x1f, 
	0x1f, 0x0f, 0x8f, 0x83, 0xc0, 0xe0, 0xf0, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xfc, 0xfe, 
	0xef, 0xcf, 0xcf, 0xcf, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe3, 
	0xe3, 0xe3, 0xe3, 0xe3, 0xe1, 0xe1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf9, 
	0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf1, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9, 0xfd, 0xfd, 0xfd, 
	0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 
	0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// QMK Logo
static const char PROGMEM qmk_logo[] = {
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
};
/**************************************************/
/* プロトタイプ                                   */
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

// slaveでprocess_record_user発火させる
bool should_process_keypress(void) { return true; }

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
   if (is_keyboard_master()) {
    return OLED_ROTATION_270;
   } else {
    return OLED_ROTATION_180;
   }
}

// キーを押す・離すタイミングで呼ばれる関数
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      count_type();
    }

  switch (keycode) {
      case TOG_JIS:
        if (record->event.pressed) {
            is_jis_mode = !is_jis_mode;
        }
        return false;
      case TOG_TAKASHI:
         if (record->event.pressed) {
            takashi_mode++;
            if (takashi_mode > WHITE_TAKASHI) {
               takashi_mode = NO_TAKASHI;
            }
         }
         return false;
  }

   if (!is_jis_mode) {
    return true;
  }

  return process_record_user_a2j(keycode, record);
}

bool oled_task_user(void) {
   if (is_keyboard_master()) {
    oled_write_layer_state();
    oled_write_ln_P(PSTR("_____"), false);
    oled_write_lockstate();
    oled_write_ln_P(PSTR("-----"), false);
    oled_write_modstate();
    //oled_write_uptime();
   } else {
      if (takashi_mode == BLACK_TAKASHI) {
        oled_write_raw_P(takashi56_black, sizeof(takashi56_black));
      } else if (takashi_mode == WHITE_TAKASHI) {
        oled_write_raw_P(takashi56_white, sizeof(takashi56_white));
      } else {
        oled_write_P(qmk_logo, false);
      }
   }
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
            if(is_jis_mode){
                oled_write_ln_P(PSTR("JIS"), false);
            } else {
                oled_write_ln_P(PSTR("ANSI"), false);
            }
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
        case EX_1:
            oled_write_ln_P(PSTR("EX_1"), false);
            break;
        case EX_2:
            oled_write_ln_P(PSTR("EX_1"), false);
            break;
        case EX_3:
            oled_write_ln_P(PSTR("EX_1"), false);
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
  