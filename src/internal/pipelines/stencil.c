#include <stddef.h>
#include "../../external/stb_ds.h"
#include "../impl.h"
#include "../../rlr/resources/shader.h"
#include "../../rlr/objects/model_occluder.h"
#include "../../rlr/math/vec.h"
#include "../../rlr/rlr.h"
#include "stencil.h"

#define RLR_PIPELINE_STENCIL    (&rlr()->pipeline_stencil)

static const char stencil_fragment[] = RLR_SHADER_INLINE(
    out vec4 out_color;
    void main() {
        out_color = vec4(1.0);
    }
);

static const char stencil_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 rect_pos;
    layout (location = 2) in vec2 rect_size;
    layout (location = 3) in vec2 screen_anchor;

    layout(std140) uniform inv_screen_size {
        float inv_x;
        float inv_y;
        float screen_width;
        float screen_height;
    } ui_data;

    void main() {
        float x = (rect_pos.x + ui_data.screen_width * screen_anchor.x) + pos.x * rect_size.x;
        float y = (rect_pos.y + ui_data.screen_height * screen_anchor.y) + pos.y * rect_size.y;
        gl_Position = vec4(x * ui_data.inv_x * 2.0 - 1.0, 1.0 - y * ui_data.inv_y * 2.0, 0.0, 1.0);
    }
);

static int32_t rlr_pipeline_stencil_occluder_instance_visability_sorter(const void* a, const void* b) {
    const rlr_pipeline_stencil_occluder_instance_t* left = a;
    const rlr_pipeline_stencil_occluder_instance_t* right = b;
    return (int32_t)right->visible - (int32_t)left->visible;
}

bool rlr_pipeline_stencil_init() {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    (*ps) = (rlr_pipeline_stencil_t){0};

    ps->is_dirty = false;
    ps->occluder_instances = NULL;
    ps->visible_occluder_instances = 0;
    ps->vao = rlr_backend()->create_vertex_array();
    ps->vbo = rlr_backend()->create_buffer();
    ps->ebo = rlr_backend()->create_buffer();
    ps->instance_vbo = rlr_backend()->create_buffer();
    if(!ps->vao || !ps->vbo || !ps->ebo || !ps->instance_vbo) {
        goto err;
    }

    ps->shader = rlr_res_shader_create(stencil_vertex, stencil_fragment);
    if(!ps->shader) {
        goto err;
    }
    rlr_res_shader_bind_uniform_slot(ps->shader, "inv_screen_size", RLR_INTERNAL_UBO_UI);

    rlr_backend()->bind_vertex_array(ps->vao);
    rlr_backend()->bind_buffer(ps->vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_quad_vertices), rlr_quad_vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_vec2_t), 0);
    rlr_backend()->bind_buffer(ps->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 1, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_stencil_occluder_instance_t), offsetof(rlr_pipeline_stencil_occluder_instance_t, pos));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_stencil_occluder_instance_t), offsetof(rlr_pipeline_stencil_occluder_instance_t, size));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_stencil_occluder_instance_t), offsetof(rlr_pipeline_stencil_occluder_instance_t, screen_anchor));
    rlr_backend()->bind_buffer(ps->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, sizeof(rlr_quad_indices), rlr_quad_indices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    return true;
err:
    rlr_pipeline_stencil_deinit();
    return false;
}

void rlr_pipeline_stencil_draw() {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    if(ps->is_dirty) {
        rlpp_sort(ps->occluder_instances, rlr_pipeline_stencil_occluder_instance_visability_sorter);
        size_t size = sizeof(rlr_pipeline_stencil_occluder_instance_t) * ps->visible_occluder_instances;
        rlr_backend()->bind_buffer(ps->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, size, ps->occluder_instances, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
        ps->is_dirty = false;
    }

    if(ps->visible_occluder_instances == 0) {
        return;
    }

    rlr_backend()->set_stencil_test(true);
    rlr_backend()->set_depth_test(false);
    rlr_backend()->set_stencil_mask(0xFF);
    rlr_backend()->set_color_mask(false, false, false, false);
    rlr_backend()->set_depth_mask(false);
    rlr_backend()->set_stencil_func(RLR_BACKEND_STENCIL_FUNC_ALWAYS, 1, 0xFF);
    rlr_backend()->set_stencil_op(RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_REPLACE);

    rlr_res_shader_bind(ps->shader);
    rlr_backend()->bind_vertex_array(ps->vao);
    rlr_backend()->draw_elements_instanced(0, 6, RLR_BACKEND_BUFFER_TYPE_U8, ps->visible_occluder_instances);

    rlr_backend()->set_stencil_mask(0x0);
    rlr_backend()->set_stencil_func(RLR_BACKEND_STENCIL_FUNC_NOTEQUAL, 1, 0xFF);
    rlr_backend()->set_stencil_op(RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP);
    rlr_backend()->set_depth_test(true);
    rlr_backend()->set_color_mask(true, true, true, true);
    rlr_backend()->set_depth_mask(true);
}

void rlr_pipeline_stencil_deinit() {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    if(!ps) {
        return;
    }

    rlr_res_shader_free(ps->shader);
    rlr_backend()->free_vertex_array(ps->vao);
    rlr_backend()->free_buffer(ps->vbo);
    rlr_backend()->free_buffer(ps->ebo);
    rlr_backend()->free_buffer(ps->instance_vbo);
    rlpp_free(ps->occluder_instances);
}

bool rlr_pipeline_stencil_add_model_occluder_instance(rlr_pipeline_stencil_occluder_instance_t data, rlpp_ref_t* out_ref) {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    *out_ref = rlpp_alloc_to_ref(ps->occluder_instances, data);
    if(rlpp_deref(ps->occluder_instances, *out_ref) != NULL) {
        ps->is_dirty = true;
        if(data.visible) {
            ps->visible_occluder_instances++;
        }
        return true;
    }
    return false;
}

void rlr_pipeline_stencil_set_model_occluder_instance_visability(rlpp_ref_t ref, bool visible) {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    rlr_pipeline_stencil_occluder_instance_t* mo = rlpp_deref(ps->occluder_instances, ref);
    uint8_t vis = visible == true ? 1 : 0;

    if(mo->visible != vis) {
        ps->is_dirty = true;
        mo->visible = vis;

        if(visible) {
            ps->visible_occluder_instances++;
        } else {
            ps->visible_occluder_instances--;
        }
    }
}

rlr_pipeline_stencil_occluder_instance_t* rlr_pipeline_stencil_get_and_dirty_model_occluder_instance(rlpp_ref_t ref) {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    ps->is_dirty = true;
    return rlpp_deref(ps->occluder_instances, ref);
}

void rlr_pipeline_stencil_remove_model_occluder_instance(rlpp_ref_t ref) {
    rlr_pipeline_stencil_t* ps = RLR_PIPELINE_STENCIL;
    rlr_pipeline_stencil_occluder_instance_t* inst = rlpp_deref(ps->occluder_instances, ref);
    if(!inst) {
        return;
    }

    if(inst->visible) {
        ps->visible_occluder_instances--;
    }
    rlpp_remove_by_ref(ps->occluder_instances, ref);
    ps->is_dirty = true;
}