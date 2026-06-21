#include <stb_ds.h>
#include "resources/texture.h"
#include "pipelines/ui.h"
#include "sprite.h"
#include "rlr.h"

rlr_obj_sprite_t* rlr_obj_sprite_create(rlr_texture_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer) {
    return rlr_obj_sprite_create_ext(texture, rectangle, screen_anchor, local_anchor, layer, rlr_rect(0, 0, texture->width, texture->height), rlr_rect(0, 0, 0, 0));
}

rlr_obj_sprite_t* rlr_obj_sprite_create_ext(rlr_texture_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer, rlr_rect_t uv, rlr_rect_t scissor) {
    rlr_obj_sprite_t* sprite = rlr_pipeline_ui_alloc_sprite();

    float uv_x = uv.x / texture->width;
    float uv_y = uv.y / texture->height;

    sprite->screen_anchor = screen_anchor;
    sprite->local_anchor = local_anchor;
    sprite->layer = layer;
    sprite->rectangle = rectangle;
    sprite->scissor = scissor;
    sprite->texture = texture == NULL ? _rlr_raw()->texture_white : texture;
    sprite->uv = rlr_rect(
        uv_x,
        uv_y,
        uv_x + (uv.width / texture->width),
        uv_y + (uv.height / texture->height)
    );

    return sprite;
err:
    rlr_obj_sprite_free(sprite);
    return NULL;
}

void rlr_obj_sprite_free(rlr_obj_sprite_t* sprite) {
    if(!sprite) {
        return;
    }
    rlr_pipeline_ui_free_sprite(sprite);
}