// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

#include "config.h"
#include "omni_layers.h"

/*
 * Vial exposes the LCD touch keys as matrix rows 4 through 15. Each row is
 * one LCD page/category combination and each column is one of the six icons.
 */
enum {
    OMNI_TOUCH_KEY_MATRIX_ROW_START = 4,
    OMNI_TOUCH_KEYS_PER_CATEGORY    = MATRIX_COLS,
    OMNI_TOUCH_KEY_MATRIX_ROW_COUNT = OMNI_TOUCH_KEY_PAGE_COUNT * OMNI_TOUCH_KEY_CATEGORY_COUNT,
    OMNI_TOUCH_KEY_COUNT            = OMNI_TOUCH_KEY_MATRIX_ROW_COUNT * OMNI_TOUCH_KEYS_PER_CATEGORY,
    OMNI_TOUCH_KEY_DISPLAY_LAYER    = _FUNC,
};

_Static_assert(OMNI_TOUCH_KEY_MATRIX_ROW_START + OMNI_TOUCH_KEY_MATRIX_ROW_COUNT <= MATRIX_ROWS,
               "LCD touch-key rows must fit in the QMK matrix");

static inline uint8_t omni_touch_key_virtual_row(uint8_t page, uint8_t category) {
    return page * OMNI_TOUCH_KEY_CATEGORY_COUNT + category;
}

static inline uint8_t omni_touch_key_matrix_row(uint8_t virtual_row) {
    return OMNI_TOUCH_KEY_MATRIX_ROW_START + virtual_row;
}
