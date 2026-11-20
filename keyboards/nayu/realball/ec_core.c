// SPDX-License-Identifier: GPL-2.0-or-later
//
// EC キー判定のハードウェア非依存部分（ec_core.h の説明を参照）。
#include "ec_core.h"
#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(ec_config_t) == 96, "EEPROM config size changed");

// CRC-16/CCITT-FALSE（多項式 0x1021、初期値 0xFFFF、ビット反転なし）。
// checksum フィールド自身を除く、その手前までの全バイトが対象。
uint16_t ec_config_checksum(const ec_config_t *config) {
    const uint8_t *bytes = (const uint8_t *)config;
    uint16_t crc = 0xffff;
    for (unsigned i = 0; i < offsetof(ec_config_t, checksum); ++i) {
        crc ^= (uint16_t)bytes[i] << 8;
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

void ec_config_defaults(ec_config_t *config) {
    // memset でパディングも0にしておく（CRC はパディングを含むバイト列で計算するため）。
    memset(config, 0, sizeof(*config));
    config->magic = EC_CONFIG_MAGIC;
    // 閾値は仮の値。実機でキーごとに測って調整する。
    for (unsigned i = 0; i < EC_KEYS; ++i) {
        config->press[i] = 180;
        config->release[i] = 90;
    }
    // タイミングは左基板 SW1 での実測から決めた。MUX で放電すれば Q1 の解放遅れを待つ必要はなく、
    // アンプ出力は駆動エッジから 4〜6 us で立ち上がる。
    config->discharge_us = 10;
    config->settle_us = 2;
    config->charge_us = 8;
    config->checksum = ec_config_checksum(config);
}

bool ec_thresholds_valid(uint16_t press, uint16_t release) {
    return release > 0 && press > release && press <= EC_ADC_MAX;
}

// EEPROM から読んだ設定や、診断コマンドで変更しようとした設定をそのまま使えるか確認する。
// 1つでも範囲外なら false（呼び出し側は既定値を使うか、変更を拒否する）。
bool ec_config_valid(const ec_config_t *config) {
    if (config->magic != EC_CONFIG_MAGIC || config->checksum != ec_config_checksum(config)) return false;
    if (config->discharge_us < 1 || config->discharge_us > EC_DISCHARGE_US_MAX ||
        config->settle_us < 1 || config->settle_us > EC_SETTLE_US_MAX ||
        config->charge_us < 1 || config->charge_us > EC_CHARGE_US_MAX) return false;
    for (unsigned i = 0; i < EC_KEYS; ++i) {
        if (!ec_thresholds_valid(config->press[i], config->release[i])) return false;
    }
    return true;
}

void ec_key_reset(ec_key_t *key) {
    memset(key, 0, sizeof(*key));
}

// 押下中のキーを解放と判定する差分のレベル。
//   max(release, min(peak / 2, press - EC_RELEASE_HYSTERESIS_MIN))
// 深く押したキーは、最大値の半分まで戻れば解放とみなす。ドームが載っているだけで差分が
// release を上回るキーでも離したことを検出できる。上限を press - 40 にしているので、
// 解放した位置ですぐ押下に戻ることはない（press との間に最低 40 counts の幅が残る）。
static int32_t release_level(int32_t peak, uint16_t press, uint16_t release) {
    int32_t level = peak / 2;
    int32_t cap = (int32_t)press - EC_RELEASE_HYSTERESIS_MIN;
    if (level > cap) level = cap;
    return level > release ? level : release;
}

// 1回の走査で得た raw を取り込み、key->pressed を更新する。状態は次の3つ。
//   校正中: 最初の EC_CALIBRATION_SAMPLES 回は未押下とみなして平均し、baseline を決める。
//           この間 pressed は常に false。
//   未押下: 差分 delta = raw - baseline が press 以上で押下。そうでなければ baseline を
//           raw にゆっくり近づける。
//   押下中: delta の最大値 peak を記録し、release_level() 以下になったら解放。
//           押している間は baseline を動かさない。
void ec_key_sample(ec_key_t *key, uint16_t raw, uint16_t press, uint16_t release) {
    key->raw = raw;
    if (key->samples < EC_CALIBRATION_SAMPLES) {
        key->sum += raw;
        ++key->samples;
        if (key->samples == EC_CALIBRATION_SAMPLES) {
            // 四捨五入した平均を基準値にし、追従用の固定小数点値も同じ値で初期化する。
            key->baseline = (key->sum + EC_CALIBRATION_SAMPLES / 2) / EC_CALIBRATION_SAMPLES;
            key->base_q16 = (int32_t)key->baseline << 16;
        }
        key->pressed = false;
        return;
    }
    // 符号付きで引き算し、raw < baseline のときのアンダーフローを防ぐ。
    int32_t delta = (int32_t)raw - key->baseline;
    if (key->pressed) {
        if (delta > key->peak) key->peak = (int16_t)delta;
        if (delta <= release_level(key->peak, press, release)) key->pressed = false;
    } else if (delta >= press) {
        key->pressed = true;
        key->peak = (int16_t)delta;
    } else {
        // 未押下: 緩やかなずれに追従する（指数移動平均）。毎回、差の 1/EC_BASELINE_TRACK_SAMPLES
        // だけ raw へ近づける。1回の変化は1カウント未満なので、固定小数点で積み上げる。
        // 除算は0方向へ切り捨てられるので、raw を追い越すことはない。
        key->base_q16 += (((int32_t)raw << 16) - key->base_q16) / EC_BASELINE_TRACK_SAMPLES;
        key->baseline = (uint16_t)((key->base_q16 + 0x8000) >> 16);
    }
}
