// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp.h"
#include "touch_lcd_omni.h"
#include "matrix.h"
#include "draw_custom.h"
#include "touch_input.h"
#include "view_keymap.h"
#include "status_view.h"
#include "../drivers/cst816t.h"
#include "../icon/omni_image_loader.h"

enum {
    TOUCH_RELEASE_GRACE_MS = 30,
};

painter_device_t display;
ImagePosition lcd_layer_app_images[MAX_LCD_CATEGORY + 1][MAX_LCD_LAYER + 1][7];
uint8_t current_layer = 0;
uint8_t current_lcd_layer = 0;
uint8_t current_lcd_category = 0;
painter_font_handle_t noto9_font;
painter_font_handle_t noto11_font;
painter_font_handle_t roboto_mono16;
painter_font_handle_t st2_mono16;
uint16_t touch_x = 0xFFFF;
uint16_t touch_y = 0xFFFF;
uint16_t touch_start_timer = 0;
bool touch_signal = false;
bool touch_start_flag = false; 
bool initial_touch_flag = false;
bool touch_signal_view_update = false;
uint8_t gesture_id = GESTURE_NONE;

display_mode_t display_mode = DISPLAY_MODE_TOUCH_KEY;

enum {
    SWIPE_PROFILE_COUNT = 4,
};

typedef enum {
    SWIPE_MODIFIER_NONE,
    SWIPE_MODIFIER_CONTROL,
    SWIPE_MODIFIER_CONTROL_GUI,
} swipe_modifier_t;

typedef struct {
    const char *right_label;
    const char *left_label;
    const char *up_label;
    uint8_t active_indicator_x;
    uint8_t previous_indicator_x;
} swipe_view_config_t;

typedef struct {
    uint8_t right_keycode;
    uint8_t left_keycode;
    uint8_t up_keycode;
    swipe_modifier_t modifier;
} swipe_action_config_t;

static const swipe_view_config_t swipe_view_configs[SWIPE_PROFILE_COUNT] = {
    {.right_label = "NEXT", .left_label = "BACK", .up_label = "Save", .active_indicator_x = 90,  .previous_indicator_x = 150},
    {.right_label = "PG>",  .left_label = "<PG",  .up_label = "NEW",  .active_indicator_x = 110, .previous_indicator_x = 90},
    {.right_label = "VD>",  .left_label = "<VD",  .up_label = "VDNEW", .active_indicator_x = 130, .previous_indicator_x = 110},
    {.right_label = "CST1", .left_label = "CST2", .up_label = "CST3", .active_indicator_x = 150, .previous_indicator_x = 130},
};

static const swipe_action_config_t swipe_action_configs[SWIPE_PROFILE_COUNT] = {
    {.right_keycode = KC_Y,     .left_keycode = KC_Z,    .up_keycode = KC_S,   .modifier = SWIPE_MODIFIER_CONTROL},
    {.right_keycode = KC_PGDN,  .left_keycode = KC_PGUP, .up_keycode = KC_N,   .modifier = SWIPE_MODIFIER_CONTROL},
    {.right_keycode = KC_RIGHT, .left_keycode = KC_LEFT, .up_keycode = KC_D,   .modifier = SWIPE_MODIFIER_CONTROL_GUI},
    {.right_keycode = KC_F13,   .left_keycode = KC_F14,  .up_keycode = KC_F15, .modifier = SWIPE_MODIFIER_NONE},
};

static uint8_t swipe_layer = 0;

static float pre_speed_adjust1;
static int pre_slope_factor1;
static float pre_speed_adjust2;
static int pre_slope_factor2;
static uint8_t hue_tbtune_r = 220;
static uint8_t sat_tbtune_r = 255;
static uint8_t val_tbtune_r = 255;
static uint8_t hue_tbtune_l = 135;
static uint8_t sat_tbtune_l = 255;
static uint8_t val_tbtune_l = 255;
static const uint8_t x_pos1 = TOUCH_LCD_WIDTH / 2 + 50;
static const uint8_t y_pos1 = TOUCH_LCD_HEIGHT / 2 - 10;
static const uint8_t x_pos2 = TOUCH_LCD_WIDTH / 2 - 50;
static const uint8_t y_pos2 = TOUCH_LCD_HEIGHT / 2 - 10;
static const uint8_t y_pos3 = TOUCH_LCD_HEIGHT / 2 - 50;
static const uint8_t y_pos4 = TOUCH_LCD_HEIGHT / 2 + 45;
static const uint8_t x_pos_save = TOUCH_LCD_WIDTH / 2;
static const uint8_t y_pos_save = TOUCH_LCD_HEIGHT / 2 + 90;
static int16_t text1_width, text3_width, text4_width, text5_width, text6_width = 0;
static const char *const text1 = "TB TUNE";
static const char *const text3 = "Right";
static const char *const text4 = "Left";
static const char *const text5 = "+      +";
static const char *const text6 = "-      -";

