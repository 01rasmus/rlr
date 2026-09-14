#pragma once
#include <stdbool.h>
#include "../backends/backend.h"
#include "../../external/rlpp.h"
#include "../../rlr/math/rect.h"

typedef struct rlr_instance_data_ui_t {
    rlr_vec2_t pos;
    rlr_vec2_t size;
    rlr_vec2_t uv;
    rlr_vec2_t uv_size;
    uint32_t color;
    float italic_sheer;
    uint8_t screen_anchor;
} rlr_instance_data_ui_t;

typedef struct rlr_pipeline_ui_draw_command_t {
    rlpp_id_t id;
    rlr_res_t shader;
    rlr_res_t texture;
    uint32_t layer;
    rlr_rect_t scissor;
    uint64_t vao;
    uint64_t instance_vbo;
    bool should_scissor;
    bool is_dirty;
    rlr_instance_data_ui_t* instance_data;
} rlr_pipeline_ui_draw_command_t;

typedef struct rlr_pipeline_ui_t {
    rlr_res_t shader_text;
    rlr_res_t shader_sprite;

    rlr_pipeline_ui_draw_command_t* commands;
    bool is_sorted;
    uint64_t quad_vbo;
    uint64_t quad_ebo;
} rlr_pipeline_ui_t;

bool rlr_pipeline_ui_init();
void rlr_pipeline_ui_draw();
void rlr_pipeline_ui_deinit();

rlr_res_t rlr_pipeline_ui_get_text_shader();

rlr_pipeline_ui_draw_command_t* rlr_pipeline_ui_find_draw_command(rlr_res_t texture, rlr_res_t shader, uint32_t layer);
uint64_t rlr_pipeline_ui_add_sprite_instance(rlr_pipeline_ui_draw_command_t* command, rlr_instance_data_ui_t data);
void rlr_pipeline_ui_remove_sprite_instance(uint64_t command_id, uint64_t instance_id);
rlr_instance_data_ui_t* rlr_pipeline_ui_get_and_dirty_sprite_instance(uint64_t cmd_id, uint64_t instance_id);
const rlr_instance_data_ui_t const* rlr_pipeline_ui_get_sprite_instance(uint64_t cmd_id, uint64_t instance_id);