# LCD仮想キーの表示と実行

## この資料の目的

omni CSのLCD仮想キーは、表示するアイコンとタップ時に実行するキーコードを
別々のQMKレイヤーから取得する。両者を直接結び付けるデータはなく、同じmatrix座標を
共有することで対応している。

この資料では、仮想キーの座標、アイコン表示、タッチ入力、キーコード実行の流れと、
それらの間にある暗黙的な対応を記録する。

## 全体像

表示経路と実行経路は、次のように分かれている。

```text
表示経路
  _FUNCレイヤーの(row, column)
    -> 表示用キーコードを取得
    -> キーコードをアイコン番号へ変換
    -> LCDへ描画

実行経路
  LCDのタッチ座標
    -> 現在のページ、カテゴリ、円形キーを特定
    -> 同じ(row, column)をmatrixの押下として通知
    -> QMKが押下時のアクティブレイヤーからキーコードを解決
    -> 通常のキー処理、またはVialマクロ処理を実行
```

`omni_cs.c`の`virtual_keycode[]`は表示専用のキャッシュである。タップ時のキーコードは
この配列から取得されず、QMKがmatrix座標とアクティブレイヤーから改めて解決する。

## 仮想キーの座標

仮想キーの定数と座標変換は`../common/omni_virtual_keys.h`に定義されている。

- LCDページ数: 4
- カテゴリ数: 3
- 1カテゴリ当たりのキー数: 6
- 仮想キー数: `4 * 3 * 6 = 72`
- 使用するmatrix row: 4から15
- 使用するmatrix column: 0から5
- アイコン表示用のQMKレイヤー: `_FUNC`

ここでいう「ページ」と「カテゴリ」はLCD表示上の状態であり、QMKのレイヤーではない。

ページとカテゴリから、0から11の仮想行を次の式で求める。

```c
virtual_row = page * OMNI_TOUCH_KEY_CATEGORY_COUNT + category;
```

仮想行からQMK matrix rowへの変換は次のとおりである。

```c
matrix_row = OMNI_TOUCH_KEY_MATRIX_ROW_START + virtual_row;
```

画面上の6個の円形キーは、それぞれmatrix column 0から5に対応する。

## アイコン表示の流れ

### 1. `_FUNC`レイヤーから表示用キーコードを読み込む

`omni_cs.c`の`load_virtual_keys()`は、matrix row 4から15、column 0から5を走査し、
各座標のキーコードを`virtual_keycode[]`へ格納する。

```c
virtual_keycode[key_index] = keymap_key_to_keycode(
    OMNI_TOUCH_KEY_DISPLAY_LAYER,
    (keypos_t){.row = row, .col = col}
);
```

`OMNI_TOUCH_KEY_DISPLAY_LAYER`は`_FUNC`なので、ここで参照するレイヤーは常に固定である。
Vialでは、最終的にdynamic keymapのEEPROMに保存された`_FUNC`レイヤーのキーコードが
読み出される。

`virtual_keycode[]`の添字は、次の式に相当する。

```c
keycode_index = virtual_row * OMNI_TOUCH_KEYS_PER_CATEGORY + key_index;
```

### 2. キーコードをアイコン画像へ変換する

`../common/touch_key_view.c`の`touch_key_view_initialize()`は、表示用キーコードを
`omni_keycode_image_handle()`へ渡し、各ページ・カテゴリの画像配置を構築する。

```c
.image = omni_keycode_image_handle(virtual_keycodes[keycode_index])
```

`../icon/omni_image_loader.c`の`omni_keycode_image_handle()`は、キーコードから
`0x7700`を引いた値をアイコン番号として使用する。

```c
icon_index = keycode - 0x7700;
```

したがって、代表的な対応は次のようになる。

| 表示用キーコード | アイコン番号 |
|---:|---:|
| `0x7700` | 0 |
| `0x7701` | 1 |
| `0x7702` | 2 |
| `0x7747` | 71 |

この互換表現は`../common/omni_keycode_encoding.h`で
`omni_icon_index_from_keycode()`として名前を付けている。

### 3. 現在のページとカテゴリを描画する

`touch_key_view_draw()`は、現在のページとカテゴリに対応する画像を選び、中央画像と
6個のキー画像をLCDへ描画する。

中央画像は仮想キーのキーコードから選ばれるものではない。ページとカテゴリから求めた
仮想行0から11を、そのまま`layer_00`から`layer_11`の画像番号として使用する。

## タッチしたキーを実行する流れ

### 1. タッチ座標から仮想キーを特定する

`../common/touch_key_view.c`の`touch_key_view_locate_key()`は、タッチ座標が6個の
円形キーのどれに含まれるかを判定する。該当する場合は、現在のページとカテゴリから
仮想行を求め、円形キー番号を仮想columnとして返す。

