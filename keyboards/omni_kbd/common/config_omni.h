#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "os_detection.h"

#define DEFAULT_CURSOR_SPEED_FACTOR 1.6f
#define DEFAULT_CURSOR_SLOPE_FACTOR 50
#define DEFAULT_SCROLL_SPEED_FACTOR 1.4f
#define DEFAULT_SCROLL_SLOPE_FACTOR 70

extern uint8_t hue_bg;
extern uint8_t sat_bg;
extern uint8_t val_bg;
extern uint8_t hue_main_color;
extern uint8_t sat_main_color;
extern uint8_t val_main_color;
extern uint8_t hue_sub_color;
extern uint8_t sat_sub_color;
extern uint8_t val_sub_color;

extern float speed_adjust1;
extern int slope_factor1;
extern float speed_adjust2;
extern int slope_factor2; 

void omni_config_save_colors(void);
void omni_config_load_colors(void);
void omni_config_save_trackball(void);
void omni_config_load_trackball(void);

extern bool         is_on_aos;
extern os_variant_t manual_os;

bool omni_config_should_reload_after_keymap_update(void);

void    omni_status_save_auto_os_enabled(bool enabled);
bool    omni_status_load_auto_os_enabled(void);
void    omni_status_save_manual_layer(uint8_t layer);
uint8_t omni_status_load_manual_layer(void);
void    omni_status_save_toggle_enabled(uint8_t profile, uint8_t toggle_index, bool enabled);
bool    omni_status_load_toggle_enabled(uint8_t profile, uint8_t toggle_index);

void    omni_status_save_bar_value(uint8_t profile, bool upper, uint8_t toggle_index, uint8_t value);
uint8_t omni_status_load_bar_value(uint8_t profile, bool upper, uint8_t toggle_index);
int     omni_status_load_parameter_value(uint8_t profile, bool upper, uint8_t toggle_index);
