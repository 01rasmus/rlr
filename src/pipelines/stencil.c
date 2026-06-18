#include "resources/shader.h"
#include "math/vec.h"
#include "stencil.h"
#include "rlr.h"

static const rlr_vec2_t quad_vertices[6] = {
    rlr_vec2(1, 1),
    rlr_vec2(1, 0),
    rlr_vec2(0, 0),
    rlr_vec2(0, 1),
    rlr_vec2(1, 1),
    rlr_vec2(0, 0)
};

static const char stencil_fragment[] = RLR_SHADER_INLINE(
    void main() {
    }
);

static const char stencil_vertex[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec2 pos;

    void main() {
        gl_Position = vec4(pos, 0.0, 1.0);
    }
);

static void rlr_pipeline_stencil_rebuild() {
    rlr_pipeline_stencil_t* ps = &_rlr_raw()->pipeline_stencil;
    rlr_vec2_t vertices[6];

    float rect_x = 4;
    float rect_y = 540 - 119 - 4;
    float rect_w = 286;
    float rect_h = 119;

    float inv_x = 1.0 / 1024.0;
    float inv_y = 1.0 / 768.0;
    for(int32_t i = 0; i < 6; i++) {
        rlr_vec2_t pos = quad_vertices[i];
        float x = rect_x + pos.x * rect_w;
        float y = rect_y + pos.y * rect_h;
        vertices[i] = rlr_vec2(x * inv_x * 2.0 - 1.0, 1.0 - y * inv_y * 2.0);
    }
    ps->vertex_count = 6;

    rlr_backend()->buffer_bind(ps->vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->buffer_update(RLR_BACKEND_BUFFER_ARRAY, sizeof(vertices), vertices, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    ps->is_dirty = false;
}

bool rlr_pipeline_stencil_init() {
    rlr_pipeline_stencil_t* ps = &_rlr_raw()->pipeline_stencil;
    (*ps) = (rlr_pipeline_stencil_t){0};

    ps->is_dirty = true;
    ps->vao = rlr_backend()->vertex_array_create();
    ps->vbo = rlr_backend()->buffer_create();
    if(!ps->vao || !ps->vbo) {
        goto err;
    }

    ps->shader = rlr_shader_create(stencil_vertex, stencil_fragment);
    if(!ps->shader) {
        goto err;
    }

    rlr_backend()->vertex_array_bind(ps->vao);
    rlr_backend()->buffer_bind(ps->vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->vertex_array_attrib_set(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_vec2_t), 0);

    return true;
err:
    rlr_pipeline_stencil_free();
    return false;
}

void rlr_pipeline_stencil_draw() {
    rlr_pipeline_stencil_t* ps = &_rlr_raw()->pipeline_stencil;

    if(ps->is_dirty) {
        rlr_pipeline_stencil_rebuild();
    }

    rlr_backend()->stencil_enable();
    rlr_backend()->stencil_mask(0xFF);
    rlr_backend()->stencil_func(RLR_BACKEND_STENCIL_FUNC_ALWAYS, 1, 0xFF);
    rlr_backend()->stencil_op(RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_REPLACE);

    rlr_shader_use(ps->shader);
    rlr_backend()->vertex_array_bind(ps->vao);
    rlr_backend()->draw_array(0, ps->vertex_count);
    rlr_backend()->stencil_mask(0x0);

    rlr_backend()->stencil_func(RLR_BACKEND_STENCIL_FUNC_EQUAL, 0, 0xFF);
    rlr_backend()->stencil_op(RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP, RLR_BACKEND_STENCIL_OP_KEEP);
}

void rlr_pipeline_stencil_free() {
    rlr_pipeline_stencil_t* ps = &_rlr_raw()->pipeline_stencil;
    if(!ps) {
        return;
    }
}