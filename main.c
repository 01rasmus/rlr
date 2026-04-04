#include <stdint.h>
#include <stdio.h>
#include "src/rlr.h"
#include "src/error.h"

#include "src/resources/shader.h"

void csv_callback(const char** columns, size_t count) {
    printf("c: %s\n", columns[0]);
}

int32_t main() {

    bool res = rlr_init("rl render", 1024, 576, 0);

    rlr_font_t* fnt = rlr_font_load("assets/texture_atlas_noto_sans.csv", "assets/texture_atlas_noto_sans.png", RLR_FONT_TYPE_MTSDF);
    if(!fnt) {
        return -1;
    }

    rlr_shader_load(NULL, NULL);

    printf("glyph count: %d\n", rlr_font_glyph_count(fnt));

    while(rlr_render()) {
        
    }

    rlr_font_free(fnt);
    rlr_deinit();
    return 0;
}