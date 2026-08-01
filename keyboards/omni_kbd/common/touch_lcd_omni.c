// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp.h"
#include "touch_lcd_omni.h"
#include "matrix.h"
#include "trackball_tuning.h"
#include "touch_input.h"
#include "touch_key_view.h"
#include "view_keymap.h"
#include "status_view.h"
#include "swipe_gesture.h"
#include "../drivers/cst816t.h"

enum {
    TOUCH_RELEASE_GRACE_MS = 30,
};

painter_device_t display;
uint8_t current_layer = 0;
painter_font_handle_t noto9_font;
painter_font_handle_t noto11_font;
painter_font_handle_t roboto_mono16;
painter_font_handle_t st2_mono16;
uint16_t touch_x = 0xFFFF;
uint16_t touch_y = 0xFFFF;
uint16_t touch_start_timer = 0;
bool touch_signal = false;
bool touch_start_flag = false; 
bool initial_touch_flag = false;
bool touch_signal_view_update = false;
uint8_t gesture_id = GESTURE_NONE;

display_mode_t display_mode = DISPLAY_MODE_TOUCH_KEY;

void draw_background_all(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, hue_bg, sat_bg, val_bg, true);  // HSV: H=0, S=0, V=0 (黒色)
}

void draw_background_all_black(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, 0, 0, 0, true);  // HSV: H=0, S=0, V=0 (黒色)
}

static void process_touch(void) {
    touch_signal = true;
    touch_signal_view_update = true;
}

typedef enum {
    TOUCH_MODE_NONE = 0,
    TOUCH_MODE_PRESS,
    TOUCH_MODE_SWIPE
} touch_mode_t;
static touch_mode_t touch_mode = TOUCH_MODE_NONE;

typedef enum {
    TOUCH_STATE_NONE = 0,
    TOUCH_STATE_START,
    TOUCH_STATE_WAIT,
    TOUCH_STATE_REPEAT,
    TOUCH_STATE_SINGLE
} touch_state_t;
static touch_state_t touch_state = TOUCH_STATE_NONE;

static uint16_t fast_touch_time = 0;
static uint16_t second_touch_time = 0;
static uint16_t touch_repeat_time = 0;
static uint16_t pre_touch_x;
static uint16_t pre_touch_y;
static uint16_t now_touch_x;
static uint16_t now_touch_y;

static void process_gesture(void){
    switch (display_mode) {    
    case DISPLAY_MODE_TOUCH_KEY:
        if (touch_mode == TOUCH_MODE_PRESS) {
            touch_x = now_touch_x;
            touch_y = now_touch_y;
            process_touch();
        } else if (touch_mode == TOUCH_MODE_SWIPE) {
            touch_key_view_handle_gesture(display, gesture_id);
        }
        break;

    case DISPLAY_MODE_TRACKBALL_TUNING:
        trackball_tuning_handle_touch(display, noto11_font, now_touch_x, now_touch_y);
        break;

    case DISPLAY_MODE_SWIPE_GESTURE:
        swipe_gesture_process(display, noto11_font, gesture_id);
        break;   

    case DISPLAY_MODE_STATUS1:
        ui_handle_touch(display, noto9_font, now_touch_x, now_touch_y);
        break;                    

    default:
        break;
    }
}

// add dedzone
static const uint8_t press_deadzone = 6;
static void touch_mode_detected(int16_t delta_x, int16_t delta_y){
    if (abs(delta_x) < press_deadzone && abs(delta_y) < press_deadzone) {
        touch_mode = TOUCH_MODE_PRESS;
    } else {
        touch_mode = TOUCH_MODE_SWIPE;
    }
}

static void swipe_gesture_id_detected(int16_t delta_x, int16_t delta_y){
    int16_t absolute_delta_x = delta_x;
    if (absolute_delta_x < 0) absolute_delta_x = -absolute_delta_x;
    int16_t absolute_delta_y = delta_y;
    if (absolute_delta_y < 0) absolute_delta_y = -absolute_delta_y;
    gesture_id = GESTURE_NONE;  
    if (absolute_delta_x > absolute_delta_y) {
        if (delta_x > 0) {
            gesture_id = CST816S_SLIDE_RIGHT;
        } else if (delta_x < 0) {
            gesture_id = CST816S_SLIDE_LEFT;
        }
    } else {
        if (delta_y > 0) {
            gesture_id = CST816S_SLIDE_DOWN;
        } else if (delta_y < 0) {
            gesture_id = CST816S_SLIDE_UP;
        }
    } 
}

