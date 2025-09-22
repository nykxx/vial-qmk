VIA_ENABLE = yes
VIAL_ENABLE = yes
VIALRGB_ENABLE = no

QMK_SETTINGS = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
GRAVE_ESC_ENABLE = yes
ENCODER_MAP_ENABLE = yes

SRC += keycode_utils.c
SRC += matrix_mask.c
SRC += translate_ansi_to_jis.c