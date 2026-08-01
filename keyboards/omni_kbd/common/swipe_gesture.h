#pragma once

#include <stdint.h>

#include "qp.h"

void swipe_gesture_draw_main(painter_device_t display, painter_font_handle_t font, uint8_t current_layer);
void swipe_gesture_draw_base(painter_device_t display);
void swipe_gesture_draw_profile(painter_device_t display, painter_font_handle_t font);
void swipe_gesture_process(painter_device_t display, painter_font_handle_t font, uint8_t gesture_id);
