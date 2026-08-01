#pragma once

#include "quantum.h"
#include "config.h"
#include "config_omni.h"

#define TOUCH_TIME_MS         70
#define TOUCH_DEBOUNCE_TIME   (TOUCH_TIME_MS + 80)
#define TAP_DIST_PX           3
#define SWIPE_DIST_PX         10

typedef enum {
    DISPLAY_MODE_TOUCH_KEY,
    DISPLAY_MODE_TRACKBALL_TUNING,
    DISPLAY_MODE_SWIPE_GESTURE,
    DISPLAY_MODE_KEY_MATRIX,
    DISPLAY_MODE_STATUS1,
} display_mode_t;

typedef struct {
    painter_image_handle_t *image;
    int x_coordinate;
    int y_coordinate;
} ImagePosition;

typedef struct {
    int16_t x_coordinate;
    int16_t y_coordinate;
} point_t;

extern painter_device_t display;
extern display_mode_t display_mode;
extern ImagePosition lcd_layer_app_images[MAX_LCD_CATEGORY + 1][MAX_LCD_LAYER + 1][7];
extern uint8_t current_layer;
extern uint8_t current_lcd_layer;
extern uint8_t current_lcd_category;
extern painter_font_handle_t noto9_font;
extern painter_font_handle_t noto11_font;
extern painter_font_handle_t roboto_mono16;
extern painter_font_handle_t st2_mono16;
extern uint16_t touch_x;
extern uint16_t touch_y;
extern uint16_t touch_start_timer;
extern bool touch_signal;
extern bool touch_start_flag;
extern bool initial_touch_flag;
extern bool touch_signal_view_update;
extern uint16_t touch_repeat_interval;
extern uint8_t gesture_id;

void display_redraw(void);
void draw_background_all(void);
void draw_background_all_black(void);
void draw_lcd_layer_category_images(void);
void process_touch_interrupt(void);
