#include <string.h>
#include "../../rlr/resources/static_model.h"
#include "../../rlr/resources/cube_map.h"
#include "../../rlr/resources/texture.h"
#include "../../rlr/resources/font.h"
#include "../impl.h"
#include "default_static_models.h"
#include "default_font.h"
#include "defaults.h"

rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_box() {
    return rlr_sta_mdl_load_mem(default_box_glb, sizeof(default_box_glb));
}

rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_cylinder() {
    return rlr_sta_mdl_load_mem(default_cylinder_glb, sizeof(default_cylinder_glb));
}

rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_plane() {
    return rlr_sta_mdl_load_mem(default_plane_glb, sizeof(default_plane_glb));
}

rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_sphere() {
    return rlr_sta_mdl_load_mem(default_sphere_glb, sizeof(default_sphere_glb));
}

rlr_tex_t* rlr_defaults_load_tex_white() {
    uint8_t data[RLR_DEFAULT_TEX_SIDE * RLR_DEFAULT_TEX_SIDE * RLR_DEFAULT_TEX_CHANNELS];
    memset(data, 255, sizeof(data));
    return rlr_tex_load_mem_raw(data, RLR_DEFAULT_TEX_SIDE, RLR_DEFAULT_TEX_SIDE, RLR_DEFAULT_TEX_CHANNELS, RLR_TEX_FILTER_NEAREST, 0);
}

rlr_cub_map_t* rlr_defaults_load_cub_map() {
    rlr_cub_map_t* cm = NULL;

    cm = malloc(sizeof(rlr_cub_map_t));
    if(!cm) {
        rlr_log_error("pointer to the cube map is null");
        goto err;
    }

    uint8_t white[RLR_DEFAULT_TEX_SIDE * RLR_DEFAULT_TEX_SIDE * RLR_DEFAULT_TEX_CHANNELS];
    memset(white, 255, sizeof(white));
    cm->texture = rlr_backend()->create_cube_map_texture(white, white, white, white, white, white, RLR_DEFAULT_TEX_SIDE, RLR_DEFAULT_TEX_SIDE, RLR_DEFAULT_TEX_CHANNELS);
    if(!cm->texture) {
        rlr_log_error("backend texture handle is null");
        goto err;
    }

    return cm;
err:
    rlr_cub_map_free(cm);
    return NULL;
}

rlr_font_t* rlr_defaults_load_font() {
    return rlr_font_load_mem(rlr_default_font_csv, rlr_default_font_tex, RLR_DEFAULT_FONT_TEX_LEN, RLR_DEFAULT_FONT_PX_RANGE);
}

/* public api functions */
rlr_tex_t* rlr_default_texture() {
    return rlr()->default_tex;
}

rlr_cub_map_t* rlr_default_cube_map() {
    return rlr()->default_cube_map;
}

rlr_font_t* rlr_default_font() {
    return rlr()->default_font;
}

rlr_sta_mdl_t* rlr_default_sta_mdl_box() {
    return rlr()->default_sta_mdl_box;
}

rlr_sta_mdl_t* rlr_default_sta_mdl_cylinder() {
    return rlr()->default_sta_mdl_cyl;
}

rlr_sta_mdl_t* rlr_default_sta_mdl_plane() {
    return rlr()->default_sta_mdl_plane;
}

rlr_sta_mdl_t* rlr_default_sta_mdl_sphere() {
    return rlr()->default_sta_mdl_sphere;
}
