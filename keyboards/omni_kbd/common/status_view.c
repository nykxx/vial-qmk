// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "os_detection.h"
#include "status_view.h"
#include "draw_custom.h"
#include "config_omni.h"
#include "omni_layers.h"
#include "touch_gesture.h"
#include "touch_lcd_omni.h"
#include "trackball_omni.h"

enum {
    STATUS_BUTTON_WIDTH          = 52,
    STATUS_BUTTON_HEIGHT         = 26,
    STATUS_BUTTON_RADIUS         = 6,
    STATUS_BAR_LEFT              = 90,
    STATUS_BAR_RIGHT             = 190,
    STATUS_UPPER_BAR_Y           = 175,
    STATUS_LOWER_BAR_Y           = 200,
    STATUS_AUTO_OS_BUTTON_X      = 90,
    STATUS_MANUAL_OS_BUTTON_X    = 150,
    STATUS_LEFT_TOGGLE_X         = 50,
    STATUS_CENTER_TOGGLE_X       = 120,
    STATUS_RIGHT_TOGGLE_X        = 190,
    STATUS_TOP_ROW_Y             = 35,
    STATUS_FIRST_TOGGLE_ROW_Y    = 70,
    STATUS_SECOND_TOGGLE_ROW_Y   = 105,
    STATUS_DETAIL_TEXT_Y         = 130,
    STATUS_DETAIL_TEXT_BOTTOM    = 150,
    STATUS_DETAIL_CONTROLS_TOP   = 160,
    STATUS_DIVIDER_Y             = 155,
    STATUS_DISPLAY_SIZE          = 240,
    STATUS_ACTIVE_BRIGHTNESS     = 190,
};

typedef enum {
    STATUS_PROFILE_BASE = 0,
    STATUS_PROFILE_SUB,
    STATUS_PROFILE_COUNT,
} status_profile_t;

typedef enum {
    STATUS_TOGGLE_VERTICAL_SCROLL = 0,
    STATUS_TOGGLE_HORIZONTAL_SCROLL,
    STATUS_TOGGLE_SCROLL_INVERTED,
    STATUS_TOGGLE_AUTO_MOUSE,
    STATUS_TOGGLE_TOUCH_REPEAT,
    STATUS_TOGGLE_COUNT,
} status_toggle_id_t;

typedef enum {
    STATUS_BAR_NONE = 0,
    STATUS_BAR_UPPER_ONLY,
    STATUS_BAR_UPPER_AND_LOWER,
} status_bar_mode_t;

typedef struct {
    uint16_t left;
    uint16_t top;
    uint16_t right;
    uint16_t bottom;
} status_rect_t;

static inline uint8_t clamp_u8(int16_t v, uint8_t lo, uint8_t hi) {
    if (v < lo) {
        v = lo;
    }
    if (v > hi) {
        v = hi;
    }
    return (uint8_t)v;
}
static inline uint8_t clamp_bar_x(int16_t x) {
    if (x < STATUS_BAR_LEFT) {
        x = STATUS_BAR_LEFT;
    }
    if (x > STATUS_BAR_RIGHT) {
        x = STATUS_BAR_RIGHT;
    }
    return (uint8_t)x;
}
static inline uint8_t clamp_parameter_value(int16_t value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 100) {
        value = 100;
    }
    return (uint8_t)value;
}

static inline os_variant_t current_os_variant(void) {
    return is_on_aos ? detected_host_os() : manual_os;
}

static inline uint8_t layer_for_os(os_variant_t os) {
    /*
     * The original firmware selected a dedicated macOS layer here. That layer
     * was removed when the keymap was reduced, so every OS currently falls
     * back to _BASE. Keep the OS cases explicit as the restoration point when
     * a dedicated layer and the corresponding Vial EEPROM budget return.
     */
    switch (os) {
        case OS_MACOS:
        case OS_IOS:
            return _BASE;

        case OS_WINDOWS:
        case OS_LINUX:
        case OS_UNSURE:
        default:
            return _BASE;
    }
}

