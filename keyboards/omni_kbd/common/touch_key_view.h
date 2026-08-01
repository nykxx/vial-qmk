#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "qp.h"

void touch_key_view_initialize(const uint16_t *virtual_keycodes);
void touch_key_view_draw(painter_device_t display);
void touch_key_view_handle_gesture(painter_device_t display, uint8_t gesture_id);

uint8_t touch_key_view_current_layer(void);
bool touch_key_view_locate_key(uint16_t touch_x, uint16_t touch_y, uint8_t *virtual_row, uint8_t *virtual_column);
