#include <stdint.h>
#include <stdio.h>
#include "src/rlr.h"
#include "src/error.h"

#include "src/resources/model_static.h"
#include "src/resources/shader.h"
#include "src/resources/font.h"
#include "src/objects/label.h"

int32_t main() {

    rlr_font_t* font = NULL;
    rlr_font_t* font2 = NULL;
    rlr_shader_t* shader = NULL;
    rlr_init("rl render", 1024, 768, 0);

    font = rlr_font_create("assets/noto_sans.csv", "assets/noto_sans.png", RLR_FONT_TYPE_MTSDF);
    font2 = rlr_font_create("assets/tinos-mtsdf.csv", "assets/tinos-mtsdf.png", RLR_FONT_TYPE_MTSDF);
    if(!font || !font2) {
        goto end;
    }

    shader = rlr_shader_create(NULL, NULL);
    if(!shader) {
        goto end;
    }

    rlr_obj_label_t* label = rlr_obj_label_create(2, 2, 24, "0 fps", font2);

    uint64_t fps = 60;
    char text_buffer[4096] = "0 fps";
    double timer = rlr_time();
    bool visible = true;
    while(rlr_draw()) {
        if(rlr_time() >= timer) {
            //snprintf(text_buffer, 4096, "%d fps\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n\nkfjnwergnwerlgknjwerlgnwepgun234g5lkjrgqwefnpqwefqwefi3u54p2ergnlkjgneqf2l34gn3p123f2131f21f4np2ergnl3kjgn2l3p4n", fps);
            snprintf(text_buffer, 4096, "%d fps", fps);
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