#pragma once
#define OLED_DISPLAY_128X32

#define I2C1_SDA_PIN GP10
#define I2C1_SCL_PIN GP11

#define SPLIT_USB_DETECT
#define SPLIT_USB_TIMEOUT 10000 //Default 2000
#define SPLIT_USB_TIMEOUT_POLL 10 //Default 10
#define MATRIX_MASKED
#define SPLIT_HAND_MATRIX_GRID GP7, GP15                                                  