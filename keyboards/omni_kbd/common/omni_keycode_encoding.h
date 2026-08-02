// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

/*
 * Compatibility encoding used by the original firmware. The same numeric
 * base is shared by Vial macro keycodes, LCD icon indices, and values stored
 * in otherwise-unused dynamic-keymap cells. Keep the semantic wrappers below
 * distinct even though their current representation is identical.
 */
enum {
    OMNI_VIAL_VALUE_KEYCODE_BASE = 0x7700,
};

#define OMNI_ENCODED_VALUE(value) ((uint16_t)(OMNI_VIAL_VALUE_KEYCODE_BASE + (value)))
#define OMNI_MACRO_KEYCODE(macro_index) OMNI_ENCODED_VALUE(macro_index)
#define OMNI_ICON_KEYCODE(icon_index) OMNI_ENCODED_VALUE(icon_index)
#define OMNI_STORED_VALUE_KEYCODE(value) OMNI_ENCODED_VALUE(value)

static inline uint16_t omni_stored_value_encode(uint16_t value) {
    return OMNI_STORED_VALUE_KEYCODE(value);
}

static inline int32_t omni_stored_value_decode(uint16_t keycode) {
    return (int32_t)keycode - OMNI_VIAL_VALUE_KEYCODE_BASE;
}

static inline int32_t omni_icon_index_from_keycode(uint16_t keycode) {
    return (int32_t)keycode - OMNI_VIAL_VALUE_KEYCODE_BASE;
}

static inline uint8_t omni_stored_u8_decode_or_zero(uint16_t keycode) {
    return keycode >= OMNI_VIAL_VALUE_KEYCODE_BASE ? (uint8_t)omni_stored_value_decode(keycode) : 0;
}
