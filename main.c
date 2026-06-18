#include <stdint.h>
#include <stdio.h>
#include "src/rlr.h"
#include "src/error.h"

#include "src/resources/model_static.h"
#include "src/resources/shader.h"
#include "src/resources/font.h"
#include "src/objects/label.h"
#include "src/objects/sprite.h"

int32_t main() {

    rlr_font_t* font = NULL;
    rlr_font_t* font2 = NULL;
    rlr_shader_t* shader = NULL;
    rlr_init("rl render", 1024, 768, 0);

    font = rlr_font_create("assets/noto_sans.csv", "assets/noto_sans.png", 2.0);
    font2 = rlr_font_create("assets/tinos-small.csv", "assets/tinos-small.png", 8.0);
    if(!font || !font2) {
        goto end;
    }

    shader = rlr_shader_create(NULL, NULL);
    if(!shader) {
        goto end;
    }

    rlr_texture_t* ta_ui = rlr_texture_load("assets/ui_texture_atlas_large.png", false, RLR_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_texture_t* ability_empty = rlr_texture_load("assets/ability_empty.png", false, RLR_TEXTURE_FILTER_LINEAR_MIPMAP);
    rlr_texture_t* hero_unknown = rlr_texture_load("assets/unknown_unit.png", false, RLR_TEXTURE_FILTER_LINEAR_MIPMAP);

    rlr_obj_label_t* label = rlr_obj_label_create(2, 2, 16, "0 fps", font);
    
    float ui_x = 4;
    float ui_y = 540 - 119 - 4;
    for(int32_t i = 0; i < 7; i++) {
        float x = ui_x + 4 + i * 32;
        float y = ui_y + 119 - 4 - 32;
        rlr_obj_sprite_create(ability_empty, rlr_rect(x, y, 32, 32), 0);
    }
    rlr_obj_sprite_create(hero_unknown, rlr_rect(ui_x + 5, ui_y + 5, 73, 73), 0);
    rlr_obj_sprite_create_ext(ta_ui, rlr_rect(ui_x, ui_y, 286, 119), 1, rlr_rect(0, 0, 286, 119), rlr_rect(0, 0, 0, 0));

    uint64_t fps = 60;
    char text_buffer[4096] = "0 fps";
    double timer = rlr_time();
    bool visible = true;
    while(rlr_draw()) {
        if(rlr_time() >= timer) {
            //snprintf(text_buffer, 4096, "%d fps\nkfjnwergnwerlgknjwerlgnwepgun233g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n", fps);
            snprintf(text_buffer, 4096, "frames_per_second\t\t%d\ndraw_calls_per_second\t%d\ndraw_calls_per_frame\t\t%d", fps, rlr_backend()->statistics_draw_calls(), rlr_backend()->statistics_draw_calls() / fps);
            rlr_backend()->statistics_reset();
            rlr_obj_label_text_set(label, text_buffer);
            timer += 1.0;
            fps = 0;
        }
        fps++;
    }

end:
    rlr_font_free(font);
    rlr_font_free(font2);
    rlr_free();
    return 0;
}