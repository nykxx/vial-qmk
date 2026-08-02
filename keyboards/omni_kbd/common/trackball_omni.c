// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include <math.h>
#include <stdint.h>
#include <print.h>
#include "trackball_omni.h"
#include "timer.h"
#include "config_omni.h"

#define constrain_hid(amt) ((amt) < -127 ? -127 : ((amt) > 127 ? 127 : (amt)))
#define constrain_hid16(amt) ((amt) < -32767 ? -32767 : ((amt) > 32767 ? 32767 : (amt)))

static float accumulated_x = 0.0f;
static float accumulated_y = 0.0f;
static float accumulated_h = 0.0f;
static float accumulated_v = 0.0f;

typedef struct {
    uint8_t vertical_interval;
    uint8_t vertical_value;
    uint8_t horizontal_interval;
    uint8_t horizontal_value;
    int8_t vertical_direction;
    int8_t horizontal_direction;
} trackball_scroll_config_t;

static trackball_scroll_config_t scroll_config = {
    .vertical_interval = 100,
    .vertical_value = 100,
    .horizontal_interval = 100,
    .horizontal_value = 100,
    .vertical_direction = 1,
    .horizontal_direction = 1,
};

void trackball_set_vertical_scroll_interval(uint8_t interval) {
    scroll_config.vertical_interval = interval;
}

void trackball_set_vertical_scroll_value(uint8_t value) {
    scroll_config.vertical_value = value;
}

void trackball_set_horizontal_scroll_interval(uint8_t interval) {
    scroll_config.horizontal_interval = interval;
}

void trackball_set_horizontal_scroll_value(uint8_t value) {
    scroll_config.horizontal_value = value;
}

void trackball_set_scroll_inverted(bool inverted) {
    int8_t direction = inverted ? -1 : 1;
    scroll_config.vertical_direction = direction;
    scroll_config.horizontal_direction = direction;
}

static inline float clamp_abs_float(float value, float limit) {
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

static inline uint8_t clamp_1_100_x(int16_t x) {
    if (x < 1) x = 1;
    if (x > 100) x = 100;
    return (uint8_t)x;
}

void process_cursor_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale) {
    if (!report.motion.b.is_lifted) {
        float x = (report.delta_x / cpi_scale);
        float y = (report.delta_y / cpi_scale);
        int sign_x = ((x > 0) - (x < 0)) * rx;
        int sign_y = ((y > 0) - (y < 0)) * ry;
        float x_corr = pow(fabs(x), speed_adjust) / pow(127, speed_adjust) * 127 / 100 * slope_factor * sign_x;
        float y_corr = pow(fabs(y), speed_adjust) / pow(127, speed_adjust) * 127 / 100 * slope_factor * sign_y;
        accumulated_x += x_corr;
        accumulated_y += y_corr;
        if (fabs(accumulated_x) >= 1.0f) {
            mouse_report->x = -constrain_hid(mouse_report->x + accumulated_x);
            accumulated_x = 0;
        }
        if (fabs(accumulated_y) >= 1.0f) {
            mouse_report->y = constrain_hid(mouse_report->y + accumulated_y);
            accumulated_y = 0;
        }
    }
}

static void send_high_res_scroll_report(report_mouse_t *mouse_report, trackball_scroll_state_t *scroll_state) {
    if (fabs(scroll_state->accumulated_v) >= 1.0f * (clamp_1_100_x(scroll_config.vertical_interval) * 12 / 10)) {
        mouse_report->v = constrain_hid16(mouse_report->v + scroll_state->accumulated_v) / (clamp_1_100_x(scroll_config.vertical_value) * 12 / 10);
        scroll_state->accumulated_v = 0;
    }

    if (fabs(scroll_state->accumulated_h) >= 1.0f * (clamp_1_100_x(scroll_config.horizontal_interval) * 12 / 10)) {
        mouse_report->h = -constrain_hid16(mouse_report->h + scroll_state->accumulated_h) / (clamp_1_100_x(scroll_config.horizontal_value) * 12 / 10);
        scroll_state->accumulated_h = 0;
    }
}

