# QMK 標準の GPIO マトリクス走査の代わりに matrix.c の matrix_init_custom() /
# matrix_scan_custom() を使う。"lite" なのでデバウンスと左右のデータ転送は QMK 側が行う。
CUSTOM_MATRIX = lite
SERIAL_DRIVER = vendor
DEBOUNCE_TYPE = sym_defer_pk
SRC += matrix.c ec_core.c symbol_shift.c

# ADC は1回ずつポーリングで変換し、行を HIGH にしてから取得するまでの間隔を一定に保つ。
# ADC は EC 走査が専有するので、QMK の analog / joystick ドライバは併用しないこと。
# そのため QMK のドライバではなく、Pico SDK の hardware_adc を直接ビルドに加える。
SRC += lib/pico-sdk/src/rp2_common/hardware_adc/adc.c
EXTRAINCDIRS += lib/pico-sdk/src/rp2_common/hardware_adc/include
