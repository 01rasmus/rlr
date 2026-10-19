#include <stdlib.h>
#include "../../internal/core/entity_types.h"
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "nine_patch.h"

typedef struct callback_ctx_t {
    rlr_npatch_t* np;
    rlr_pipeline_ui_draw_command_t* cmd;
} callback_ctx_t;

static void generate_instance_data(rlr_tex_t* texture, bool visible, rlr_npatch_desc_t nine_patch, rlr_anch_rect_t anch_rect, void* user, void (*on_instance)(uint8_t index, rlr_instance_data_ui_t data, void* user)) {

    //rectangle coordinates
    float x0 = 0.0;
    float x1 = nine_patch.left;
    float x2 = anch_rect.rect.width - nine_patch.right;
    float x3 = anch_rect.rect.width;
    float y0 = 0.0;
    float y1 = nine_patch.top;
    float y2 = anch_rect.rect.height - nine_patch.bottom;
    float y3 = anch_rect.rect.height;

    //uv coordianates
    float u0 = 0.0;
    float u1 = nine_patch.left;
    float u2 = texture->width - nine_patch.right;
    float u3 = texture->width;
    float v0 = 0.0;
    float v1 = nine_patch.top;
    float v2 = texture->height - nine_patch.bottom;
    float v3 = texture->height;

    rlr_rect_t r[9] = {
        rlr_rect(x0, y0, x1, y1),
        rlr_rect(x2, y0, x3 - x2, y1),
        rlr_rect(x0, y2, x1, y3 - y2),
        rlr_rect(x2, y2, x3 - x2, y3 - y2),
        rlr_rect(x1, y0, x2 - x1, y1),
        rlr_rect(x1, y2, x2 - x1, y3 - y2),
        rlr_rect(x0, y1, x1, y2 - y1),
        rlr_rect(x2, y1, x3 - x2, y2 - y1),
    };
    rlr_rect_t uv[9] = {
        rlr_rect(u0, v0, u1, v1),
        rlr_rect(u2, v0, u3 - u2, v1),
        rlr_rect(u0, v2, u1, v3 - v2),
        rlr_rect(u2, v2, u3 - u2, v3 - v2),
        rlr_rect(u1, v0, u2 - u1, v1),
        rlr_rect(u1, v2, u2 - u1, v3 - v2),
        rlr_rect(u0, v1, u1, v2 - v1),
        rlr_rect(u2, v1, u3 - u2, v2 - v1),
    };

    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(anch_rect.local_anchor);
    size_t count = sizeof(r) / sizeof(r[0]);
    if(!nine_patch.show_middle) {
        count--;
    }

    for(size_t i = 0; i < count; i++) {
        rlr_rect_t cr = r[i];
        rlr_rect_t cuv = uv[i];
        on_instance(
            i,
            ((rlr_instance_data_ui_t){
                .color = UINT32_MAX,
                .pos = rlr_vec2(anch_rect.rect.x + cr.x - (anch_rect.rect.width * local_anchor_vec.x), anch_rect.rect.y + cr.y - (anch_rect.rect.height * local_anchor_vec.y)),
                .size = rlr_vec2(cr.width, cr.height),
                .uv = rlr_vec2(cuv.x / texture->width, cuv.y / texture->height),
                .uv_size = rlr_vec2(cuv.width / texture->width, cuv.height / texture->height),
                .screen_anchor = anch_rect.screen_anchor,
                .visible = visible,
            }),
            user
        );
    }
}

static void callback_instance_create(uint8_t index, rlr_instance_data_ui_t data, void* user) {
    callback_ctx_t* ctx = user;
    ctx->np->instances[index] = rlr_pipeline_ui_add_sprite_instance(ctx->cmd, data);
}

static void callback_instance_update(uint8_t index, rlr_instance_data_ui_t data, void* user) {
    rlr_npatch_t* np = user;
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(np->cmd_id, np->instances[index]);
    *instance = data;
}

rlr_npatch_t* rlr_npatch_create(rlr_tex_t* texture, rlr_npatch_desc_t nine_patch, rlr_anch_rect_t anch_rect, uint32_t layer) {
    rlr_npatch_t* np = malloc(sizeof(rlr_npatch_t));
    if(!np) {
        goto err;
    }

    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(texture, NULL, layer);
    if(!cmd) {
        goto err;
    }
    *np = (rlr_npatch_t){
        .cmd_id = cmd->id,
        .instances = {0},
        .nine_patch = nine_patch,
        .anch_rect = anch_rect,
        .texture = texture,
        .visible = true,
    };

    //generate instances
    callback_ctx_t ctx = {
        .np = np,
        .cmd = cmd,
    };
    generate_instance_data(texture, np->visible, nine_patch, anch_rect, &ctx, callback_instance_create);
    return np;
err:
    return NULL;
}

void rlr_npatch_set_vis(rlr_npatch_t* np, bool visible) {
    np->visible = visible;
    size_t count = sizeof(np->instances) / sizeof(np->instances[0]);
    if(!np->nine_patch.show_middle) {
        count--;
    }

    for(size_t i = 0; i < count; i++) {
        rlr_pipeline_ui_set_sprite_instance_visability(np->cmd_id, np->instances[i], visible);
    }
}

void rlr_npatch_set_anch_rect(rlr_npatch_t* np, rlr_anch_rect_t anch_rect) {
    np->anch_rect = anch_rect;
    generate_instance_data(np->texture, np->visible, np->nine_patch, np->anch_rect, np, callback_instance_update);
}

void rlr_npatch_free(rlr_npatch_t* np) {
    if(!np) {
        return;
    }

    size_t count = (sizeof(np->instances) / sizeof(np->instances[0]));
    if(!np->nine_patch.show_middle) {
        count--;
    }
    for(size_t i = 0; i < count; i++) {
        rlr_pipeline_ui_remove_sprite_instance(np->cmd_id, np->instances[i]);
    }

    free(np);
}