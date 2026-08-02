#pragma once

#include <stdint.h>
#include "qp.h"

void status_view_init(painter_device_t dev, painter_font_handle_t font);
void ui_handle_touch(painter_device_t dev, painter_font_handle_t font, int16_t tx, int16_t ty);
void persist_load_all(void);
void sync_default_layer_to_os(void);
