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

    if(!rlr_init("rl render", 1024, 576, 0)) {
        return 1;
    }

    rlr_font_t* fnt = rlr_font_load("assets/texture_atlas_noto_sans.csv", "assets/texture_atlas_noto_sans.png", RLR_FONT_TYPE_MTSDF);
    if(!fnt) {
        return -1;
    }

    rlr_shader_load(NULL, shader_fragment_font_mtsdf);

    printf("glyph count: %d\n", rlr_font_glyph_count(fnt));

    while(rlr_render()) {
        
    }

    rlr_font_free(fnt);
    rlr_deinit();
    return 0;
}