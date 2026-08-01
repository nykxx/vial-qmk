// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include <math.h>
#include <stdint.h>
#include <print.h>
#include "trackball_omni.h"
#include "timer.h"
#include "config_omni.h"
#include "status_view.h"

#define constrain_hid(amt) ((amt) < -127 ? -127 : ((amt) > 127 ? 127 : (amt)))
#define constrain_hid16(amt) ((amt) < -32767 ? -32767 : ((amt) > 32767 ? 32767 : (amt)))

static float accumulated_x = 0.0f;
static float accumulated_y = 0.0f;
static float accumulated_h = 0.0f;
static float accumulated_v = 0.0f;

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
    if (fabs(scroll_state->accumulated_v) >= 1.0f * (clamp_1_100_x(hi_res_interval_v) * 12 / 10)) {
        mouse_report->v = constrain_hid16(mouse_report->v + scroll_state->accumulated_v) / (clamp_1_100_x(hi_res_value_v) * 12 / 10);
        scroll_state->accumulated_v = 0;
    }

    if (fabs(scroll_state->accumulated_h) >= 1.0f * (clamp_1_100_x(hi_res_interval_h) * 12 / 10)) {
        mouse_report->h = -constrain_hid16(mouse_report->h + scroll_state->accumulated_h) / (clamp_1_100_x(hi_res_value_h) * 12 / 10);
        scroll_state->accumulated_h = 0;
    }
}

static void apply_scroll_inertia(trackball_scroll_state_t *scroll_state) {
    scroll_state->inertia_h *= OMNI_SCROLL_INERTIA_DECAY;
    scroll_state->inertia_v *= OMNI_SCROLL_INERTIA_DECAY;

    if (fabs(scroll_state->inertia_h) < OMNI_SCROLL_INERTIA_STOP) scroll_state->inertia_h = 0.0f;
    if (fabs(scroll_state->inertia_v) < OMNI_SCROLL_INERTIA_STOP) scroll_state->inertia_v = 0.0f;

    scroll_state->accumulated_h += scroll_state->inertia_h;
    scroll_state->accumulated_v += scroll_state->inertia_v;
}

void process_high_res_scroll_report(report_mouse_t *mouse_report, pmw33xx_report_t report, trackball_scroll_state_t *scroll_state, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale) {
    if (!scroll_state) return;

    if (report.motion.b.is_lifted) {
        scroll_state->inertia_h = 0.0f;
        scroll_state->inertia_v = 0.0f;
        return;
    }

    uint16_t corr_calc_rapport_max = 600;
    float x = (report.delta_x * cpi_scale);
    float y = (report.delta_y * cpi_scale);
    int sign_x = ((x > 0) - (x < 0)) * rx * lr_sc_mode_flag;
    int sign_y = ((y > 0) - (y < 0)) * ry * ud_sc_mode_flag;
    float x_corr = pow(fabs(x), speed_adjust) / pow(corr_calc_rapport_max, speed_adjust) * corr_calc_rapport_max / 100 * slope_factor * sign_x;
    float y_corr = pow(fabs(y), speed_adjust) / pow(corr_calc_rapport_max, speed_adjust) * corr_calc_rapport_max / 100 * slope_factor * sign_y;

    if (report.motion.b.is_motion && (fabs(x_corr) > 0.0f || fabs(y_corr) > 0.0f)) {
        const float diagonal_limit = 0.5f;
        float ratio = (fabs(x_corr) > 0.0f) ? (fabs(y_corr) / fabs(x_corr)) : 9999.0f;
        if (ratio > diagonal_limit && ratio < (1.0f / diagonal_limit)) {
            scroll_state->inertia_h = 0.0f;
            scroll_state->inertia_v = 0.0f;
            return;
        } else if (ratio <= diagonal_limit) {
            scroll_state->accumulated_h += x_corr;
            scroll_state->inertia_h = clamp_abs_float(x_corr, OMNI_SCROLL_INERTIA_MAX);
            scroll_state->inertia_v = 0.0f;
        } else {
            scroll_state->accumulated_v += y_corr;
            scroll_state->inertia_v = clamp_abs_float(y_corr, OMNI_SCROLL_INERTIA_MAX);
            scroll_state->inertia_h = 0.0f;
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
        int sign_x = ((x > 0) - (x < 0)) * rx * lr_sc_mode_flag;
        int sign_y = ((y > 0) - (y < 0)) * ry * ud_sc_mode_flag;
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