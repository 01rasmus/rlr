#include "../../external/stb_ds.h"
#include "../../rlr/resources/texture.h"
#include "../../rlr/resources/uniform.h"
#include "../../rlr/resources/shader.h"
#include "../../rlr/resources/font.h"
#include "../../rlr/objects/label.h"
#include "../../rlr/objects/sprite.h"
#include "../../rlr/rlr.h"
#include "../impl.h"
#include "ui.h"

#define RLR_PIPELINE_UI    (&rlr()->pipeline_ui)

static const char mtsdf_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    flat in vec4 frag_color;
    out vec4 final_color;
    uniform sampler2D tex;
    flat in float screen_px_range;

    float median(float r, float g, float b) {
        return max(min(r, g), min(max(r, g), b));
    }

    void main() {
        vec3 msd = texture(tex, frag_uv).rgb;
        float sd = median(msd.r, msd.g, msd.b);
        float screen_px_distance = 1.0 * (sd - 0.5);
        float alpha = clamp(screen_px_distance + 0.5, 0.0, 1.0);
        final_color = frag_color * vec4(1.0, 1.0, 1.0, alpha);
    }
);

static const char mtsdf_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 rect_pos;
    layout (location = 2) in vec2 rect_size;
    layout (location = 3) in vec2 uv;
    layout (location = 4) in vec2 uv_size;
    layout (location = 5) in int screen_anchor;
    layout (location = 6) in vec4 color;
    layout (location = 7) in float italic_shear;

    layout(std140) uniform inv_screen_size {
        float inv_x;
        float inv_y;
        float screen_width;
        float screen_height;
    } ui_data;

    out vec2 frag_uv;
    flat out vec4 frag_color;
    flat out float screen_px_range;

    const vec2 screen_anchor_vecs[9] = vec2[](
        vec2(0.0, 0.0),
        vec2(0.5, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 0.5),
        vec2(0.5, 0.5),
        vec2(1.0, 0.5),
        vec2(0.0, 1.0),
        vec2(0.5, 1.0),
        vec2(1.0, 1.0)
    );

    void main() {
        float local_x = pos.x * rect_size.x;
        float local_y = pos.y * rect_size.y;
        local_x += (1.0 - pos.y) * rect_size.y * italic_shear;

        frag_color = color;
        frag_uv = vec2(uv.x + pos.x * uv_size.x, uv.y + pos.y * uv_size.y);
        float x = (rect_pos.x + screen_anchor_vecs[screen_anchor].x * ui_data.screen_width) + local_x;
        float y = (rect_pos.y + screen_anchor_vecs[screen_anchor].y * ui_data.screen_height) + local_y;
        gl_Position = vec4(x * ui_data.inv_x * 2.0 - 1.0, 1.0 - y * ui_data.inv_y * 2.0, 0.0, 1.0);
    }
);

static const char sprite_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    flat in vec4 frag_color;
    out vec4 final_color;
    uniform sampler2D tex;

    void main() {
        final_color = texture(tex, frag_uv) * frag_color;
    }
);

static const char sprite_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 rect_pos;
    layout (location = 2) in vec2 rect_size;
    layout (location = 3) in vec2 uv;
    layout (location = 4) in vec2 uv_size;
    layout (location = 5) in int screen_anchor;
    layout (location = 6) in vec4 color;

    layout(std140) uniform inv_screen_size {
        float inv_x;
        float inv_y;
        float screen_width;
        float screen_height;
    } ui_data;

    out vec2 frag_uv;
    flat out vec4 frag_color;

    const vec2 screen_anchor_vecs[9] = vec2[](
        vec2(0.0, 0.0),
        vec2(0.5, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 0.5),
        vec2(0.5, 0.5),
        vec2(1.0, 0.5),
        vec2(0.0, 1.0),
        vec2(0.5, 1.0),
        vec2(1.0, 1.0)
    );

    void main() {
        frag_color = color;
        frag_uv = vec2(uv.x + pos.x * uv_size.x, uv.y + pos.y * uv_size.y);
        float x = (rect_pos.x + screen_anchor_vecs[screen_anchor].x * ui_data.screen_width) + pos.x * rect_size.x;
        float y = (rect_pos.y + screen_anchor_vecs[screen_anchor].y * ui_data.screen_height) + pos.y * rect_size.y;
        gl_Position = vec4(x * ui_data.inv_x * 2.0 - 1.0, 1.0 - y * ui_data.inv_y * 2.0, 0.0, 1.0);
    }
);

