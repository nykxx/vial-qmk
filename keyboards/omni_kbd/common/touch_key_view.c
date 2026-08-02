// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "touch_key_view.h"

#include <math.h>

#include "config.h"
#include "omni_virtual_keys.h"
#include "../drivers/cst816t.h"
#include "../icon/omni_image_loader.h"

enum {
    TOUCH_KEY_IMAGE_COUNT    = OMNI_TOUCH_KEYS_PER_CATEGORY + 1,
    TOUCH_KEY_RADIUS         = 30,
    INVALID_TOUCH_KEY_INDEX  = UINT8_MAX,
};

typedef struct {
    int16_t x;
    int16_t y;
} touch_key_position_t;

typedef struct {
    painter_image_handle_t *image;
    touch_key_position_t center;
} touch_key_image_t;

static const touch_key_position_t layer_image_center = {
    .x = TOUCH_LCD_WIDTH / 2,
    .y = TOUCH_LCD_HEIGHT / 2,
};

static const touch_key_position_t key_centers[OMNI_TOUCH_KEYS_PER_CATEGORY] = {
    {.x = 200, .y = 120},
    {.x = 160, .y = 189},
    {.x = 80,  .y = 189},
    {.x = 40,  .y = 120},
    {.x = 80,  .y = 51},
    {.x = 160, .y = 51},
};

static touch_key_image_t page_images[OMNI_TOUCH_KEY_CATEGORY_COUNT][OMNI_TOUCH_KEY_PAGE_COUNT][TOUCH_KEY_IMAGE_COUNT];
static uint8_t current_page;
static uint8_t current_category;

void touch_key_view_initialize(const uint16_t *virtual_keycodes) {
    for (int page_index = 0; page_index < OMNI_TOUCH_KEY_PAGE_COUNT; page_index++) {
        for (int category_index = 0; category_index < OMNI_TOUCH_KEY_CATEGORY_COUNT; category_index++) {
            int image_index = omni_touch_key_virtual_row(page_index, category_index);
            page_images[category_index][page_index][0] = (touch_key_image_t){
                .image  = omni_layer_image_handle(image_index),
                .center = layer_image_center,
            };

            for (int key_index = 0; key_index < OMNI_TOUCH_KEYS_PER_CATEGORY; key_index++) {
                int keycode_index = image_index * OMNI_TOUCH_KEYS_PER_CATEGORY + key_index;
                page_images[category_index][page_index][key_index + 1] = (touch_key_image_t){
                    .image  = omni_keycode_image_handle(virtual_keycodes[keycode_index]),
                    .center = key_centers[key_index],
                };
            }
        }
    }
}

void touch_key_view_draw(painter_device_t display) {
    touch_key_image_t *current_images = page_images[current_category][current_page];
    for (int image_index = 0; image_index < TOUCH_KEY_IMAGE_COUNT; image_index++) {
        painter_image_handle_t image = *current_images[image_index].image;
        if (image != NULL) {
            int draw_x = current_images[image_index].center.x - image->width / 2;
            int draw_y = current_images[image_index].center.y - image->height / 2;
            qp_drawimage(display, draw_x, draw_y, image);
        }
    }
    qp_flush(display);
}

static void clear_and_draw(painter_device_t display) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, 0, 0, 0, true);
    touch_key_view_draw(display);
}

void touch_key_view_handle_gesture(painter_device_t display, uint8_t gesture_id) {
    switch (gesture_id) {
        case CST816S_SLIDE_RIGHT:
            if (current_page + 1 < OMNI_TOUCH_KEY_PAGE_COUNT) {
                current_page++;
                clear_and_draw(display);
            }
            break;
        case CST816S_SLIDE_LEFT:
            if (current_page > 0) {
                current_page--;
                clear_and_draw(display);
            }
            break;
        case CST816S_SLIDE_UP:
            if (current_category > 0) {
                current_category--;
                clear_and_draw(display);
            }
            break;
        case CST816S_SLIDE_DOWN:
            if (current_category + 1 < OMNI_TOUCH_KEY_CATEGORY_COUNT) {
                current_category++;
                clear_and_draw(display);
            }
            break;
        default:
            break;
    }
}

uint8_t touch_key_view_current_page(void) {
    return current_page;
}

static uint8_t find_touched_key(uint16_t touch_x, uint16_t touch_y) {
    for (uint8_t key_index = 0; key_index < OMNI_TOUCH_KEYS_PER_CATEGORY; key_index++) {
        int16_t delta_x = touch_x - key_centers[key_index].x;
        int16_t delta_y = touch_y - key_centers[key_index].y;
        int distance = sqrt(delta_x * delta_x + delta_y * delta_y);
        if (distance <= TOUCH_KEY_RADIUS) {
            return key_index;
        }
    }
    return INVALID_TOUCH_KEY_INDEX;
}

bool touch_key_view_locate_key(uint16_t touch_x, uint16_t touch_y, uint8_t *virtual_row, uint8_t *virtual_column) {
    uint8_t key_index = find_touched_key(touch_x, touch_y);
    if (key_index == INVALID_TOUCH_KEY_INDEX || touch_x > TOUCH_LCD_WIDTH || touch_y > TOUCH_LCD_HEIGHT) {
        return false;
    }

    *virtual_row = omni_touch_key_virtual_row(current_page, current_category);
    *virtual_column = key_index;
    return true;
}
