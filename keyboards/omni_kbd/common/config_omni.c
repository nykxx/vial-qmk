// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later
#include <math.h>
#include "config_omni.h"
#include "dynamic_keymap.h"
#include "omni_keycode_encoding.h"
#include "omni_layers.h"

enum {
    OMNI_GENERAL_CONFIG_LAYER       = _NUM,
    OMNI_STATUS_CONFIG_LAYER        = _MOUSE,
    COLOR_BACKGROUND_CONFIG_ROW     = 4,
    COLOR_FOREGROUND_CONFIG_ROW     = 5,
    TRACKBALL_CONFIG_ROW            = 7,
    STATUS_GLOBAL_CONFIG_ROW        = 4,
    STATUS_WINDOWS_TOGGLE_ROW       = 7,
    STATUS_WINDOWS_UPPER_VALUE_ROW  = 8,
    STATUS_WINDOWS_LOWER_VALUE_ROW  = 9,
    STATUS_MACOS_TOGGLE_ROW         = 13,
    STATUS_MACOS_UPPER_VALUE_ROW    = 14,
    STATUS_MACOS_LOWER_VALUE_ROW    = 15,
    STATUS_AUTO_OS_CONFIG_COLUMN    = 0,
    STATUS_MANUAL_OS_CONFIG_COLUMN  = 1,
    CONFIG_LAST_USED_COLUMN         = 5,
    STATUS_WINDOWS_PROFILE          = 0,
    STATUS_MACOS_PROFILE            = 1,
};

_Static_assert(OMNI_GENERAL_CONFIG_LAYER < DYNAMIC_KEYMAP_LAYER_COUNT, "General config layer must be stored in the dynamic keymap");
_Static_assert(OMNI_STATUS_CONFIG_LAYER < DYNAMIC_KEYMAP_LAYER_COUNT, "Status config layer must be stored in the dynamic keymap");
_Static_assert(STATUS_MACOS_LOWER_VALUE_ROW < MATRIX_ROWS, "Status config rows must fit in the matrix");
_Static_assert(CONFIG_LAST_USED_COLUMN < MATRIX_COLS, "Config columns must fit in the matrix");

typedef struct {
    uint8_t layer;
    uint8_t row;
    uint8_t column;
} config_keymap_cell_t;

typedef enum {
    COLOR_CONFIG_BACKGROUND_HUE,
    COLOR_CONFIG_BACKGROUND_SATURATION,
    COLOR_CONFIG_BACKGROUND_VALUE,
    COLOR_CONFIG_MAIN_HUE,
    COLOR_CONFIG_MAIN_SATURATION,
    COLOR_CONFIG_MAIN_VALUE,
    COLOR_CONFIG_SUB_HUE,
    COLOR_CONFIG_SUB_SATURATION,
    COLOR_CONFIG_SUB_VALUE,
    COLOR_CONFIG_ITEM_COUNT,
} color_config_item_t;

static const config_keymap_cell_t color_config_cells[COLOR_CONFIG_ITEM_COUNT] = {
    [COLOR_CONFIG_BACKGROUND_HUE]        = {OMNI_GENERAL_CONFIG_LAYER, COLOR_BACKGROUND_CONFIG_ROW, 0},
    [COLOR_CONFIG_BACKGROUND_SATURATION] = {OMNI_GENERAL_CONFIG_LAYER, COLOR_BACKGROUND_CONFIG_ROW, 1},
    [COLOR_CONFIG_BACKGROUND_VALUE]      = {OMNI_GENERAL_CONFIG_LAYER, COLOR_BACKGROUND_CONFIG_ROW, 2},
    [COLOR_CONFIG_MAIN_HUE]              = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 0},
    [COLOR_CONFIG_MAIN_SATURATION]       = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 1},
    [COLOR_CONFIG_MAIN_VALUE]            = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 2},
    [COLOR_CONFIG_SUB_HUE]               = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 3},
    [COLOR_CONFIG_SUB_SATURATION]        = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 4},
    [COLOR_CONFIG_SUB_VALUE]             = {OMNI_GENERAL_CONFIG_LAYER, COLOR_FOREGROUND_CONFIG_ROW, 5},
};