static int32_t rlr_pipeline_ui_draw_command_sorter(const void* a, const void* b) {
    const rlr_pipeline_ui_draw_command_t* left = a;
    const rlr_pipeline_ui_draw_command_t* right = b;
    if(left->layer != right->layer) {
        return (left->layer > right->layer) - (left->layer < right->layer);
    }
    if(left->texture != right->texture) {
        return (left->texture > right->texture) - (left->texture < right->texture);
    }
    if(left->shader != right->shader) {
        return (left->shader > right->shader) - (left->shader < right->shader);
    }
    return 0;
}

static int32_t rlr_pipeline_ui_instance_visability_sorter(const void* a, const void* b) {
    const rlr_instance_data_ui_t* left = a;
    const rlr_instance_data_ui_t* right = b;
    return (int32_t)right->visible - (int32_t)left->visible;
}

bool rlr_pipeline_ui_init() {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    (*pu) = (rlr_pipeline_ui_t){0};
    pu->is_sorted = true;
    pu->shader_sprite = rlr_res_shader_create(sprite_vertex, sprite_fragment);
    pu->shader_text = rlr_res_shader_create(mtsdf_vertex, mtsdf_fragment);
    if(!pu->shader_sprite || !pu->shader_text) {
        goto err;
    }
    rlr_res_shader_bind_uniform_slot(pu->shader_sprite, "inv_screen_size", RLR_INTERNAL_UBO_UI);
    rlr_res_shader_bind_uniform_slot(pu->shader_text, "inv_screen_size", RLR_INTERNAL_UBO_UI);

    pu->quad_vbo = rlr_backend()->create_buffer();
    pu->quad_ebo = rlr_backend()->create_buffer();
    if(!pu->quad_vbo || !pu->quad_ebo) {
        goto err;
    }

    rlr_backend()->bind_vertex_array(0);
    rlr_backend()->bind_buffer(pu->quad_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_quad_vertices), rlr_quad_vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    rlr_backend()->bind_buffer(pu->quad_ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, sizeof(rlr_quad_indices), rlr_quad_indices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    return true;
err:
    return false;
}

static rlr_pipeline_ui_draw_command_t rlr_pipeline_ui_new_command(rlr_res_t texture, rlr_res_t shader, uint32_t layer) {
    rlr_pipeline_ui_draw_command_t command = {0};
    command.instance_data = NULL;
    command.texture = texture;
    command.shader = shader;
    command.layer = layer;
    command.is_dirty = true;
    command.vao = rlr_backend()->create_vertex_array();
    command.instance_vbo = rlr_backend()->create_buffer();
    command.visible_instances = 0;
    rlr_backend()->bind_vertex_array(command.vao);
    rlr_backend()->bind_buffer(RLR_PIPELINE_UI->quad_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_vec2_t), 0);
    rlr_backend()->bind_buffer(command.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 1, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, pos));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, size));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, uv));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 4, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, uv_size));
    rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 1, RLR_BACKEND_BUFFER_TYPE_U8, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, screen_anchor));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 6, 4, RLR_BACKEND_BUFFER_TYPE_U8, true, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, color));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 7, 1, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, italic_sheer));
    rlr_backend()->bind_buffer(RLR_PIPELINE_UI->quad_ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
    return command;
}

rlr_pipeline_ui_draw_command_t* rlr_pipeline_ui_find_draw_command(rlr_res_t texture, rlr_res_t shader, uint32_t layer) {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;

    for(size_t i = 0; i < rlpp_len(pu->commands); i++) {
        rlr_pipeline_ui_draw_command_t* cmd = &pu->commands[i];
        if(cmd->texture == texture && cmd->shader == shader && cmd->layer == layer) {
            return cmd;
        }
    }

    pu->is_sorted = false;
    rlr_pipeline_ui_draw_command_t new_cmd = rlr_pipeline_ui_new_command(texture, shader, layer);
    rlpp_id_t id = rlpp_alloc(pu->commands, new_cmd);
    rlr_pipeline_ui_draw_command_t* cmd = &pu->commands[rlpp_len(pu->commands) - 1];
    cmd->id = id;
    return cmd;
}

