// SPDX-License-Identifier: GPL-2.0-or-later
//
// Realball の EC（静電容量）スイッチ用カスタムマトリクス走査。
//
// rules.mk の CUSTOM_MATRIX = lite により、QMK 標準の「行を駆動して列の GPIO を読む」走査の
// 代わりに、このファイルの matrix_init_custom() / matrix_scan_custom() を使う。
// デバウンス・左右の行データ転送・キーマップ処理は QMK 側がそのまま行う。
//
// ■ 検出回路の概要（左右の PCB で共通）
//   - 行駆動線 GP8〜GP11 を LOW から HIGH にすると、その立ち上がりがキーの静電容量を通じて
//     列の電極へ電荷を送る。キーを押すと結合容量が増え、送られる電荷も増える。
//   - 6本の列のうち1本だけを、アナログ MUX U2 (74HC4051) で共通ノード SENSE_SUM (C7) へつなぐ。
//     U2 の Y6 は基板上で GND に接続されているので、Y6 を選ぶと SENSE_SUM を GND へ放電できる。
//   - SENSE_SUM の電圧をアンプ U3 で増幅し、GP26 (ADC0) で 12 bit 値として読む。
//   ADC 値は「未押下時の基準値＋押し込みに応じた増分」になるので、押下は基準値との差分で
//   判定する（ec_core.c）。
//
// ■ 1キーの読み取り（read_key()、既定の放電方式 EC_MODE_MUX_GND_ROW_FIRST の場合）
//   1. 全行 LOW・MUX は Y6 (GND) のまま discharge_us 待ち、SENSE_SUM の電荷を抜く。
//   2. MUX を対象の列へ切り替え（＝放電を止める）、settle_us 待つ。
//   3. 対象の行を HIGH にし、アンプの出力が立ち上がるまで charge_us 待つ。
//   4. ADC を1回だけ変換して読む。
//   5. 行を LOW に戻してから MUX を Y6 へ戻す（次のキーの放電を兼ねる）。
//   2〜5 は µs 単位の時刻で値が変わるため、割り込みを禁止して実行する。
//
// ■ 21キーの配置
//   キー番号 k（0〜20 = PCB の SW1〜SW21）を row = k / 6、col = k % 6 として、
//   この手の4行×6列に並べる。col はそのまま MUX のチャネル番号 Y0〜Y5 になる。
#include "quantum.h"
// ADC とタイマは QMK/ChibiOS のドライバではなく、Pico SDK を直接使う。
// ChibiOS の ADC / TIMER マクロが Pico SDK 内の識別子と衝突するため、include の前に外す。
#undef ADC
#undef TIMER
#include "hardware/adc.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "ec_core.h"
#include "ec_matrix.h"
#include <string.h>

// SENSE_SUM の放電方式。値は診断プロトコルの定数（応答の byte 30）なので変えないこと。
//
// 当初の回路設計は Q1 で放電する EC_MODE_Q1 だった。しかし 2026-11-19 の実機確認で、
// Q1（BC847B）はコレクタ電流がほぼ0のまま深く飽和させるため、GP15 を LOW にしても約 12 us は
// SENSE_SUM を GND 付近に保持し、その後も約 200 us 弱く導通し続けて、駆動エッジの信号が
// 失われることがわかった。そのため全ビルドで Q1 を OFF のままにし、基板を改造せずに済む
// U2 経由で放電する。通常版・サービス版は EC_DEFAULT_SCAN_MODE で固定。
// 他の方式は比較・切り分け用に残しており、試験版だけが実行時に選べる（コマンド6）。
enum {
    EC_MODE_Q1,                // GP15 HIGH で Q1 を ON、LOW で OFF（当初の回路設計）。
    EC_MODE_MUX_GND,           // Q1 は OFF。放電中は U2 で GND 接続の Y6 を選ぶ。
    EC_MODE_PULLDOWN,          // Q1 は OFF。選択中の列の 100k だけで SENSE_SUM を下げる。
    EC_MODE_Q1_WEAK,           // GP15 の内蔵プルアップ（50〜80k）で Q1 を弱く ON、GP15 LOW で OFF。
    EC_MODE_MUX_GND_ROW_FIRST, // MUX_GND と同じだが、列を切り離す前に行を LOW へ戻す。
    EC_MODE_COUNT
};
#define EC_DEFAULT_SCAN_MODE EC_MODE_MUX_GND_ROW_FIRST
#define EC_MUX_GND_CHANNEL 6 // U2 の Y6（2番ピン）は左右の PCB とも GND に接続されている。
#define EC_CRITICAL_US 10    // settle が長い場合、割り込みを禁止するのは最後のこの時間だけにする。

