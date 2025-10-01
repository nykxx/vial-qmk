// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"

#define SCROLL_CPI 200
#define POINTING_CPI 800
#define SCROLL_DIVISOR 10

#define _DEFAULT 0
#define _PRICISE 1
#define _SCROLL  2
#define _EXTRA   3
#define _CONFIG  4

static bool set_scrolling = false;
static int16_t scroll_h_remaining = 0;
static int16_t scroll_v_remaining = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	
    [_DEFAULT] = LAYOUT(
        MS_BTN4,                MS_BTN5,
        MS_BTN1,    MS_BTN3,    MS_BTN2 
    ),

    [_PRICISE] = LAYOUT(
        KC_TRNS,               KC_TRNS,
        KC_TRNS,   KC_TRNS,    KC_TRNS
    ),

    [_SCROLL] = LAYOUT(
        KC_TRNS,               KC_TRNS,
        KC_TRNS,   KC_TRNS,    KC_TRNS
    ),

    [_EXTRA] = LAYOUT(
        KC_TRNS,               KC_TRNS,
        KC_TRNS,   KC_TRNS,    KC_TRNS
    ),

    [_CONFIG] = LAYOUT(
        KC_TRNS,               KC_TRNS,
        KC_TRNS,   KC_TRNS,    KC_TRNS
    )
};

void pointing_device_init_user(void) {
    //set_auto_mouse_layer(_MOUSE);
    //set_auto_mouse_enable(true);
}

layer_state_t default_layer_state_set_kb(layer_state_t state) {
    if (set_scrolling) {
        set_scrolling = false;
        pointing_device_set_cpi(POINTING_CPI);
    }
    return default_layer_state_set_user(state);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);
    if (highest_layer == _SCROLL) {
        set_scrolling = true;
        pointing_device_set_cpi(SCROLL_CPI);
    } else {
        if (set_scrolling) {
            set_scrolling = false;
            pointing_device_set_cpi(POINTING_CPI);
        }
    }

    return layer_state_set_user(state);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (set_scrolling) {
        int16_t h          = (int16_t)(mouse_report.x) + scroll_h_remaining;
        int16_t v          = (int16_t)(mouse_report.y) + scroll_v_remaining;
        mouse_report.h     = h / SCROLL_DIVISOR;
        scroll_h_remaining = h % SCROLL_DIVISOR;
        // invert Vertical
        mouse_report.v     = -(v / SCROLL_DIVISOR);
        scroll_v_remaining = v % SCROLL_DIVISOR;
        mouse_report.x     = 0;
        mouse_report.y     = 0;
    } else {
        scroll_h_remaining = 0;
        scroll_v_remaining = 0;
    }
    return pointing_device_task_user(mouse_report);
}