
#pragma once
#include "quantum.h"
#include <stdint.h>
#include "../drivers/pmw33xx_common.h"

extern int8_t ud_sc_mode_flag;
extern int8_t lr_sc_mode_flag;

void process_cursor_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_high_res_scroll_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);
void process_tap_report(report_mouse_t *mouse_report, pmw33xx_report_t report, float speed_adjust, uint8_t slope_factor, int rx, int ry, uint8_t cpi_scale);