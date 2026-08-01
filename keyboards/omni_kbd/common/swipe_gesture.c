// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "swipe_gesture.h"

#include "config.h"
#include "config_omni.h"
#include "draw_custom.h"
#include "quantum.h"
#include "view_keymap.h"
#include "../drivers/cst816t.h"

enum {
    SWIPE_PROFILE_COUNT = 4,
};

typedef enum {
    SWIPE_MODIFIER_NONE,
    SWIPE_MODIFIER_CONTROL,
    SWIPE_MODIFIER_CONTROL_GUI,
} swipe_modifier_t;

typedef struct {
    const char *right_label;
    const char *left_label;
    const char *up_label;
    uint8_t active_indicator_x;
    uint8_t previous_indicator_x;
} swipe_view_config_t;

typedef struct {
    uint8_t right_keycode;
    uint8_t left_keycode;
    uint8_t up_keycode;
    swipe_modifier_t modifier;
} swipe_action_config_t;

static const swipe_view_config_t swipe_view_configs[SWIPE_PROFILE_COUNT] = {
    {.right_label = "NEXT", .left_label = "BACK", .up_label = "Save", .active_indicator_x = 90,  .previous_indicator_x = 150},
    {.right_label = "PG>",  .left_label = "<PG",  .up_label = "NEW",  .active_indicator_x = 110, .previous_indicator_x = 90},
    {.right_label = "VD>",  .left_label = "<VD",  .up_label = "VDNEW", .active_indicator_x = 130, .previous_indicator_x = 110},
    {.right_label = "CST1", .left_label = "CST2", .up_label = "CST3", .active_indicator_x = 150, .previous_indicator_x = 130},
};

static const swipe_action_config_t swipe_action_configs[SWIPE_PROFILE_COUNT] = {
    {.right_keycode = KC_Y,     .left_keycode = KC_Z,    .up_keycode = KC_S,   .modifier = SWIPE_MODIFIER_CONTROL},
    {.right_keycode = KC_PGDN,  .left_keycode = KC_PGUP, .up_keycode = KC_N,   .modifier = SWIPE_MODIFIER_CONTROL},
    {.right_keycode = KC_RIGHT, .left_keycode = KC_LEFT, .up_keycode = KC_D,   .modifier = SWIPE_MODIFIER_CONTROL_GUI},
    {.right_keycode = KC_F13,   .left_keycode = KC_F14,  .up_keycode = KC_F15, .modifier = SWIPE_MODIFIER_NONE},
};

static uint8_t current_swipe_profile = 0;

static void draw_background(painter_device_t display, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom) {
    qp_rect(display, left, top, right, bottom, hue_bg, sat_bg, val_bg, true);
}

void swipe_gesture_draw_main(painter_device_t display, painter_font_handle_t font, uint8_t current_layer) {
    const char *layer_text = get_layer_name(current_layer);
    uint16_t layer_text_width = qp_textwidth(font, layer_text);
    draw_background(display, 85, 110, 155, 130);
    qp_drawtext_recolor(display, 120 - layer_text_width / 2, 120 - font->line_height / 2, font, layer_text, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_flush(display);
}

void swipe_gesture_draw_base(painter_device_t display) {
    qp_donut(display, 120, 120, 117, 4, hue_main_color, sat_main_color, 50, val_main_color);
    qp_donut(display, 120, 120, 114, 4, hue_main_color, sat_main_color, val_main_color, 50);
    qp_circle(display, 90, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 110, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 130, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 150, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
}

static void draw_profile(painter_device_t display, painter_font_handle_t font, const swipe_view_config_t *view_config) {
    uint8_t right_text_width = qp_textwidth(font, view_config->right_label);
    uint8_t left_text_width = qp_textwidth(font, view_config->left_label);
    uint8_t up_text_width = qp_textwidth(font, view_config->up_label);
    uint8_t text_height = font->line_height;

    draw_background(display, 150, 120 - text_height / 2, 230, 120 + text_height / 2);
    draw_background(display, 10, 120 - text_height / 2, 90, 120 + text_height / 2);
    draw_background(display, 80, 40 - text_height / 2, 160, 40 + text_height / 2);
    qp_drawtext_recolor(display, 190 - right_text_width / 2, 120 - text_height / 2, font, view_config->right_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_drawtext_recolor(display, 50 - left_text_width / 2, 120 - text_height / 2, font, view_config->left_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_drawtext_recolor(display, 120 - up_text_width / 2, 40 - text_height / 2, font, view_config->up_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_circle(display, view_config->active_indicator_x, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, true);
    qp_circle(display, view_config->previous_indicator_x, 190, 4, hue_bg, sat_bg, val_bg, true);
    qp_flush(display);
}

void swipe_gesture_draw_profile(painter_device_t display, painter_font_handle_t font) {
    if (current_swipe_profile < SWIPE_PROFILE_COUNT) {
        draw_profile(display, font, &swipe_view_configs[current_swipe_profile]);
    }
}

static void tap_swipe_keycode(uint8_t keycode, swipe_modifier_t modifier) {
    if (modifier == SWIPE_MODIFIER_CONTROL || modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        register_code(KC_LCTL);
    }
    if (modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        register_code(KC_LGUI);
    }

    tap_code(keycode);

    if (modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        unregister_code(KC_LGUI);
    }
    if (modifier == SWIPE_MODIFIER_CONTROL || modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        unregister_code(KC_LCTL);
    }
}

void swipe_gesture_process(painter_device_t display, painter_font_handle_t font, uint8_t gesture_id) {
    if (current_swipe_profile >= SWIPE_PROFILE_COUNT) {
        return;
    }

    if (gesture_id == CST816S_SLIDE_DOWN) {
        current_swipe_profile = (current_swipe_profile + 1) % SWIPE_PROFILE_COUNT;
        swipe_gesture_draw_profile(display, font);
        return;
    }

    const swipe_action_config_t *action_config = &swipe_action_configs[current_swipe_profile];
    switch (gesture_id) {
        case CST816S_SLIDE_RIGHT:
            tap_swipe_keycode(action_config->right_keycode, action_config->modifier);
            break;
        case CST816S_SLIDE_LEFT:
            tap_swipe_keycode(action_config->left_keycode, action_config->modifier);
            break;
        case CST816S_SLIDE_UP:
            tap_swipe_keycode(action_config->up_keycode, action_config->modifier);
            break;
        default:
            break;
    }
}
