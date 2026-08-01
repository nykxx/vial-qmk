// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "touch_input.h"

#include <print.h>

#include "config.h"
#include "gpio.h"
#include "hardware/gpio.h"
#include "timer.h"

enum {
    TOUCH_SAMPLE_INTERVAL_MS  = 1,
    TOUCH_WINDOW_SLOT_COUNT   = 85,
    TOUCH_CONTACT_OFF_HOLD_MS = 15,
};

static uint8_t touch_window[TOUCH_WINDOW_SLOT_COUNT];
static uint8_t touch_window_index = 0;
static uint8_t low_sample_count = 0;

static bool touch_input_initialized = false;
static bool low_seen_in_slot = false;
static bool touch_contact_active = false;

static uint16_t slot_start_time = 0;
static uint16_t contact_off_start_time = 0;

static void initialize_touch_irq(void) {
    setPinInputHigh(INT_PIN);
    gpio_set_irq_enabled(INT_PIN, GPIO_IRQ_EDGE_FALL, true);
}

static void initialize_touch_window(void) {
    for (int slot_index = 0; slot_index < TOUCH_WINDOW_SLOT_COUNT; slot_index++) {
        touch_window[slot_index] = 0;
    }
    touch_window_index = 0;
    low_sample_count = 0;
    touch_contact_active = false;
    low_seen_in_slot = false;
    slot_start_time = timer_read();
}

static void initialize_touch_input_once(void) {
    if (touch_input_initialized) {
        return;
    }

    touch_input_initialized = true;
    initialize_touch_irq();
    initialize_touch_window();
}

static void update_contact_state(void) {
    if (!touch_contact_active) {
        if (low_sample_count >= 1) {
            touch_contact_active = true;
            contact_off_start_time = 0;
            uprintf("[TOUCH] DOWN (count=%u)\n", low_sample_count);
        }
        return;
    }

    if (low_sample_count != 0) {
        contact_off_start_time = 0;
        return;
    }

    if (contact_off_start_time == 0) {
        contact_off_start_time = timer_read();
    } else if (timer_elapsed(contact_off_start_time) >= TOUCH_CONTACT_OFF_HOLD_MS) {
        touch_contact_active = false;
        contact_off_start_time = 0;
        uprintf("[TOUCH] UP   (count=%u)\n", low_sample_count);
    }
}

static void sample_touch_window(void) {
    while (timer_elapsed(slot_start_time) >= TOUCH_SAMPLE_INTERVAL_MS) {
        uint8_t low_seen = low_seen_in_slot ? 1 : 0;
        low_sample_count += low_seen;
        low_sample_count -= touch_window[touch_window_index];
        touch_window[touch_window_index] = low_seen;

        if (++touch_window_index >= TOUCH_WINDOW_SLOT_COUNT) {
            touch_window_index = 0;
        }

        slot_start_time += TOUCH_SAMPLE_INTERVAL_MS;
        low_seen_in_slot = false;
        update_contact_state();
    }
}

void touch_input_task(void) {
    initialize_touch_input_once();

    if (gpio_get_irq_event_mask(INT_PIN) & GPIO_IRQ_EDGE_FALL) {
        low_seen_in_slot = true;
        gpio_acknowledge_irq(INT_PIN, GPIO_IRQ_EDGE_FALL);
    }

    sample_touch_window();
}

bool touch_input_is_active(void) {
    return touch_contact_active;
}
