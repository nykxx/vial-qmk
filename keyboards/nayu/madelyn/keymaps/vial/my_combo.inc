#include "layer_define.h"

enum combos {
  KEY_ESC,
  KEY_TAB,
  KEY_RCTRL,
  KEY_RSHIFT1,
  KEY_RSHIFT2,
  KEY_LCTRL,
  KEY_LSHIFT,
  KEY_LSLC,
  KEY_LGUI,
  MO_NUM1,
  MO_NUM2,
  ALT_SPACE,
  CTRL_SPACE,
  TO_LIGHT,
};

const uint16_t PROGMEM esc_combo[] = {KC_Q, KC_L, COMBO_END};
const uint16_t PROGMEM tab_combo[] = {KC_Q, KC_E, COMBO_END};

const uint16_t PROGMEM rshift1_combo[] = {LT(MARK, KC_ENT), LT(FN, KC_BSPC), COMBO_END};
const uint16_t PROGMEM rshift2_combo[] = {KC_Z, KC_B, COMBO_END};
const uint16_t PROGMEM rctrl_combo[] = {KC_E, KC_I, COMBO_END};

const uint16_t PROGMEM lshift_combo[] = {KC_J, KC_X, COMBO_END};
const uint16_t PROGMEM shift_ctrl_combo[] = {KC_J, KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM lctrl_combo[] = {KC_S, KC_H, COMBO_END};
const uint16_t PROGMEM gui_combo[] = {KC_J, LALT_T(KC_F14), COMBO_END};

const uint16_t PROGMEM num1_combo[] = {LT(MARK, KC_SPC), LT(FN, KC_SPC), COMBO_END};
const uint16_t PROGMEM num2_combo[] = {KC_B, RALT_T(KC_F13), COMBO_END};

const uint16_t PROGMEM altspc_combo[] = {LT(MARK, KC_SPC), LT(MARK, KC_ENT), COMBO_END};
const uint16_t PROGMEM ctrlspc_combo[] = {LT(FN, KC_SPC), LT(FN, KC_BSPC), COMBO_END};

const uint16_t PROGMEM to_light_combo[] = {KC_Q, KC_P, LALT_T(KC_F14), RALT_T(KC_F13), COMBO_END};

combo_t local_key_combos[] = {
  [KEY_ESC] = COMBO(esc_combo, KC_ESC),
  [KEY_TAB] = COMBO(tab_combo, KC_TAB),
  [KEY_RCTRL] = COMBO(rctrl_combo, KC_RCTL),
  [KEY_RSHIFT1] = COMBO(rshift1_combo, KC_RSFT),
  [KEY_RSHIFT2] = COMBO(rshift2_combo, KC_RSFT),
  [KEY_LCTRL] = COMBO(lctrl_combo, KC_LCTL),
  [KEY_LSHIFT] = COMBO(lshift_combo, KC_LSFT),
  [KEY_LSLC] = COMBO(shift_ctrl_combo, C(KC_LSFT)),
  [KEY_LGUI] = COMBO(gui_combo, KC_LGUI),
  [MO_NUM1] = COMBO(num1_combo, MO(NUM)),
  [MO_NUM2] = COMBO(num2_combo, MO(NUM)),
  [ALT_SPACE] = COMBO(altspc_combo, A(KC_SPC)),
  [CTRL_SPACE] = COMBO(ctrlspc_combo, C(KC_SPC)),
  [TO_LIGHT] = COMBO(to_light_combo, TO(LIGHT)),
};

const uint16_t local_combo_size = ARRAY_SIZE(local_key_combos);
