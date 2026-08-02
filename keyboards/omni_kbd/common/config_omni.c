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
    STATUS_BASE_PROFILE             = 0,
    STATUS_SUB_PROFILE              = 1,
};

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

bool keymap_change_update_flag = true;

void save_omni_color_config(void) {
    keymap_change_update_flag = false;
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_HUE], hue_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_SATURATION], sat_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_BACKGROUND_VALUE], val_bg);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_HUE], hue_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_SATURATION], sat_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_MAIN_VALUE], val_main_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_HUE], hue_sub_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_SATURATION], sat_sub_color);
    write_config_value(&color_config_cells[COLOR_CONFIG_SUB_VALUE], val_sub_color);
    keymap_change_update_flag = true;
}

void load_omni_color_config(void) {
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

void save_omni_tb_config(void) {
    keymap_change_update_flag = false;
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SPEED], (int)round(speed_adjust1 * 10));
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SLOPE], slope_factor1);
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SPEED], (int)round(speed_adjust2 * 10));
    write_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SLOPE], slope_factor2);
    keymap_change_update_flag = true;
}

void load_omni_tb_config(void) {
    speed_adjust1 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SPEED]) / 10.0f;
    slope_factor1 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_CURSOR_SLOPE]);
    speed_adjust2 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SPEED]) / 10.0f;
    slope_factor2 = read_config_value(&trackball_config_cells[TRACKBALL_CONFIG_SCROLL_SLOPE]);
    if (speed_adjust1 > 3.0f || speed_adjust1 < 0.1f) {
        speed_adjust1 = DEFAULT_SPEED_ADJUST1;
    }
    if (slope_factor1 > 100 || slope_factor1 < 10) {
        slope_factor1 = DEFAULT_SLOPE_FACTOR1;
    }
    if (speed_adjust2 > 3.0f || speed_adjust2 < 0.1f) {
        speed_adjust2 = DEFAULT_SPEED_ADJUST2;
    }
    if (slope_factor2 > 100 || slope_factor2 < 10) {
        slope_factor2= DEFAULT_SLOPE_FACTOR2;
    }
}



static inline uint16_t encode_stored_bool(bool value) {
    return omni_stored_value_encode(value ? 1u : 0u);
}

static inline uint8_t status_toggle_row(os_variant_t os) {
    return os == OS_MACOS ? STATUS_MACOS_TOGGLE_ROW : STATUS_WINDOWS_TOGGLE_ROW;
}

static inline uint8_t status_upper_value_row(os_variant_t os) {
    return os == OS_MACOS ? STATUS_MACOS_UPPER_VALUE_ROW : STATUS_WINDOWS_UPPER_VALUE_ROW;
}

static inline uint8_t status_lower_value_row(os_variant_t os) {
    return os == OS_MACOS ? STATUS_MACOS_LOWER_VALUE_ROW : STATUS_WINDOWS_LOWER_VALUE_ROW;
}

bool         is_on_aos  = false;
os_variant_t manual_os  = OS_WINDOWS;

void omni_status_save_global(void) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN, encode_stored_bool(is_on_aos));
    uint8_t mos = (manual_os == OS_MACOS) ? 1 : 0;
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN, omni_stored_value_encode(mos));
    keymap_change_update_flag = true;
}

void omni_status_load_global(void) {
    uint16_t kc_aos = dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN);
    uint16_t kc_mos = dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN);
    is_on_aos = omni_stored_u8_decode_or_zero(kc_aos) != 0;
    manual_os = omni_stored_u8_decode_or_zero(kc_mos) == 1 ? OS_MACOS : OS_WINDOWS;
}

uint8_t osbuf_tog_win[STATUS_TOG_COUNT] = {0};
uint8_t osbuf_up_win [STATUS_TOG_COUNT] = {0};
uint8_t osbuf_lo_win [STATUS_TOG_COUNT] = {0};
uint8_t osbuf_tog_mac[STATUS_TOG_COUNT] = {0};
uint8_t osbuf_up_mac [STATUS_TOG_COUNT] = {0};
uint8_t osbuf_lo_mac [STATUS_TOG_COUNT] = {0};

static inline uint8_t  clamp100(uint8_t v) { return (v > 100) ? 100 : v; }

