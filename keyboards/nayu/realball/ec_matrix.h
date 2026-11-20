// SPDX-License-Identifier: GPL-2.0-or-later
//
// matrix.c（EC 走査）が realball.c に公開する関数。
#pragma once
#include <stdint.h>

// 受け付けるコマンド番号の上限。コマンド6（放電方式の選択）は試験版にだけある。
// REALBALL_EC_SCANTEST（試験版）/ REALBALL_EC_SERVICE（サービス版）は、それぞれのキーマップの
// config.h で定義する。このリポジトリには通常版の vial キーマップだけを含めている。
#ifdef REALBALL_EC_SCANTEST
#    define EC_LAST_COMMAND 6
#else
#    define EC_LAST_COMMAND 5
#endif

// 保存済みの EC 設定を読み込み、再校正を始める。
void ec_load_config(void);
// EEPROM 保存の予約があれば実行する。メインループから呼ぶこと。
void ec_save_task(void);
// EC 診断コマンドを1つ処理して応答を作る（形式は matrix.c の ec_command() の説明を参照）。
void ec_command(const uint8_t request[32], uint8_t response[32]);
