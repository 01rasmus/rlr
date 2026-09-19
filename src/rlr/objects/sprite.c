#include "../../internal/core/obj_types.h"
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../resources/texture.h"
#include "../rlr.h"
#include "sprite.h"

static rlr_rect_t _rlr_obj_sprite_make_instance_rectangle(rlr_anchor_t local_anchor, rlr_rect_t rectangle) {
    float width = rectangle.width;
    float height = rectangle.height;
    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(local_anchor);
    return (rlr_rect_t) {
        .x = rectangle.x - (width * local_anchor_vec.x),
        .y = rectangle.y - (height * local_anchor_vec.y),
        .width = width,
        .height = height,
    };
}

rlr_obj_sprite_t* rlr_obj_sprite_create(rlr_res_texture_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer) {
    return rlr_obj_sprite_create_ext(texture == NULL ? rlr_res_texture_default() : texture, rectangle, screen_anchor, local_anchor, layer, rlr_rect(0, 0, texture->width, texture->height), rlr_rect(0, 0, 0, 0));
}

rlr_obj_sprite_t* rlr_obj_sprite_create_from_atlas_tile(rlr_res_texture_atlas_tile_t* tile, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer) {
    return rlr_obj_sprite_create_ext(tile->texture, rectangle, screen_anchor, local_anchor, layer, rlr_rect(tile->uv.x, tile->uv.y, tile->size.x, tile->size.y), rlr_rect(0, 0, 0, 0));
}

rlr_obj_sprite_t* rlr_obj_sprite_create_ext(rlr_res_texture_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer, rlr_rect_t uv, rlr_rect_t scissor) {
    rlr_obj_sprite_t* sprite = NULL;
    rlr_res_texture_t* tex = texture == NULL ? rlr_default_texture() : texture;
    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(tex, NULL, layer);

    rlr_rect_t instance_rect = _rlr_obj_sprite_make_instance_rectangle(local_anchor, rectangle);
    rlr_instance_data_ui_t data = {
        .color = UINT32_MAX,
        .pos = rlr_vec2(instance_rect.x, instance_rect.y),
        .size = rlr_vec2(instance_rect.width, instance_rect.height),
        .uv = rlr_vec2(uv.x / texture->width, uv.y / texture->height),
        .uv_size = rlr_vec2(uv.width / texture->width, uv.height / texture->height),
        .screen_anchor = screen_anchor,
        .visible = true,
    };
    uint64_t instance_index = rlr_pipeline_ui_add_sprite_instance(cmd, data);
    sprite = malloc(sizeof(rlr_obj_sprite_t));
    if(!sprite) {
        goto err;
    }

    *sprite = (rlr_obj_sprite_t){
        .rect = rectangle,
        .local_anchor = local_anchor,
        .cmd_id = cmd->id,
        .instance_index = instance_index,
    };

    return sprite;
err:
    rlr_obj_sprite_free(sprite);
    return sprite;
}

void rlr_obj_sprite_set_color(rlr_obj_sprite_t* sprite, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    instance->color = (a << 24) | (b << 16) | (g << 8) | r;
}

void rlr_obj_sprite_set_visability(rlr_obj_sprite_t* sprite, bool visible) {
    rlr_pipeline_ui_set_sprite_instance_visability(sprite->cmd_id, sprite->instance_index, visible);
}

void rlr_obj_sprite_set_rectangle(rlr_obj_sprite_t* sprite, rlr_rect_t rect) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    rlr_rect_t instance_rect = _rlr_obj_sprite_make_instance_rectangle(sprite->local_anchor, rect);
    instance->pos = rlr_vec2(instance_rect.x, instance_rect.y);
    instance->size = rlr_vec2(instance_rect.width, instance_rect.height);
    sprite->rect = rect;
}

void rlr_obj_sprite_set_local_anchor(rlr_obj_sprite_t* sprite, rlr_anchor_t anchor) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    rlr_rect_t instance_rect = _rlr_obj_sprite_make_instance_rectangle(anchor, sprite->rect);
    instance->pos = rlr_vec2(instance_rect.x, instance_rect.y);
    instance->size = rlr_vec2(instance_rect.width, instance_rect.height);
    sprite->local_anchor = anchor;
}

void rlr_obj_sprite_set_screen_anchor(rlr_obj_sprite_t* sprite, rlr_anchor_t anchor) {
    rlr_instance_data_ui_t* instance = rlr_pipeline_ui_get_and_dirty_sprite_instance(sprite->cmd_id, sprite->instance_index);
    instance->screen_anchor = anchor;
}

rlr_rect_t rlr_obj_sprite_get_rectangle(const rlr_obj_sprite_t* sprite) {
    return sprite->rect;
}

rlr_anchor_t rlr_obj_sprite_get_local_anchor(const rlr_obj_sprite_t* sprite) {
    return sprite->local_anchor;
}

rlr_anchor_t rlr_obj_sprite_get_screen_anchor(const rlr_obj_sprite_t* sprite) {
    const rlr_instance_data_ui_t const* instance = rlr_pipeline_ui_get_sprite_instance(sprite->cmd_id, sprite->instance_index);
    return instance->screen_anchor;
}

uint32_t rlr_obj_sprite_get_color(const rlr_obj_sprite_t* sprite) {
    const rlr_instance_data_ui_t const* instance = rlr_pipeline_ui_get_sprite_instance(sprite->cmd_id, sprite->instance_index);
    return instance->color;
}

void rlr_obj_sprite_free(rlr_obj_sprite_t* sprite) {
    if(!sprite) {
        return;
    }
    rlr_pipeline_ui_remove_sprite_instance(sprite->cmd_id, sprite->instance_index);
    free(sprite);
}