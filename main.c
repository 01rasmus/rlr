#include <stdint.h>
#include <stdio.h>
#include "src/rlr/rlr.h"
#include "src/rlr/error.h"

#include "src/rlr/resources/static_model.h"
#include "src/rlr/resources/shader.h"
#include "src/rlr/resources/font.h"
#include "src/rlr/objects/label.h"
#include "src/rlr/objects/sprite.h"
#include "src/rlr/objects/model_occluder.h"

int32_t main() {

    rlr_res_font_t* font = NULL;
    rlr_res_font_t* font2 = NULL;
    rlr_init("rl render", 1024, 768, 0);

    font = rlr_res_font_create("assets/noto_sans.csv", "assets/noto_sans.png", 2.0);
    font2 = rlr_res_font_create("assets/tinos-small.csv", "assets/tinos-small.png", 8.0);
    if(!font || !font2) {
        goto end;
    }

    rlr_res_texture_t* ta_ui = rlr_res_texture_load("assets/ui_texture_atlas_large.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_res_texture_t* ability_empty = rlr_res_texture_load("assets/ability_empty_small.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_res_texture_t* hero_unknown = rlr_res_texture_load("assets/unknown_unit_small.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);

    rlr_obj_label_t* label = rlr_obj_label_create(2, 2, 20, "0 fps", font);
    
    float ui_x = 8;
    float ui_y = -8;
    for(int32_t i = 0; i < 7; i++) {
        float x = ui_x + 4 + i * 32;
        float y = ui_y - 4;
        rlr_obj_sprite_create(ability_empty, rlr_rect(x, y, 32, 32), RLR_ANCHOR_BOTTOM_LEFT, RLR_ANCHOR_BOTTOM_LEFT, 0);
    }
    rlr_obj_sprite_create(hero_unknown, rlr_rect(ui_x + 5, ui_y - 41, 73, 73), RLR_ANCHOR_BOTTOM_LEFT, RLR_ANCHOR_BOTTOM_LEFT, 0);
    rlr_obj_sprite_create_ext(ta_ui, rlr_rect(ui_x, ui_y, 286, 119), RLR_ANCHOR_BOTTOM_LEFT, RLR_ANCHOR_BOTTOM_LEFT, 1, rlr_rect(0, 0, 286, 119), rlr_rect(0, 0, 0, 0));
    rlr_obj_model_occluder_create(rlr_rect(ui_x + 2, ui_y - 2, 282, 115), RLR_ANCHOR_BOTTOM_LEFT, RLR_ANCHOR_BOTTOM_LEFT);
    rlr_obj_model_occluder_create(rlr_rect(-8, -8, 256, 256), RLR_ANCHOR_BOTTOM_RIGHT, RLR_ANCHOR_BOTTOM_RIGHT);
    rlr_obj_model_occluder_create(rlr_rect(0, -32, 360, 96), RLR_ANCHOR_BOTTOM_CENTER, RLR_ANCHOR_BOTTOM_CENTER);

    char text_buffer[4096] = "0 fps";
    double timer = 0.0;
    bool visible = true;
    while(rlr_update()) {
        rlr_statistics_t* second_stats = rlr_get_statistics();
        rlr_statistics_t* total_stats = rlr_get_total_statistics();
        if(total_stats->time >= timer) {
            snprintf(
                text_buffer,
                4096,
                "fps %lld\ndraw calls %lld\ntotal draw calls %lld\nbackend %s",
                second_stats->frame_count,
                second_stats->draw_call_count,
                total_stats->draw_call_count,
                rlr_get_backend_implementation()
            );

            rlr_obj_label_set_text(label, text_buffer);
            timer += 1.0;
        }
    }

end:
    rlr_res_font_free(font);
    rlr_res_font_free(font2);
    rlr_free();
    return 0;
}