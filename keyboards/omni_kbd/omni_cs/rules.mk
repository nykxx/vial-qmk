# Core keyboard features
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CONSOLE_ENABLE = yes
COMMAND_ENABLE = no
NKRO_ENABLE = yes
DYNAMIC_MACRO_ENABLE = yes
DYNAMIC_KEYMAP_ENABLE = yes
OS_DETECTION_ENABLE = yes

# Matrix
CUSTOM_MATRIX = lite
SRC += matrix.c

# Pointing devices
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom

OMNI_POINTING_DEVICE_SOURCES := \
    ../drivers/pmw3360.c \
    ../drivers/pmw33xx_common.c

SRC += $(OMNI_POINTING_DEVICE_SOURCES)
QUANTUM_LIB_SRC += spi_master.c

# LCD and generated Quantum Painter resources
QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += gc9a01_spi

OMNI_FONT_SOURCES := \
    ../font/noto9.qff.c \
    ../font/noto11.qff.c \
    ../font/roboto_mono16.qff.c \
    ../font/st2_mono16.qff.c

OMNI_GENERATED_IMAGE_DIR := ../icon/generated
OMNI_GENERATED_IMAGE_SEARCH_DIR := $(KEYBOARD_PATH_1)/$(OMNI_GENERATED_IMAGE_DIR)
# Keep arbitrary .c files out of the build; only supported generated-image names are discovered.
OMNI_GENERATED_IMAGE_FILES := \
    omni_logo.qgf.c \
    save.qgf.c \
    $(notdir $(wildcard $(OMNI_GENERATED_IMAGE_SEARCH_DIR)/layer_*.qgf.c)) \
    $(notdir $(wildcard $(OMNI_GENERATED_IMAGE_SEARCH_DIR)/[0-9][0-9][0-9].qgf.c))
OMNI_GENERATED_IMAGE_SOURCES := $(addprefix $(OMNI_GENERATED_IMAGE_DIR)/,$(OMNI_GENERATED_IMAGE_FILES))

SRC += ../common/draw_custom.c
SRC += $(OMNI_FONT_SOURCES)
SRC += ../icon/omni_image_loader.c
SRC += $(OMNI_GENERATED_IMAGE_SOURCES)

# Touch controller
I2C_ENABLE = yes
QUANTUM_LIB_SRC += i2c_master.c
SRC += drivers/cst816t.c

# Shared omni modules
OMNI_COMMON_SOURCES := \
    ../common/sleeping_view.c \
    ../common/swipe_gesture.c \
    ../common/view_keymap.c \
    ../common/trackball_omni.c \
    ../common/trackball_tuning.c \
    ../common/config_omni.c \
    ../common/touch_gesture.c \
    ../common/touch_input.c \
    ../common/touch_key_view.c \
    ../common/touch_lcd_omni.c \
    ../common/power_lcd.c \
    ../common/status_view.c

SRC += $(OMNI_COMMON_SOURCES)

# Reload display and device settings after Vial changes the dynamic keymap.
LDFLAGS += -Wl,--wrap=dynamic_keymap_set_keycode
