// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later
#include "draw_custom.h"
#include "config.h"
#include <math.h>
#include <stdint.h>
#include "qp_internal.h"
#include "qp_comms.h"
#include "qp_draw.h"
#include "qgf.h"
#include "qp.h"

static inline int16_t constrain_hid(int16_t value) {
    if (value > MAX_HID_VALUE) return MAX_HID_VALUE;
    if (value < MIN_HID_VALUE) return MIN_HID_VALUE;
    return value;
}

bool qp_curve(painter_device_t device, float speed_adjust, int slope_factor, uint8_t hue, uint8_t sat, uint8_t val, uint8_t x_pos, uint8_t y_pos) {
    painter_driver_t *driver = (painter_driver_t *)device;
    if (!driver || !driver->validate_ok) {
        return false;
    }
    if (!qp_comms_start(device)) {
        return false;
    }
    qp_internal_fill_pixdata(device, 1, hue, sat, val);
    int center_x = x_pos;
    int center_y = y_pos;
    int draw_size = 70;
    int draw_scale_y = 110;
    int hurf_value = draw_size / 2;
    int delta_values[draw_size + 1];
    for (int i = 0; i < draw_size + 1; i++) {
        delta_values[i] = i;
    }
    bool ret = true;
    for (int i = 0; i < draw_size; i++) {
        float delta_x0 = delta_values[i];
        float delta_x1 = delta_values[i+1];
        float delta_y0 = delta_values[i];
        float delta_y1 = delta_values[i+1];
        float scaled_y0 = pow(delta_y0, speed_adjust) / pow(draw_size, speed_adjust) * draw_size / draw_scale_y * slope_factor;
        float scaled_y1 = pow(delta_y1, speed_adjust) / pow(draw_size, speed_adjust) * draw_size / draw_scale_y * slope_factor;
        int draw_x0 = (center_x - hurf_value) + (int)delta_x0;
        int draw_x1 = (center_x - hurf_value) + (int)delta_x1;
        int draw_y0 = center_y + hurf_value - (int)scaled_y0;
        int draw_y1 = center_y + hurf_value - (int)scaled_y1;
        if (draw_x0 >= 0 && draw_x0 < 240 && draw_y0 >= 0 && draw_y0 < 240 &&
            draw_x1 >= 0 && draw_x1 < 240 && draw_y1 >= 0 && draw_y1 < 240) {
            if (!qp_internal_setpixel_impl(device, draw_x0, draw_y0) ||
                !qp_internal_setpixel_impl(device, draw_x1, draw_y1)) {
                ret = false;
                break;
            }
        }
    }
    qp_comms_stop(device);
    qp_dprintf("qp_curve: %s\n", ret ? "ok" : "fail");
    return ret;
}


#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)
#define ANGLE_STEP 1 

bool qp_fill_arc(painter_device_t device, uint16_t centerx, uint16_t centery, uint16_t outer_radius, uint16_t inner_radius, uint16_t start_angle, uint16_t end_angle, uint8_t hue, uint8_t sat, uint8_t val) {
    if (outer_radius <= inner_radius) {
        return false; 
    }
    qp_internal_fill_pixdata(device, (outer_radius * 2) + 1, hue, sat, val);
    if (!qp_comms_start(device)) {
        return false;
    }
    bool ret = true;
    double cos_table[360 / ANGLE_STEP];
    double sin_table[360 / ANGLE_STEP];
    for (uint16_t angle = start_angle; angle <= end_angle; angle += ANGLE_STEP) {
        double rad = DEG_TO_RAD(angle);
        cos_table[(angle - start_angle) / ANGLE_STEP] = cos(rad);
        sin_table[(angle - start_angle) / ANGLE_STEP] = sin(rad);
    }
    for (uint16_t angle = start_angle; angle <= end_angle; angle += ANGLE_STEP) {
        for (uint16_t r = inner_radius; r <= outer_radius; r++) {
            uint16_t index = (angle - start_angle) / ANGLE_STEP;
            int16_t x = centerx + (int16_t)(r * cos_table[index]);
            int16_t y = centery + (int16_t)(r * sin_table[index]);
            if (!qp_internal_setpixel_impl(device, x, y)) {
                ret = false;
                break;
            }
        }
        if (!ret) break;
    }
    qp_comms_stop(device);
    return ret;
}

void qp_donut(painter_device_t device, uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, uint8_t hue, uint8_t sat, uint8_t val1, uint8_t val2) {
    int add_val = (val2 - val1);
    if (add_val != 0) {
        add_val = add_val / thickness;
    }
    for (size_t i = 0; i < thickness; i++) {
        qp_circle(device, x, y, radius - i, hue, sat, val1, false);
        val1 = (val1 + add_val < 0) ? 0 : val1 + add_val;
    }
}



