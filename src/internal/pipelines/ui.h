#pragma once
#include <stdbool.h>
#include "internal/backends/backend.h"
#include "rlr/math/rect.h"

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;
typedef struct rlr_res_uniform_t rlr_res_uniform_t;

typedef struct rlr_pipeline_ui_draw_command_t {
    rlr_res_shader_t* shader;
    rlr_res_texture_t* texture;
    rlr_rect_t scissor;
    uint64_t vao;
    uint64_t instance_vbo;
    uint32_t instance_count;
    bool should_scissor;
} rlr_pipeline_ui_draw_command_t;

typedef struct rlr_pipeline_ui_t {
    rlr_res_shader_t* shader_text;
    rlr_res_shader_t* shader_sprite;

    rlr_pipeline_ui_draw_command_t* commands;
    uint64_t command_count;
    uint64_t quad_vbo;
    uint64_t quad_ebo;

    bool is_dirty;
} rlr_pipeline_ui_t;

bool rlr_pipeline_ui_init();
void rlr_pipeline_ui_draw();
void rlr_pipeline_ui_deinit();