static status_rect_t status_button_rect(uint16_t center_x, uint16_t center_y) {
    return (status_rect_t){
        .left   = center_x - STATUS_BUTTON_WIDTH / 2,
        .top    = center_y - STATUS_BUTTON_HEIGHT / 2,
        .right  = center_x + STATUS_BUTTON_WIDTH / 2,
        .bottom = center_y + STATUS_BUTTON_HEIGHT / 2,
    };
}

static bool point_in_rect(int16_t x, int16_t y, status_rect_t rect) {
    return x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom;
}

static const char *const os_name_tbl[] = {
    [OS_UNSURE]  = "???",
    [OS_LINUX]   = "Lnx",
    [OS_WINDOWS] = "Win",
    [OS_MACOS]   = "Mac",
    [OS_IOS]     = "Mac", // 誤判定があるからiOSはMac扱い
};

static const char *os_name_short(os_variant_t os) {
    if (!is_on_aos) {
        return (get_highest_layer(default_layer_state) == _BASE) ? "Sub" : "Bas";
    }
    if (os < 0 || os >= 5) return "???";
    return os_name_tbl[os];
}

typedef struct {
    const char *label;
    uint16_t    center_x;
    uint16_t    center_y;
    status_bar_mode_t bar_mode;
    const char *bar_upper;
    const char *bar_lower;
    const char *summary;
} status_toggle_config_t;

static const status_toggle_config_t toggle_configs[STATUS_TOGGLE_COUNT] = {
    [STATUS_TOGGLE_VERTICAL_SCROLL] = {
        .label       = "HRV",
        .center_x    = STATUS_LEFT_TOGGLE_X,
        .center_y    = STATUS_FIRST_TOGGLE_ROW_Y,
        .bar_mode    = STATUS_BAR_UPPER_AND_LOWER,
        .bar_upper   = "RES",
        .bar_lower   = "VAL",
        .summary     = "Vertical hi-res scroll",
    },
    [STATUS_TOGGLE_HORIZONTAL_SCROLL] = {
        .label       = "HRH",
        .center_x    = STATUS_CENTER_TOGGLE_X,
        .center_y    = STATUS_FIRST_TOGGLE_ROW_Y,
        .bar_mode    = STATUS_BAR_UPPER_AND_LOWER,
        .bar_upper   = "RES",
        .bar_lower   = "VAL",
        .summary     = "Horizontal hi-res scroll",
    },
    [STATUS_TOGGLE_SCROLL_INVERTED] = {
        .label       = "SCI",
        .center_x    = STATUS_RIGHT_TOGGLE_X,
        .center_y    = STATUS_FIRST_TOGGLE_ROW_Y,
        .bar_mode    = STATUS_BAR_NONE,
        .bar_upper   = "---",
        .bar_lower   = "---",
        .summary     = "Invert scroll",
    },
    [STATUS_TOGGLE_AUTO_MOUSE] = {
        .label       = "AML",
        .center_x    = STATUS_LEFT_TOGGLE_X,
        .center_y    = STATUS_SECOND_TOGGLE_ROW_Y,
        .bar_mode    = STATUS_BAR_NONE,
        .bar_upper   = "---",
        .bar_lower   = "---",
        .summary     = "Auto mouse layer",
    },
    [STATUS_TOGGLE_TOUCH_REPEAT] = {
        .label       = "TRP",
        .center_x    = STATUS_CENTER_TOGGLE_X,
        .center_y    = STATUS_SECOND_TOGGLE_ROW_Y,
        .bar_mode    = STATUS_BAR_UPPER_AND_LOWER,
        .bar_upper   = "HLD",
        .bar_lower   = "RPT",
        .summary     = "Touch repeat interval",
    },
};

