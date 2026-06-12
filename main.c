#include <stdint.h>
#include <stdio.h>
#include "src/rlr.h"
#include "src/error.h"

#include "src/resources/shader.h"

int32_t main() {

    rlr_font_t* font = NULL;
    rlr_shader_t* shader = NULL;
    rlr_init("rl render", 960, 540, 0);

    font = rlr_font_create("assets/texture_atlas_noto_sans.csv", "assets/texture_atlas_noto_sans.png", RLR_FONT_TYPE_MTSDF);
    if(!font) {
        goto end;
    }

    shader = rlr_shader_create(NULL, NULL);
    if(!shader) {
        goto end;
    }

    printf("glyph count: %d\n", rlr_font_glyph_count(font));

    rlr_obj_label_t* label = rlr_obj_label_create(8, 8, 16, "0 fps", font);

    uint64_t fps = 0;
    char text_buffer[4096] = "0 fps";
    double timer = rlr_time();
    while(rlr_draw()) {
        fps++;

        if(rlr_time() >= timer) {
            snprintf(text_buffer, 4096, "%d fps\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n", fps);
            rlr_obj_label_text_set(label, text_buffer);
            printf("%d fps\n", fps);
            timer += 1.0;
            fps = 0;
        }
    }

end:
    rlr_font_free(font);
    rlr_free();
    return 0;
}