static uint16_t isqrt_u32(uint32_t n) {
    uint32_t res = 0, bit = 1UL << 30;
    while (bit > n) bit >>= 2;
    while (bit) {
        if (n >= res + bit) { n -= res + bit; res = (res >> 1) + bit; }
        else                { res >>= 1; }
        bit >>= 2;
    }
    return (uint16_t)res;
}

static bool rr_filled(painter_device_t d,
                      uint16_t l, uint16_t t,
                      uint16_t r, uint16_t b,
                      uint16_t rad)
{
    uint16_t h = b - t + 1;

    for (uint16_t row = 0; row < h; row++) {
        uint16_t y = t + row;
        uint16_t dx;

        if (row < rad) {
            uint16_t dy = rad - row;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else if (row >= h - rad) {
            uint16_t dy = row - (h - rad) + 1;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else {
            dx = 0;
        }

        uint16_t xl = l + dx;
        uint16_t xr = r - dx;
        if (!qp_internal_fillrect_helper_impl(d, xl, y, xr, y))
            return false;
    }
    return true;
}

static bool rr_outline(painter_device_t d,
                       uint16_t l, uint16_t t,
                       uint16_t r, uint16_t b,
                       uint16_t rad)
{
    uint16_t h = b - t + 1;

    for (uint16_t row = 0; row < h; row++) {
        uint16_t y  = t + row;
        uint16_t dx;

        if (row < rad) {
            uint16_t dy = rad - row;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else if (row >= h - rad) {
            uint16_t dy = row - (h - rad) + 1;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else {
            dx = 0;
        }

        uint16_t xl = l + dx;
        uint16_t xr = r - dx;

        if (row == 0 || row == h - 1) {
            if (!qp_internal_fillrect_helper_impl(d, xl, y, xr, y))
                return false;
        } else {
            if (!qp_internal_fillrect_helper_impl(d, xl, y, xl, y) ||
                !qp_internal_fillrect_helper_impl(d, xr, y, xr, y))
                return false;
        }
    }
    return true;
}

static bool rr_outline_w(painter_device_t d, uint16_t l, uint16_t t, uint16_t r, uint16_t b, uint16_t rad, uint8_t  thick) {
    uint16_t h = b - t + 1;

    for (uint16_t row = 0; row < h; row++) {
        uint16_t y = t + row;
        uint16_t dx;

        if (row < rad) {
            uint16_t dy = rad - row;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else if (row >= h - rad) {
            uint16_t dy = row - (h - rad) + 1;
            dx = rad - isqrt_u32(rad*rad - dy*dy);
        } else {
            dx = 0;
        }

        for (uint8_t w = 0; w < thick; w++) {
            uint16_t xl = l + dx + w;
            uint16_t xr = r - dx - w;

            if (row <= thick || row >= h - 1 - thick) {
                if (!qp_internal_fillrect_helper_impl(d, xl, y, xr, y))
                    return false;
            } else {
                if (!qp_internal_fillrect_helper_impl(d, xl, y, xl, y) ||
                    !qp_internal_fillrect_helper_impl(d, xr, y, xr, y))
                    return false;
            }
        }
    }
    return true;
}

/* ────────── PUBLIC API ────────── */
bool qp_round_rect(painter_device_t dev,
                   uint16_t left,  uint16_t top,
                   uint16_t right, uint16_t bottom,
                   uint16_t radius,
                   uint8_t hue, uint8_t sat, uint8_t val,
                   bool filled,
                   uint8_t stroke_w)
{
    uint16_t l = QP_MIN(left,  right);
    uint16_t r = QP_MAX(left,  right);
    uint16_t t = QP_MIN(top,   bottom);
    uint16_t b = QP_MAX(top,   bottom);
    uint16_t w = r - l + 1, h = b - t + 1;
    radius = QP_MIN(radius, QP_MIN(w, h) / 2);

    qp_internal_fill_pixdata(dev, w, hue, sat, val);

    if (!qp_comms_start(dev)) return false;

    bool ok;
    if (filled) {
        ok = rr_filled(dev, l, t, r, b, radius);
    } else if (stroke_w <= 1) {
        ok = rr_outline(dev, l, t, r, b, radius);
    } else {
        ok = rr_outline_w(dev, l, t, r, b, radius, stroke_w);
    }

    qp_comms_stop(dev);
    return ok;
}
