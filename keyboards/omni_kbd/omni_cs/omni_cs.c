// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <string.h>
#include <print.h>
#include "wait.h"
#include "timer.h"
#include "i2c_master.h"
#include "gpio.h"
#include "qp.h"
#include "matrix.h"
#include "config.h"
#include "omni_cs.h"
#include "dynamic_keymap.h"

#include "../common/trackball_omni.h"
#include "../common/config_omni.h"
#include "../common/sleeping_view.h"
#include "../common/draw_custom.h"
#include "../common/view_keymap.h"
#include "../common/power_lcd.h"
#include "../drivers/pmw33xx_common.h"
#include "../font/noto11.qff.h"
#include "../font/roboto_mono16.qff.h"
#include "../font/st2_mono16.qff.h"
#include "../icon/omni_image_loader.h"

bool matrix_changed = false;

static bool tb_state = false;
trackball_mode_t tb_mode_r = TRACKBALL_CURSOR;
trackball_mode_t tb_mode_l = TRACKBALL_TAP;

bool is_first_frame = true;  
bool is_second_frame = true;  
static deferred_token my_anim;
// const uint32_t update_interval = 50;  
static uint8_t pre_layer = 0; 

uint32_t lcd_current_time = 0;
uint32_t lcd_fast_res_time = 0;
point_t home_point = {TOUCH_LCD_WIDTH / 2, TOUCH_LCD_HEIGHT / 2};
point_t circles[] = {{200, 120}, {160, 189}, {80, 189}, {40, 120}, {80, 51}, {160, 51}};
uint16_t sleeping_timer;
bool sleeping_state = false;
uint16_t draw_matrix_code_rain_timer = 0;
bool fast_draw_matrix_code_rain = false;
// bool fast_draw_radar_view = false;
// uint16_t draw_radar_view_timer = 0;

// point_t home_point = {TOUCH_LCD_WIDTH / 2, 55};
// point_t circles[] = {{60, 125}, {180, 125}, {60, 195}, {180, 195}, {60 , 265}, {180, 265}};
uint16_t virtual_keycode[KEYCODE_SIZE];
// uint16_t pre_virtual_keycode[KEYCODE_SIZE];
uint16_t import_keymaps[MATRIX_ROWS / 2][MATRIX_COLS];
static uint16_t blink_start_time = 0;
static bool is_backlight_off = false;
bool lcd_is_on = true;
// uint16_t last_usb_check_time = 0;

void initialize_lcd_layer_app_images(void) {
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 3; i++) {
            lcd_layer_app_images[i][j][0] = (ImagePosition){get_layer_img_func(i + j*3), home_point.x, home_point.y};
            for (int k = 0; k < 6; k++) {
                lcd_layer_app_images[i][j][k + 1] = (ImagePosition){get_img_func(virtual_keycode[k + i*6 + j*18]), circles[k].x, circles[k].y};
            }
        }
    }
}

void update_lcd_view_data(void){
    // if (memcmp(virtual_keycode, pre_virtual_keycode, KEYCODE_SIZE * sizeof(uint16_t)) != 0) {
    //     memcpy(pre_virtual_keycode, virtual_keycode, KEYCODE_SIZE * sizeof(uint16_t));
        draw_background_all_black();
        initialize_lcd_layer_app_images();
        draw_lcd_layer_category_images();
    // }
}

void load_virtual_keys(void) {
    for (int row = 0; row < MATRIX_ROWS / 2; row++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            import_keymaps[row][j] = keymap_key_to_keycode(1, (keypos_t){.row = row, .col = j});
        }
    }
    int i = 0;
    for (int row = 0; row < MATRIX_ROWS / 2; row++) {
        if (row == 0 || row == 1 || row == 2 || row == 3) {
            continue;
        }
        for (int col = 0; col < MATRIX_COLS; col++) {
            virtual_keycode[i] = import_keymaps[row][col];
            i++;
        }
    }
}

void pointing_device_init_kb(void) {
    pmw33xx_init(0);         
    pmw33xx_init(1);         
    pmw33xx_set_cpi(0, 3000);
    pmw33xx_set_cpi(1, 3000);
    pointing_device_init_user();
}

