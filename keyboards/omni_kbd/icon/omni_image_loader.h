#pragma once

#include <stdint.h>

#include "qp.h"

void initialize_images(void);

painter_image_handle_t omni_logo_image(void);
painter_image_handle_t omni_save_image(void);
painter_image_handle_t *omni_layer_image_handle(uint16_t layer_index);
painter_image_handle_t *omni_keycode_image_handle(uint16_t keycode);
