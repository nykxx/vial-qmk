
#pragma once
#include "quantum.h"
#include <stdint.h>
#include "../drivers/pmw33xx_common.h"

#ifndef OMNI_SCROLL_INERTIA_DECAY
#    define OMNI_SCROLL_INERTIA_DECAY 0.88f
#endif
#ifndef OMNI_SCROLL_INERTIA_STOP
#    define OMNI_SCROLL_INERTIA_STOP 0.08f
#endif
#ifndef OMNI_SCROLL_INERTIA_MAX
#    define OMNI_SCROLL_INERTIA_MAX 60.0f
#endif

typedef struct {
    float accumulated_h;
    float accumulated_v;
    float inertia_h;
    float inertia_v;
} trackball_scroll_state_t;

void process_cursor_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_high_res_scroll_report(report_mouse_t *mouse_report, pmw33xx_report_t report, trackball_scroll_state_t *scroll_state, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_tap_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
