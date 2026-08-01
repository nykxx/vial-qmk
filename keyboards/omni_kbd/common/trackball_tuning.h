#pragma once

#include <stdint.h>

#include "qp.h"

void trackball_tuning_draw(painter_device_t display, painter_font_handle_t font);
void trackball_tuning_handle_touch(painter_device_t display, painter_font_handle_t font, uint16_t touch_x, uint16_t touch_y);