```c
*virtual_row = omni_touch_key_virtual_row(current_page, current_category);
*virtual_column = key_index;
```

### 2. 仮想座標をQMK matrix座標へ変換する

`matrix.c`の`get_touch_coordinates()`は仮想行に開始rowの4を加え、QMK matrix上の
row 4から15へ変換する。columnは円形キー番号0から5をそのまま使用する。

### 3. matrixへ押下を通知する

`matrix.c`の`read_touch()`は、求めたmatrix座標のbitを押下または解放状態へ更新する。

```c
matrix[row_index] |= MATRIX_ROW_SHIFTER << col_index;
```

この時点ではキーコードを取得していない。「指定したrow、columnが押された」という
位置情報だけをQMKへ通知している。

現在は既存のパルスタイミングを維持するため、QMKの確定済み`matrix`を直接更新している。
この方式を将来変更する案については保留中であり、リファクタリング完了後に再検討する。

### 4. QMKがアクティブレイヤーからキーコードを解決する

QMKのmatrix処理はbitの変化を検出すると、rowとcolumnを持つキーイベントを生成する。
その後、`layer_switch_get_layer()`が押下時のアクティブレイヤーを調べ、透過キーを考慮して
実行するキーコードを決定する。

したがって、タップ時に実行されるのは表示用の`_FUNC`レイヤーにあるキーコードではなく、
押下時のアクティブレイヤーにある同じmatrix座標のキーコードである。

押下後にレイヤーが変化しても正しく解放できるよう、QMKは押下時に選択したレイヤーを
キャッシュし、解放時にも同じアクションを使用する。

### 5. マクロキーコードを実行する

アクティブレイヤー側のキーコードが`0x7700 + N`の場合、この範囲はQMK/Vialでは
`QK_MACRO + N`を意味する。通常のQMKキー処理を経由して`process_record_via()`へ到達し、
次のようにマクロ番号へ変換される。

```c
macro_id = keycode - QK_MACRO;
dynamic_keymap_macro_send(macro_id);
```

したがって、実行経路では次の対応になる。

| 実行用キーコード | 処理 |
|---:|---|
| `0x7700` | Vialマクロ0を実行 |
| `0x7701` | Vialマクロ1を実行 |
| `0x7702` | Vialマクロ2を実行 |
| `0x7747` | Vialマクロ71を実行 |

同じ`0x7700 + N`という数値が、表示経路ではアイコン番号N、実行経路ではマクロ番号Nとして
解釈される。この数値表現の共有は既存ファームウェアとの互換性のため維持している。

## 座標対応の例

ページ1、カテゴリ2、画面上のキー3を例にする。

```text
virtual_row = 1 * 3 + 2 = 5
matrix row  = 4 + 5     = 9
matrix col  = 3
配列index   = 5 * 6 + 3 = 33
```

表示経路では、`_FUNC`レイヤーの`(row 9, column 3)`からキーコードを取得し、
`virtual_keycode[33]`へ格納してアイコンを選択する。

実行経路では、現在のアクティブレイヤーの`(row 9, column 3)`をQMKが解決し、その位置に
設定されたキーコードを実行する。

## 表示データを更新するタイミング

起動時は、`matrix_init_user()`が`load_virtual_keys()`を呼び出して表示用キーコードを
読み込む。起動ロゴの表示終了時に`touch_key_view_initialize()`が呼ばれ、読み込み済みの
キーコードから画像配置を構築する。

Vialがdynamic keymapを書き換えた場合は、`omni_cs.c`の
`__wrap_dynamic_keymap_set_keycode()`が次の処理を行う。

1. `load_virtual_keys()`で`_FUNC`レイヤーの表示用キーコードを再読込する。
2. `update_lcd_view_data()`で画像配置を再構築し、LCDを再描画する。

設定保存のためにdynamic keymapを書き換える処理では、不要な再描画を避けるため
`keymap_change_update_flag`を一時的に無効化している。

## 保守時の注意点

- `virtual_keycode[]`は表示専用であり、実行用キーコードのキャッシュではない。
- 表示レイヤーは`_FUNC`に固定されているが、実行レイヤーは押下時のQMKレイヤー状態で決まる。
- 表示と実行の対応は、row 4から15、column 0から5という座標の一致に依存する。
- ページ数、カテゴリ数、開始rowを変更する場合は、表示側とmatrix側の双方で
  `omni_virtual_keys.h`の変換APIを使用する。
- `_FUNC`レイヤーの表示用キーコードだけを変更すると、アイコンと実際の操作が一致しなくなる
  可能性がある。必要に応じて実行対象レイヤーの同じ座標も確認する。
- `0x7700 + N`には、アイコン番号とVialマクロ番号という異なる意味がある。
  新しい処理では`omni_keycode_encoding.h`の用途別APIを使い、数値を直接演算しない。

