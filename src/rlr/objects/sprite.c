#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../resources/texture.h"
#include "../rlr.h"
#include "sprite.h"

rlr_obj_t rlr_obj_sprite_create(rlr_res_t texture_id, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer) {
    rlr_res_texture_t* texture = rlr_mem_man_get_res_texture(rlr_mem_man(), texture_id == RLR_NULL ? rlr_res_texture_default() : texture_id);
    return rlr_obj_sprite_create_ext(texture_id, rectangle, screen_anchor, local_anchor, layer, rlr_rect(0, 0, texture->width, texture->height), rlr_rect(0, 0, 0, 0));
}

rlr_obj_t rlr_obj_sprite_create_ext(rlr_res_t texture_id, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer, rlr_rect_t uv, rlr_rect_t scissor) {
    rlr_res_texture_t* texture = rlr_mem_man_get_res_texture(rlr_mem_man(), texture_id == RLR_NULL ? rlr_res_texture_default() : texture_id);
    float uv_x = uv.x / texture->width;
    float uv_y = uv.y / texture->height;
    rlr_obj_t id = rlr_mem_man_allocate_obj_sprite(rlr_mem_man(), (rlr_obj_sprite_t){
        .screen_anchor = screen_anchor,
        .local_anchor = local_anchor,
        .layer = layer,
        .rectangle = rectangle,
        .scissor = scissor,
        .texture = texture_id == RLR_NULL ? rlr_internal_get_white_texture() : texture_id,
        .uv = rlr_rect(
            uv_x,
            uv_y,
            uv_x + (uv.width / texture->width),
            uv_y + (uv.height / texture->height)
        ),
    });
    if(id == RLR_NULL) {
        goto err;
    }
    return id;
err:
    rlr_obj_sprite_free(id);
    return RLR_NULL;
}

void rlr_obj_sprite_free(rlr_obj_t obj) {
    if(obj == RLR_NULL) {
        return;
    }
    rlr_obj_sprite_t* sprite = rlr_mem_man_get_obj_sprite(rlr_mem_man(), obj);
    if(!sprite) {
        return;
    }
    rlr_mem_man_free_obj_sprite(rlr_mem_man(), obj);
}