static bool is_same_direction(float a, float b) {
    return (a > 0.0f && b > 0.0f) || (a < 0.0f && b < 0.0f);
}

static void clear_scroll_inertia(trackball_scroll_state_t *scroll_state) {
    scroll_state->inertia_h = 0.0f;
    scroll_state->inertia_v = 0.0f;
    scroll_state->active_axis = TRACKBALL_SCROLL_AXIS_NONE;
}

static void update_scroll_inertia_h(trackball_scroll_state_t *scroll_state, float h) {
    float next_h = clamp_abs_float(h * OMNI_SCROLL_INERTIA_GAIN, OMNI_SCROLL_INERTIA_MAX);
    if (scroll_state->active_axis == TRACKBALL_SCROLL_AXIS_V) {
        if (fabs(h) >= OMNI_SCROLL_INERTIA_MIN_INPUT) clear_scroll_inertia(scroll_state);
    }

    if (fabs(h) >= OMNI_SCROLL_INERTIA_START_INPUT) {
        if (scroll_state->active_axis != TRACKBALL_SCROLL_AXIS_H || !is_same_direction(scroll_state->inertia_h, next_h) || fabs(next_h) > fabs(scroll_state->inertia_h)) {
            scroll_state->inertia_h = next_h;
        }
        scroll_state->inertia_v = 0.0f;
        scroll_state->active_axis = TRACKBALL_SCROLL_AXIS_H;
        scroll_state->last_inertia_timer = timer_read();
    }
}

static void update_scroll_inertia_v(trackball_scroll_state_t *scroll_state, float v) {
    float next_v = clamp_abs_float(v * OMNI_SCROLL_INERTIA_GAIN, OMNI_SCROLL_INERTIA_MAX);
    if (scroll_state->active_axis == TRACKBALL_SCROLL_AXIS_H) {
        if (fabs(v) >= OMNI_SCROLL_INERTIA_MIN_INPUT) clear_scroll_inertia(scroll_state);
    }

    if (fabs(v) >= OMNI_SCROLL_INERTIA_START_INPUT) {
        if (scroll_state->active_axis != TRACKBALL_SCROLL_AXIS_V || !is_same_direction(scroll_state->inertia_v, next_v) || fabs(next_v) > fabs(scroll_state->inertia_v)) {
            scroll_state->inertia_v = next_v;
        }
        scroll_state->inertia_h = 0.0f;
        scroll_state->active_axis = TRACKBALL_SCROLL_AXIS_V;
        scroll_state->last_inertia_timer = timer_read();
    }
}

static void apply_scroll_inertia(trackball_scroll_state_t *scroll_state) {
    if (scroll_state->inertia_h == 0.0f && scroll_state->inertia_v == 0.0f) {
        scroll_state->active_axis = TRACKBALL_SCROLL_AXIS_NONE;
        return;
    }

    if (timer_elapsed(scroll_state->last_inertia_timer) < OMNI_SCROLL_INERTIA_INTERVAL_MS) return;
    scroll_state->last_inertia_timer = timer_read();

    scroll_state->accumulated_h += scroll_state->inertia_h;
    scroll_state->accumulated_v += scroll_state->inertia_v;

    scroll_state->inertia_h *= OMNI_SCROLL_INERTIA_DECAY;
    scroll_state->inertia_v *= OMNI_SCROLL_INERTIA_DECAY;

    if (fabs(scroll_state->inertia_h) < OMNI_SCROLL_INERTIA_STOP) scroll_state->inertia_h = 0.0f;
    if (fabs(scroll_state->inertia_v) < OMNI_SCROLL_INERTIA_STOP) scroll_state->inertia_v = 0.0f;
    if (scroll_state->inertia_h == 0.0f && scroll_state->inertia_v == 0.0f) scroll_state->active_axis = TRACKBALL_SCROLL_AXIS_NONE;
}

