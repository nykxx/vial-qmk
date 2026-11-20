// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// ---- マトリクス・左右分割 ----
// 論理マトリクスは 8行×6列。行0〜3が左手、行4〜7が右手（QMK split は各手 MATRIX_ROWS/2 行を持つ）。
// 各手のキーは PCB 上の SW1〜SW21 の21個で、SWn は (row, col) = ((n-1) / 6, (n-1) % 6) に置く。
// 行3/7 の列3〜5 は存在しない（matrix.c は21キーだけを走査する）。
#define MATRIX_ROWS 8
#define MATRIX_COLS 6
// GP27 の識別抵抗で左右を判定する（HIGH=左、LOW=右）。左右に同じ UF2 を書き込める。
#define SPLIT_HAND_PIN GP27
#define SPLIT_USB_DETECT
// 左右間は GP0 TX / GP1 RX の全二重 UART（rules.mk の SERIAL_DRIVER = vendor で RP2040 の PIO を使う）。
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1
#define SERIAL_USART_SPEED 230400
// EC 診断コマンドを相手側の手へ転送するための独自 split トランザクション（realball.c）。
#define SPLIT_TRANSACTION_IDS_KB REALBALL_EC_RPC
#define SPLIT_MAX_CONNECTION_ERRORS 5
#define SPLIT_CONNECTION_CHECK_TIMEOUT 500

// ---- EC（静電容量）検出回路のピン。使い方は matrix.c 冒頭の説明を参照 ----
// 行駆動線。HIGH にした瞬間の立ち上がりがキーの静電容量を通って列へ伝わる。
#define EC_ROW_PINS {GP8, GP9, GP10, GP11}
// アナログ MUX U2 (74HC4051) の選択線 S0 / S1 / S2（S0 が LSB）。列0〜5 → Y0〜Y5、Y6 は GND。
#define EC_MUX_PINS {GP12, GP13, GP14}
// Q1 (BC847B) による放電用。現行の放電方式では Q1 を使わず、常に LOW（Q1 OFF）にしておく。
#define EC_DISCHARGE_PIN GP15
// SENSE_SUM をアンプで増幅した出力を読む ADC0。
#define EC_ADC_PIN GP26
// 起動時・再校正時に手が離れるのを待つ時間。この後で未押下時の基準値を測る。
#define EC_CALIBRATION_DELAY_MS 1000
// EC 設定（キーごとの閾値・走査タイミング）は QMK の KB データブロックに保存する。
// サイズは ec_config_t（ec_core.h）と一致させること。保存形式の判別は ec_config_t.magic で行う。
#define EECONFIG_KB_DATA_SIZE 96
#define EECONFIG_KB_DATA_VERSION 0x52420101
// EC の判定（ec_core.c）はヒステリシスだけを持つ。チャタリング対策はこの QMK デバウンス
// （rules.mk の sym_defer_pk: キーごとに、押下・解放とも 5ms 安定してから確定）に任せる。
#define DEBOUNCE 5
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
// Shift 付きキーコード（KC_HASH など）の weak Shift を、それらをすべて離すまで保つ（symbol_shift.c）。
// 0 にすると QMK 標準の動作（複数押したうち最初の1つを離した時点で Shift が外れる）に戻る。
#define REALBALL_SYMBOL_SHIFT_HOLD 1
