// Copyright 2025 mass
// SPDX-License-Identifier: GPL-2.0-or-later

#include "omni_image_loader.h"
#include "generated/omni_logo.qgf.h"
#include "generated/save.qgf.h"
#include "generated/layer_00.qgf.h"
#include "generated/layer_01.qgf.h"
#include "generated/layer_02.qgf.h"
#include "generated/layer_03.qgf.h"
#include "generated/layer_04.qgf.h"
#include "generated/layer_05.qgf.h"
#include "generated/layer_06.qgf.h"
#include "generated/layer_07.qgf.h"
#include "generated/layer_08.qgf.h"
#include "generated/layer_09.qgf.h"
#include "generated/layer_10.qgf.h"
#include "generated/layer_11.qgf.h"
#include "generated/000.qgf.h"
#include "generated/001.qgf.h"
#include "generated/002.qgf.h"
#include "generated/003.qgf.h"
#include "generated/004.qgf.h"
#include "generated/005.qgf.h"
#include "generated/006.qgf.h"
#include "generated/007.qgf.h"
#include "generated/008.qgf.h"
#include "generated/009.qgf.h"
#include "generated/010.qgf.h"
#include "generated/011.qgf.h"
#include "generated/012.qgf.h"
#include "generated/013.qgf.h"
#include "generated/014.qgf.h"
#include "generated/015.qgf.h"
#include "generated/016.qgf.h"
#include "generated/017.qgf.h"
#include "generated/018.qgf.h"
#include "generated/019.qgf.h"
#include "generated/020.qgf.h"
#include "generated/021.qgf.h"
#include "generated/022.qgf.h"
#include "generated/023.qgf.h"
#include "generated/024.qgf.h"
#include "generated/025.qgf.h"
#include "generated/026.qgf.h"
#include "generated/027.qgf.h"
#include "generated/028.qgf.h"
#include "generated/029.qgf.h"
#include "generated/030.qgf.h"
#include "generated/031.qgf.h"
#include "generated/032.qgf.h"
#include "generated/033.qgf.h"
#include "generated/034.qgf.h"
#include "generated/035.qgf.h"
#include "generated/036.qgf.h"
#include "generated/037.qgf.h"
#include "generated/038.qgf.h"
#include "generated/039.qgf.h"
#include "generated/040.qgf.h"
#include "generated/041.qgf.h"
#include "generated/042.qgf.h"
#include "generated/043.qgf.h"
#include "generated/044.qgf.h"
#include "generated/045.qgf.h"
#include "generated/046.qgf.h"
#include "generated/047.qgf.h"
#include "generated/048.qgf.h"
#include "generated/049.qgf.h"
#include "generated/050.qgf.h"
#include "generated/051.qgf.h"
#include "generated/052.qgf.h"
#include "generated/053.qgf.h"
#include "generated/054.qgf.h"
#include "generated/055.qgf.h"
#include "generated/056.qgf.h"
#include "generated/057.qgf.h"
#include "generated/058.qgf.h"
#include "generated/059.qgf.h"
#include "generated/060.qgf.h"
#include "generated/061.qgf.h"
#include "generated/062.qgf.h"
#include "generated/063.qgf.h"
#include "generated/064.qgf.h"
#include "generated/065.qgf.h"
#include "generated/066.qgf.h"
#include "generated/067.qgf.h"
#include "generated/068.qgf.h"
#include "generated/069.qgf.h"
#include "generated/070.qgf.h"
#include "generated/071.qgf.h"
#include "generated/072.qgf.h"
#include "generated/073.qgf.h"
#include "generated/074.qgf.h"
#include "generated/075.qgf.h"
#include "generated/076.qgf.h"
#include "generated/077.qgf.h"
#include "generated/078.qgf.h"
#include "generated/079.qgf.h"
#include "generated/080.qgf.h"
#include "generated/081.qgf.h"
#include "generated/082.qgf.h"
#include "generated/083.qgf.h"
#include "generated/084.qgf.h"
#include "generated/085.qgf.h"
#include "generated/086.qgf.h"

