#include "../../internal/core/entity_types.h"
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../resources/texture.h"
#include "../rlr.h"
#include "sprite.h"

static void _rlr_spr_make_instance_rectangle(rlr_rect_t rect, rlr_vec2_t* out_pos, rlr_vec2_t* out_size) {
    float width = rect.width;
    float height = rect.height;
    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(rect.local_anchor);
    out_pos->x = rect.x - (width * local_anchor_vec.x);
    out_pos->y = rect.y - (height * local_anchor_vec.y);
    out_size->x = width;
    out_size->y = height;
}

static inline rlr_spr_t* _rlr_spr_create_impl(rlr_tex_t* tex, rlr_rect_t rect, float u, float v, float tile_w, float tile_h, uint32_t layer) {
    rlr_spr_t* sprite = NULL;
    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(tex, NULL, layer);

    rlr_instance_data_ui_t data = {
        .color = UINT32_MAX,
        .uv = rlr_vec2(u, v),
        .uv_size = rlr_vec2(tile_w, tile_h),
        .screen_anchor = rect.screen_anchor,
        .visible = true,
    };
    _rlr_spr_make_instance_rectangle(rect, &data.pos, &data.size);

    uint64_t instance_index = rlr_pipeline_ui_add_sprite_instance(cmd, data);
    sprite = malloc(sizeof(rlr_spr_t));
    if(!sprite) {
        goto err;
    }

    *sprite = (rlr_spr_t){
        .rect = rect,
        .cmd_id = cmd->id,
        .instance_index = instance_index,
        .layer = layer,
    };

    return sprite;
err:
    rlr_spr_free(sprite);
    return sprite;
}

rlr_spr_t* rlr_spr_new(rlr_tex_t* texture, rlr_rect_t rect, uint32_t layer) {
    rlr_tex_t* tex = texture == NULL ? rlr_default_texture() : texture;
    return _rlr_spr_create_impl(tex, rect, 0.0, 0.0, tex->width, tex->height, layer);
}

rlr_spr_t* rlr_spr_new_from_tile(rlr_tex_atl_tile_t* tile, rlr_rect_t rect, uint32_t layer) {
    rlr_tex_t* tex = tile->texture == NULL ? rlr_default_texture() : tile->texture;
    return _rlr_spr_create_impl(tex, rect, tile->uv.x / tex->width, tile->uv.y / tex->height, tile->size.x / tex->width, tile->size.y / tex->height, layer);
}

rlr_spr_t* rlr_spr_new_col_rect(rlr_color_t col, rlr_rect_t rect, uint32_t layer) {
    rlr_spr_t* spr = rlr_spr_new(NULL, rect, layer);
    if(spr) {
        rlr_spr_set_col(spr, col);
    }
    return spr;
}

void rlr_spr_set_tex_from_tile(rlr_spr_t* sprite, rlr_tex_atl_tile_t* tile) {
    rlr_pipeline_ui_draw_command_t* new_cmd = rlr_pipeline_ui_find_draw_command(tile->texture, NULL, sprite->layer);
    rlr_vec2_t new_uv = rlr_vec2(tile->uv.x / tile->texture->width, tile->uv.y / tile->texture->height);
    rlr_vec2_t new_uv_size = rlr_vec2(tile->size.x / tile->texture->width, tile->size.y / tile->texture->height);

    //it is the same texture, so we just update the instance's uv
    if(new_cmd->id == sprite->cmd_id) {
        rlr_instance_data_ui_t* ins = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
        ins->uv = new_uv;
        ins->uv_size = new_uv_size;
        return;
    }

    //make new instance
    rlr_instance_data_ui_t* old_instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    old_instance->uv = new_uv;
    old_instance->uv_size = new_uv_size;

    uint64_t new_instance = rlr_pipeline_ui_add_sprite_instance(new_cmd, *old_instance);
    rlr_pipeline_ui_remove_sprite_instance(sprite->cmd_id, sprite->instance_index);

    sprite->instance_index = new_instance;
    sprite->cmd_id = new_cmd->id;
}

void rlr_spr_set_tex(rlr_spr_t* sprite, rlr_tex_t* texture) {
    rlr_pipeline_ui_draw_command_t* new_cmd = rlr_pipeline_ui_find_draw_command(texture, NULL, sprite->layer);
    if(new_cmd->id == sprite->cmd_id) {
        return;
    }
    
    rlr_instance_data_ui_t* old_instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    old_instance->uv = rlr_vec2(0.0, 0.0);
    old_instance->uv_size = rlr_vec2(1.0, 1.0);
    uint64_t new_instance = rlr_pipeline_ui_add_sprite_instance(new_cmd, *old_instance);

    rlr_pipeline_ui_remove_sprite_instance(sprite->cmd_id, sprite->instance_index);
    sprite->instance_index = new_instance;
    sprite->cmd_id = new_cmd->id;
}

void rlr_spr_set_col(rlr_spr_t* sprite, rlr_color_t col) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    instance->color = rlr_color_as_uint(col);
}

void rlr_spr_set_vis(rlr_spr_t* sprite, bool visible) {
    rlr_pipeline_ui_set_sprite_instance_visability(sprite->cmd_id, sprite->instance_index, visible);
}

void rlr_spr_set_rect(rlr_spr_t* sprite, rlr_rect_t rect) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    _rlr_spr_make_instance_rectangle(rect, &instance->pos, &instance->size);
    sprite->rect = rect;
}

rlr_rect_t rlr_spr_get_rect(const rlr_spr_t* sprite) {
    return sprite->rect;
}

rlr_anchor_t rlr_spr_get_lcl_anch(const rlr_spr_t* sprite) {
    return sprite->rect.local_anchor;
}

rlr_anchor_t rlr_spr_get_scrn_anch(const rlr_spr_t* sprite) {
    return sprite->rect.screen_anchor;
}

uint32_t rlr_spr_get_col(const rlr_spr_t* sprite) {
    const rlr_instance_data_ui_t const* instance = rlr_pipeline_ui_get_sprite_instance(sprite->cmd_id, sprite->instance_index);
    return instance->color;
}

void rlr_spr_free(rlr_spr_t* sprite) {
    if(!sprite) {
        return;
    }
    rlr_pipeline_ui_remove_sprite_instance(sprite->cmd_id, sprite->instance_index);
    free(sprite);
}