static void start_touch_sequence(void) {
    cst816t_XY point = cst816t_Get_Point();
    pre_touch_x = point.x_point;
    pre_touch_y = point.y_point;
    fast_touch_time = timer_read();
    touch_state = TOUCH_STATE_START;
    uprintf("------START------\n");
}

static void detect_initial_touch(void) {
    if (timer_elapsed(fast_touch_time) <= touch_repeat_interval) {
        return;
    }

    cst816t_XY point = cst816t_Get_Point();
    now_touch_x = point.x_point;
    now_touch_y = point.y_point;
    second_touch_time = timer_read();

    int16_t delta_x = now_touch_x - pre_touch_x;
    int16_t delta_y = now_touch_y - pre_touch_y;
    uprintf("crood: %d %d\n", delta_x, delta_y);

    touch_mode_detected(delta_x, delta_y);
    swipe_gesture_id_detected(delta_x, delta_y);
    process_gesture();
    touch_state = TOUCH_STATE_SINGLE;
}

static void finish_single_touch(void) {
    if (timer_elapsed(second_touch_time) > touch_repeat_interval / 4) {
        touch_signal = false;
        touch_state = TOUCH_STATE_WAIT;
    }
}

static void start_touch_repeat(void) {
    if (timer_elapsed(second_touch_time) > touch_single_interval) {
        touch_state = TOUCH_STATE_REPEAT;
        touch_repeat_time = timer_read();
    }
}

static void repeat_touch(void) {
    if (timer_elapsed(touch_repeat_time) <= touch_repeat_interval) {
        return;
    }

    touch_repeat_time = timer_read();
    cst816t_XY point = cst816t_Get_Point();
    now_touch_x = point.x_point;
    now_touch_y = point.y_point;
    process_gesture();
}

static void process_active_touch(void) {
    switch (touch_state) {
        case TOUCH_STATE_NONE:
            start_touch_sequence();
            break;
        case TOUCH_STATE_START:
            detect_initial_touch();
            break;
        case TOUCH_STATE_SINGLE:
            finish_single_touch();
            break;
        case TOUCH_STATE_WAIT:
            start_touch_repeat();
            break;
        case TOUCH_STATE_REPEAT:
            repeat_touch();
            break;
    }
}

static void reset_released_touch(void) {
    if (timer_elapsed(fast_touch_time) <= touch_repeat_interval + TOUCH_RELEASE_GRACE_MS || touch_state == TOUCH_STATE_NONE) {
        return;
    }

    touch_signal = false;
    touch_mode = TOUCH_MODE_NONE;
    touch_state = TOUCH_STATE_NONE;
    gesture_id = GESTURE_NONE;
}

void process_touch_interrupt(void) {
    touch_input_task();

    if (touch_input_is_active()) {
        process_active_touch();
    } else {
        reset_released_touch();
    }
}

void display_redraw(void) {
    switch (display_mode) {
        case DISPLAY_MODE_TOUCH_KEY:
            draw_background_all_black();
            touch_key_view_draw(display);
            break;
        case DISPLAY_MODE_TRACKBALL_TUNING:
            trackball_tuning_draw(display, noto11_font);
            break;
        case DISPLAY_MODE_SWIPE_GESTURE:
            draw_background_all();
            swipe_gesture_draw_profile(display, noto11_font);
            swipe_gesture_draw_base(display);
            swipe_gesture_draw_main(display, noto11_font, touch_key_view_current_layer());
            break;
        case DISPLAY_MODE_KEY_MATRIX:
            draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
            break;
        case DISPLAY_MODE_STATUS1:
            status_view_init(display, noto9_font);
            break;
        default:
            break;
    }
}
