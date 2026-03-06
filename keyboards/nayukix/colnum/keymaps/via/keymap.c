// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4      Col5          Col6         Col7        Col8     Col9     Col10    Col11    Col12    Col13  */
 /*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_LALT,     KC_UP,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
 /*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,    KC_COMM, KC_DOT,       KC_LALT,     KC_RALT,    KC_F,    KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
 /*Row2*/ KC_LCTL, KC_E,    KC_I,    KC_A,    KC_O,    KC_MINS,      KC_LEFT,     KC_RGHT,    KC_K,    KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
 /*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_SCLN,      KC_RALT,     KC_DOWN,    KC_D,    KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
 /*Row4*/ MO(1),    KC_F5,   KC_LGUI, KC_LALT, KC_SPC,  KC_NO,        KC_NO,       KC_NO,      KC_ENT,  KC_BSPC, KC_RGUI, MO(1),   MO(1),  KC_NO
    ),
    [1] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4      Col5          Col6         Col7        Col8     Col9     Col10    Col11    Col12    Col13  */
 /*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_LALT,     KC_UP,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
 /*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,    KC_COMM, KC_DOT,       KC_LALT,     KC_RALT,    KC_F,    KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
 /*Row2*/ KC_LCTL, KC_E,    KC_I,    KC_A,    KC_O,    KC_MINS,      KC_LEFT,     KC_RGHT,    KC_K,    KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
 /*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_SCLN,      KC_RALT,     KC_DOWN,    KC_D,    KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
 /*Row4*/ MO(1),    KC_F5,   KC_LGUI, KC_LALT, KC_SPC,  KC_NO,        KC_NO,       KC_NO,      KC_ENT,  KC_BSPC, KC_RGUI, MO(1),   MO(1),  KC_NO
    ),
    [2] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4      Col5          Col6         Col7        Col8     Col9     Col10    Col11    Col12    Col13  */
 /*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_LALT,     KC_UP,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
 /*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,    KC_COMM, KC_DOT,       KC_LALT,     KC_RALT,    KC_F,    KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
 /*Row2*/ KC_LCTL, KC_E,    KC_I,    KC_A,    KC_O,    KC_MINS,      KC_LEFT,     KC_RGHT,    KC_K,    KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
 /*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_SCLN,      KC_RALT,     KC_DOWN,    KC_D,    KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
 /*Row4*/ MO(1),    KC_F5,   KC_LGUI, KC_LALT, KC_SPC,  KC_NO,        KC_NO,       KC_NO,      KC_ENT,  KC_BSPC, KC_RGUI, MO(1),   MO(1),  KC_NO
    ),
  [3] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4      Col5          Col6         Col7        Col8     Col9     Col10    Col11    Col12    Col13  */
 /*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_LALT,     KC_UP,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
 /*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,    KC_COMM, KC_DOT,       KC_LALT,     KC_RALT,    KC_F,    KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
 /*Row2*/ KC_LCTL, KC_E,    KC_I,    KC_A,    KC_O,    KC_MINS,      KC_LEFT,     KC_RGHT,    KC_K,    KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
 /*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_SCLN,      KC_RALT,     KC_DOWN,    KC_D,    KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
 /*Row4*/ MO(1),    KC_F5,   KC_LGUI, KC_LALT, KC_SPC,  KC_NO,        KC_NO,       KC_NO,      KC_ENT,  KC_BSPC, KC_RGUI, MO(1),   MO(1),  KC_NO
    ),
   [4] = LAYOUT(
       /* Col0     Col1    Col2     Col3      Col4      Col5          Col6         Col7        Col8     Col9     Col10    Col11    Col12    Col13  */
 /*Row0*/ KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_LALT,     KC_UP,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_SLSH,
 /*Row1*/ KC_TAB,   KC_Q,    KC_L,    KC_U,    KC_COMM, KC_DOT,       KC_LALT,     KC_RALT,    KC_F,    KC_W,    KC_R,    KC_Y,    KC_P,    KC_BSLS,
 /*Row2*/ KC_LCTL, KC_E,    KC_I,    KC_A,    KC_O,    KC_MINS,      KC_LEFT,     KC_RGHT,    KC_K,    KC_T,    KC_N,    KC_S,    KC_H,    KC_QUOT,
 /*Row3*/ KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_SCLN,      KC_RALT,     KC_DOWN,    KC_D,    KC_D,    KC_M,    KC_J,    KC_B,    KC_RSFT,
 /*Row4*/ MO(1),    KC_F5,   KC_LGUI, KC_LALT, KC_SPC,  KC_NO,        KC_NO,       KC_NO,      KC_ENT,  KC_BSPC, KC_RGUI, MO(1),   MO(1),  KC_NO
    ),
};
