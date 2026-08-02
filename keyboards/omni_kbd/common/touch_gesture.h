#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TOUCH_INTERACTION_NONE,
    TOUCH_INTERACTION_PRESS,
    TOUCH_INTERACTION_SWIPE,
} touch_interaction_t;

typedef struct {
    touch_interaction_t interaction;
    uint16_t x;
    uint16_t y;
    uint8_t gesture_id;
} touch_gesture_event_t;

typedef void (*touch_gesture_handler_t)(const touch_gesture_event_t *event, const void *context);

void touch_gesture_task(touch_gesture_handler_t event_handler, const void *context);

uint16_t touch_gesture_repeat_interval(void);
void touch_gesture_set_repeat_interval(uint16_t interval);
void touch_gesture_set_hold_interval(uint16_t interval);

void touch_gesture_activate_key(uint16_t x, uint16_t y);
bool touch_gesture_key_is_active(void);
void touch_gesture_get_key_position(uint16_t *x, uint16_t *y);
void touch_gesture_clear_key_position(void);

bool touch_gesture_feedback_pending(void);
void touch_gesture_clear_feedback(void);
