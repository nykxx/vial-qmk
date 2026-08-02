// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "touch_gesture.h"

#include <print.h>

#include "timer.h"
#include "touch_input.h"
#include "../drivers/cst816t.h"

enum {
    TOUCH_PRESS_DEADZONE_PX = 6,
    TOUCH_RELEASE_GRACE_MS  = 30,
    INVALID_TOUCH_POSITION  = UINT16_MAX,
};

typedef enum {
    TOUCH_STATE_NONE,
    TOUCH_STATE_START,
    TOUCH_STATE_WAIT,
    TOUCH_STATE_REPEAT,
    TOUCH_STATE_SINGLE,
} touch_state_t;

static touch_interaction_t current_interaction = TOUCH_INTERACTION_NONE;
static touch_state_t current_state = TOUCH_STATE_NONE;
static uint8_t current_gesture_id = GESTURE_NONE;

static uint16_t sequence_start_time;
static uint16_t initial_event_time;
static uint16_t repeat_time;
static uint16_t start_x;
static uint16_t start_y;
static uint16_t current_x;
static uint16_t current_y;

static uint16_t key_x = INVALID_TOUCH_POSITION;
static uint16_t key_y = INVALID_TOUCH_POSITION;
static bool key_active;
static bool feedback_pending;
static uint16_t repeat_interval = 70;
static uint16_t hold_interval = 400;

static int16_t absolute_value(int16_t value) {
    return value < 0 ? -value : value;
}

static touch_interaction_t detect_interaction(int16_t delta_x, int16_t delta_y) {
    if (absolute_value(delta_x) < TOUCH_PRESS_DEADZONE_PX && absolute_value(delta_y) < TOUCH_PRESS_DEADZONE_PX) {
        return TOUCH_INTERACTION_PRESS;
    }
    return TOUCH_INTERACTION_SWIPE;
}

static uint8_t detect_gesture(int16_t delta_x, int16_t delta_y) {
    int16_t absolute_delta_x = absolute_value(delta_x);
    int16_t absolute_delta_y = absolute_value(delta_y);

    if (absolute_delta_x > absolute_delta_y) {
        if (delta_x > 0) {
            return CST816S_SLIDE_RIGHT;
        }
        if (delta_x < 0) {
            return CST816S_SLIDE_LEFT;
        }
    } else {
        if (delta_y > 0) {
            return CST816S_SLIDE_DOWN;
        }
        if (delta_y < 0) {
            return CST816S_SLIDE_UP;
        }
    }
    return GESTURE_NONE;
}

static void dispatch_event(touch_gesture_handler_t event_handler, const void *context) {
    touch_gesture_event_t event = {
        .interaction = current_interaction,
        .x = current_x,
        .y = current_y,
        .gesture_id = current_gesture_id,
    };
    event_handler(&event, context);
}

static void start_sequence(void) {
    cst816t_XY point = cst816t_Get_Point();
    start_x = point.x_point;
    start_y = point.y_point;
    sequence_start_time = timer_read();
    current_state = TOUCH_STATE_START;
    uprintf("------START------\n");
}

static void detect_initial_event(uint16_t repeat_interval, touch_gesture_handler_t event_handler, const void *context) {
    if (timer_elapsed(sequence_start_time) <= repeat_interval) {
        return;
    }

    cst816t_XY point = cst816t_Get_Point();
    current_x = point.x_point;
    current_y = point.y_point;
    initial_event_time = timer_read();

    int16_t delta_x = current_x - start_x;
    int16_t delta_y = current_y - start_y;
    uprintf("crood: %d %d\n", delta_x, delta_y);

    current_interaction = detect_interaction(delta_x, delta_y);
    current_gesture_id = detect_gesture(delta_x, delta_y);
    dispatch_event(event_handler, context);
    current_state = TOUCH_STATE_SINGLE;
}

static void finish_single_event(uint16_t repeat_interval) {
    if (timer_elapsed(initial_event_time) > repeat_interval / 4) {
        key_active = false;
        current_state = TOUCH_STATE_WAIT;
    }
}

static void start_repeat(uint16_t hold_interval) {
    if (timer_elapsed(initial_event_time) > hold_interval) {
        current_state = TOUCH_STATE_REPEAT;
        repeat_time = timer_read();
    }
}

static void repeat_event(uint16_t repeat_interval, touch_gesture_handler_t event_handler, const void *context) {
    if (timer_elapsed(repeat_time) <= repeat_interval) {
        return;
    }

    repeat_time = timer_read();
    cst816t_XY point = cst816t_Get_Point();
    current_x = point.x_point;
    current_y = point.y_point;
    dispatch_event(event_handler, context);
}

static void process_active_touch(uint16_t repeat_interval, uint16_t hold_interval, touch_gesture_handler_t event_handler, const void *context) {
    switch (current_state) {
        case TOUCH_STATE_NONE:
            start_sequence();
            break;
        case TOUCH_STATE_START:
            detect_initial_event(repeat_interval, event_handler, context);
            break;
        case TOUCH_STATE_SINGLE:
            finish_single_event(repeat_interval);
            break;
        case TOUCH_STATE_WAIT:
            start_repeat(hold_interval);
            break;
        case TOUCH_STATE_REPEAT:
            repeat_event(repeat_interval, event_handler, context);
            break;
    }
}

static void reset_released_touch(uint16_t repeat_interval) {
    if (timer_elapsed(sequence_start_time) <= repeat_interval + TOUCH_RELEASE_GRACE_MS || current_state == TOUCH_STATE_NONE) {
        return;
    }

    key_active = false;
    current_interaction = TOUCH_INTERACTION_NONE;
    current_state = TOUCH_STATE_NONE;
    current_gesture_id = GESTURE_NONE;
}

void touch_gesture_task(touch_gesture_handler_t event_handler, const void *context) {
    touch_input_task();

    if (touch_input_is_active()) {
        process_active_touch(repeat_interval, hold_interval, event_handler, context);
    } else {
        reset_released_touch(repeat_interval);
    }
}

uint16_t touch_gesture_repeat_interval(void) {
    return repeat_interval;
}

void touch_gesture_set_repeat_interval(uint16_t interval) {
    repeat_interval = interval;
}

void touch_gesture_set_hold_interval(uint16_t interval) {
    hold_interval = interval;
}

void touch_gesture_activate_key(uint16_t x, uint16_t y) {
    key_x = x;
    key_y = y;
    key_active = true;
    feedback_pending = true;
}

bool touch_gesture_key_is_active(void) {
    return key_active;
}

void touch_gesture_get_key_position(uint16_t *x, uint16_t *y) {
    *x = key_x;
    *y = key_y;
}

void touch_gesture_clear_key_position(void) {
    key_x = INVALID_TOUCH_POSITION;
    key_y = INVALID_TOUCH_POSITION;
}

bool touch_gesture_feedback_pending(void) {
    return feedback_pending;
}

void touch_gesture_clear_feedback(void) {
    feedback_pending = false;
}
