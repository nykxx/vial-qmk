#pragma once
//#define HAL_USE_I2C TRUE

#define OLED_DISPLAY_128X32
//#define OLED_FONT_H "keymaps/vial/glcdfont.c"

#define I2C1_SDA_PIN GP10
#define I2C1_SCL_PIN GP11

//#define SPLIT_TRANSPORT_MIRROR 		// マスターのマトリックス接続状況
#define SPLIT_LAYER_STATE_ENABLE	// レイヤー状態
#define SPLIT_LED_STATE_ENABLE		// CapsLockなどのLED点灯状態
#define SPLIT_MODS_ENABLE			// シフトキーなどのMODキーの状態
//#define SPLIT_WPM_ENABLE			// キーボードWPMの状態
#define SPLIT_OLED_ENABLE			// OLEDのオン、オフの状態

#define SPLIT_USB_DETECT
#define SPLIT_USB_TIMEOUT 10000 //Default 2000
#define SPLIT_USB_TIMEOUT_POLL 10 //Default 10
#define MATRIX_MASKED
#define SPLIT_HAND_MATRIX_GRID GP7, GP27