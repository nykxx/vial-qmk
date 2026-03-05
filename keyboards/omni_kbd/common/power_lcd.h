#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "qp.h"

void power_lcd_init(painter_device_t display, uint8_t blk_pin);
bool power_on_lcd(void);
bool power_off_lcd(void);