static uint8_t profile_toggle_states[STATUS_PROFILE_COUNT][STATUS_TOGGLE_COUNT];
static uint8_t profile_upper_bar_positions[STATUS_PROFILE_COUNT][STATUS_TOGGLE_COUNT];
static uint8_t profile_lower_bar_positions[STATUS_PROFILE_COUNT][STATUS_TOGGLE_COUNT];
static int8_t selected_toggle_index = -1;
static uint8_t active_default_layer = _BASE;

static status_profile_t current_profile(void) {
    /* Preserve the existing EEPROM profile mapping. */
    return active_default_layer == _BASE ? STATUS_PROFILE_SUB : STATUS_PROFILE_BASE;
}

static bool current_profile_vertical_scroll_enabled(void) {
    return profile_toggle_states[current_profile()][STATUS_TOGGLE_VERTICAL_SCROLL] != 0;
}
static bool current_profile_horizontal_scroll_enabled(void) {
    return profile_toggle_states[current_profile()][STATUS_TOGGLE_HORIZONTAL_SCROLL] != 0;
}

static void apply_toggle_parameter(status_toggle_id_t toggle_id, bool is_upper_bar, uint8_t parameter_value) {

    switch (toggle_id) {
        case STATUS_TOGGLE_VERTICAL_SCROLL:
            if (is_upper_bar) {
                if (current_profile_vertical_scroll_enabled()) {
                    trackball_set_vertical_scroll_interval(clamp_parameter_value(parameter_value));
                } else {
                    trackball_set_vertical_scroll_interval(100);
                }
            } else {
                if (current_profile_vertical_scroll_enabled()) {
                    trackball_set_vertical_scroll_value(clamp_parameter_value(parameter_value));
                } else {
                    if (active_default_layer == 0) {
                        trackball_set_vertical_scroll_value(1);
                    } else if (active_default_layer == 1) {
                        trackball_set_vertical_scroll_value(100);
                    }
                }
            }
            break;

        case STATUS_TOGGLE_HORIZONTAL_SCROLL:
            if (is_upper_bar) {
                if (current_profile_horizontal_scroll_enabled()) {
                    trackball_set_horizontal_scroll_interval(clamp_parameter_value(parameter_value));
                } else {
                    trackball_set_horizontal_scroll_interval(100);
                }
            } else {
                if (current_profile_horizontal_scroll_enabled()) {
                    trackball_set_horizontal_scroll_value(clamp_parameter_value(parameter_value));
                } else {
                    if (active_default_layer == 0) {
                        trackball_set_horizontal_scroll_value(1);
                    } else if (active_default_layer == 1) {
                        trackball_set_horizontal_scroll_value(100);
                    }
                }
            }
            break;

        case STATUS_TOGGLE_SCROLL_INVERTED:
            trackball_set_scroll_inverted(profile_toggle_states[current_profile()][STATUS_TOGGLE_SCROLL_INVERTED] != 0);
            break;

        case STATUS_TOGGLE_AUTO_MOUSE:
            if (profile_toggle_states[current_profile()][STATUS_TOGGLE_AUTO_MOUSE]) {
                set_auto_mouse_enable(true);
            } else {
                set_auto_mouse_enable(false);
            }
            break;
        case STATUS_TOGGLE_TOUCH_REPEAT:
            if (is_upper_bar) {
                touch_gesture_set_hold_interval(150 + (parameter_value * (1500 - 150) / 100));
            } else {
                touch_gesture_set_repeat_interval(40 + (parameter_value * (150 - 40) / 100));
            }
            break;
        default:
            break;
    }
}

