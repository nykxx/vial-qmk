#include "layer_define.h"

#define MY_TAPPING_TERM 180

// Tap Dance declarations
enum {
    TD_FN_BSPC,
};

vial_tap_dance_entry_t td_entries[] = {
  [TD_FN_BSPC] = { KC_BSPC, MO(FN), KC_BSPC, KC_BSPC, MY_TAPPING_TERM }
};

const uint16_t local_tapdance_size = ARRAY_SIZE(td_entries);
