// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "power_lcd.h"
#include "wait.h"
#include "gpio.h"

static painter_device_t lcd_display = NULL;
static uint8_t       lcd_blk_pin = 0;
static bool          lcd_is_on   = false;

void power_lcd_init(painter_device_t display, uint8_t blk_pin) {
    lcd_display = display;
    lcd_blk_pin = blk_pin;
}

bool power_on_lcd(void) {
    if (!lcd_display) return false;          
    qp_power(lcd_display, true);
    wait_ms(10);
    writePinHigh(lcd_blk_pin);
    return lcd_is_on = true;
}

bool power_off_lcd(void) {
    if (!lcd_display) return false;
    writePinLow(lcd_blk_pin);
    wait_ms(1000);
    qp_power(lcd_display, false);
    return lcd_is_on = false;
}

