
#pragma once
#include "quantum.h"
#include <stdint.h>
#include "../drivers/pmw33xx_common.h"

#ifndef OMNI_SCROLL_INERTIA_DECAY
#    define OMNI_SCROLL_INERTIA_DECAY 0.95f
#endif
#ifndef OMNI_SCROLL_INERTIA_STOP
#    define OMNI_SCROLL_INERTIA_STOP 0.01f
#endif
#ifndef OMNI_SCROLL_INERTIA_GAIN
#    define OMNI_SCROLL_INERTIA_GAIN 1.5f
#endif
#ifndef OMNI_SCROLL_INERTIA_MAX
#    define OMNI_SCROLL_INERTIA_MAX 80.0f
#endif
#ifndef OMNI_SCROLL_INERTIA_MIN_INPUT
#    define OMNI_SCROLL_INERTIA_MIN_INPUT 0.5f
#endif
#ifndef OMNI_SCROLL_INERTIA_START_INPUT
#    define OMNI_SCROLL_INERTIA_START_INPUT 4.0f
#endif
#ifndef OMNI_SCROLL_INERTIA_INTERVAL_MS
#    define OMNI_SCROLL_INERTIA_INTERVAL_MS 8
#endif
#ifndef OMNI_SCROLL_INERTIA_AXIS_SWITCH_RATIO
#    define OMNI_SCROLL_INERTIA_AXIS_SWITCH_RATIO 1.5f
#endif

typedef enum {
    TRACKBALL_SCROLL_AXIS_NONE = 0,
    TRACKBALL_SCROLL_AXIS_H,
    TRACKBALL_SCROLL_AXIS_V,
} trackball_scroll_axis_t;

typedef struct {
    float accumulated_h;
    float accumulated_v;
    float inertia_h;
    float inertia_v;
    uint16_t last_inertia_timer;
    trackball_scroll_axis_t active_axis;
} trackball_scroll_state_t;

void process_cursor_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_high_res_scroll_report(report_mouse_t *mouse_report, pmw33xx_report_t report, trackball_scroll_state_t *scroll_state, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_tap_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