static void draw_auto_os_button(painter_device_t display, painter_font_handle_t font, uint8_t hue, uint8_t saturation, uint8_t value) {
    const char *label = "OS";
    const bool  state = is_on_aos;
    /* Existing behavior: every redraw synchronizes this value to storage. */
    omni_status_save_toggle_aos(state);

    const uint16_t center_x = STATUS_AUTO_OS_BUTTON_X;
    const uint16_t center_y = STATUS_TOP_ROW_Y;
    const status_rect_t button_rect = status_button_rect(center_x, center_y);

    const uint8_t main_value_on  = clamp_u8(value, 160, 255);
    const uint8_t main_value_off = clamp_u8(value - 80, 80, 255);
    const uint8_t background_value_on  = clamp_u8(value - 110, 50, 255);
    const uint8_t background_value_off = clamp_u8(value - 140, 30, 255);

    const uint8_t text_x = (uint8_t)(center_x - qp_textwidth(font, label) / 2);
    const uint8_t text_y = (uint8_t)(center_y - font->line_height / 2 + 2);

    if (state) {
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, background_value_on, true, 0);
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, main_value_on, false, 1);
        qp_drawtext_recolor(display, text_x, text_y, font, label, hue, saturation, main_value_on, hue, saturation, background_value_on);
    } else {
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, background_value_off, true, 0);
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, main_value_off, false, 1);
        qp_drawtext_recolor(display, text_x, text_y, font, label, hue, saturation, main_value_off, hue, saturation, background_value_off);
    }
}

static void draw_manual_os_button(painter_device_t display, painter_font_handle_t font, uint8_t hue, uint8_t saturation, uint8_t value) {
    const os_variant_t current_os = current_os_variant();
    const char *label = os_name_short(current_os);

    /* Existing behavior: every redraw synchronizes this value to storage. */
    omni_status_save_toggle_osc(active_default_layer);

    const uint16_t center_x = STATUS_MANUAL_OS_BUTTON_X;
    const uint16_t center_y = STATUS_TOP_ROW_Y;
    const status_rect_t button_rect = status_button_rect(center_x, center_y);

    const uint8_t main_value = clamp_u8(value, 160, 255);
    const uint8_t background_value = clamp_u8(value - 110, 50, 255);

    const uint8_t text_x = (uint8_t)(center_x - qp_textwidth(font, label) / 2);
    const uint8_t text_y = (uint8_t)(center_y - font->line_height / 2 + 2);

    qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, background_value, true, 0);
    qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, main_value, false, 1);
    qp_drawtext_recolor(display, text_x, text_y, font, label, hue, saturation, main_value, hue, saturation, background_value);
}

static void draw_toggle_button(status_toggle_id_t toggle_id, painter_device_t display, painter_font_handle_t font, uint8_t hue, uint8_t saturation, uint8_t value) {
    const status_toggle_config_t *toggle_config = &toggle_configs[toggle_id];
    const status_profile_t profile = current_profile();
    const bool state = profile_toggle_states[profile][toggle_id] != 0;

    /* Existing behavior: every redraw synchronizes this value to storage. */
    omni_status_save_toggle_normal(profile, toggle_id, state);

    const status_rect_t button_rect = status_button_rect(toggle_config->center_x, toggle_config->center_y);
    const uint8_t main_value_on  = clamp_u8(value, 160, 255);
    const uint8_t main_value_off = clamp_u8(value - 80, 80, 255);
    const uint8_t background_value_on  = clamp_u8(value - 110, 50, 255);
    const uint8_t background_value_off = clamp_u8(value - 140, 30, 255);

    const uint8_t text_x = (uint8_t)(toggle_config->center_x - qp_textwidth(font, toggle_config->label) / 2);
    const uint8_t text_y = (uint8_t)(toggle_config->center_y - font->line_height / 2 + 2);

    if (state) {
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, background_value_on, true, 0);
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, main_value_on, false, 1);
        qp_drawtext_recolor(display, text_x, text_y, font, toggle_config->label, hue, saturation, main_value_on, hue, saturation, background_value_on);
    } else {
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, background_value_off, true, 0);
        qp_round_rect(display, button_rect.left, button_rect.top, button_rect.right, button_rect.bottom, STATUS_BUTTON_RADIUS, hue, saturation, main_value_off, false, 1);
        qp_drawtext_recolor(display, text_x, text_y, font, toggle_config->label, hue, saturation, main_value_off, hue, saturation, background_value_off);
    }
}

