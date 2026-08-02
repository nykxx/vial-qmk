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
#include "dynamic_keymap.h"
#include "omni_cs.h"
#include "os_detection.h"

#include "../common/trackball_omni.h"
#include "../common/config_omni.h"
#include "../common/sleeping_view.h"
#include "../common/draw_custom.h"
#include "../common/view_keymap.h"
#include "../common/power_lcd.h"
#include "../common/status_view.h"
#include "../common/swipe_gesture.h"
#include "../common/touch_gesture.h"
#include "../common/touch_key_view.h"
#include "../drivers/pmw33xx_common.h"
#include "../font/noto9.qff.h"
#include "../font/noto11.qff.h"
#include "../font/roboto_mono16.qff.h"
#include "../font/st2_mono16.qff.h"
#include "../icon/omni_image_loader.h"

static bool tb_state = false;
static trackball_scroll_state_t tb_scroll_state_r = {0};
static trackball_scroll_state_t tb_scroll_state_l = {0};
static trackball_mode_t tb_mode_r = TRACKBALL_CURSOR;
static trackball_mode_t tb_mode_l = TRACKBALL_TAP;
static bool is_first_frame = true;
static bool is_second_frame = true;
static deferred_token my_anim;
static uint8_t pre_layer = 0;
static uint32_t lcd_fast_res_time = 0;
static uint16_t sleeping_timer;
static bool sleeping_state = false;
static uint16_t draw_matrix_code_rain_timer = 0;
static bool fast_draw_matrix_code_rain = false;
static uint16_t virtual_keycode[KEYCODE_SIZE];
static uint16_t blink_start_time = 0;
static bool is_backlight_off = false;
static bool lcd_is_on = true;
static uint8_t current_layer;

enum {
    LCD_LAYER_COUNT        = MAX_LCD_LAYER + 1,
    LCD_CATEGORY_COUNT     = MAX_LCD_CATEGORY + 1,
    LCD_BACKLIGHT_BLINK_MS = 50,
    STARTUP_ANIMATION_MS   = 3000,
    STARTUP_SETTLE_MS      = 300,
    LCD_SPI_DIVISOR        = 4,
    TOUCH_CONTROLLER_MODE  = 2,
};

static void update_layer_display(void) {
    if (current_layer == pre_layer) {
        return;
    }

    display_mode_t current_mode = display_get_mode();
    if (current_mode == DISPLAY_MODE_SWIPE_GESTURE) {
        swipe_gesture_draw_main(display, noto11_font, current_layer);
    } else if (current_mode == DISPLAY_MODE_KEY_MATRIX) {
        bool should_redraw = !get_auto_mouse_enable() || (current_layer != _MOUSE && pre_layer != _MOUSE);
        if (should_redraw) {
            draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
        }
    }
}

static void update_touch_feedback(void) {
    if (ENABLE_TOUCH_UPDATE != 1 || !touch_gesture_feedback_pending()) {
        return;
    }

    if (!is_backlight_off) {
        writePinLow(BLK_PIN);
        blink_start_time = timer_read();
        is_backlight_off = true;
    }
    if (timer_elapsed(blink_start_time) >= LCD_BACKLIGHT_BLINK_MS) {
        writePinHigh(BLK_PIN);
        is_backlight_off = false;
        touch_gesture_clear_feedback();
    }
}

static void draw_second_frame(void) {
    if (!is_first_frame && is_second_frame) {
        touch_key_view_draw(display);
        qp_flush(display);
        is_second_frame = false;
    }
}

static bool process_color_keycode(uint16_t keycode) {
    switch (keycode) {
        case KC_hue_bg_UP:
            hue_bg = (hue_bg + 16) % 256;
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
        default:
            return false;
    }
    return true;
}

static bool process_display_keycode(uint16_t keycode) {
    display_mode_t selected_mode;
    switch (keycode) {
        case KC_DP_TOUCH_KEY:
            selected_mode = DISPLAY_MODE_TOUCH_KEY;
            break;
        case KC_DP_TB_TUNE:
            selected_mode = DISPLAY_MODE_TRACKBALL_TUNING;
            break;
        case KC_DP_SWIPE_GESTURE:
            selected_mode = DISPLAY_MODE_SWIPE_GESTURE;
            break;
        case KC_DP_KEY_MAT:
            selected_mode = DISPLAY_MODE_KEY_MATRIX;
            break;
        case KC_DP_STAT1:
            selected_mode = DISPLAY_MODE_STATUS1;
            break;
        default:
            return false;
    }

    display_set_mode(selected_mode);
    display_redraw(current_layer);
    return true;
}

static void refresh_swipe_gesture_view(void) {
    save_omni_color_config();
    draw_background_all();
    swipe_gesture_draw_base(display);
    swipe_gesture_draw_main(display, noto11_font, touch_key_view_current_layer());
    swipe_gesture_draw_profile(display, noto11_font);
}

static void load_persistent_config(void) {
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }
    load_omni_tb_config();
    load_omni_color_config();
    persist_load_all();
}

