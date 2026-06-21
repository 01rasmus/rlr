#include <stb_ds.h>
#include "rlr/resources/texture.h"
#include "rlr/resources/uniform.h"
#include "rlr/resources/shader.h"
#include "rlr/resources/font.h"
#include "rlr/objects/label.h"
#include "rlr/objects/sprite.h"
#include "rlr/rlr.h"
#include "ui.h"

static const rlr_vec2_t quad_vertices[6] = {
    rlr_vec2(1, 1),
    rlr_vec2(1, 0),
    rlr_vec2(0, 0),
    rlr_vec2(0, 1),
    rlr_vec2(1, 1),
    rlr_vec2(0, 0)
};

static const char mtsdf_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    flat in vec3 frag_color;
    flat in float screen_px_range;
    out vec4 final_color;
    uniform sampler2D tex;

    float median(float r, float g, float b) {
        return max(min(r, g), min(max(r, g), b));
    }

    void main() {
        vec3 msd = texture(tex, frag_uv).rgb;
        float sd = median(msd.r, msd.g, msd.b);
        float screen_px_distance = screen_px_range * (sd - 0.5);
        float alpha = clamp(screen_px_distance + 0.5, 0.0, 1.0);
        final_color = vec4(frag_color, alpha);
    }
);

static const char basic_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    in vec3 frag_color;
    out vec4 final_color;
    uniform sampler2D tex;

    void main() {
        final_color = texture(tex, frag_uv);
    }
);

static const char sprite_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 rect_pos;
    layout (location = 2) in vec2 rect_size;
    layout (location = 3) in vec2 uv;
    layout (location = 4) in vec2 uv_size;
    layout (location = 5) in vec2 screen_anchor;

    layout(std140) uniform inv_screen_size {
        float inv_x;
        float inv_y;
        float screen_width;
        float screen_height;
    } ui_data;

    out vec2 frag_uv;
    out vec3 frag_color;

    void main() {
        frag_uv = vec2(uv.x + pos.x * uv_size.x, uv.y + pos.y * uv_size.y);
        float x = (rect_pos.x + screen_anchor.x * ui_data.screen_width) + pos.x * rect_size.x;
        float y = (rect_pos.y + screen_anchor.y * ui_data.screen_height) + pos.y * rect_size.y;
        gl_Position = vec4(x * ui_data.inv_x * 2.0 - 1.0, 1.0 - y * ui_data.inv_y * 2.0, 0.0, 1.0);
    }
);

static const char mtsdf_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec3 color;
    layout (location = 1) in vec2 pos;
    layout (location = 2) in vec2 uv;
    layout (location = 3) in float spr;
    layout(std140) uniform inv_screen_size {
        float inv_x;
        float inv_y;
    } ui_data;

    out vec2 frag_uv;
    flat out vec3 frag_color;
    flat out float screen_px_range;

    void main() {
        frag_uv = uv;
        frag_color = color;
        screen_px_range = spr;
        gl_Position = vec4(pos.x * ui_data.inv_x * 2.0 - 1.0, 1.0 - pos.y * ui_data.inv_y * 2.0, 0.0, 1.0);
    }
);

typedef struct rlr_instance_data_ui_t {
    rlr_vec2_t pos;
    rlr_vec2_t size;
    rlr_vec2_t uv;
    rlr_vec2_t uv_size;
    rlr_vec2_t screen_anchor;
} rlr_instance_data_ui_t;

typedef struct rlr_uniform_screen_size_t {
    float inv_x;
    float inv_y;
    float screen_width;
    float screen_height;
} rlr_uniform_screen_size_t;

static int32_t rlr_pipeline_ui_sorter_sprite(const void* a, const void* b) {
    const rlr_obj_sprite_t* spr_a = a;
    const rlr_obj_sprite_t* spr_b = b;

    if (spr_a->layer != spr_b->layer)
        return (spr_a->layer > spr_b->layer) - (spr_a->layer < spr_b->layer);

    if (spr_a->texture->texture != spr_b->texture->texture)
        return (spr_a->texture->texture > spr_b->texture->texture) - (spr_a->texture->texture < spr_b->texture->texture);

    return 0;
}

