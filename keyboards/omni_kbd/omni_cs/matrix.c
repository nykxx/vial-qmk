// Copyright 2025 mass
// Copyright 2012-2018 Jun Wako, Jack Humbert, Yiancar
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "atomic_util.h"
#include "omni_cs.h"
#include "config.h"
#include "timer.h"
#include "../common/touch_gesture.h"
#include "../common/touch_key_view.h"
#include "../common/omni_virtual_keys.h"

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

/*
 * QMK's debounced matrix is defined in quantum/matrix_common.c but has no
 * public setter. Touch keys update it directly so they retain the original
 * no-debounce pulse timing without relying on adjacent array placement.
 */
extern matrix_row_t matrix[MATRIX_ROWS];

enum {
    PHYSICAL_MATRIX_ROWS = MATRIX_ROWS / 2,
};

static void select_row(uint8_t row)
{
    setPinOutput(row_pins[row]);
    writePinLow(row_pins[row]);
}

static void unselect_row(uint8_t row)
{
    setPinInputHigh(row_pins[row]);
}

static void unselect_rows(void)
{
    for(uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
        setPinInputHigh(row_pins[row_index]);
    }
}

static void select_col(uint8_t col)
{
    setPinOutput(col_pins[col]);
    writePinLow(col_pins[col]);
}

static void unselect_col(uint8_t col)
{
    setPinInputHigh(col_pins[col]);
}

static void unselect_cols(void)
{
    for(uint8_t column_index = 0; column_index < MATRIX_COLS; column_index++) {
        setPinInputHigh(col_pins[column_index]);
    }
}

static void init_pins(void) {
    unselect_rows();
    unselect_cols();
}

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row)
{
    // Store last value of row prior to reading
    matrix_row_t last_row_value = current_matrix[current_row];

    // Clear data in matrix row
    current_matrix[current_row] = 0;

    // Select row and wait for row selecton to stabilize
    select_row(current_row);
    matrix_io_delay();

    // For each col...
    for(uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {

        // Select the col pin to read (active low)
        uint8_t pin_state = readPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_matrix[current_row] |=  pin_state ? 0 : (MATRIX_ROW_SHIFTER << col_index);
    }

    // Unselect row
    unselect_row(current_row);

    return (last_row_value != current_matrix[current_row]);
}

static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col)
{
    bool matrix_changed = false;

    // Select col and wait for col selecton to stabilize
    select_col(current_col);
    matrix_io_delay();

    // For each row...
    for(uint8_t row_index = 0; row_index < PHYSICAL_MATRIX_ROWS; row_index++)
    {
        uint8_t tmp = row_index + PHYSICAL_MATRIX_ROWS;
        // Store last value of row prior to reading
        matrix_row_t last_row_value = current_matrix[tmp];

        // Check row pin state
        if (readPin(row_pins[row_index]) == 0)
        {
            // Pin LO, set col bit
            current_matrix[tmp] |= (MATRIX_ROW_SHIFTER << current_col);
        }
        else
        {
            // Pin HI, clear col bit
            current_matrix[tmp] &= ~(MATRIX_ROW_SHIFTER << current_col);
        }

        // Determine if the matrix changed state
        if ((last_row_value != current_matrix[tmp]) && !(matrix_changed))
        {
            matrix_changed = true;
        }
    }

    // Unselect col
    unselect_col(current_col);

    return matrix_changed;
}

void matrix_init_custom(void) {
    // initialize key pins
    init_pins();
}

static bool get_touch_coordinates(uint8_t *row, uint8_t *col, uint16_t touch_x, uint16_t touch_y) {
    uint8_t virtual_row;
    uint8_t virtual_column;
    if (touch_key_view_locate_key(touch_x, touch_y, &virtual_row, &virtual_column)) {
        *row = omni_touch_key_matrix_row(virtual_row);
        *col = virtual_column;
        return true;
    }
    *row = 0xFF;
    *col = 0xFF;
    return false;
}

static bool read_touch(void) {
    uint16_t touch_x;
    uint16_t touch_y;
    touch_gesture_get_key_position(&touch_x, &touch_y);

    uint8_t row_index = 0, col_index = 0;
    if (get_touch_coordinates(&row_index, &col_index, touch_x, touch_y)) {
        select_col(col_index);
        matrix_io_delay();
        matrix_row_t last_row_value = matrix[row_index];
        if (touch_gesture_key_is_active()) {
            matrix[row_index] |= (MATRIX_ROW_SHIFTER << col_index);
        } else {
            matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << col_index);
        }
        bool matrix_changed = last_row_value != matrix[row_index];
        unselect_col(col_index);
        return matrix_changed;
    }
    return false;
}


static uint16_t last_touch_time = 0;
static bool last_matrix_state = false;

bool matrix_scan_custom(matrix_row_t current_matrix[])
{
    bool changed = false;
    // Set row, read cols
    for (uint8_t current_row = 0; current_row < PHYSICAL_MATRIX_ROWS; current_row++) {
        changed |= read_cols_on_row(current_matrix, current_row);
    }
    //else
    // Set col, read rows
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++) {
        changed |= read_rows_on_col(current_matrix, current_col);
    }

    static bool touch_signal_latch = false;
    if (touch_gesture_key_is_active()) {
        touch_signal_latch = true;
    }
    

    if (display_get_mode() == DISPLAY_MODE_TOUCH_KEY) {
        if (!touch_signal_latch) {
            touch_gesture_clear_key_position();
        }
        if (timer_elapsed(last_touch_time) > touch_gesture_repeat_interval()) {
            if (read_touch()) {
                changed = true;
                last_touch_time = timer_read();
                touch_signal_latch = false;
            }
        }
    }

    last_matrix_state = changed;
    return changed;
}

bool get_last_matrix_state(void) {
    return last_matrix_state;
}
