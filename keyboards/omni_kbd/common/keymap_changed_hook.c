// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later


#include "quantum.h"          
#include "dynamic_keymap.h"   
#include <print.h>

static void test(uint8_t layer, uint8_t row, uint8_t col, uint16_t kc);

void __real_dynamic_keymap_set_keycode(uint8_t layer,
                                       uint8_t row,
                                       uint8_t col,
                                       uint16_t keycode);


void __wrap_dynamic_keymap_set_keycode(uint8_t layer,
                                       uint8_t row,
                                       uint8_t col,
                                       uint16_t keycode) {
    __real_dynamic_keymap_set_keycode(layer, row, col, keycode);

    test(layer, row, col, keycode);
}


static void test(uint8_t layer, uint8_t row,
                 uint8_t col, uint16_t kc) {

    static bool first_time = true;
    if (first_time) {
        uprintf("Keymap changed → L%d R%d C%d KC:%04X\n",
                layer, row, col, kc);
        first_time = false;
    }

    wait_ms(100);
}
