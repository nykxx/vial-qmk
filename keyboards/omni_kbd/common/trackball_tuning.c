// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "trackball_tuning.h"

#include <stdbool.h>

#include "config.h"
#include "config_omni.h"
#include "draw_custom.h"
#include "wait.h"
#include "../icon/omni_image_loader.h"

enum {
    TUNING_BUTTON_RADIUS         = 22,
    TUNING_BUTTON_RADIUS_GAP     = 1,
    TUNING_CURVE_REDRAW_WAIT_MS = 10,

    RIGHT_CURVE_X  = TOUCH_LCD_WIDTH / 2 + 50,
    RIGHT_CURVE_Y  = TOUCH_LCD_HEIGHT / 2 - 10,
    LEFT_CURVE_X   = TOUCH_LCD_WIDTH / 2 - 50,
    LEFT_CURVE_Y   = TOUCH_LCD_HEIGHT / 2 - 10,
    UPPER_BUTTON_Y = TOUCH_LCD_HEIGHT / 2 - 50,
    LOWER_BUTTON_Y = TOUCH_LCD_HEIGHT / 2 + 45,
    SAVE_BUTTON_X  = TOUCH_LCD_WIDTH / 2,
    SAVE_BUTTON_Y  = TOUCH_LCD_HEIGHT / 2 + 90,
};

typedef struct {
    int16_t x;
    int16_t y;
} tuning_point_t;

static const char *const title_text = "TB TUNE";
static const char *const right_text = "Right";
static const char *const left_text = "Left";
static const char *const plus_text = "+      +";
static const char *const minus_text = "-      -";

static int16_t title_text_width = 0;
static int16_t right_text_width = 0;
static int16_t left_text_width = 0;
static int16_t plus_text_width = 0;
static int16_t minus_text_width = 0;

static float previous_speed_adjust1;
static int previous_slope_factor1;
static float previous_speed_adjust2;
static int previous_slope_factor2;

static bool is_touch_in_circle(uint16_t touch_x, uint16_t touch_y, tuning_point_t center, uint16_t radius) {
    uint32_t distance_squared = (center.x - touch_x) * (center.x - touch_x) + (center.y - touch_y) * (center.y - touch_y);
    return distance_squared <= radius * radius;
}

static void draw_black_rectangle(painter_device_t display, uint8_t left, uint8_t top, uint8_t right, uint8_t bottom) {
    qp_rect(display, left, top, right, bottom, 0, 0, 0, true);
}

static void cache_text_widths(painter_font_handle_t font) {
    if (title_text_width == 0) {
        title_text_width = qp_textwidth(font, title_text);
    }
    if (right_text_width == 0) {
        right_text_width = qp_textwidth(font, right_text);
    }
    if (left_text_width == 0) {
        left_text_width = qp_textwidth(font, left_text);
    }
    if (plus_text_width == 0) {
        plus_text_width = qp_textwidth(font, plus_text);
    }
    if (minus_text_width == 0) {
        minus_text_width = qp_textwidth(font, minus_text);
    }
}

void trackball_tuning_draw(painter_device_t display, painter_font_handle_t font) {
    draw_black_rectangle(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT);
    cache_text_widths(font);

    qp_drawtext(display, TOUCH_LCD_WIDTH / 2 - title_text_width / 2, TOUCH_LCD_HEIGHT / 2 - 70 - font->line_height, font, title_text);
    qp_drawtext(display, RIGHT_CURVE_X - right_text_width / 2, TOUCH_LCD_HEIGHT / 2 + 80 - font->line_height, font, right_text);
    qp_drawtext(display, RIGHT_CURVE_X - plus_text_width / 2, UPPER_BUTTON_Y - font->line_height / 2, font, plus_text);
    qp_drawtext(display, RIGHT_CURVE_X - minus_text_width / 2, LOWER_BUTTON_Y - font->line_height / 2, font, minus_text);
    qp_drawtext(display, LEFT_CURVE_X - left_text_width / 2, TOUCH_LCD_HEIGHT / 2 + 80 - font->line_height, font, left_text);
    qp_drawtext(display, LEFT_CURVE_X - plus_text_width / 2, UPPER_BUTTON_Y - font->line_height / 2, font, plus_text);
    qp_drawtext(display, LEFT_CURVE_X - minus_text_width / 2, LOWER_BUTTON_Y - font->line_height / 2, font, minus_text);

    qp_curve(display, speed_adjust1, slope_factor1, 220, 255, 255, RIGHT_CURVE_X, RIGHT_CURVE_Y);
    qp_curve(display, speed_adjust2, slope_factor2, 135, 255, 255, LEFT_CURVE_X, LEFT_CURVE_Y);

    painter_image_handle_t save_image = omni_save_image();
    qp_drawimage(display, SAVE_BUTTON_X - save_image->width / 2, SAVE_BUTTON_Y - save_image->height / 2, save_image);
    qp_flush(display);
}

