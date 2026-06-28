#include <stdint.h>
#include <stdio.h>
#include <stb_ds.h>
#include "src/rlr/rlr.h"
#include "src/rlr/error.h"
#include "src/rlr/resources/static_model.h"
#include "src/rlr/resources/shader.h"
#include "src/rlr/resources/font.h"
#include "src/rlr/objects/label.h"
#include "src/rlr/objects/sprite.h"
#include "src/rlr/objects/model_occluder.h"
#include "src/rlr/objects/static_model.h"

int32_t main() {

    rlr_res_font_t* font = NULL;
    rlr_res_font_t* font2 = NULL;
    rlr_init("rl render", 1024, 768, 0);

    font = rlr_res_font_load("assets/noto_sans.csv", "assets/noto_sans.png", 2.0);
    font2 = rlr_res_font_load("assets/tinos-small.csv", "assets/tinos-small.png", 8.0);
    if(!font || !font2) {
        goto end;
    }

    rlr_res_texture_t* ta_ui = rlr_res_texture_load("assets/ui_texture_atlas_large.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_res_texture_t* ability_empty = rlr_res_texture_load("assets/ability_empty_small.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_res_texture_t* hero_unknown = rlr_res_texture_load("assets/unknown_unit_small.png", false, RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_res_static_model_t* plane = rlr_res_static_model_load_glb("assets/plane.glb");
    rlr_res_static_model_t* monkey = rlr_res_static_model_load_glb("assets/monkey.glb");

    rlr_vec3_t rot = rlr_vec3(3.14, -0, 0);
 
    int32_t amount = 10;
    float xStart = -1.0;
    float yStart = -1.0;
    float interval = 2.0 / (float)amount;
    rlr_obj_static_model_handle_t* monkey_objects = NULL;
    for(int32_t y = 0; y < amount; y++) {
        for(int32_t x = 0; x < amount; x++) {
            rlr_vec3_t pos = rlr_vec3(xStart + (float)x * interval, 0.05, yStart + (float)y * interval);
            arrpush(monkey_objects, rlr_obj_static_model_create(monkey, pos, rlr_quat_from_euler(&rot), rlr_vec3(0.05, 0.05, 0.05)));
        }
    }
    rlr_obj_static_model_handle_t model = rlr_obj_static_model_create(plane, rlr_vec3(0, 0, 0), rlr_quat_ident, rlr_vec3(1, 1, 1));

    rlr_obj_label_t* label = rlr_obj_label_create(2, 2, 16, "", font);
    
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

    char text_buffer[4096] = "";
    double timer = 0.0;
    bool visible = true;
    float rotation = 0.0;
    double last_time = 0.0;
    while(rlr_update()) {
        rlr_statistics_t* second_stats = rlr_get_statistics();
        rlr_statistics_t* total_stats = rlr_get_total_statistics();
        if(total_stats->time >= timer) {
            rlr_vec2_t size = rlr_get_framebuffer_size();
            snprintf(
                text_buffer,
                4096,
                "fps\t\t\t\t\t\t\t%lld\ndraw calls per second\t\t%lld\ndraw calls per frame\t\t%lld\ntotal draw calls\t\t\t\t%lld\nbackend\t\t\t\t\t%s\nframebuffer size\t\t\t%dx%d\ngpu\t\t\t\t\t\t\t%s",
                second_stats->frame_count,
                second_stats->draw_call_count,
                second_stats->draw_call_count / second_stats->frame_count,
                total_stats->draw_call_count,
                rlr_get_backend_implementation(),
                (int32_t)size.x,
                (int32_t)size.y,
                rlr_get_gpu_name()
            );

            rlr_obj_label_set_text(label, text_buffer);
            timer += 1.0;
        }
        
        double delta = total_stats->time - last_time;
        last_time = total_stats->time;
        rotation += 3.1415926535 * delta / 2;
        for(int64_t i = 0; i < arrlen(monkey_objects); i++) {
            rlr_obj_static_model_set_trs(monkey_objects[i], NULL, &rlr_quat(cos(rotation / 2), 0, 1 * sin(rotation / 2), 0), NULL);
        }
        rlr_obj_static_model_set_trs(model, NULL, &rlr_quat(cos(rotation / 2), 0, 1 * sin(rotation / 2), 0), NULL);
    }

end:
    rlr_res_font_free(font);
    rlr_res_font_free(font2);
    rlr_free();
    return 0;
}