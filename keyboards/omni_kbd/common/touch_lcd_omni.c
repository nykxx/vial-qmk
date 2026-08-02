// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp.h"
#include "touch_lcd_omni.h"
#include "config.h"
#include "config_omni.h"
#include "matrix.h"
#include "trackball_tuning.h"
#include "touch_gesture.h"
#include "touch_key_view.h"
#include "view_keymap.h"
#include "status_view.h"
#include "swipe_gesture.h"

static display_mode_t current_display_mode = DISPLAY_MODE_TOUCH_KEY;

void draw_background_all(painter_device_t device) {
    qp_rect(device, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, hue_bg, sat_bg, val_bg, true);
}

void draw_background_all_black(painter_device_t device) {
    qp_rect(device, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, 0, 0, 0, true);
}

static void process_touch_event(const touch_gesture_event_t *event, const omni_display_context_t *context) {
    switch (current_display_mode) {
        case DISPLAY_MODE_TOUCH_KEY:
            if (event->interaction == TOUCH_INTERACTION_PRESS) {
                touch_gesture_activate_key(event->x, event->y);
            } else if (event->interaction == TOUCH_INTERACTION_SWIPE) {
                touch_key_view_handle_gesture(context->device, event->gesture_id);
            }
            break;
        case DISPLAY_MODE_TRACKBALL_TUNING:
            trackball_tuning_handle_touch(context->device, context->interface_font, event->x, event->y);
            break;
        case DISPLAY_MODE_SWIPE_GESTURE:
            swipe_gesture_process(context->device, context->interface_font, event->gesture_id);
            break;
        case DISPLAY_MODE_STATUS1:
            ui_handle_touch(context->device, context->status_font, event->x, event->y);
            break;
        default:
            break;
    }
}

void process_touch_interrupt(const omni_display_context_t *context) {
    touch_gesture_event_t event;
    if (touch_gesture_task(&event)) {
        process_touch_event(&event, context);
    }
}

display_mode_t display_get_mode(void) {
    return current_display_mode;
}

void display_set_mode(display_mode_t mode) {
    current_display_mode = mode;
}

void display_redraw(const omni_display_context_t *context, uint8_t current_layer) {
    switch (current_display_mode) {
        case DISPLAY_MODE_TOUCH_KEY:
            draw_background_all_black(context->device);
            touch_key_view_draw(context->device);
            break;
        case DISPLAY_MODE_TRACKBALL_TUNING:
            trackball_tuning_draw(context->device, context->interface_font);
            break;
        case DISPLAY_MODE_SWIPE_GESTURE:
            draw_background_all(context->device);
            swipe_gesture_draw_profile(context->device, context->interface_font);
            swipe_gesture_draw_base(context->device);
            swipe_gesture_draw_main(context->device, context->interface_font, touch_key_view_current_page());
            break;
        case DISPLAY_MODE_KEY_MATRIX:
            draw_key_matrix(context->device, context->keymap_font, context->symbol_font, current_layer);
            break;
        case DISPLAY_MODE_STATUS1:
            status_view_init(context->device, context->status_font);
            break;
        default:
            break;
    }
}
