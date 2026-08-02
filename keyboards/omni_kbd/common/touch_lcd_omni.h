#pragma once

#include "quantum.h"

typedef enum {
    DISPLAY_MODE_TOUCH_KEY,
    DISPLAY_MODE_TRACKBALL_TUNING,
    DISPLAY_MODE_SWIPE_GESTURE,
    DISPLAY_MODE_KEY_MATRIX,
    DISPLAY_MODE_STATUS1,
} display_mode_t;

typedef struct {
    painter_device_t device;
    painter_font_handle_t status_font;
    painter_font_handle_t interface_font;
    painter_font_handle_t keymap_font;
    painter_font_handle_t symbol_font;
} omni_display_context_t;

display_mode_t display_get_mode(void);
void display_set_mode(display_mode_t mode);
void display_redraw(const omni_display_context_t *context, uint8_t current_layer);
void draw_background_all(painter_device_t device);
void draw_background_all_black(painter_device_t device);
void process_touch_interrupt(const omni_display_context_t *context);
