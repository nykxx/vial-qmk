// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp.h"
#include "touch_lcd_omni.h"
#include "matrix.h"
#include "trackball_tuning.h"
#include "touch_gesture.h"
#include "touch_key_view.h"
#include "view_keymap.h"
#include "status_view.h"
#include "swipe_gesture.h"

painter_device_t display;
uint8_t current_layer = 0;
painter_font_handle_t noto9_font;
painter_font_handle_t noto11_font;
painter_font_handle_t roboto_mono16;
painter_font_handle_t st2_mono16;
display_mode_t display_mode = DISPLAY_MODE_TOUCH_KEY;

void draw_background_all(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, hue_bg, sat_bg, val_bg, true);  // HSV: H=0, S=0, V=0 (黒色)
}

void draw_background_all_black(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, 0, 0, 0, true);  // HSV: H=0, S=0, V=0 (黒色)
}

static void process_touch_event(const touch_gesture_event_t *event) {
    switch (display_mode) {
        case DISPLAY_MODE_TOUCH_KEY:
            if (event->interaction == TOUCH_INTERACTION_PRESS) {
                touch_gesture_activate_key(event->x, event->y);
            } else if (event->interaction == TOUCH_INTERACTION_SWIPE) {
                touch_key_view_handle_gesture(display, event->gesture_id);
            }
            break;
        case DISPLAY_MODE_TRACKBALL_TUNING:
            trackball_tuning_handle_touch(display, noto11_font, event->x, event->y);
            break;
        case DISPLAY_MODE_SWIPE_GESTURE:
            swipe_gesture_process(display, noto11_font, event->gesture_id);
            break;
        case DISPLAY_MODE_STATUS1:
            ui_handle_touch(display, noto9_font, event->x, event->y);
            break;
        default:
            break;
    }
}

void process_touch_interrupt(void) {
    touch_gesture_task(process_touch_event);
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