typedef enum {
    TRACKBALL_CONFIG_CURSOR_SPEED,
    TRACKBALL_CONFIG_CURSOR_SLOPE,
    TRACKBALL_CONFIG_SCROLL_SPEED,
    TRACKBALL_CONFIG_SCROLL_SLOPE,
    TRACKBALL_CONFIG_ITEM_COUNT,
} trackball_config_item_t;

static const config_keymap_cell_t trackball_config_cells[TRACKBALL_CONFIG_ITEM_COUNT] = {
    [TRACKBALL_CONFIG_CURSOR_SPEED] = {OMNI_GENERAL_CONFIG_LAYER, TRACKBALL_CONFIG_ROW, 0},
    [TRACKBALL_CONFIG_CURSOR_SLOPE] = {OMNI_GENERAL_CONFIG_LAYER, TRACKBALL_CONFIG_ROW, 1},
    [TRACKBALL_CONFIG_SCROLL_SPEED] = {OMNI_GENERAL_CONFIG_LAYER, TRACKBALL_CONFIG_ROW, 2},
    [TRACKBALL_CONFIG_SCROLL_SLOPE] = {OMNI_GENERAL_CONFIG_LAYER, TRACKBALL_CONFIG_ROW, 3},
};

static void write_config_value(const config_keymap_cell_t *cell, uint16_t value) {
    dynamic_keymap_set_keycode(cell->layer, cell->row, cell->column, omni_stored_value_encode(value));
}

static int read_config_value(const config_keymap_cell_t *cell) {
    return omni_stored_value_decode(dynamic_keymap_get_keycode(cell->layer, cell->row, cell->column));
}

uint8_t hue_bg = 0;
uint8_t sat_bg = 0;
uint8_t val_bg = 0;
uint8_t hue_main_color = 255;
uint8_t sat_main_color = 0;
uint8_t val_main_color = 255;
uint8_t hue_sub_color = 255;
uint8_t sat_sub_color = 0;
uint8_t val_sub_color = 255;

float speed_adjust1;
int slope_factor1;
float speed_adjust2;
int slope_factor2; 

static bool config_write_in_progress;

static void begin_config_write(void) {
    config_write_in_progress = true;
}

static void end_config_write(void) {
    config_write_in_progress = false;
}

bool omni_config_should_reload_after_keymap_update(void) {
    return !config_write_in_progress;
}

void omni_config_save_colors(void) {
    begin_config_write();
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_HUE], hue_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_SATURATION], sat_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_VALUE], val_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_HUE], hue_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_SATURATION], sat_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_VALUE], val_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_HUE], hue_sub_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_SATURATION], sat_sub_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_VALUE], val_sub_color);
    end_config_write();
}

void omni_config_load_colors(void) {
    hue_bg = read_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_HUE]);
    sat_bg = read_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_SATURATION]);
    val_bg = read_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_VALUE]);
    hue_main_color = read_config_value(&color_config_cells[COLOR_CONFIG_MAIN_HUE]);
    sat_main_color = read_config_value(&color_config_cells[COLOR_CONFIG_MAIN_SATURATION]);
    val_main_color = read_config_value(&color_config_cells[COLOR_CONFIG_MAIN_VALUE]);
    hue_sub_color = read_config_value(&color_config_cells[COLOR_CONFIG_SUB_HUE]);
    sat_sub_color = read_config_value(&color_config_cells[COLOR_CONFIG_SUB_SATURATION]);
    val_sub_color = read_config_value(&color_config_cells[COLOR_CONFIG_SUB_VALUE]);
}

void omni_config_save_trackball(void) {
    begin_config_write();
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SPEED], (int)round(speed_adjust1 * 10));
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SLOPE], slope_factor1);
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SPEED], (int)round(speed_adjust2 * 10));
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SLOPE], slope_factor2);
    end_config_write();
}

