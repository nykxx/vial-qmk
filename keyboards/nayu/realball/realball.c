// SPDX-License-Identifier: GPL-2.0-or-later
//
// キーボード全体のフック。EC 走査（matrix.c）の初期化と、EC 診断プロトコルの入口を置く。
//
// 診断プロトコルは Raw HID の 32 バイトパケットで、PC のツールから各キーの ADC 値の確認や
// 閾値・タイミングの変更を行う。USB につながっていない側の手への要求は split RPC で転送する
// ので、USB 側の手を1本つなぐだけで左右とも調整できる。
#include "quantum.h"
#include "ec_matrix.h"
#include "transactions.h"
#include "vial.h"
#include <string.h>

// split RPC の受信側（USB につながっていない手）で呼ばれる。USB 側から転送された要求を
// この手で処理し、out に入れた応答が USB 側へ返る。
static void ec_rpc(uint8_t in_size, const void *in, uint8_t out_size, void *out) {
    if (in_size == 32 && out_size == 32) ec_command(in, out);
}

void keyboard_post_init_kb(void) {
    ec_load_config();
    // どちらの手が USB 側になるかは接続で決まるので、両方の手で登録しておく。
    transaction_register_rpc(REALBALL_EC_RPC, ec_rpc);
    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    // EC 設定の EEPROM 保存は、RPC の処理中ではなくここ（メインループ）で行う。
    ec_save_task();
}

// VIA/Vial が自分で処理しないコマンドの Raw HID パケットを受け取ったときに呼ばれる。
// data は応答バッファを兼ねていて、この関数から戻ると VIA が data をそのまま PC へ返す
// （ここで raw_hid_send() を呼んではいけない）。先頭が 0x52 0x42（'R' 'B'）のものを
// EC 診断プロトコルとして扱う。
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (length != 32 || data[0] != 0x52 || data[1] != 0x42) {
        data[0] = 0xff; // VIA の「未対応コマンド」応答と同じ。
        return;
    }
    // data を応答で上書きするので、要求を先に退避する。
    uint8_t request[32];
    memcpy(request, data, sizeof(request));
    // バージョン・対象の手・コマンド番号・キー番号を確認する。エラー時は byte 4 のみ意味を持つ。
    if (request[2] != 1 || request[3] > 2 || request[4] > EC_LAST_COMMAND || request[5] >= 21) {
        data[4] = 1;
        return;
    }
#ifndef REALBALL_EC_SERVICE
    // 通常版では、読み取り（コマンド0）以外は Vial のアンロックが必要。
    // サービス版はキー入力を送らない代わりに、アンロックなしで変更できる。
    if (request[4] != 0 && !vial_unlocked) {
        data[4] = 3; // Vial のアンロックが必要。
        return;
    }
#endif
    // request[3] が 2（USB 側）か自分の手ならここで処理し、そうでなければ相手の手へ転送する。
    uint8_t local_side = is_keyboard_left() ? 0 : 1;
    if (request[3] == 2 || request[3] == local_side) {
        ec_command(request, data);
    } else if (!transaction_rpc_exec(REALBALL_EC_RPC, 32, request, 32, data)) {
        // 左右の通信に失敗（片手だけの接続など）。
        memcpy(data, request, 32);
        data[4] = 2;
    }
}