static bool update_tuning_values(uint16_t touch_x, uint16_t touch_y) {
    const tuning_point_t speed1_increase = {RIGHT_CURVE_X - (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), LOWER_BUTTON_Y};
    const tuning_point_t speed1_decrease = {RIGHT_CURVE_X - (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), UPPER_BUTTON_Y};
    const tuning_point_t speed2_increase = {LEFT_CURVE_X - (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), LOWER_BUTTON_Y};
    const tuning_point_t speed2_decrease = {LEFT_CURVE_X - (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), UPPER_BUTTON_Y};
    const tuning_point_t slope1_increase = {RIGHT_CURVE_X + (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), UPPER_BUTTON_Y};
    const tuning_point_t slope1_decrease = {RIGHT_CURVE_X + (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), LOWER_BUTTON_Y};
    const tuning_point_t slope2_increase = {LEFT_CURVE_X + (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), UPPER_BUTTON_Y};
    const tuning_point_t slope2_decrease = {LEFT_CURVE_X + (TUNING_BUTTON_RADIUS + TUNING_BUTTON_RADIUS_GAP), LOWER_BUTTON_Y};

    if (is_touch_in_circle(touch_x, touch_y, speed1_increase, TUNING_BUTTON_RADIUS)) {
        speed_adjust1 += 0.2;
        if (speed_adjust1 > 3.0f) speed_adjust1 = 3.0f;
    } else if (is_touch_in_circle(touch_x, touch_y, speed1_decrease, TUNING_BUTTON_RADIUS)) {
        speed_adjust1 -= 0.2;
        if (speed_adjust1 < 0.2f) speed_adjust1 = 0.2f;
    } else if (is_touch_in_circle(touch_x, touch_y, speed2_increase, TUNING_BUTTON_RADIUS)) {
        speed_adjust2 += 0.2;
        if (speed_adjust2 > 3.0f) speed_adjust2 = 3.0f;
    } else if (is_touch_in_circle(touch_x, touch_y, speed2_decrease, TUNING_BUTTON_RADIUS)) {
        speed_adjust2 -= 0.2;
        if (speed_adjust2 < 0.2f) speed_adjust2 = 0.2f;
    } else if (is_touch_in_circle(touch_x, touch_y, slope1_increase, TUNING_BUTTON_RADIUS)) {
        slope_factor1 += 5;
        if (slope_factor1 > 100) slope_factor1 = 100;
    } else if (is_touch_in_circle(touch_x, touch_y, slope1_decrease, TUNING_BUTTON_RADIUS)) {
        slope_factor1 -= 5;
        if (slope_factor1 < 10) slope_factor1 = 10;
    } else if (is_touch_in_circle(touch_x, touch_y, slope2_increase, TUNING_BUTTON_RADIUS)) {
        slope_factor2 += 5;
        if (slope_factor2 > 100) slope_factor2 = 100;
    } else if (is_touch_in_circle(touch_x, touch_y, slope2_decrease, TUNING_BUTTON_RADIUS)) {
        slope_factor2 -= 5;
        if (slope_factor2 < 10) slope_factor2 = 10;
    } else {
        return false;
    }
    return true;
}

static void redraw_changed_curve(painter_device_t display) {
    if (previous_speed_adjust1 != speed_adjust1 || previous_slope_factor1 != slope_factor1) {
        draw_black_rectangle(display, TOUCH_LCD_WIDTH / 2 + 15, TOUCH_LCD_HEIGHT / 2 - 40, TOUCH_LCD_WIDTH / 2 + 85, TOUCH_LCD_HEIGHT / 2 + 25);
        wait_ms(TUNING_CURVE_REDRAW_WAIT_MS);
        qp_curve(display, speed_adjust1, slope_factor1, 220, 255, 255, RIGHT_CURVE_X, RIGHT_CURVE_Y);
    } else if (previous_speed_adjust2 != speed_adjust2 || previous_slope_factor2 != slope_factor2) {
        draw_black_rectangle(display, TOUCH_LCD_WIDTH / 2 - 85, TOUCH_LCD_HEIGHT / 2 - 40, TOUCH_LCD_WIDTH / 2 - 15, TOUCH_LCD_HEIGHT / 2 + 25);
        wait_ms(TUNING_CURVE_REDRAW_WAIT_MS);
        qp_curve(display, speed_adjust2, slope_factor2, 135, 255, 255, LEFT_CURVE_X, LEFT_CURVE_Y);
    }

    previous_speed_adjust1 = speed_adjust1;
    previous_speed_adjust2 = speed_adjust2;
    previous_slope_factor1 = slope_factor1;
    previous_slope_factor2 = slope_factor2;
}

void trackball_tuning_handle_touch(painter_device_t display, painter_font_handle_t font, uint16_t touch_x, uint16_t touch_y) {
    const tuning_point_t save_button_center = {SAVE_BUTTON_X, SAVE_BUTTON_Y + 5};
    bool tuning_value_changed = update_tuning_values(touch_x, touch_y);
    if (!tuning_value_changed && is_touch_in_circle(touch_x, touch_y, save_button_center, TUNING_BUTTON_RADIUS)) {
        omni_config_save_trackball();
        trackball_tuning_draw(display, font);
    }
    redraw_changed_curve(display);
}