static const pin_t row_pins[] = EC_ROW_PINS;
static const pin_t mux_pins[] = EC_MUX_PINS;
static ec_config_t config;          // 使用中の閾値・タイミング。EEPROM への保存は ec_save_task() で行う。
static ec_key_t keys[EC_KEYS];      // キーごとの判定状態（基準値など）。RAM のみ。
static uint32_t calibration_start;  // 再校正を始めた時刻。EC_CALIBRATION_DELAY_MS 後に基準値の測定を始める。
static uint32_t adc_errors;         // ADC のタイムアウト・変換エラーの累積（診断用）。
static bool save_pending;           // EEPROM 保存の予約（EC コマンド3）。
static bool config_loaded;          // ec_load_config() が終わるまでキーを判定しない。
static uint8_t scan_mode = EC_DEFAULT_SCAN_MODE; // 現在の放電方式。
// 要求された放電方式。ec_command() が split RPC の処理中に書き換えることがあるため volatile に
// しておき、反映は走査の合間に行う。
static volatile uint8_t requested_mode = EC_DEFAULT_SCAN_MODE;

// 全キーを校正前に戻して再校正を始める。実際の基準値測定は matrix_scan_custom() が
// EC_CALIBRATION_DELAY_MS 待ってから、各キー EC_CALIBRATION_SAMPLES 回の平均で行う。
// 閾値やタイミングを変えると基準値も変わりうるので、それらを変更したときにも呼ぶ。
static void calibrate(void) {
    for (unsigned k = 0; k < EC_KEYS; ++k) ec_key_reset(&keys[k]);
    calibration_start = timer_read32();
}

// 保存済みの EC 設定を読み込み、再校正を始める。keyboard_post_init_kb() から呼ばれる。
// 保存データがない・壊れている・形式が古い場合は既定値を使う。
void ec_load_config(void) {
    ec_config_defaults(&config);
    ec_config_t stored = {0};
    if (eeconfig_is_kb_datablock_valid()) {
        eeconfig_read_kb_datablock(&stored, 0, sizeof(stored));
        if (ec_config_valid(&stored)) config = stored;
    }
    config_loaded = true;
    calibrate();
}

// housekeeping_task_kb() から毎ループ呼ばれ、予約された保存を実行する。
// RP2040 の EEPROM はフラッシュのエミュレーションで、書き込みに時間がかかる。そのため
// 書き込みはこのメインループで行い、split RPC のコールバック内では行わない。
void ec_save_task(void) {
    if (save_pending) {
        config.checksum = ec_config_checksum(&config);
        eeconfig_update_kb_datablock(&config, 0, sizeof(config));
        save_pending = false;
    }
}

// U2 の選択線 S0〜S2 を channel（0〜7）に設定する。0〜5 が列、6 が GND。
// 3本を1回の書き込みで同時に変え、途中で別の列がつながらないようにする。
static void mux_select(uint8_t channel) {
    uint32_t mask = 0, value = 0;
    for (unsigned b = 0; b < 3; ++b) {
        mask |= 1u << mux_pins[b];
        if (channel & (1u << b)) value |= 1u << mux_pins[b];
    }
    gpio_put_masked(mask, value); // Pico SDK: SIO の GPIO_OUT_XOR への1回の書き込みになる。
}

// MUX で放電する方式なら true。この場合、放電中の MUX は Y6 を選んでいて、
// 列への切り替えが放電の解除を兼ねる（release_sense()）。
static bool mux_discharge(void) {
    return scan_mode == EC_MODE_MUX_GND || scan_mode == EC_MODE_MUX_GND_ROW_FIRST;
}

