#pragma once
#include "quantum.h"
#include <stdint.h>

#define DEFAULT_SPEED_ADJUST1 1.6f
#define DEFAULT_SLOPE_FACTOR1 50
#define DEFAULT_SPEED_ADJUST2 1.4f
#define DEFAULT_SLOPE_FACTOR2 70

extern bool keymap_change_update_flag;

extern uint8_t hue_bg;
extern uint8_t sat_bg;
extern uint8_t val_bg;
extern uint8_t hue_main_color;
extern uint8_t sat_main_color;
extern uint8_t val_main_color;
extern uint8_t hue_sub_color;
extern uint8_t sat_sub_color;
extern uint8_t val_sub_color;

extern float speed_adjust1;
extern int slope_factor1;
extern float speed_adjust2;
extern int slope_factor2; 

extern uint16_t initial_touch_time;

void save_omni_color_config(void);
void load_omni_color_config(void);
void save_omni_tb_config(void);
void load_omni_tb_config(void);
void save_omni_LCD_config(void);
void load_omni_LCD_config(void);