static void initialize_display(void) {
    display = qp_gc9a01_make_spi_device(TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, CS_PIN, DC_PIN, RST_PIN, LCD_SPI_DIVISOR, 0);
    qp_init(display, QP_ROTATION_0);
    power_lcd_init(display, BLK_PIN);

    noto9_font    = qp_load_font_mem(font_noto9);
    noto11_font   = qp_load_font_mem(font_noto11);
    roboto_mono16 = qp_load_font_mem(font_roboto_mono16);
    st2_mono16    = qp_load_font_mem(font_st2_mono16);
}

static void initialize_touch_controller(void) {
    uint8_t init_status = cst816t_init(TOUCH_CONTROLLER_MODE);
    if (init_status == 0) {
        uprintf("Touch initialization failed.\n");
    } else {
        uprintf("Touch initialization successful.\n");
    }
}

static void show_startup_logo(void) {
    draw_background_all_black();
    initialize_images();
    qp_flush(display);

    painter_image_handle_t logo_image = omni_logo_image();
    if (logo_image != NULL) {
        int logo_x_coordinate = (TOUCH_LCD_WIDTH - logo_image->width) / 2;
        int logo_y_coordinate = (TOUCH_LCD_HEIGHT - logo_image->height) / 2;
        my_anim = qp_animate(display, logo_x_coordinate, logo_y_coordinate, logo_image);
        lcd_fast_res_time = timer_read();
    }
}

static void update_lcd_view_data(void){
    draw_background_all_black();
    touch_key_view_initialize(virtual_keycode);
    touch_key_view_draw(display);
}

static void load_virtual_keys(void) {
    int key_index = 0;
    for (int row = LCD_LAYER_COUNT; row < MATRIX_ROWS / 2; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            virtual_keycode[key_index] = keymap_key_to_keycode(2, (keypos_t){.row = row, .col = col});
            key_index++;
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

void matrix_init_user(void) {
    setPinOutput(BLK_PIN);
    writePinHigh(BLK_PIN);
    i2c_init();
    setPinInputHigh(INT_PIN); 
    touch_key_view_initialize(virtual_keycode);
    load_virtual_keys();
}

void keyboard_post_init_kb(void) {
    load_persistent_config();
    initialize_display();
    initialize_touch_controller();
    show_startup_logo();
    wait_ms(STARTUP_SETTLE_MS);
    sync_default_layer_to_os();
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    current_layer = get_highest_layer(layer_state);
    static bool hscroll = false;
    if (current_layer == _MARK) {
        hscroll = true;
    } else if (current_layer == _MOUSE && hscroll) {
        // auto mouse layerによってマウスレイヤーに切り替わるので
        // hscrollフラグが立っている場合そのままにする
    } else {
        hscroll = false;
    }

    update_layer_display();
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
        process_high_res_scroll_report(&mouse_report, report0, &tb_scroll_state_r, speed_adjust2, slope_factor2, -1, 1, 3);
    }

    if (tb_mode_l == TRACKBALL_CURSOR){
        process_cursor_report(&mouse_report, report1, speed_adjust1, slope_factor1, -1, -1, 2);
    } else if (tb_mode_l == TRACKBALL_TAP) {
        process_high_res_scroll_report(&mouse_report, report1, &tb_scroll_state_l, speed_adjust2, slope_factor2, 1, -1, 3);
    }

    update_touch_feedback();
    draw_second_frame();
    return pointing_device_task_user(mouse_report);
}


void matrix_scan_user(void) {
    process_touch_interrupt();
}

static void sleeping_kb(bool matrix_changed) {
    if (matrix_changed || tb_state) {
        sleeping_timer = timer_read();
        if (sleeping_state) {
            if (!lcd_is_on){
                lcd_is_on = power_on_lcd();
            }
            display_redraw(current_layer);
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
    uint32_t lcd_current_time = timer_read();
    if (lcd_current_time - lcd_fast_res_time > STARTUP_ANIMATION_MS) {
        if (is_first_frame) {
            is_first_frame = false;
            qp_stop_animation(my_anim);
            draw_background_all_black();
            touch_key_view_initialize(virtual_keycode);
        }
    }
    bool matrix_changed = get_last_matrix_state();
    sleeping_kb(matrix_changed);

}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        tb_mode_l = TRACKBALL_TAP;
        tb_mode_r = TRACKBALL_CURSOR;
        return true;
    }

    bool keycode_handled = process_color_keycode(keycode);
    if (!keycode_handled) {
        keycode_handled = process_display_keycode(keycode);
    }
    if (keycode_handled) {
        if (display_get_mode() == DISPLAY_MODE_SWIPE_GESTURE) {
            refresh_swipe_gesture_view();
        }
        return false;
    }

    switch (keycode) {
        case TB_R_MODE_TOGGLE:
            tb_mode_r = TRACKBALL_TAP;
            return false;
        case TB_L_MODE_TOGGLE:
            tb_mode_l = TRACKBALL_CURSOR;
            return false;
        default:
            return true;
    }
}

void suspend_power_down_user(void){
    lcd_is_on = power_off_lcd();
    sleeping_state = true;
};

void __real_dynamic_keymap_set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);

void __wrap_dynamic_keymap_set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {
    __real_dynamic_keymap_set_keycode(layer, row, col, keycode);
    if (keymap_change_update_flag){
        load_virtual_keys();
        update_lcd_view_data();
        load_omni_tb_config();
        load_omni_color_config();
        persist_load_all();
        display_set_mode(DISPLAY_MODE_TOUCH_KEY);
    }
}