enum {
    LAYER_IMAGE_COUNT = 12,
    MACRO_KEY_START   = 0x7700,
    MACRO_KEY_END     = 0x77FE,
    MACRO_KEY_COUNT   = MACRO_KEY_END - MACRO_KEY_START + 1,
};

static painter_image_handle_t logo_image;
static painter_image_handle_t save_image;
static painter_image_handle_t layer_images[LAYER_IMAGE_COUNT];
static painter_image_handle_t keycode_images[MACRO_KEY_COUNT];

void initialize_images(void) {
    logo_image = qp_load_image_mem(gfx_omni_logo);
    save_image = qp_load_image_mem(gfx_save);
    layer_images[0] = qp_load_image_mem(gfx_layer_00);
    layer_images[1] = qp_load_image_mem(gfx_layer_01);
    layer_images[2] = qp_load_image_mem(gfx_layer_02);
    layer_images[3] = qp_load_image_mem(gfx_layer_03);
    layer_images[4] = qp_load_image_mem(gfx_layer_04);
    layer_images[5] = qp_load_image_mem(gfx_layer_05);
    layer_images[6] = qp_load_image_mem(gfx_layer_06);
    layer_images[7] = qp_load_image_mem(gfx_layer_07);
    layer_images[8] = qp_load_image_mem(gfx_layer_08);
    layer_images[9] = qp_load_image_mem(gfx_layer_09);
    layer_images[10] = qp_load_image_mem(gfx_layer_10);
    layer_images[11] = qp_load_image_mem(gfx_layer_11);
    keycode_images[0] = qp_load_image_mem(gfx_000);
    keycode_images[1] = qp_load_image_mem(gfx_001);
    keycode_images[2] = qp_load_image_mem(gfx_002);
    keycode_images[3] = qp_load_image_mem(gfx_003);
    keycode_images[4] = qp_load_image_mem(gfx_004);
    keycode_images[5] = qp_load_image_mem(gfx_005);
    keycode_images[6] = qp_load_image_mem(gfx_006);
    keycode_images[7] = qp_load_image_mem(gfx_007);
    keycode_images[8] = qp_load_image_mem(gfx_008);
    keycode_images[9] = qp_load_image_mem(gfx_009);
    keycode_images[10] = qp_load_image_mem(gfx_010);
    keycode_images[11] = qp_load_image_mem(gfx_011);
    keycode_images[12] = qp_load_image_mem(gfx_012);
    keycode_images[13] = qp_load_image_mem(gfx_013);
    keycode_images[14] = qp_load_image_mem(gfx_014);
    keycode_images[15] = qp_load_image_mem(gfx_015);
    keycode_images[16] = qp_load_image_mem(gfx_016);
    keycode_images[17] = qp_load_image_mem(gfx_017);
    keycode_images[18] = qp_load_image_mem(gfx_018);
    keycode_images[19] = qp_load_image_mem(gfx_019);
    keycode_images[20] = qp_load_image_mem(gfx_020);
    keycode_images[21] = qp_load_image_mem(gfx_021);
    keycode_images[22] = qp_load_image_mem(gfx_022);
    keycode_images[23] = qp_load_image_mem(gfx_023);
    keycode_images[24] = qp_load_image_mem(gfx_024);
    keycode_images[25] = qp_load_image_mem(gfx_025);
    keycode_images[26] = qp_load_image_mem(gfx_026);
    keycode_images[27] = qp_load_image_mem(gfx_027);
    keycode_images[28] = qp_load_image_mem(gfx_028);
    keycode_images[29] = qp_load_image_mem(gfx_029);
    keycode_images[30] = qp_load_image_mem(gfx_030);
    keycode_images[31] = qp_load_image_mem(gfx_031);
    keycode_images[32] = qp_load_image_mem(gfx_032);
    keycode_images[33] = qp_load_image_mem(gfx_033);
    keycode_images[34] = qp_load_image_mem(gfx_034);
    keycode_images[35] = qp_load_image_mem(gfx_035);
    keycode_images[36] = qp_load_image_mem(gfx_036);
    keycode_images[37] = qp_load_image_mem(gfx_037);
    keycode_images[38] = qp_load_image_mem(gfx_038);
    keycode_images[39] = qp_load_image_mem(gfx_039);
    keycode_images[40] = qp_load_image_mem(gfx_040);
    keycode_images[41] = qp_load_image_mem(gfx_041);
    keycode_images[42] = qp_load_image_mem(gfx_042);
    keycode_images[43] = qp_load_image_mem(gfx_043);
    keycode_images[44] = qp_load_image_mem(gfx_044);
    keycode_images[45] = qp_load_image_mem(gfx_045);
    keycode_images[46] = qp_load_image_mem(gfx_046);
    keycode_images[47] = qp_load_image_mem(gfx_047);
    keycode_images[48] = qp_load_image_mem(gfx_048);
    keycode_images[49] = qp_load_image_mem(gfx_049);
    keycode_images[50] = qp_load_image_mem(gfx_050);
    keycode_images[51] = qp_load_image_mem(gfx_051);
    keycode_images[52] = qp_load_image_mem(gfx_052);
    keycode_images[53] = qp_load_image_mem(gfx_053);
    keycode_images[54] = qp_load_image_mem(gfx_054);
    keycode_images[55] = qp_load_image_mem(gfx_055);
    keycode_images[56] = qp_load_image_mem(gfx_056);
    keycode_images[57] = qp_load_image_mem(gfx_057);
    keycode_images[58] = qp_load_image_mem(gfx_058);
    keycode_images[59] = qp_load_image_mem(gfx_059);
    keycode_images[60] = qp_load_image_mem(gfx_060);
    keycode_images[61] = qp_load_image_mem(gfx_061);
    keycode_images[62] = qp_load_image_mem(gfx_062);
    keycode_images[63] = qp_load_image_mem(gfx_063);
    keycode_images[64] = qp_load_image_mem(gfx_064);
    keycode_images[65] = qp_load_image_mem(gfx_065);
    keycode_images[66] = qp_load_image_mem(gfx_066);
    keycode_images[67] = qp_load_image_mem(gfx_067);
    keycode_images[68] = qp_load_image_mem(gfx_068);
    keycode_images[69] = qp_load_image_mem(gfx_069);
    keycode_images[70] = qp_load_image_mem(gfx_070);
    keycode_images[71] = qp_load_image_mem(gfx_071);
    keycode_images[72] = qp_load_image_mem(gfx_072);
    keycode_images[73] = qp_load_image_mem(gfx_073);
    keycode_images[74] = qp_load_image_mem(gfx_074);
    keycode_images[75] = qp_load_image_mem(gfx_075);
    keycode_images[76] = qp_load_image_mem(gfx_076);
    keycode_images[77] = qp_load_image_mem(gfx_077);
    keycode_images[78] = qp_load_image_mem(gfx_078);
    keycode_images[79] = qp_load_image_mem(gfx_079);
    keycode_images[80] = qp_load_image_mem(gfx_080);
    keycode_images[81] = qp_load_image_mem(gfx_081);
    keycode_images[82] = qp_load_image_mem(gfx_082);
    keycode_images[83] = qp_load_image_mem(gfx_083);
    keycode_images[84] = qp_load_image_mem(gfx_084);
    keycode_images[85] = qp_load_image_mem(gfx_085);
    keycode_images[86] = qp_load_image_mem(gfx_086);
}

painter_image_handle_t omni_logo_image(void) {
    return logo_image;
}

painter_image_handle_t omni_save_image(void) {
    return save_image;
}

painter_image_handle_t *omni_layer_image_handle(uint16_t layer_index) {
    return &layer_images[layer_index];
}

painter_image_handle_t *omni_keycode_image_handle(uint16_t keycode) {
    if (keycode >= MACRO_KEY_START && keycode <= MACRO_KEY_END) {
        return &keycode_images[keycode - MACRO_KEY_START];
    }
    return &keycode_images[1];
}
