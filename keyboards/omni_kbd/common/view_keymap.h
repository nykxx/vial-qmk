#pragma once

#include <stdint.h>

#include "qp.h"

const char *get_layer_name(uint8_t layer);
void draw_key_matrix(painter_device_t display, painter_font_handle_t font1, painter_font_handle_t font2, uint8_t current_layer);
