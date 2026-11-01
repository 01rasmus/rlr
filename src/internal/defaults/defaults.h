#pragma once

#define RLR_DEFAULT_TEX_SIDE        4
#define RLR_DEFAULT_TEX_CHANNELS    4

typedef struct rlr_tex_t rlr_tex_t;
typedef struct rlr_cub_map_t rlr_cub_map_t;
typedef struct rlr_font_t rlr_font_t;
typedef struct rlr_sta_mdl_t rlr_sta_mdl_t;

rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_box();
rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_cylinder();
rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_plane();
rlr_sta_mdl_t* rlr_defaults_load_sta_mdl_sphere();
rlr_tex_t* rlr_defaults_load_tex_white();
rlr_cub_map_t* rlr_defaults_load_cub_map();
rlr_font_t* rlr_defaults_load_font();