bool rlr_pipeline_ui_init() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;
    (*pu) = (rlr_pipeline_ui_t){0};
    pu->shader_sprite = rlr_res_create_shader(sprite_vertex, basic_fragment);
    pu->shader_text = rlr_res_create_shader(mtsdf_vertex, mtsdf_fragment);
    if(!pu->shader_sprite || !pu->shader_text) {
        goto err;
    }
    rlr_res_shader_bind_uniform_slot(pu->shader_sprite, "inv_screen_size", 0);
    rlr_res_shader_bind_uniform_slot(pu->shader_text, "inv_screen_size", 0);

    pu->quad_vbo = rlr_backend()->create_buffer();
    if(!pu->quad_vbo) {
        goto err;
    }

    pu->ubo_screen_size = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_screen_size_t));
    rlr_res_uniform_bind(pu->ubo_screen_size, 0);

    rlr_backend()->bind_buffer(pu->quad_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(quad_vertices), quad_vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    pu->is_dirty = true;
    return true;
err:
    return false;
}

static rlr_pipline_ui_draw_command_t rlr_pipeline_ui_new_command() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;

    rlr_pipline_ui_draw_command_t command = {0};
    command.vao = rlr_backend()->create_vertex_array();
    command.instance_vbo = rlr_backend()->create_buffer();
    rlr_backend()->bind_vertex_array(command.vao);
    rlr_backend()->bind_buffer(pu->quad_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_vec2_t), 0);
    rlr_backend()->bind_buffer(command.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 1, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, pos));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, size));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, uv));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 4, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, uv_size));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_instance_data_ui_t), offsetof(rlr_instance_data_ui_t, screen_anchor));
    return command;
}

static void rlr_pipeline_ui_rebuild_commands() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;

    uint64_t needed_commands = arrlenu(pu->obj_sprites);
    uint64_t commands_available = arrlenu(pu->commands);
    while(needed_commands > commands_available) {
        arrpush(pu->commands, rlr_pipeline_ui_new_command());
        commands_available++;
    }

    rlr_instance_data_ui_t* instance_data = NULL;

    qsort(pu->obj_sprites, arrlenu(pu->obj_sprites), sizeof(rlr_obj_sprite_t), rlr_pipeline_ui_sorter_sprite);
    int64_t current_command = 0;
    rlr_res_texture_t* current_texture = NULL;

    for(int64_t i = 0; i < arrlen(pu->obj_sprites); i++) {
        rlr_obj_sprite_t* sprite = &pu->obj_sprites[i];
        if(current_texture == NULL) {
            current_texture = sprite->texture;
        }

        if(current_texture->texture != sprite->texture->texture) {
            rlr_pipline_ui_draw_command_t* command = &pu->commands[current_command];
            command->shader = pu->shader_sprite;
            command->instance_count = arrlenu(instance_data);
            command->should_scissor = false;
            command->texture = current_texture;
            rlr_backend()->bind_buffer(command->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_instance_data_ui_t) * arrlenu(instance_data), instance_data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);

            current_texture = sprite->texture;
            current_command++;
            arrsetlen(instance_data, 0);
        }

        rlr_vec2_t screen_anchor_vec = rlr_anchor_vec(sprite->screen_anchor);
        rlr_vec2_t local_anchor_vec = rlr_anchor_vec(sprite->local_anchor);
        float width = sprite->rectangle.width;
        float height = sprite->rectangle.height;
        rlr_instance_data_ui_t data = {
            .pos = rlr_vec2(sprite->rectangle.x - (width * local_anchor_vec.x), sprite->rectangle.y - (height * local_anchor_vec.y)),
            .size = rlr_vec2(width, height),
            .uv = rlr_vec2(sprite->uv.x, sprite->uv.y),
            .uv_size = rlr_vec2(sprite->uv.width - sprite->uv.x, sprite->uv.height - sprite->uv.y),
            .screen_anchor = screen_anchor_vec,
        };
        arrpush(instance_data, data);
    }
    if(arrlenu(instance_data) > 0) {
        rlr_pipline_ui_draw_command_t* command = &pu->commands[current_command++];
        command->shader = pu->shader_sprite;
        command->instance_count = arrlenu(instance_data);
        command->should_scissor = false;
        command->texture = current_texture;
        rlr_backend()->bind_buffer(command->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_instance_data_ui_t) * arrlenu(instance_data), instance_data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    }

    pu->command_count = current_command;
    pu->is_dirty = false;
    arrfree(instance_data);
}

