#pragma once
#include <stdbool.h>
#include "internal/backends/backend.h"
#include "rlr/math/rect.h"

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;
typedef struct rlr_res_uniform_t rlr_res_uniform_t;

typedef struct rlr_obj_label_t rlr_obj_label_t;
typedef struct rlr_obj_sprite_t rlr_obj_sprite_t;

typedef struct rlr_pipline_ui_draw_command_t {
    rlr_res_shader_t* shader;
    rlr_res_texture_t* texture;
    rlr_rect_t scissor;
    uint64_t vao;
    uint64_t instance_vbo;
    uint32_t instance_count;
    bool should_scissor;
} rlr_pipline_ui_draw_command_t;

typedef struct rlr_pipeline_ui_t {
    rlr_res_shader_t* shader_text;
    rlr_res_shader_t* shader_sprite;
    rlr_res_uniform_t* ubo_screen_size;
    rlr_obj_label_t* obj_labels;
    rlr_obj_sprite_t* obj_sprites;

    rlr_pipline_ui_draw_command_t* commands;
    uint64_t command_count;
    uint64_t quad_vbo;

    bool is_dirty;
} rlr_pipeline_ui_t;

bool rlr_pipeline_ui_init(rlr_pipeline_ui_t* pu);
void rlr_pipeline_ui_draw(rlr_pipeline_ui_t* pu);
void rlr_pipeline_ui_deinit(rlr_pipeline_ui_t* pu);

void rlr_pipeline_ui_set_viewport(float width, float height);

rlr_obj_label_t* rlr_pipeline_ui_alloc_label();
rlr_obj_sprite_t* rlr_pipeline_ui_alloc_sprite();
void rlr_pipeline_ui_free_label(rlr_obj_label_t* label);
void rlr_pipeline_ui_free_sprite(rlr_obj_sprite_t* sprite);