// SENSE_SUM の放電を止め、選択した列が駆動エッジの電荷を受け取れるようにする。
// 放電方式ごとに操作が異なる。MUX 方式では Y6 から対象列へ切り替える。
static void release_sense(uint8_t col) {
    switch (scan_mode) {
        case EC_MODE_MUX_GND:
        case EC_MODE_MUX_GND_ROW_FIRST:
            mux_select(col);
            break;
        case EC_MODE_PULLDOWN:
            break; // 放電用の素子を使わない方式なので、何もしない。
        case EC_MODE_Q1_WEAK:
            // 内蔵プルアップで弱く ON にしていた Q1 を、出力 LOW に戻して OFF にする。
            gpio_write_pin_low(EC_DISCHARGE_PIN);
            gpio_set_pin_output(EC_DISCHARGE_PIN);
            break;
        default: // EC_MODE_Q1
            gpio_write_pin_low(EC_DISCHARGE_PIN);
            break;
    }
}

// SENSE_SUM の放電を始める。MUX 方式では Y6 = GND を選ぶ。
// 読み取りの直後と、放電方式の切り替え時に呼ぶ。
static void engage_discharge(void) {
    switch (scan_mode) {
        case EC_MODE_MUX_GND:
        case EC_MODE_MUX_GND_ROW_FIRST:
            mux_select(EC_MUX_GND_CHANNEL);
            break;
        case EC_MODE_PULLDOWN:
            break;
        case EC_MODE_Q1_WEAK:
            gpio_set_pin_input_high(EC_DISCHARGE_PIN); // GP15 を入力＋内蔵プルアップにする。
            break;
        default: // EC_MODE_Q1
            gpio_write_pin_high(EC_DISCHARGE_PIN);
            break;
    }
}

// 放電方式を切り替え、その方式の待機状態（放電中）にする。
static void apply_scan_mode(uint8_t mode) {
    scan_mode = mode;
    gpio_write_pin_low(EC_DISCHARGE_PIN); // 下の engage_discharge() で Q1 を使う方式以外は Q1 OFF。
    gpio_set_pin_output(EC_DISCHARGE_PIN);
    engage_discharge();
}

// QMK の matrix_init() から起動時に1回呼ばれる。全行 LOW・放電中の状態にし、ADC を準備する。
// この時点では既定の設定で動かし、保存済みの設定は keyboard_post_init_kb() で読み込む。
void matrix_init_custom(void) {
    for (unsigned r = 0; r < 4; ++r) {
        gpio_write_pin_low(row_pins[r]);
        gpio_set_pin_output(row_pins[r]);
    }
    for (unsigned b = 0; b < 3; ++b) {
        gpio_write_pin_low(mux_pins[b]);
        gpio_set_pin_output(mux_pins[b]);
    }
    apply_scan_mode(scan_mode);
    adc_init();
    adc_gpio_init(EC_ADC_PIN);
    adc_select_input(0); // ADC0 = GP26
    adc_set_clkdiv(0);
    ec_config_defaults(&config);
    calibrate();
}