void keyboard_post_init_kb(void) {
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }
    load_omni_tb_config(); 
    load_omni_color_config();
    load_omni_LCD_config();
    display = qp_gc9a01_make_spi_device(TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, CS_PIN, DC_PIN, RST_PIN, 4, 0); // パネル幅, パネル高さ,,,,SPIディバイザ,SPIモード
    qp_init(display, QP_ROTATION_0);
    power_lcd_init(display, BLK_PIN);
    noto11_font = qp_load_font_mem(font_noto11); 
    roboto_mono16 = qp_load_font_mem(font_roboto_mono16); 
    st2_mono16 = qp_load_font_mem(font_st2_mono16); 
    uint8_t init_status = cst816t_init(2);
    if (init_status == 0) {
        uprintf("Touch initialization failed.\n");
    } else {
        uprintf("Touch initialization successful.\n");
    }
    draw_background_all_black();
    initialize_images();
    qp_flush(display);
    if (image_logo != NULL) {
        int x = (TOUCH_LCD_WIDTH - image_logo->width) / 2;
        int y = (TOUCH_LCD_HEIGHT - image_logo->height) / 2;
        my_anim = qp_animate(display, x, y, image_logo);
        lcd_fast_res_time = timer_read();
    }
}

void matrix_init_user(void) {
    setPinOutput(BLK_PIN);
    writePinHigh(BLK_PIN);
    i2c_init();
    setPinInputHigh(INT_PIN); 
    initialize_lcd_layer_app_images();
    load_virtual_keys();
}

// bool high_res_scroll_mode = false;

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    current_layer = get_highest_layer(layer_state);
    if(display_mode == DISPLAY_MODE_SWIPE_GESTURE) {
        if (current_layer != pre_layer) {
            swipe_gesture_main_view_update(current_layer);
        }
    } else if (display_mode ==  DISPLAY_MODE_KEY_MATRIX) {
        if (!get_auto_mouse_enable()) {
            if(current_layer != pre_layer){
                draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
            }            
        } else {
            if (current_layer != 3){
                if(pre_layer !=  3){
                    if(current_layer != pre_layer){
                        draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
                    }
                }
            }
        }
    }
    pre_layer = current_layer;

    pmw33xx_report_t report0 = pmw33xx_read_burst(0); // Sensor #1
    pmw33xx_report_t report1 = pmw33xx_read_burst(1); // Sensor #2

    if (report0.motion.b.is_motion || report1.motion.b.is_motion) {
        tb_state = true;
    }else {
        tb_state = false;
    }

    if (tb_mode_r == TRACKBALL_CURSOR){
        process_cursor_report(&mouse_report, report0, speed_adjust1, slope_factor1, 1, 1, 2);
    } else if (tb_mode_r == TRACKBALL_TAP) {
        // process_tap_report(&mouse_report, report0, speed_adjust2, slope_factor2, -1, -1, 10);
        process_high_res_scroll_report(&mouse_report, report0, speed_adjust2, slope_factor2, 1, -1, 3);
    }

    if (tb_mode_l == TRACKBALL_CURSOR){
        process_cursor_report(&mouse_report, report1, speed_adjust1, slope_factor1, -1, -1, 2);
    } else if (tb_mode_l == TRACKBALL_TAP) {
        // if (high_res_scroll_mode){
            process_high_res_scroll_report(&mouse_report, report1, speed_adjust2, slope_factor2, -1, 1, 3);
        // } else if (!high_res_scroll_mode){
        //     process_tap_report(&mouse_report, report1, speed_adjust2, slope_factor2, 1, 1, 10);
        // }
    }

    if (ENABLE_TOUCH_UPDATE == 1) {
        if (touch_signal_view_update) {
            if (!is_backlight_off) {
                writePinLow(BLK_PIN);
                blink_start_time = timer_read();
                is_backlight_off = true;
            }
            if (is_backlight_off && timer_elapsed(blink_start_time) >= 50) {
                writePinHigh(BLK_PIN);
                is_backlight_off = false;
                touch_signal_view_update = false;
            }
        }
    }
    if (!is_first_frame) {
        if (is_second_frame) {
            draw_lcd_layer_category_images();
            qp_flush(display);
            is_second_frame = false;
        }
    }
    return pointing_device_task_user(mouse_report);
}

void matrix_scan_user(void) {
    process_touch_interrupt();
    // load_virtual_keys();
    // update_lcd_view_data();
}