void process_high_res_scroll_report(report_mouse_t *mouse_report, pmw33xx_report_t report, trackball_scroll_state_t *scroll_state, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale) {
    if (!scroll_state) return;

    uint16_t corr_calc_rapport_max = 600;
    float x = (report.delta_x * cpi_scale);
    float y = (report.delta_y * cpi_scale);
    int sign_x = ((x > 0) - (x < 0)) * rx * scroll_config.horizontal_direction;
    int sign_y = ((y > 0) - (y < 0)) * ry * scroll_config.vertical_direction;
    float x_corr = pow(fabs(x), speed_adjust) / pow(corr_calc_rapport_max, speed_adjust) * corr_calc_rapport_max / 100 * slope_factor * sign_x;
    float y_corr = pow(fabs(y), speed_adjust) / pow(corr_calc_rapport_max, speed_adjust) * corr_calc_rapport_max / 100 * slope_factor * sign_y;

    if (!report.motion.b.is_lifted && (fabs(x_corr) > 0.0f || fabs(y_corr) > 0.0f)) {
        const float diagonal_limit = 0.5f;
        float ratio = (fabs(x_corr) > 0.0f) ? (fabs(y_corr) / fabs(x_corr)) : 9999.0f;
        if (ratio > diagonal_limit && ratio < (1.0f / diagonal_limit)) {
            // Keep the last strong inertia; diagonal tail noise should not consume it.
        } else if (ratio <= diagonal_limit) {
            update_scroll_inertia_h(scroll_state, x_corr);
            scroll_state->accumulated_h += x_corr;
        } else {
            update_scroll_inertia_v(scroll_state, y_corr);
            scroll_state->accumulated_v += y_corr;
        }
    } else {
        apply_scroll_inertia(scroll_state);
    }

    send_high_res_scroll_report(mouse_report, scroll_state);
}



void process_tap_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale) {
    if (!report.motion.b.is_lifted) {
        int x = (report.delta_x / cpi_scale);
        int y = (report.delta_y / cpi_scale);
        int sign_x = ((x > 0) - (x < 0)) * rx * scroll_config.horizontal_direction;
        int sign_y = ((y > 0) - (y < 0)) * ry * scroll_config.vertical_direction;
        float x_corr = pow(fabs(x), speed_adjust) / pow(127, speed_adjust) * 127 / 100 * slope_factor * sign_x;
        float y_corr = pow(fabs(y), speed_adjust) / pow(127, speed_adjust) * 127 / 100 * slope_factor * sign_y;
        const float diagonal_limit = 0.6f;
        float ratio = fabs(y_corr) / fabs(x_corr);
        if (ratio > diagonal_limit && ratio < (1.0f / diagonal_limit)) {
            return;
        } else if (ratio <= diagonal_limit) {
            accumulated_h += x_corr / 2;
        } else {
            accumulated_v += y_corr / 1;
        }
        int tap_cycle_max = 20;
        if (fabs(accumulated_h) >= 1.0f) {
            int tap_cycle_h = round(fabs(accumulated_h));
            tap_cycle_h = (tap_cycle_h > tap_cycle_max) ? tap_cycle_max : tap_cycle_h;
            for (int i = 0; i < tap_cycle_h; i += 2) {
                if (accumulated_h > 0) {
                    tap_code(KC_MS_WH_RIGHT);
                    // tap_code(KC_RIGHT);
                } else if (accumulated_h < 0){
                    tap_code(KC_MS_WH_LEFT);
                    // tap_code(KC_LEFT);
                }
            }
            accumulated_h = 0.0f;
        }
        if (fabs(accumulated_v) >= 1.0f) {
            int tap_cycle_v = round(fabs(accumulated_v));
            tap_cycle_v = (tap_cycle_v > tap_cycle_max) ? tap_cycle_max : tap_cycle_v;
            for (int i = 0; i < tap_cycle_v; i += 2) {
                if (accumulated_v > 0) {
                    tap_code(KC_MS_WH_UP);
                    // tap_code(KC_UP);
                } else if (accumulated_v < 0){
                    tap_code(KC_MS_WH_DOWN);
                    // tap_code(KC_DOWN);
                }
            }
            accumulated_v = 0;
        }
    }
}