// 1キーを読み取り、12 bit の ADC 値を *result に入れる。ADC がタイムアウト・エラーなら false。
// 手順はファイル冒頭の「1キーの読み取り」を参照。呼び出し時は直前の読み取りによって
// 全行 LOW・放電中になっており、戻るときも同じ状態にする。
static bool read_key(uint8_t row, uint8_t col, uint16_t *result) {
    // 1. 放電。MUX 以外の方式では、放電中に MUX を対象列へ向けておく。
    if (!mux_discharge()) mux_select(col);
    wait_us(config.discharge_us);
    // 2. 放電を止め、settle_us 待つ。settle_us が EC_CRITICAL_US を超える場合は、前半を
    //    割り込み許可のまま待ち、最後の EC_CRITICAL_US だけ割り込みを禁止する。
    //    USB などの処理を長く止めずに、取得直前の時刻だけは正確に保つため。
    uint16_t early = config.settle_us > EC_CRITICAL_US ? config.settle_us - EC_CRITICAL_US : 0;
    if (early) {
        release_sense(col);
        wait_us(early);
    }
    // ここから放電に戻るまで割り込み禁止。割り込みで待ち時間が延びると ADC 値がずれる。
    uint32_t irq_state = save_and_disable_interrupts();
    if (!early) release_sense(col);
    wait_us(config.settle_us - early);
    // 3. 行を HIGH にして、キーの静電容量を通じて列へ電荷を送る。
    gpio_write_pin_high(row_pins[row]);
    // アンプ出力は駆動エッジから 4〜6 us かかって 0 V 付近から立ち上がる。
    // これより早く読むと信号の大半を取り逃がす。
    wait_us(config.charge_us);
    // 4. ADC の単発変換。Pico SDK の adc_read() は完了を無期限に待つので、レジスタを直接操作し、
    //    タイムアウトとエラーの確認を加えている。変換は通常約 2 us で終わる（20 us で打ち切り）。
    adc_hw->cs |= ADC_CS_START_ONCE_BITS;
    uint32_t start = time_us_32();
    while (!(adc_hw->cs & ADC_CS_READY_BITS) && (uint32_t)(time_us_32() - start) < 20) {}
    bool ok = (adc_hw->cs & ADC_CS_READY_BITS) && !(adc_hw->cs & ADC_CS_ERR_BITS);
    *result = adc_hw->result & EC_ADC_MAX;
    // 5. 行を LOW に戻し、放電を始める。2つの操作の順番は放電方式で異なる。
    if (scan_mode == EC_MODE_MUX_GND_ROW_FIRST) {
        // 列をつないだまま行を下げ、立ち下がりエッジを C7 に吸収させる。先に列を切り離すと、
        // その列が GND より低い電圧に引かれ、次のキーへ MUX を切り替える瞬間に漏れ込む。
        gpio_write_pin_low(row_pins[row]);
        wait_us(1);
        engage_discharge();
    } else {
        engage_discharge(); // 行を下げる前に放電を始め、立ち下がりで生じる負電圧を抑える。
        gpio_write_pin_low(row_pins[row]);
    }
    restore_interrupts(irq_state);
    if (!ok) ++adc_errors;
    return ok;
}

// QMK のメインループから毎回呼ばれる。この手の21キーを読み、押下中のキーのビットを立てた
// マトリクスを current_matrix に書く。戻り値は前回の内容から変化があったか。
// current_matrix はこの手の4行分（split では MATRIX_ROWS / 2 行）。QMK はこの値を
// デバウンスしてから、左右を合わせた8行のマトリクスに入れる。
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    // 試験版のコマンド6で要求された放電方式は、走査の合間であるここで切り替える。
    if (requested_mode != scan_mode) apply_scan_mode(requested_mode);
    matrix_row_t next[4] = {0};
    // 起動・再校正の直後 EC_CALIBRATION_DELAY_MS の間は、手が離れるのを待つ期間。
    // raw（診断用）だけ更新し、基準値の測定と押下判定はしない。
    bool ready_to_sample = config_loaded && timer_elapsed32(calibration_start) >= EC_CALIBRATION_DELAY_MS;
    for (uint8_t k = 0; k < EC_KEYS; ++k) {
        uint8_t row = k / MATRIX_COLS, col = k % MATRIX_COLS;
        uint16_t raw;
        if (read_key(row, col, &raw)) {
            if (ready_to_sample) ec_key_sample(&keys[k], raw, config.press[k], config.release[k]);
            else keys[k].raw = raw;
        } else {
            // ADC が失敗したキーが押されたまま残らないよう、解放扱いにする。
            keys[k].pressed = false;
        }
#ifndef REALBALL_EC_SERVICE
        // サービス版・試験版は EC の判定までは行うが、結果を QMK に渡さない。
        // 閾値が未調整でも、勝手にキー入力が送られないようにするため。
        if (keys[k].pressed) next[row] |= (matrix_row_t)1 << col;
#endif
    }
    bool changed = memcmp(next, current_matrix, sizeof(next)) != 0;
    memcpy(current_matrix, next, sizeof(next));
    return changed;
}