void sleeping_kb(bool matrix_changed) {
    if (matrix_changed || tb_state || touch_start_flag){ 
        sleeping_timer = timer_read();
        if (sleeping_state) {
            if (!lcd_is_on){
                lcd_is_on = power_on_lcd();
            }
            display_redraw();
            sleeping_state = false;
        }
    }
    if (!sleeping_state) {
        if (timer_elapsed(sleeping_timer) >= SLEEPING_KB_TIME) {
            lcd_is_on = power_off_lcd();
            sleeping_state = true;
        }
    } else if (SLEEP_VIEW == 0) {
        return;
    } else if(SLEEP_VIEW == 1) {
        if (timer_elapsed(draw_matrix_code_rain_timer) > 50) {
            draw_matrix_code_rain_timer = timer_read();
            if (!lcd_is_on){
                lcd_is_on = power_on_lcd();
            }
            if (!fast_draw_matrix_code_rain) {
                init_matrix_code_rain();
                fast_draw_matrix_code_rain = true;
            }
            update_matrix_code_rain();
            draw_matrix_code_rain(display, noto11_font);
        }
    } 
}

void housekeeping_task_user(void) {
    lcd_current_time = timer_read();
    if (lcd_current_time - lcd_fast_res_time > 3000) {
        if (is_first_frame) {
            is_first_frame = false;
            qp_stop_animation(my_anim);
            draw_background_all_black();
            initialize_lcd_layer_app_images();
        }
    }
    matrix_changed = get_last_matrix_state();
    sleeping_kb(matrix_changed);

    if (touch_start_flag) {
        if (timer_elapsed(touch_start_timer) >= TOUCH_DEBOUNCE_TIME) {
            touch_start_flag = false;
            initial_touch_flag = false;

            gesture_id = GESTURE_NONE;
        }   
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        tb_mode_l = TRACKBALL_TAP;
        tb_mode_r = TRACKBALL_CURSOR;
        return true;
    }
    switch (keycode) {
        case KC_hue_bg_UP:
            hue_bg = (hue_bg + 16) % 256;
            // if(high_res_scroll_mode) {
            //     high_res_scroll_mode = false;
            // } else if(!high_res_scroll_mode){
            //     high_res_scroll_mode = true;
            // }
            // デバッグ用
            // uprintf("--------------------------------------\n");
            // uprintf("x : %d, y : %d\n", touch_x, touch_y);
            // uprintf("px: %d, py: %d\n", pre_touch_x, pre_touch_y);
            break;
        case KC_hue_bg_DOWN:
            hue_bg = (hue_bg >= 16) ? (hue_bg - 16) : (hue_bg + 240); 
            break;
        case KC_sat_bg_UP:
            sat_bg = (sat_bg + 16 <= 254) ? sat_bg + 16 : 254;
            break;
        case KC_sat_bg_DOWN:
            sat_bg = (sat_bg > 16) ? sat_bg - 16 : 0;
            break;
        case KC_val_bg_UP:
            val_bg = (val_bg + 16 <= 254) ? val_bg + 16 : 254;
            break;
        case KC_val_bg_DOWN:
            val_bg = (val_bg > 16) ? val_bg - 16 : 0;
            break;
        case KC_hue_main_color_UP:
            hue_main_color = (hue_main_color + 16) % 256;
            break;
        case KC_hue_main_color_DOWN:
            hue_main_color = (hue_main_color >= 16) ? (hue_main_color - 16) : (hue_main_color + 240);
            break;
        case KC_sat_main_color_UP:
            sat_main_color = (sat_main_color + 16 <= 254) ? sat_main_color + 16 : 254;
            break;
        case KC_sat_main_color_DOWN:
            sat_main_color = (sat_main_color > 16) ? sat_main_color - 16 : 0;
            break;
        case KC_val_main_color_UP:
            val_main_color = (val_main_color + 16 <= 254) ? val_main_color + 16 : 254;
            break;
        case KC_val_main_color_DOWN:
            val_main_color = (val_main_color > 16) ? val_main_color - 16 : 0;
            break;
        case KC_hue_sub_color_UP:
            hue_sub_color = (hue_sub_color + 16) % 256;
            break;
        case KC_hue_sub_color_DOWN:
            hue_sub_color = (hue_sub_color >= 16) ? (hue_sub_color - 16) : (hue_sub_color + 240);
            break;
        case KC_sat_sub_color_UP:
            sat_sub_color = (sat_sub_color + 16 <= 254) ? sat_sub_color + 16 : 254;
            break;
        case KC_sat_sub_color_DOWN:
            sat_sub_color = (sat_sub_color > 16) ? sat_sub_color - 16 : 0;
            break;
        case KC_val_sub_color_UP:
            val_sub_color = (val_sub_color + 16 <= 254) ? val_sub_color + 16 : 254;
            break;
        case KC_val_sub_color_DOWN:
            val_sub_color = (val_sub_color > 16) ? val_sub_color - 16 : 0;
            break;
        case AUTO_MOUSE_TOGGLE:
            if (dynamic_keymap_get_keycode(3,9,0) == 0x7700) {
                dynamic_keymap_set_keycode(3,9,0, 0x7701);
                set_auto_mouse_enable(true);
            } else if (dynamic_keymap_get_keycode(3,9,0) == 0x7701) {
                dynamic_keymap_set_keycode(3,9,0, 0x7700);
                set_auto_mouse_enable(false);
            }
            break;
        case TB_R_MODE_TOGGLE:
            if (record->event.pressed) {
                tb_mode_r = TRACKBALL_TAP;
            } else {
                tb_mode_r = TRACKBALL_CURSOR;
            }
            return false;
        case TB_L_MODE_TOGGLE:
            tb_mode_l = TRACKBALL_CURSOR;
            return false;

        case TB_UD_SC_MODE_TOGGLE:
            if (dynamic_keymap_get_keycode(3,8,0) == 0x7700) {
                dynamic_keymap_set_keycode(3,8,0, 0x7701);
                ud_sc_mode_flag = -1;
            } else if (dynamic_keymap_get_keycode(3,8,0) == 0x7701) {
                dynamic_keymap_set_keycode(3,8,0, 0x7700);
                ud_sc_mode_flag = 1;
            }
            break;

        case TB_LR_SC_MODE_TOGGLE:
            if (dynamic_keymap_get_keycode(3,8,1) == 0x7700) {
                dynamic_keymap_set_keycode(3,8,1, 0x7701);
                lr_sc_mode_flag = -1;
            } else if (dynamic_keymap_get_keycode(3,8,1) == 0x7701) {
                dynamic_keymap_set_keycode(3,8,1, 0x7700);
                lr_sc_mode_flag = 1;
            }
            break;

        case KC_DP_TOUCH_KEY:
            display_mode =  DISPLAY_MODE_TOUCH_KEY;
            draw_background_all_black();
            draw_lcd_layer_category_images();
            break;
        case KC_DP_TB_TUNE: 
            display_mode =  DISPLAY_MODE_TRACKBALL_TUNING;
            show_trackball_tuning_mode();
            break;
        case KC_DP_SWIPE_GESTURE:
            display_mode =  DISPLAY_MODE_SWIPE_GESTURE;
            draw_background_all();
            swipe_gesture_layer_view_update();
            swipe_gesture_base_view_update();
            swipe_gesture_main_view_update(current_lcd_layer);
            break;
        case KC_DP_KEY_MAT:
            display_mode =  DISPLAY_MODE_KEY_MATRIX;
            draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
            break;
        default:
            return true;
    }
    if(display_mode == DISPLAY_MODE_SWIPE_GESTURE) {
        save_omni_color_config();
        draw_background_all();
        swipe_gesture_base_view_update();
        swipe_gesture_main_view_update(current_lcd_layer);
        swipe_gesture_layer_view_update();
    }
    return false;
}

void suspend_power_down_user(void){
    lcd_is_on = power_off_lcd();
    sleeping_state = true;
};


// static void test(uint8_t layer, uint8_t row, uint8_t col, uint16_t kc) {
    // static bool first_time = true;
    // if (first_time) {
        // uprintf("Keymap changed → L%d R%d C%d KC:%04X\n",
        //         layer, row, col, kc);
    //     first_time = false;
    // }
//     wait_ms(100);
// }

void __real_dynamic_keymap_set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);

void __wrap_dynamic_keymap_set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {
    __real_dynamic_keymap_set_keycode(layer, row, col, keycode);
    if (keymap_change_update_flag){
        load_virtual_keys();
        update_lcd_view_data();
        // test(layer, row, col, keycode);
    }
}