// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later
#include <math.h>
#include "config_omni.h"
#include "dynamic_keymap.h"
#include "trackball_omni.h"

uint8_t hue_bg = 0;
uint8_t sat_bg = 0;
uint8_t val_bg = 0;
uint8_t hue_main_color = 255;
uint8_t sat_main_color = 0;
uint8_t val_main_color = 255;
uint8_t hue_sub_color = 255;
uint8_t sat_sub_color = 0;
uint8_t val_sub_color = 255;

float speed_adjust1;
int slope_factor1;
float speed_adjust2;
int slope_factor2; 

uint16_t initial_touch_time;
bool keymap_change_update_flag = true;

void save_omni_color_config(void) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(3,4,0, 0x7700 + hue_bg);
    dynamic_keymap_set_keycode(3,4,1, 0x7700 + sat_bg);
    dynamic_keymap_set_keycode(3,4,2, 0x7700 + val_bg);
    dynamic_keymap_set_keycode(3,5,0, 0x7700 + hue_main_color);
    dynamic_keymap_set_keycode(3,5,1, 0x7700 + sat_main_color);
    dynamic_keymap_set_keycode(3,5,2, 0x7700 + val_main_color);
    dynamic_keymap_set_keycode(3,5,3, 0x7700 + hue_sub_color);
    dynamic_keymap_set_keycode(3,5,4, 0x7700 + sat_sub_color);
    dynamic_keymap_set_keycode(3,5,5, 0x7700 + val_sub_color);
    keymap_change_update_flag = true;
}

void load_omni_color_config(void) {
    hue_bg = dynamic_keymap_get_keycode(3,4,0) - 0x7700;
    sat_bg = dynamic_keymap_get_keycode(3,4,1) - 0x7700;
    val_bg = dynamic_keymap_get_keycode(3,4,2) - 0x7700;
    hue_main_color = dynamic_keymap_get_keycode(3,5,0) - 0x7700;
    sat_main_color = dynamic_keymap_get_keycode(3,5,1) - 0x7700;
    val_main_color = dynamic_keymap_get_keycode(3,5,2) - 0x7700;
    hue_sub_color = dynamic_keymap_get_keycode(3,5,3) - 0x7700;
    sat_sub_color = dynamic_keymap_get_keycode(3,5,4) - 0x7700;
    val_sub_color = dynamic_keymap_get_keycode(3,5,5) - 0x7700;
}

void save_omni_tb_config(void) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(3,7,0, 0x7700 + (int)round(speed_adjust1 * 10));
    dynamic_keymap_set_keycode(3,7,1, 0x7700 + slope_factor1);
    dynamic_keymap_set_keycode(3,7,2, 0x7700 + (int)round(speed_adjust2 * 10));
    dynamic_keymap_set_keycode(3,7,3, 0x7700 + slope_factor2);
    keymap_change_update_flag = true;
}

void load_omni_tb_config(void) {
    speed_adjust1 =  (dynamic_keymap_get_keycode(3,7,0) - 0x7700) / 10.0f;
    slope_factor1 =  dynamic_keymap_get_keycode(3,7,1) - 0x7700;
    speed_adjust2 =  (dynamic_keymap_get_keycode(3,7,2) - 0x7700) / 10.0f;
    slope_factor2 =  dynamic_keymap_get_keycode(3,7,3) - 0x7700;
    if (speed_adjust1 > 3.0f || speed_adjust1 < 0.1f) {
        speed_adjust1 = DEFAULT_SPEED_ADJUST1;
    }
    if (slope_factor1 > 100 || slope_factor1 < 10) {
        slope_factor1 = DEFAULT_SLOPE_FACTOR1;
    }
    if (speed_adjust2 > 3.0f || speed_adjust2 < 0.1f) {
        speed_adjust2 = DEFAULT_SPEED_ADJUST2;
    }
    if (slope_factor2 > 100 || slope_factor2 < 10) {
        slope_factor2= DEFAULT_SLOPE_FACTOR2;
    }


    if (dynamic_keymap_get_keycode(3,8,0) == 0x7701) {
        ud_sc_mode_flag = -1;
    } else if (dynamic_keymap_get_keycode(3,8,0) == 0x7700) {
        ud_sc_mode_flag = 1;
    }

    if (dynamic_keymap_get_keycode(3,8,1) == 0x7701) {
        lr_sc_mode_flag = -1;
    } else if (dynamic_keymap_get_keycode(3,8,1) == 0x7700) {
        lr_sc_mode_flag = 1;
    }
}

void save_omni_LCD_config(void) {
    keymap_change_update_flag = false;
    dynamic_keymap_set_keycode(3,10,0, 0x770A + ((initial_touch_time - 100) / 10));
    keymap_change_update_flag = true;
}
void load_omni_LCD_config(void) {
    initial_touch_time = (dynamic_keymap_get_keycode(3,10,0) - 0x770A) * 10 + 100;
    if (initial_touch_time < 100) {
        initial_touch_time = 100;
        keymap_change_update_flag = false;
        dynamic_keymap_set_keycode(3,10,0, 0x770A + ((initial_touch_time - 100) / 10));
        keymap_change_update_flag = true;
    }else if (initial_touch_time > 2000) {
        initial_touch_time = 500;
        keymap_change_update_flag = false;
        dynamic_keymap_set_keycode(3,10,0, 0x770A + ((initial_touch_time - 100) / 10));   
        keymap_change_update_flag = true;
    }

}