uint64_t rlr_pipeline_ui_add_sprite_instance(rlr_pipeline_ui_draw_command_t* command, rlr_instance_data_ui_t data) {
    command->is_dirty = true;
    command->visible_instances++;
    return rlpp_alloc(command->instance_data, data);
}

void rlr_pipeline_ui_remove_sprite_instance(uint64_t command_id, uint64_t instance_id) {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(pu->commands, command_id);
    rlr_instance_data_ui_t* instance = rlpp_get_unchecked(cmd->instance_data, instance_id);
    cmd->is_dirty = true;

    rlpp_remove(cmd->instance_data, instance_id);
    if(instance->visible) {
        cmd->visible_instances--;
    }
}

void rlr_pipeline_ui_set_sprite_instance_visability(uint64_t command_id, uint64_t instance_id, bool visible) {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(pu->commands, command_id);
    rlr_instance_data_ui_t* instance = rlpp_get_unchecked(cmd->instance_data, instance_id);

    if(instance->visible != visible) {
        cmd->is_dirty = true;
        instance->visible = visible;

        if(!visible) {
            cmd->visible_instances--;
        } else {
            cmd->visible_instances++;
        }
    }
}

rlr_instance_data_ui_t* rlr_pipeline_ui_get_and_dirty_sprite_instance(uint64_t cmd_id, uint64_t instance_id) {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(pu->commands, cmd_id);
    if(!cmd) {
        return NULL;
    }
    cmd->is_dirty = true;
    return rlpp_get_unchecked(cmd->instance_data, instance_id);
}

const rlr_instance_data_ui_t const* rlr_pipeline_ui_get_sprite_instance(uint64_t cmd_id, uint64_t instance_id) {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(pu->commands, cmd_id);
    if(!cmd) {
        return NULL;
    }
    return rlpp_get_unchecked(cmd->instance_data, instance_id);
}

rlr_res_t rlr_pipeline_ui_get_text_shader() {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    return pu->shader_text;
}

void rlr_pipeline_ui_draw() {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;

    if(!pu->is_sorted) {
        rlpp_sort(pu->commands, rlr_pipeline_ui_draw_command_sorter);
        pu->is_sorted = true;
    }

    for(size_t i = 0; i < rlpp_len(pu->commands); i++) {
        rlr_pipeline_ui_draw_command_t* cmd = &pu->commands[i];

        if(cmd->is_dirty) {
            rlpp_sort(cmd->instance_data, rlr_pipeline_ui_instance_visability_sorter);
            rlr_backend()->bind_buffer(cmd->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_instance_data_ui_t) * cmd->visible_instances, cmd->instance_data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
            cmd->is_dirty = false;
        }
    }

    rlr_backend()->set_stencil_test(false);
    rlr_backend()->set_blending(true);

    rlr_backend()->set_depth_test(false);
    for(uint64_t i = 0; i < rlpp_len(pu->commands); i++) {
        rlr_pipeline_ui_draw_command_t* cmd = &pu->commands[i];
        uint64_t instance_len = cmd->visible_instances;
        if(instance_len == 0) {
            continue;
        }
        rlr_res_shader_bind(cmd->shader == RLR_NULL ? pu->shader_sprite : cmd->shader);
        rlr_res_texture_bind(cmd->texture, 0);
        rlr_backend()->bind_vertex_array(cmd->vao);
        rlr_backend()->draw_elements_instanced(0, 6, RLR_BACKEND_BUFFER_TYPE_U8, instance_len);
    }

    rlr_backend()->set_blending(false);
}

void rlr_pipeline_ui_deinit() {
    rlr_pipeline_ui_t* pu = RLR_PIPELINE_UI;
    if(!pu) {
        return;
    }

    rlr_res_shader_free(pu->shader_text);
    rlr_res_shader_free(pu->shader_sprite);

    rlr_backend()->free_buffer(pu->quad_vbo);
    rlr_backend()->free_buffer(pu->quad_ebo);
}