void omni_config_load_trackball(void) {
    speed_adjust1 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SPEED]) / 10.0f;
    slope_factor1 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SLOPE]);
    speed_adjust2 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SPEED]) / 10.0f;
    slope_factor2 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SLOPE]);
    if (speed_adjust1 > 3.0f || speed_adjust1 < 0.1f) {
        speed_adjust1 = DEFAULT_CURSOR_SPEED_FACTOR;
    }
    if (slope_factor1 > 100 || slope_factor1 < 10) {
        slope_factor1 = DEFAULT_CURSOR_SLOPE_FACTOR;
    }
    if (speed_adjust2 > 3.0f || speed_adjust2 < 0.1f) {
        speed_adjust2 = DEFAULT_SCROLL_SPEED_FACTOR;
    }
    if (slope_factor2 > 100 || slope_factor2 < 10) {
        slope_factor2 = DEFAULT_SCROLL_SLOPE_FACTOR;
    }
}



static inline uint16_t encode_stored_bool(bool value) {
    return omni_stored_value_encode(value ? 1u : 0u);
}

bool         is_on_aos  = false;
os_variant_t manual_os  = OS_WINDOWS;

static uint8_t clamp_status_parameter(int value) {
    if (value < 0) {
        return 0;
    }
    if (value > 100) {
        return 100;
    }
    return (uint8_t)value;
}

static uint8_t status_toggle_row_for_profile(uint8_t profile) {
    if (profile == STATUS_WINDOWS_PROFILE) {
        return STATUS_WINDOWS_TOGGLE_ROW;
    }
    if (profile == STATUS_MACOS_PROFILE) {
        return STATUS_MACOS_TOGGLE_ROW;
    }
    /* Preserve the legacy fallback for an invalid profile. */
    return 0;
}

static uint8_t status_parameter_row_for_profile(uint8_t profile, bool upper) {
    /* Preserve the legacy mapping: profile zero is Windows; any other value is macOS. */
    if (profile == STATUS_WINDOWS_PROFILE) {
        return upper ? STATUS_WINDOWS_UPPER_VALUE_ROW : STATUS_WINDOWS_LOWER_VALUE_ROW;
    }
    return upper ? STATUS_MACOS_UPPER_VALUE_ROW : STATUS_MACOS_LOWER_VALUE_ROW;
}


void omni_status_save_auto_os_enabled(bool enabled) {
    begin_config_write();
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN, encode_stored_bool(enabled));
    end_config_write();
}

void omni_status_save_manual_layer(uint8_t layer) {
    begin_config_write();
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN, omni_stored_value_encode(layer));
    end_config_write();
}

void omni_status_save_toggle_enabled(uint8_t profile, uint8_t toggle_index, bool enabled) {
    const uint8_t toggle_row = status_toggle_row_for_profile(profile);
    begin_config_write();
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, toggle_index, encode_stored_bool(enabled));
    end_config_write();
}

bool omni_status_load_auto_os_enabled(void) {
    return omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN));
}

uint8_t omni_status_load_manual_layer(void) {
    const uint16_t keycode = dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN);
    return omni_stored_value_decode(keycode) != 0;
}

bool omni_status_load_toggle_enabled(uint8_t profile, uint8_t toggle_index) {
    const uint8_t toggle_row = status_toggle_row_for_profile(profile);
    return omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, toggle_index));
}

uint8_t omni_status_load_bar_value(uint8_t profile, bool upper, uint8_t toggle_index) {
    return clamp_status_parameter(omni_status_load_parameter_value(profile, upper, toggle_index));
}

int omni_status_load_parameter_value(uint8_t profile, bool upper, uint8_t toggle_index) {
    const uint8_t value_row = status_parameter_row_for_profile(profile, upper);
    return omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, value_row, toggle_index));
}

void omni_status_save_bar_value(uint8_t profile, bool upper, uint8_t toggle_index, uint8_t value) {
    const uint8_t clamped_value = clamp_status_parameter(value);
    const uint8_t value_row = status_parameter_row_for_profile(profile, upper);
    const uint16_t encoded_value = omni_stored_value_encode(clamped_value);

    if (dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, value_row, toggle_index) == encoded_value) {
        return;
    }

    begin_config_write();
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, value_row, toggle_index, encoded_value);
    end_config_write();
}