static void process_touch_trackball_tuning_mode(uint16_t touch_x, uint16_t touch_y);

painter_image_handle_t* get_layer_img_func(uint16_t layer_count) {
    static painter_image_handle_t* layer_image_ptrs[12] = {
        &layer_00, &layer_01, &layer_02, &layer_03, &layer_04, &layer_05, &layer_06, &layer_07, &layer_08, &layer_09, &layer_10, &layer_11
    };
    return layer_image_ptrs[layer_count];
}

painter_image_handle_t* get_img_func(uint16_t keycode) {
    static painter_image_handle_t* image_ptrs[MACRO_KEY_COUNT] = {
        &image_000, &image_001, &image_002, &image_003, &image_004, &image_005, &image_006, &image_007, &image_008, &image_009, &image_010, &image_011, &image_012, &image_013, &image_014, &image_015, &image_016, &image_017, &image_018, &image_019, &image_020, &image_021, &image_022, &image_023, &image_024, &image_025, &image_026, &image_027, &image_028, &image_029, &image_030, &image_031, &image_032, &image_033, &image_034, &image_035, &image_036, &image_037, &image_038, &image_039, &image_040, &image_041, &image_042, &image_043, &image_044, &image_045, &image_046, &image_047, &image_048, &image_049, &image_050, &image_051, &image_052, &image_053, &image_054, &image_055, &image_056, &image_057, &image_058, &image_059, &image_060, &image_061, &image_062, &image_063, &image_064, &image_065, &image_066, &image_067, &image_068, &image_069, &image_070, &image_071, &image_072, &image_073, &image_074, &image_075, &image_076, &image_077, &image_078, &image_079, &image_080, &image_081, &image_082, &image_083, &image_084, &image_085, &image_086, &image_087, &image_088, &image_089, &image_090, &image_091, &image_092, &image_093, &image_094, &image_095, &image_096, &image_097, &image_098, &image_099, &image_100, &image_101, &image_102, &image_103, &image_104, &image_105, &image_106, &image_107, &image_108, &image_109, &image_110, &image_111, &image_112, &image_113, &image_114, &image_115, &image_116, &image_117, &image_118, &image_119, &image_120, &image_121, &image_122, &image_123, &image_124, &image_125, &image_126, &image_127, &image_128, &image_129, &image_130, &image_131, &image_132, &image_133, &image_134, &image_135, &image_136, &image_137, &image_138, &image_139, &image_140, &image_141, &image_142, &image_143, &image_144, &image_145, &image_146, &image_147, &image_148, &image_149, &image_150, &image_151, &image_152, &image_153, &image_154, &image_155, &image_156, &image_157, &image_158, &image_159, &image_160, &image_161, &image_162, &image_163, &image_164, &image_165, &image_166, &image_167, &image_168, &image_169, &image_170, &image_171, &image_172, &image_173, &image_174, &image_175, &image_176, &image_177, &image_178, &image_179, &image_180, &image_181, &image_182, &image_183, &image_184, &image_185, &image_186, &image_187, &image_188, &image_189, &image_190, &image_191, &image_192, &image_193, &image_194, &image_195, &image_196, &image_197, &image_198, &image_199, &image_200, &image_201, &image_202, &image_203, &image_204, &image_205, &image_206, &image_207, &image_208, &image_209, &image_210, &image_211, &image_212, &image_213, &image_214, &image_215, &image_216, &image_217, &image_218, &image_219, &image_220, &image_221, &image_222, &image_223, &image_224, &image_225, &image_226, &image_227, &image_228, &image_229, &image_230, &image_231, &image_232, &image_233, &image_234, &image_235, &image_236, &image_237, &image_238, &image_239, &image_240, &image_241, &image_242, &image_243, &image_244, &image_245, &image_246, &image_247, &image_248, &image_249, &image_250, &image_251, &image_252, &image_253, &image_254
    };
    if (keycode >= MACRO_KEY_START && keycode <= MACRO_KEY_END) {
        return image_ptrs[keycode - MACRO_KEY_START];
    }
    return &image_001;
}

