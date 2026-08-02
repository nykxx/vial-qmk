#pragma once

#include "quantum.h"
#include "config.h"
#include "config_omni.h"

typedef enum {
    DISPLAY_MODE_TOUCH_KEY,
    DISPLAY_MODE_TRACKBALL_TUNING,
    DISPLAY_MODE_SWIPE_GESTURE,
    DISPLAY_MODE_KEY_MATRIX,
    DISPLAY_MODE_STATUS1,
} display_mode_t;

extern painter_device_t display;
extern painter_font_handle_t noto9_font;
extern painter_font_handle_t noto11_font;
extern painter_font_handle_t roboto_mono16;
extern painter_font_handle_t st2_mono16;
display_mode_t display_get_mode(void);
void display_set_mode(display_mode_t mode);
void display_redraw(uint8_t current_layer);
void draw_background_all(void);
void draw_background_all_black(void);
void process_touch_interrupt(void);