void rlr_pipeline_ui_draw() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;
    if(pu->is_dirty) {
        rlr_pipeline_ui_rebuild_commands();
    }

    rlr_backend()->set_stencil_test(false);
    rlr_backend()->set_blending(true);

    rlr_backend()->set_depth_test(false);
    for(uint64_t i = 0; i < pu->command_count; i++) {
        rlr_res_bind_shader(pu->commands[i].shader);
        rlr_res_bind_texture(pu->commands[i].texture, 0);
        rlr_backend()->bind_vertex_array(pu->commands[i].vao);
        rlr_backend()->draw_arrays_instanced(0, 6, pu->commands[i].instance_count);
    }

    rlr_res_bind_shader(pu->shader_text);
    for(int64_t i = 0; i < arrlen(pu->obj_labels); i++) {
        rlr_obj_label_t* label = &pu->obj_labels[i];
        if(!label->visible) {
            continue;
        }
        rlr_res_bind_texture(label->font->texture, 0);
        rlr_backend()->bind_vertex_array(label->vao);
        rlr_backend()->draw_arrays(0, label->vertex_count);
    }

    rlr_backend()->set_blending(false);
}

void rlr_pipeline_ui_set_viewport(float width, float height) {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;

    rlr_uniform_screen_size_t data = {
        .inv_x = 1.0 / (float)width,
        .inv_y = 1.0 / (float)height,
        .screen_width = (float)width,
        .screen_height = (float)height
    };
    rlr_res_uniform_update(pu->ubo_screen_size, 0, &data, sizeof(rlr_uniform_screen_size_t));
}

void rlr_pipeline_ui_free() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;
    if(!pu) {
        return;
    }

    rlr_res_free_shader(pu->shader_text);
    rlr_res_free_shader(pu->shader_sprite);
    rlr_res_uniform_free(pu->ubo_screen_size);

    for(int64_t i = 0; i < arrlen(pu->obj_labels); i++) {
        rlr_obj_label_free(&pu->obj_labels[i]);
    }
    for(int64_t i = 0; i < arrlen(pu->obj_sprites); i++) {
        rlr_obj_sprite_free(&pu->obj_sprites[i]);
    }
    arrfree(pu->obj_labels);
    arrfree(pu->obj_sprites);
}

rlr_obj_label_t* rlr_pipeline_ui_alloc_label() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;
    arrpush(pu->obj_labels, (rlr_obj_label_t){0});
    rlr_obj_label_t* label = &arrlast(pu->obj_labels);
    label->index = arrlenu(pu->obj_labels) - 1;
    return label;
}

rlr_obj_sprite_t* rlr_pipeline_ui_alloc_sprite() {
    rlr_pipeline_ui_t* pu = &_rlr_raw()->pipeline_ui;
    arrpush(pu->obj_sprites, (rlr_obj_sprite_t){0});
    rlr_obj_sprite_t* sprite = &arrlast(pu->obj_sprites);
    sprite->index = arrlenu(pu->obj_sprites) - 1;
    return sprite;
}

void rlr_pipeline_ui_free_label(rlr_obj_label_t* label) {

}

void rlr_pipeline_ui_free_sprite(rlr_obj_sprite_t* sprite) {

}
