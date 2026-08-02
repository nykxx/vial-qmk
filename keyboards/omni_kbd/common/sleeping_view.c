// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sleeping_view.h"

#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "config_omni.h"

enum {
    CODE_RAIN_COLUMN_COUNT = 10,
    CODE_RAIN_FALL_SPEED   = 3,
    CODE_RAIN_FADE_STEPS   = 30,
};

typedef struct {
    int x;
    int y;
    char character;
    int fade;
} code_drop_t;

static code_drop_t code_drops[CODE_RAIN_COLUMN_COUNT];
static const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const uint8_t character_count = sizeof(characters) - 1;

void init_matrix_code_rain(void) {
    for (int column_index = 0; column_index < CODE_RAIN_COLUMN_COUNT; column_index++) {
        code_drops[column_index].x = rand() % TOUCH_LCD_WIDTH;
        code_drops[column_index].y = rand() % (TOUCH_LCD_HEIGHT / 2);
        code_drops[column_index].character = characters[rand() % character_count];
        code_drops[column_index].fade = CODE_RAIN_FADE_STEPS;
    }
}

void draw_matrix_code_rain(painter_device_t display, painter_font_handle_t font) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, hue_bg, sat_bg, val_bg, true);
    for (int column_index = 0; column_index < CODE_RAIN_COLUMN_COUNT; column_index++) {
        if (code_drops[column_index].fade > 0) {
            char character_text[2] = {code_drops[column_index].character, '\0'};
            uint8_t brightness = 255 * code_drops[column_index].fade / CODE_RAIN_FADE_STEPS;
            if (brightness < 30) brightness = 30;
            qp_drawtext_recolor(display, code_drops[column_index].x, code_drops[column_index].y, font, character_text,
                                hue_main_color, sat_main_color, brightness, hue_bg, sat_bg, val_bg);
        }
    }
    qp_flush(display);
}

void update_matrix_code_rain(void) {
    for (int column_index = 0; column_index < CODE_RAIN_COLUMN_COUNT; column_index++) {
        code_drops[column_index].y += CODE_RAIN_FALL_SPEED;
        if (code_drops[column_index].y > TOUCH_LCD_HEIGHT) {
            code_drops[column_index].y = rand() % (TOUCH_LCD_HEIGHT / 2);
            code_drops[column_index].x = rand() % TOUCH_LCD_WIDTH;
            code_drops[column_index].character = characters[rand() % character_count];
            code_drops[column_index].fade = CODE_RAIN_FADE_STEPS;
        } else {
            code_drops[column_index].fade--;
        }
    }
}
