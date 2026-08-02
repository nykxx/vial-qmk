#pragma once

#include <stdint.h>
#include "qp.h"

void status_view_init(painter_device_t display, painter_font_handle_t font);
void status_view_handle_touch(painter_device_t display, painter_font_handle_t font, int16_t touch_x, int16_t touch_y);
void status_view_load_persistent_state(void);
void status_view_sync_default_layer_to_os(void);