static void draw_parameter_bar(painter_device_t display, uint16_t y, uint8_t hue, uint8_t saturation, uint8_t current_x, painter_font_handle_t font, uint8_t line_height, const char *label) {
    qp_round_rect(display, STATUS_BAR_LEFT, y - 2, STATUS_BAR_RIGHT, y + 2, 2, hue, saturation, 90, true, 0);
    qp_round_rect(display, STATUS_BAR_LEFT, y - 2, current_x, y + 2, 2, hue, saturation, 180, true, 0);
    qp_drawtext_recolor(display, STATUS_BAR_LEFT - 45, y - line_height, font, label, hue, saturation, 180, 0, 0, 0);
}

static void redraw_status_view(painter_device_t display, painter_font_handle_t font) {
    qp_line(display, 0, STATUS_DIVIDER_Y, STATUS_DISPLAY_SIZE, STATUS_DIVIDER_Y, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
    draw_auto_os_button(display, font, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
    draw_manual_os_button(display, font, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
    for (status_toggle_id_t toggle_id = 0; toggle_id < STATUS_TOGGLE_COUNT; toggle_id++) {
        draw_toggle_button(toggle_id, display, font, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
    }
}

static void apply_current_profile_parameters(void) {
    for (status_toggle_id_t toggle_id = 0; toggle_id < STATUS_TOGGLE_COUNT; toggle_id++) {
        apply_toggle_parameter(toggle_id, true, omni_status_load_parameter_value(active_default_layer, true, toggle_id));
        apply_toggle_parameter(toggle_id, false, omni_status_load_parameter_value(active_default_layer, false, toggle_id));
    }
}

void status_view_load_persistent_state(void) {
    is_on_aos = omni_status_load_toggle_aos();
    manual_os = omni_status_load_toggle_osc();

    if (!is_on_aos) {
        active_default_layer = manual_os;
        default_layer_set(1UL << active_default_layer);
    } else {
        const os_variant_t detected_os = detected_host_os();
        active_default_layer = layer_for_os(detected_os);
        default_layer_set(1UL << active_default_layer);
    }
    for (status_toggle_id_t toggle_id = 0; toggle_id < STATUS_TOGGLE_COUNT; toggle_id++) {
        profile_toggle_states[active_default_layer][toggle_id] = omni_status_load_toggle_normal(active_default_layer, toggle_id);
        profile_upper_bar_positions[active_default_layer][toggle_id] = omni_status_load_bar(active_default_layer, true, toggle_id);
        profile_lower_bar_positions[active_default_layer][toggle_id] = omni_status_load_bar(active_default_layer, false, toggle_id);
    }
    apply_current_profile_parameters();
}

static void clear_status_detail_area(painter_device_t display) {
    qp_rect(display, 0, STATUS_DETAIL_TEXT_Y, STATUS_DISPLAY_SIZE, STATUS_DETAIL_TEXT_BOTTOM, 0, 0, 0, true);
    qp_rect(display, 0, STATUS_DETAIL_CONTROLS_TOP, STATUS_DISPLAY_SIZE, STATUS_DISPLAY_SIZE, 0, 0, 0, true);
}

static void draw_status_detail_message(painter_device_t display, painter_font_handle_t font, const char *message) {
    const uint16_t text_x = STATUS_DISPLAY_SIZE / 2 - qp_textwidth(font, message) / 2;
    qp_drawtext_recolor(display, text_x, STATUS_DETAIL_TEXT_Y, font, message, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS, 0, 0, 0);
}

static bool handle_auto_os_button_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y) {
    if (!point_in_rect(touch_x, touch_y, status_button_rect(STATUS_AUTO_OS_BUTTON_X, STATUS_TOP_ROW_Y))) {
        return false;
    }

    is_on_aos = !is_on_aos;
    if (is_on_aos) {
        const os_variant_t detected_os = detected_host_os();
        default_layer_set(1UL << layer_for_os(detected_os));
    }
    active_default_layer = get_highest_layer(default_layer_state);
    apply_current_profile_parameters();

    selected_toggle_index = -1;
    clear_status_detail_area(display);
    draw_status_detail_message(display, font, "Automatic OS detection");
    omni_status_save_toggle_aos(is_on_aos);
    redraw_status_view(display, font);
    return true;
}

static bool handle_manual_os_button_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y) {
    if (!point_in_rect(touch_x, touch_y, status_button_rect(STATUS_MANUAL_OS_BUTTON_X, STATUS_TOP_ROW_Y))) {
        return false;
    }

    if (!is_on_aos) {
        /* Preserve the existing behavior: this control does not cycle layers and always selects _BASE. */
        const uint8_t next_layer = _BASE;
        default_layer_set(1UL << next_layer);
        active_default_layer = next_layer;
        apply_current_profile_parameters();

        selected_toggle_index = -1;
        clear_status_detail_area(display);
        const char *message = (next_layer == _BASE) ? "Keymap Change Base" : "Keymap Change Sub";
        draw_status_detail_message(display, font, message);
        omni_status_save_toggle_osc(active_default_layer);
        redraw_status_view(display, font);
    } else {
        redraw_status_view(display, font);
    }
    return true;
}

static bool handle_toggle_button_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y) {
    const uint8_t label_line_height = (uint8_t)(font->line_height / 2);

    for (status_toggle_id_t toggle_id = 0; toggle_id < STATUS_TOGGLE_COUNT; toggle_id++) {
        const status_toggle_config_t *toggle_config = &toggle_configs[toggle_id];
        const status_rect_t toggle_rect = status_button_rect(toggle_config->center_x, toggle_config->center_y);
        if (!point_in_rect(touch_x, touch_y, toggle_rect)) {
            continue;
        }

        const status_profile_t profile = current_profile();
        profile_toggle_states[profile][toggle_id] ^= 1;
        const bool is_enabled = profile_toggle_states[profile][toggle_id] != 0;
        clear_status_detail_area(display);

        if (!is_enabled) {
            selected_toggle_index = -1;
            draw_toggle_button(toggle_id, display, font, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
            apply_toggle_parameter(toggle_id, true, omni_status_load_parameter_value(active_default_layer, true, toggle_id));
            apply_toggle_parameter(toggle_id, false, omni_status_load_parameter_value(active_default_layer, false, toggle_id));
            return true;
        }

        draw_status_detail_message(display, font, toggle_config->summary);
        if (toggle_config->bar_mode == STATUS_BAR_NONE) {
            selected_toggle_index = -1;
        } else {
            selected_toggle_index = toggle_id;
            const uint8_t upper_position = STATUS_BAR_LEFT + omni_status_load_bar(profile, true, toggle_id);
            const uint8_t lower_position = STATUS_BAR_LEFT + omni_status_load_bar(profile, false, toggle_id);
            if (toggle_config->bar_mode >= STATUS_BAR_UPPER_ONLY) {
                draw_parameter_bar(display, STATUS_UPPER_BAR_Y, hue_main_color, sat_main_color, upper_position, font, label_line_height, toggle_config->bar_upper);
            }
            if (toggle_config->bar_mode == STATUS_BAR_UPPER_AND_LOWER) {
                draw_parameter_bar(display, STATUS_LOWER_BAR_Y, hue_main_color, sat_main_color, lower_position, font, label_line_height, toggle_config->bar_lower);
            }
        }

        draw_toggle_button(toggle_id, display, font, hue_main_color, sat_main_color, STATUS_ACTIVE_BRIGHTNESS);
        apply_toggle_parameter(toggle_id, true, omni_status_load_parameter_value(active_default_layer, true, toggle_id));
        apply_toggle_parameter(toggle_id, false, omni_status_load_parameter_value(active_default_layer, false, toggle_id));
        return true;
    }
    return false;
}

static void handle_parameter_bar_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y) {
    if (selected_toggle_index < 0) {
        return;
    }

    const status_toggle_id_t toggle_id = (status_toggle_id_t)selected_toggle_index;
    const status_profile_t profile = current_profile();
    const status_toggle_config_t *toggle_config = &toggle_configs[toggle_id];
    const uint8_t label_line_height = (uint8_t)(font->line_height / 2);
    const status_rect_t upper_touch_area = {
        .left = STATUS_BAR_LEFT - 10,
        .top = STATUS_UPPER_BAR_Y - 25,
        .right = STATUS_BAR_RIGHT + 10,
        .bottom = STATUS_UPPER_BAR_Y + 7,
    };
    const status_rect_t lower_touch_area = {
        .left = STATUS_BAR_LEFT - 10,
        .top = STATUS_LOWER_BAR_Y - 7,
        .right = STATUS_BAR_RIGHT + 10,
        .bottom = STATUS_LOWER_BAR_Y + 25,
    };

    if (toggle_config->bar_mode >= STATUS_BAR_UPPER_ONLY && point_in_rect(touch_x, touch_y, upper_touch_area)) {
        profile_upper_bar_positions[profile][toggle_id] = clamp_bar_x(touch_x);
        const uint8_t parameter_value = profile_upper_bar_positions[profile][toggle_id] - STATUS_BAR_LEFT;
        draw_parameter_bar(display, STATUS_UPPER_BAR_Y, hue_main_color, sat_main_color, profile_upper_bar_positions[profile][toggle_id], font, label_line_height, toggle_config->bar_upper);
        apply_toggle_parameter(toggle_id, true, parameter_value);
        omni_status_save_bar(profile, true, toggle_id, parameter_value);
    }
    if (toggle_config->bar_mode == STATUS_BAR_UPPER_AND_LOWER && point_in_rect(touch_x, touch_y, lower_touch_area)) {
        profile_lower_bar_positions[profile][toggle_id] = clamp_bar_x(touch_x);
        const uint8_t parameter_value = profile_lower_bar_positions[profile][toggle_id] - STATUS_BAR_LEFT;
        draw_parameter_bar(display, STATUS_LOWER_BAR_Y, hue_main_color, sat_main_color, profile_lower_bar_positions[profile][toggle_id], font, label_line_height, toggle_config->bar_lower);
        apply_toggle_parameter(toggle_id, false, parameter_value);
        omni_status_save_bar(profile, false, toggle_id, parameter_value);
    }
}

void status_view_handle_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y) {
    if (handle_auto_os_button_touch(display, font, touch_x, touch_y)) {
        return;
    }
    if (handle_manual_os_button_touch(display, font, touch_x, touch_y)) {
        return;
    }
    if (handle_toggle_button_touch(display, font, touch_x, touch_y)) {
        return;
    }
    handle_parameter_bar_touch(display, font, touch_x, touch_y);
}

void status_view_init(painter_device_t display, painter_font_handle_t font) {
    draw_background_all_black(display);
    status_view_load_persistent_state();
    redraw_status_view(display, font);
}

static void load_profile_toggle_states(void) {
    is_on_aos = omni_status_load_toggle_aos();
    if (!is_on_aos) {
        active_default_layer = omni_status_load_toggle_osc();
        default_layer_set(1UL << active_default_layer);
    }
    for (status_profile_t profile = 0; profile < STATUS_PROFILE_COUNT; profile++) {
        for (status_toggle_id_t toggle_id = 0; toggle_id < STATUS_TOGGLE_COUNT; toggle_id++) {
            profile_toggle_states[profile][toggle_id] = omni_status_load_toggle_normal(profile, toggle_id);
        }
    }
}

void status_view_sync_default_layer_to_os(void) {
    const os_variant_t current_os = current_os_variant();
    if (is_on_aos) {
        const uint8_t target_layer = layer_for_os(current_os);
        default_layer_set(1UL << target_layer);
    }
    active_default_layer = get_highest_layer(default_layer_state);
    load_profile_toggle_states();
    apply_current_profile_parameters();
}