static void draw_background(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    qp_rect(display, x1, y1, x2, y2, hue_bg, sat_bg, val_bg, true);  // HSV: H=0, S=0, V=0 (黒色)
}

static void draw_background_black(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    qp_rect(display, x1, y1, x2, y2, 0, 0, 0, true);  // HSV: H=0, S=0, V=0 (黒色)
}

void draw_background_all(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, hue_bg, sat_bg, val_bg, true);  // HSV: H=0, S=0, V=0 (黒色)
}

void draw_background_all_black(void) {
    qp_rect(display, 0, 0, TOUCH_LCD_WIDTH, TOUCH_LCD_HEIGHT, 0, 0, 0, true);  // HSV: H=0, S=0, V=0 (黒色)
}

void draw_lcd_layer_category_images(void) {
    ImagePosition *current_images = lcd_layer_app_images[current_lcd_category][current_lcd_layer];
    int image_count = sizeof(lcd_layer_app_images[current_lcd_category][current_lcd_layer]) / sizeof(lcd_layer_app_images[current_lcd_category][current_lcd_layer][0]);
    for (int image_index = 0; image_index < image_count; image_index++) {
        if (*current_images[image_index].image != NULL) {
            int draw_x = current_images[image_index].x_coordinate - (*current_images[image_index].image)->width / 2;
            int draw_y = current_images[image_index].y_coordinate - (*current_images[image_index].image)->height / 2;
            qp_drawimage(display, draw_x, draw_y, *current_images[image_index].image);
        }
    }
    qp_flush(display);
}

static void update_lcd_layer_category_by_gesture(void) {
    switch (gesture_id) {
        case CST816S_SLIDE_RIGHT:
            if (current_lcd_layer < MAX_LCD_LAYER) {
                current_lcd_layer++;
                draw_background_all_black();
                draw_lcd_layer_category_images();
            } else {
                current_lcd_layer = MAX_LCD_LAYER; 
            }
            break;

        case CST816S_SLIDE_LEFT:
            if (current_lcd_layer > 0) {
                current_lcd_layer--;
                draw_background_all_black();
                draw_lcd_layer_category_images();
            } else {
                current_lcd_layer = 0; 
            }
            break;

        case CST816S_SLIDE_UP:
            if (current_lcd_category > 0) {
                current_lcd_category--;
                draw_background_all_black();
                draw_lcd_layer_category_images();
            } else {
                current_lcd_category = 0;
            }
            break;

        case CST816S_SLIDE_DOWN:
            if (current_lcd_category < MAX_LCD_CATEGORY) {
                current_lcd_category++;
                draw_background_all_black();
                draw_lcd_layer_category_images();
            } else {
                current_lcd_category = MAX_LCD_CATEGORY; 
            }
            break;

        default:
            break;
    }
}
    
static void change_swipe_layer(void) {
    swipe_layer = (swipe_layer + 1) % SWIPE_PROFILE_COUNT;
}

const char* get_layer_name(uint8_t layer) {
    switch (layer) {
        case _BASE:
            return "BASE";
        case _MARK:
            return "MARK ";
        case _FUNC:
            return "FUNC";
        case _NUM:
            return "NUM";
        case _MOUSE:
            return "MOUS";
        case _CUSTOM:
            return "CUST";
        default:
            return "UNKN";
    }
}