static void save_rows(os_variant_t os, const uint8_t *tog, const uint8_t *up, const uint8_t *lo){
    const uint8_t toggle_row = status_toggle_row(os);
    const uint8_t upper_value_row = status_upper_value_row(os);
    const uint8_t lower_value_row = status_lower_value_row(os);
    keymap_change_update_flag = false;
    for (uint8_t i=0; i<STATUS_TOG_COUNT; i++){
        dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, i, omni_stored_value_encode(tog ? (tog[i] ? 1 : 0) : 0));
        dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, upper_value_row, i, omni_stored_value_encode(up ? clamp100(up[i]) : 0));
        dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, lower_value_row, i, omni_stored_value_encode(lo ? clamp100(lo[i]) : 0));
    }
    keymap_change_update_flag = true;
}
static void load_rows(os_variant_t os, uint8_t *tog, uint8_t *up, uint8_t *lo){
    const uint8_t toggle_row = status_toggle_row(os);
    const uint8_t upper_value_row = status_upper_value_row(os);
    const uint8_t lower_value_row = status_lower_value_row(os);
    for (uint8_t i=0; i<STATUS_TOG_COUNT; i++){
        if (tog) tog[i] = omni_stored_u8_decode_or_zero(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, i)) ? 1 : 0;
        if (up)  up[i]  = clamp100(omni_stored_u8_decode_or_zero(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, upper_value_row, i)));
        if (lo)  lo[i]  = clamp100(omni_stored_u8_decode_or_zero(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, lower_value_row, i)));
    }
}

void omni_status_save_os_win(void){ save_rows(OS_WINDOWS, osbuf_tog_win, osbuf_up_win, osbuf_lo_win); }
void omni_status_save_os_mac(void){ save_rows(OS_MACOS,  osbuf_tog_mac, osbuf_up_mac, osbuf_lo_mac); }
void omni_status_load_os_win(void){ load_rows(OS_WINDOWS, osbuf_tog_win, osbuf_up_win, osbuf_lo_win); }
void omni_status_load_os_mac(void){ load_rows(OS_MACOS,  osbuf_tog_mac, osbuf_up_mac, osbuf_lo_mac); }


static inline uint8_t clamp_1_100_int(int v) {
    if (v < 1)   return 1;
    if (v > 100) return 100;
    return (uint8_t)v;
}
static inline uint8_t clamp_0_100_int(int v) {
    if (v < 0)   return 0;
    if (v > 100) return 100;
    return (uint8_t)v;
}


void omni_status_save_toggle_aos(bool state) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN, encode_stored_bool(state));
    keymap_change_update_flag = true;
}

void omni_status_save_toggle_osc(uint8_t state) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN, omni_stored_value_encode(state));
    keymap_change_update_flag = true;
}

void omni_status_save_toggle_normal(uint8_t profile, uint8_t index, bool state) {
    keymap_change_update_flag = false;
    uint8_t toggle_row = profile == STATUS_BASE_PROFILE ? STATUS_WINDOWS_TOGGLE_ROW : (profile == STATUS_SUB_PROFILE ? STATUS_MACOS_TOGGLE_ROW : 0);
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, index, encode_stored_bool(state));
    keymap_change_update_flag = true;
}

bool omni_status_load_toggle_aos(void) {
    bool state = omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_AUTO_OS_CONFIG_COLUMN));
    return state;
}

bool omni_status_load_toggle_osc(void) {
    bool state = omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, STATUS_GLOBAL_CONFIG_ROW, STATUS_MANUAL_OS_CONFIG_COLUMN));
    return state;
}

bool omni_status_load_toggle_normal(uint8_t profile, uint8_t index) {
    uint8_t toggle_row = profile == STATUS_BASE_PROFILE ? STATUS_WINDOWS_TOGGLE_ROW : (profile == STATUS_SUB_PROFILE ? STATUS_MACOS_TOGGLE_ROW : 0);
    bool state = omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, toggle_row, index));
    return state;
}

static inline uint8_t row_bar(uint8_t profile, bool upper) {
    if (upper) return profile ? STATUS_MACOS_UPPER_VALUE_ROW : STATUS_WINDOWS_UPPER_VALUE_ROW;
    else       return profile ? STATUS_MACOS_LOWER_VALUE_ROW : STATUS_WINDOWS_LOWER_VALUE_ROW;
}

uint8_t omni_status_load_bar(uint8_t profile, bool upper, uint8_t index) {
    return clamp_0_100_int(omni_status_load_parameter_value(profile, upper, index));
}

int omni_status_load_parameter_value(uint8_t profile, bool upper, uint8_t index) {
    return omni_stored_value_decode(dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, row_bar(profile, upper), index));
}

void omni_status_save_bar(uint8_t profile, bool upper, uint8_t index, uint8_t value) {
    uint8_t  v   = clamp_0_100_int(value);
    uint8_t  row = row_bar(profile, upper);
    uint16_t kc  = omni_stored_value_encode(v);

    if (dynamic_keymap_get_keycode(OMNI_STATUS_CONFIG_LAYER, row, index) == kc) return;

    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(OMNI_STATUS_CONFIG_LAYER, row, index, kc);
    keymap_change_update_flag = true;
}
