// SPDX-License-Identifier: GPL-2.0-or-later
//
// EC（静電容量）キー判定のうち、ハードウェアに依存しない部分。
// EEPROM に保存する設定の形式と、ADC 値から押下・解放を決める処理（ec_core.c）を定義する。
// QMK のヘッダに依存しないので、ホスト PC 上の単体テストでもそのままビルドできる。
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define EC_KEYS 21                // 片手のキー数（PCB 上の SW1〜SW21）。
#define EC_ADC_MAX 4095           // RP2040 ADC は 12 bit。
#define EC_CALIBRATION_SAMPLES 64 // 未押下時の基準値を、この回数の平均で決める。

// ec_config_t の形式識別子。保存内容の意味や配置を変えたら値を変え、古い保存データを読み込まない
// ようにする。0x...02 は、2026-11-19 に放電方式を MUX 経由へ変えたときのタイミング既定値に対応する。
// Q1 で放電していた頃の 0x...01 で保存された設定は、charge_us = 2 us では信号を取り逃がすため使わない。
#define EC_CONFIG_MAGIC 0x52424502u

// EC コマンド5 で受け付ける走査タイミングの上限 [us]。待ち時間が長いほど1回の走査は遅くなる。
// 試験版は遅い放電経路を調べるため、キーマップの config.h でこれらを大きくする。
#ifndef EC_DISCHARGE_US_MAX
#    define EC_DISCHARGE_US_MAX 100
#endif
#ifndef EC_SETTLE_US_MAX
#    define EC_SETTLE_US_MAX 20
#endif
#ifndef EC_CHARGE_US_MAX
#    define EC_CHARGE_US_MAX 20
#endif

// EEPROM（QMK の KB データブロック, 96 バイト）にそのまま書き込む EC 設定。
// 配置を変えると保存済みデータと合わなくなるため、変更時は EC_CONFIG_MAGIC も更新する。
typedef struct {
    uint32_t magic;            // 形式識別子 EC_CONFIG_MAGIC。
    uint16_t press[EC_KEYS];   // 押下と判定する差分（raw - baseline）の閾値 [ADC counts]。
    uint16_t release[EC_KEYS]; // 解放と判定する差分の閾値。0 < release < press <= 4095。
    uint16_t discharge_us;     // 走査タイミング（matrix.c の read_key()）[us]: SENSE_SUM の放電時間。
    uint16_t settle_us;        // 放電を止めて列をつないでから、行を HIGH にするまでの待ち。
    uint16_t charge_us;        // 行を HIGH にしてから ADC 変換を始めるまでの待ち。
    uint16_t checksum;         // checksum より前の全バイトの CRC-16/CCITT-FALSE。
} ec_config_t;

// 押下中のキーは、差分が release 以下になるか、押下中の最大値の半分以下まで下がると解放する
// （ec_core.c の release_level()）。半分の値は press よりこの値だけ下を上限とし、解放した位置で
// 再び押下にならないようにする。
// 2026-11-19 の実機確認では、ドームを載せ直したキーが未押下でも約 +110 カウントを示し、
// release = 90 を上回ったまま解放されなかった。
#define EC_RELEASE_HYSTERESIS_MIN 40

// 離している間、基準値をこのサンプル数を時定数として raw に追従させる（走査周期 0.5〜1 ms で
// 約 2〜4 秒）。ドームの座りや温度による緩やかなずれを吸収する。押している間は基準値を動かさない。
#define EC_BASELINE_TRACK_SAMPLES 4096

// キー1つ分の判定状態。RAM のみで保存しない（基準値は起動・再校正のたびに測り直す）。
typedef struct {
    uint32_t sum;      // 校正中の raw の合計。
    int32_t base_q16;  // 追従中の基準値。下位16ビットを小数部とする固定小数点。
    uint16_t raw;      // 直近の ADC 値（診断コマンドで返す）。
    uint16_t baseline; // 未押下時の基準値（base_q16 を整数に丸めた値）。
    int16_t peak;      // 今回の押下中の差分の最大値。
    uint8_t samples;   // 校正に使ったサンプル数。EC_CALIBRATION_SAMPLES に達したら校正完了。
    bool pressed;      // EC としての押下状態（QMK のデバウンス前）。
} ec_key_t;

// 既定の閾値・タイミングを設定し、checksum も計算する。
void ec_config_defaults(ec_config_t *config);
uint16_t ec_config_checksum(const ec_config_t *config);
// magic・checksum・タイミング・全キーの閾値がすべて有効なら true。
bool ec_config_valid(const ec_config_t *config);
bool ec_thresholds_valid(uint16_t press, uint16_t release);
// 判定状態を消して校正前に戻す。
void ec_key_reset(ec_key_t *key);
// 新しい ADC 値を1つ取り込み、校正・基準値追従・押下判定を進める。
void ec_key_sample(ec_key_t *key, uint16_t raw, uint16_t press, uint16_t release);