void swipe_gesture_main_view_update(uint8_t current_layer) {
    const char *layer_text = get_layer_name(current_layer);
    uint16_t layer_text_width = qp_textwidth(noto11_font, layer_text);
    draw_background(85, 110, 155, 130); 
    qp_drawtext_recolor(display, 120 - layer_text_width / 2, 120 - noto11_font->line_height / 2, noto11_font, layer_text, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_flush(display);  
} 

void swipe_gesture_base_view_update(void) {
    qp_donut(display, 120, 120, 117, 4, hue_main_color, sat_main_color, 50, val_main_color);
    qp_donut(display, 120, 120, 114, 4, hue_main_color, sat_main_color, val_main_color, 50);
    qp_circle(display, 90 , 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 110, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 130, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
    qp_circle(display, 150, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, false);
}

static void swipe_gesture_view_update(const swipe_view_config_t *view_config) {
    uint8_t right_text_width = qp_textwidth(noto11_font, view_config->right_label);
    uint8_t left_text_width = qp_textwidth(noto11_font, view_config->left_label);
    uint8_t up_text_width = qp_textwidth(noto11_font, view_config->up_label);
    uint8_t text_height = noto11_font->line_height;
    draw_background(150, 120 - text_height / 2, 230, 120 + text_height / 2);
    draw_background(10, 120 - text_height / 2, 90, 120 + text_height / 2);
    draw_background(80, 40 - text_height / 2, 160, 40 + text_height / 2);
    qp_drawtext_recolor(display, 190 - right_text_width / 2, 120 - text_height / 2, noto11_font, view_config->right_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_drawtext_recolor(display, 50 - left_text_width / 2, 120 - text_height / 2, noto11_font, view_config->left_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_drawtext_recolor(display, 120 - up_text_width / 2, 40 - text_height / 2, noto11_font, view_config->up_label, hue_main_color, sat_main_color, val_main_color, hue_bg, sat_bg, val_bg);
    qp_circle(display, view_config->active_indicator_x, 190, 5, hue_sub_color, sat_sub_color, val_sub_color, true);
    qp_circle(display, view_config->previous_indicator_x, 190, 4, hue_bg, sat_bg, val_bg, true);
    qp_flush(display);
}

void swipe_gesture_layer_view_update(void){
    if (swipe_layer < SWIPE_PROFILE_COUNT) {
        swipe_gesture_view_update(&swipe_view_configs[swipe_layer]);
    }
}

static void tap_swipe_keycode(uint8_t keycode, swipe_modifier_t modifier) {
    if (modifier == SWIPE_MODIFIER_CONTROL || modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        register_code(KC_LCTL);
    }
    if (modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        register_code(KC_LGUI);
    }

    tap_code(keycode);

    if (modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        unregister_code(KC_LGUI);
    }
    if (modifier == SWIPE_MODIFIER_CONTROL || modifier == SWIPE_MODIFIER_CONTROL_GUI) {
        unregister_code(KC_LCTL);
    }
}

static void swipe_gesture_process(void) {
    if (swipe_layer >= SWIPE_PROFILE_COUNT) {
        return;
    }

    if (gesture_id == CST816S_SLIDE_DOWN) {
        change_swipe_layer();
        swipe_gesture_layer_view_update();
        return;
    }

    const swipe_action_config_t *action_config = &swipe_action_configs[swipe_layer];
    switch (gesture_id) {
        case CST816S_SLIDE_RIGHT:
            tap_swipe_keycode(action_config->right_keycode, action_config->modifier);
            break;
        case CST816S_SLIDE_LEFT:
            tap_swipe_keycode(action_config->left_keycode, action_config->modifier);
            break;
        case CST816S_SLIDE_UP:
            tap_swipe_keycode(action_config->up_keycode, action_config->modifier);
            break;
        default:
            break;
    }

}

static bool is_touch_in_circle(uint16_t touch_x, uint16_t touch_y, point_t circle, uint16_t radius) {
    uint32_t distance_squared = (circle.x_coordinate - touch_x) * (circle.x_coordinate - touch_x) + (circle.y_coordinate - touch_y) * (circle.y_coordinate - touch_y);
    return distance_squared <= (radius * radius);
}

static void process_touch(void) {
    touch_signal = true;
    touch_signal_view_update = true;
}

typedef enum {
    TOUCH_MODE_NONE = 0,
    TOUCH_MODE_PRESS,
    TOUCH_MODE_SWIPE
} touch_mode_t;
static touch_mode_t touch_mode = TOUCH_MODE_NONE;

typedef enum {
    TOUCH_STATE_NONE = 0,
    TOUCH_STATE_START,
    TOUCH_STATE_WAIT,
    TOUCH_STATE_REPEAT,
    TOUCH_STATE_SINGLE
} touch_state_t;
static touch_state_t touch_state = TOUCH_STATE_NONE;

static uint16_t fast_touch_time = 0;
static uint16_t second_touch_time = 0;
static uint16_t touch_repeat_time = 0;
static uint16_t pre_touch_x;
static uint16_t pre_touch_y;
static uint16_t now_touch_x;
static uint16_t now_touch_y;

static void process_gesture(void){
    switch (display_mode) {    
    case DISPLAY_MODE_TOUCH_KEY:
        if (touch_mode == TOUCH_MODE_PRESS) {
            touch_x = now_touch_x;
            touch_y = now_touch_y;
            process_touch();
        } else if (touch_mode == TOUCH_MODE_SWIPE) {
            update_lcd_layer_category_by_gesture();
        }
        break;

    case DISPLAY_MODE_TRACKBALL_TUNING:
        process_touch_trackball_tuning_mode(now_touch_x, now_touch_y);
        break;

    case DISPLAY_MODE_SWIPE_GESTURE:
        swipe_gesture_process();
        break;   

    case DISPLAY_MODE_STATUS1:
        ui_handle_touch(display, noto9_font, now_touch_x, now_touch_y);
        break;                    

    default:
        break;
    }
}

// add dedzone
static const uint8_t press_deadzone = 6;
static void touch_mode_detected(int16_t delta_x, int16_t delta_y){
    if (abs(delta_x) < press_deadzone && abs(delta_y) < press_deadzone) {
        touch_mode = TOUCH_MODE_PRESS;
    } else {
        touch_mode = TOUCH_MODE_SWIPE;
    }
}

static void swipe_gesture_id_detected(int16_t delta_x, int16_t delta_y){
    int16_t absolute_delta_x = delta_x;
    if (absolute_delta_x < 0) absolute_delta_x = -absolute_delta_x;
    int16_t absolute_delta_y = delta_y;
    if (absolute_delta_y < 0) absolute_delta_y = -absolute_delta_y;
    gesture_id = GESTURE_NONE;  
    if (absolute_delta_x > absolute_delta_y) {
        if (delta_x > 0) {
            gesture_id = CST816S_SLIDE_RIGHT;
        } else if (delta_x < 0) {
            gesture_id = CST816S_SLIDE_LEFT;
        }
    } else {
        if (delta_y > 0) {
            gesture_id = CST816S_SLIDE_DOWN;
        } else if (delta_y < 0) {
            gesture_id = CST816S_SLIDE_UP;
        }
    } 
}

static void start_touch_sequence(void) {
    cst816t_XY point = cst816t_Get_Point();
    pre_touch_x = point.x_point;
    pre_touch_y = point.y_point;
    fast_touch_time = timer_read();
    touch_state = TOUCH_STATE_START;
    uprintf("------START------\n");
}

static void detect_initial_touch(void) {
    if (timer_elapsed(fast_touch_time) <= touch_repeat_interval) {
        return;
    }

    cst816t_XY point = cst816t_Get_Point();
    now_touch_x = point.x_point;
    now_touch_y = point.y_point;
    second_touch_time = timer_read();

    int16_t delta_x = now_touch_x - pre_touch_x;
    int16_t delta_y = now_touch_y - pre_touch_y;
    uprintf("crood: %d %d\n", delta_x, delta_y);

    touch_mode_detected(delta_x, delta_y);
    swipe_gesture_id_detected(delta_x, delta_y);
    process_gesture();
    touch_state = TOUCH_STATE_SINGLE;
}

static void finish_single_touch(void) {
    if (timer_elapsed(second_touch_time) > touch_repeat_interval / 4) {
        touch_signal = false;
        touch_state = TOUCH_STATE_WAIT;
    }
}

static void start_touch_repeat(void) {
    if (timer_elapsed(second_touch_time) > touch_single_interval) {
        touch_state = TOUCH_STATE_REPEAT;
        touch_repeat_time = timer_read();
    }
}

static void repeat_touch(void) {
    if (timer_elapsed(touch_repeat_time) <= touch_repeat_interval) {
        return;
    }

    touch_repeat_time = timer_read();
    cst816t_XY point = cst816t_Get_Point();
    now_touch_x = point.x_point;
    now_touch_y = point.y_point;
    process_gesture();
}

static void process_active_touch(void) {
    switch (touch_state) {
        case TOUCH_STATE_NONE:
            start_touch_sequence();
            break;
        case TOUCH_STATE_START:
            detect_initial_touch();
            break;
        case TOUCH_STATE_SINGLE:
            finish_single_touch();
            break;
        case TOUCH_STATE_WAIT:
            start_touch_repeat();
            break;
        case TOUCH_STATE_REPEAT:
            repeat_touch();
            break;
    }
}

static void reset_released_touch(void) {
    if (timer_elapsed(fast_touch_time) <= touch_repeat_interval + TOUCH_RELEASE_GRACE_MS || touch_state == TOUCH_STATE_NONE) {
        return;
    }

    touch_signal = false;
    touch_mode = TOUCH_MODE_NONE;
    touch_state = TOUCH_STATE_NONE;
    gesture_id = GESTURE_NONE;
}

void process_touch_interrupt(void) {
    touch_input_task();

    if (touch_input_is_active()) {
        process_active_touch();
    } else {
        reset_released_touch();
    }
}

void show_trackball_tuning_mode(void) {
    draw_background_all_black();
    if (text1_width == 0) {
        text1_width = qp_textwidth(noto11_font, text1);
    }
    if (text3_width == 0) {
        text3_width = qp_textwidth(noto11_font, text3);
    }
    if (text4_width == 0) {
        text4_width = qp_textwidth(noto11_font, text4);
    }
    if (text5_width == 0) {
        text5_width = qp_textwidth(noto11_font, text5);
    }
    if (text6_width == 0) {
        text6_width = qp_textwidth(noto11_font, text6);
    }
    qp_drawtext(display, TOUCH_LCD_WIDTH / 2 - text1_width / 2, TOUCH_LCD_HEIGHT / 2 - 70 - noto11_font->line_height, noto11_font, text1);
    qp_drawtext(display, x_pos1 - text3_width / 2, TOUCH_LCD_HEIGHT / 2 + 80 - noto11_font->line_height, noto11_font, text3);
    qp_drawtext(display, x_pos1 - text5_width / 2, y_pos3 - noto11_font->line_height / 2, noto11_font, text5);
    qp_drawtext(display, x_pos1 - text6_width / 2, y_pos4 - noto11_font->line_height / 2, noto11_font, text6);
    qp_drawtext(display, x_pos2 - text4_width / 2, TOUCH_LCD_HEIGHT / 2 + 80 - noto11_font->line_height, noto11_font, text4);
    qp_drawtext(display, x_pos2 - text5_width / 2, y_pos3 - noto11_font->line_height / 2, noto11_font, text5);
    qp_drawtext(display, x_pos2 - text6_width / 2, y_pos4 - noto11_font->line_height / 2, noto11_font, text6);
    qp_curve(display, speed_adjust1, slope_factor1, hue_tbtune_r, sat_tbtune_r, val_tbtune_r, x_pos1, y_pos1);
    qp_curve(display, speed_adjust2, slope_factor2, hue_tbtune_l, sat_tbtune_l, val_tbtune_l, x_pos2, y_pos2);
    qp_drawimage(display, x_pos_save - image_save->width / 2, y_pos_save - image_save->height / 2, image_save);
    qp_flush(display);
}

void display_redraw(void) {
    switch (display_mode) {
        case DISPLAY_MODE_TOUCH_KEY:
            draw_background_all_black();
            draw_lcd_layer_category_images();
            break;
        case DISPLAY_MODE_TRACKBALL_TUNING:
            show_trackball_tuning_mode();
            break;
        case DISPLAY_MODE_SWIPE_GESTURE:
            draw_background_all();
            swipe_gesture_layer_view_update();
            swipe_gesture_base_view_update();
            swipe_gesture_main_view_update(current_lcd_layer);
            break;
        case DISPLAY_MODE_KEY_MATRIX:
            draw_key_matrix(display, roboto_mono16, st2_mono16, current_layer);
            break;
        case DISPLAY_MODE_STATUS1:
            status_view_init(display, noto9_font);
            break;
        default:
            break;
    }
}

static void process_touch_trackball_tuning_mode(uint16_t touch_x, uint16_t touch_y) {
    const uint8_t radius = 22;
    const uint8_t radius_gap = 1;
    point_t plus1_center = {x_pos1 - (radius + radius_gap), y_pos4}; 
    point_t minus1_center = {x_pos1 - (radius + radius_gap), y_pos3};
    point_t plus2_center = {x_pos2 - (radius + radius_gap), y_pos4}; 
    point_t minus2_center = {x_pos2 - (radius + radius_gap), y_pos3};
    point_t plus3_center = {x_pos1 + (radius + radius_gap), y_pos3}; 
    point_t minus3_center = {x_pos1 + (radius + radius_gap), y_pos4};
    point_t plus4_center = {x_pos2 + (radius + radius_gap), y_pos3}; 
    point_t minus4_center = {x_pos2 + (radius + radius_gap), y_pos4};
    point_t save_center = {x_pos_save, y_pos_save + 5};
    if (is_touch_in_circle(touch_x, touch_y, plus1_center, radius)) {
        speed_adjust1 += 0.2;
        if (speed_adjust1 > 3.0f) speed_adjust1 = 3.0f;
    }
    else if (is_touch_in_circle(touch_x, touch_y, minus1_center, radius)) {
        speed_adjust1 -= 0.2;
        if (speed_adjust1 < 0.2f) speed_adjust1 =  0.2f;
    }
    else if (is_touch_in_circle(touch_x, touch_y, plus2_center, radius)) {
        speed_adjust2 += 0.2;
        if (speed_adjust2 > 3.0f) speed_adjust2 = 3.0f;
    }
    else if (is_touch_in_circle(touch_x, touch_y, minus2_center, radius)) {
        speed_adjust2 -= 0.2;
        if (speed_adjust2 < 0.2f) speed_adjust2 = 0.2f;
    }
    else if (is_touch_in_circle(touch_x, touch_y, plus3_center, radius)) {
        slope_factor1 += 5;
        if (slope_factor1 > 100) slope_factor1 = 100;
    }
    else if (is_touch_in_circle(touch_x, touch_y, minus3_center, radius)) {
        slope_factor1 -= 5;
        if (slope_factor1 < 10) slope_factor1 = 10;
    }
    else if (is_touch_in_circle(touch_x, touch_y, plus4_center, radius)) {
        slope_factor2 += 5;
        if (slope_factor2 > 100) slope_factor2 = 100;
    }
    else if (is_touch_in_circle(touch_x, touch_y, minus4_center, radius)) {
        slope_factor2 -= 5;
        if (slope_factor2 < 10) slope_factor2 = 10;
    }
    else if (is_touch_in_circle(touch_x, touch_y, save_center, radius)) {
        save_omni_tb_config();
        show_trackball_tuning_mode();
    }
    if (pre_speed_adjust1 != speed_adjust1 || pre_slope_factor1 != slope_factor1) {
        draw_background_black(TOUCH_LCD_WIDTH / 2 + 15, TOUCH_LCD_HEIGHT / 2 - 40, TOUCH_LCD_WIDTH / 2 + 85, TOUCH_LCD_HEIGHT / 2 + 25);

        wait_ms(10);
        qp_curve(display, speed_adjust1, slope_factor1, hue_tbtune_r, sat_tbtune_r, val_tbtune_r, x_pos1, y_pos1);
    }else if (pre_speed_adjust2 != speed_adjust2 || pre_slope_factor2 != slope_factor2) {
        draw_background_black(TOUCH_LCD_WIDTH / 2 - 85, TOUCH_LCD_HEIGHT / 2 - 40, TOUCH_LCD_WIDTH / 2 - 15, TOUCH_LCD_HEIGHT / 2 + 25);

        wait_ms(10);
        qp_curve(display, speed_adjust2, slope_factor2, hue_tbtune_l, sat_tbtune_l, val_tbtune_l, x_pos2, y_pos2);
    }
    pre_speed_adjust1 = speed_adjust1;
    pre_speed_adjust2 = speed_adjust2;
    pre_slope_factor1 = slope_factor1;
    pre_slope_factor2 = slope_factor2;
}
