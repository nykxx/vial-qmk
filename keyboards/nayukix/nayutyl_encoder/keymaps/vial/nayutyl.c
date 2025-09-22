#include QMK_KEYBOARD_H

const matrix_row_t matrix_mask[MATRIX_ROWS] =
{
// 0でマスクされた交点は無視される
// LSBがcol0
  0b00111111,
  0b00111111,
  0b00111111,
  0b00111000,
  0b00110000,
  0b00111111,
  0b00111111,
  0b00111111,
  0b00000111, 
  0b00000011,
};