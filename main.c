#include <stdint.h>
#include <stdio.h>
#include "src/rlr.h"
#include "src/error.h"

#include "src/resources/shader.h"

const char shader_fragment_font_mtsdf[] = RLR_SHADER_INLINE(
    in vec2 fragTexCoord;
    out vec4 finalColor;

    uniform sampler2D tex;
    uniform vec4 textColor;

    float median(float r, float g, float b) {
        return max(min(r,g), min(max(r,g),b));
    }

    void main() {
        float pxRange = -1.0;
        vec3 sample = texture(tex, fragTexCoord).rgb;
        float sd = median(sample.r, sample.g, sample.b);
        float smoothing = 1.0 / fwidth(sd);
        float screenPxRange  = max(0.5f * dot(pxRange, smoothing), 1.0);
        float screenPxDistance = screenPxRange * (sd - 0.5f); 
        float alpha = clamp(screenPxDistance + 0.5f, 0.0f, 1.0f);
        finalColor = vec4(textColor.rgb, textColor.a * alpha);
    }
);

int32_t main() {

    rlr_font_t* font = NULL;
    rlr_shader_t* shader = NULL;
    rlr_t* rlr = rlr_init("rl render", 1024, 576, 0);
    if(!rlr) {
        goto end;
    }

    font = rlr_font_create(rlr, "assets/texture_atlas_noto_sans.csv", "assets/texture_atlas_noto_sans.png", RLR_FONT_TYPE_MTSDF);
    if(!font) {
        goto end;
    }

    shader = rlr_shader_create(rlr, NULL, NULL);
    if(!shader) {
        goto end;
    }

    printf("glyph count: %d\n", rlr_font_glyph_count(font));

    rlr_obj_label_t* label = rlr_obj_label_create(rlr, 8, 8, 16, "10 fps", font);

    uint64_t fps = 0;
    char text_buffer[64] = "0 fps";
    double timer = rlr_time() + 1.0;
    while(rlr_draw(rlr)) {
        fps++;

        if(rlr_time() >= timer) {
            snprintf(text_buffer, 64, "%d fps", fps);
            rlr_obj_label_text_set(rlr, label, text_buffer);
            timer += 1.0;
            fps = 0;
        }
    }

end:
    rlr_font_free(rlr, font);
    rlr_free(rlr);
    return 0;
}