// 診断プロトコルの多バイト値は little endian。
static uint16_t get16(const uint8_t *p) { return p[0] | (uint16_t)p[1] << 8; }
static void put16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }

// EC 診断コマンドを1つ処理し、32 バイトの応答を作る。USB につながっている手では
// raw_hid_receive_kb() から、もう片方の手では split RPC（realball.c の ec_rpc()）から呼ばれる。
//
// 要求: [0..2] 52 42 01（識別子 'R' 'B' とバージョン）、[3] 対象の手（0=左, 1=右, 2=USB 側）、
//       [4] コマンド、[5] キー番号 0〜20、[6..] 引数。
// 応答: [4] ステータス（0=成功、1=不正な要求、4=保存中）。その後に、どのコマンドでも
//       この手の状態と、[5] で指定したキーの測定値を入れる（下の put16 の並びを参照）。
// 変更はすべて RAM 上で行い、EEPROM へはコマンド3で保存するまで書き込まない。
void ec_command(const uint8_t request[32], uint8_t response[32]) {
    uint8_t cmd = request[4], index = request[5], status = 0;
    memset(response, 0, 32);
    response[0] = 0x52; response[1] = 0x42; response[2] = 1;
    response[3] = request[3]; response[5] = cmd;
    if (request[0] != 0x52 || request[1] != 0x42 || request[2] != 1 || index >= EC_KEYS) {
        response[4] = 1;
        return;
    }
    // 保存の実行待ちの間は、保存される内容が変わらないよう状態取得以外を断る。
    if (save_pending && cmd != 0) status = 4;
    else switch (cmd) {
        case 0: break; // 状態と指定キーの測定値を読むだけ。
        case 1: { // キー index の閾値を変更する。
            uint16_t press = get16(request + 6), release = get16(request + 8);
            if (!ec_thresholds_valid(press, release)) status = 1;
            else { config.press[index] = press; config.release[index] = release; }
            break;
        }
        case 2: calibrate(); break;           // 再校正（全キーから手を離して実行する）。
        case 3: save_pending = true; break;   // 保存を予約。書き込みは ec_save_task() が行う。
        case 4: ec_config_defaults(&config); calibrate(); break; // 既定値に戻す（保存はしない）。
        case 5: { // 走査タイミングを変更する。値が変わるので再校正する。
            ec_config_t candidate = config;
            candidate.discharge_us = get16(request + 6);
            candidate.settle_us = get16(request + 8);
            candidate.charge_us = get16(request + 10);
            candidate.checksum = ec_config_checksum(&candidate);
            if (!ec_config_valid(&candidate)) status = 1;
            else { config = candidate; calibrate(); }
            break;
        }
#ifdef REALBALL_EC_SCANTEST
        case 6: // 放電方式の選択（試験版のみ）。RAM のみで、次の走査の前に反映する。
            if (request[6] >= EC_MODE_COUNT) status = 1;
            else { requested_mode = request[6]; calibrate(); }
            break;
#endif
        default: status = 1;
    }
    // ready: 全キーの校正が終わっているか。
    bool ready = true;
    for (unsigned k = 0; k < EC_KEYS; ++k) ready &= keys[k].samples == EC_CALIBRATION_SAMPLES;
    response[4] = status;
    response[6] = is_keyboard_left() ? 0 : 1; // 実際に応答した手。
    response[7] = ready;
    response[8] = index;
    response[9] = keys[index].pressed;
    put16(response + 10, keys[index].raw);
    put16(response + 12, keys[index].baseline);
    put16(response + 14, config.press[index]);
    put16(response + 16, config.release[index]);
    put16(response + 18, config.discharge_us);
    put16(response + 20, config.settle_us);
    put16(response + 22, config.charge_us);
    for (unsigned i = 0; i < 4; ++i) response[24 + i] = adc_errors >> (8 * i);
    response[28] = save_pending;
#ifdef REALBALL_EC_SERVICE
    response[29] = 1; // サービス版（キー入力を送らない）であることを示す。
#endif
    response[30] = requested_mode;
}
