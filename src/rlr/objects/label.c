#include <utf8.h>
#include "../../external/stb_ds.h"
#include "../../internal/backends/backend.h"
#include "../../internal/core/obj_types.h"
#include "../../internal/core/res_types.h"
#include "../../internal/core/word.h"
#include "../../internal/impl.h"
#include "../resources/font.h"
#include "../math/vec.h"
#include "../rlr.h"
#include "label.h"

struct text_gen_ctx_t {
    rlr_pipeline_ui_draw_command_t* cmd;
    rlr_obj_label_t* label;
};

void gen_text(void* user, uint32_t color, float italic_sheer, rlr_vec2_t pos, rlr_vec2_t size, rlr_anchor_t screen_anchor, rlr_vec2_t uv, rlr_vec2_t uv_size, float px_range) {
    struct text_gen_ctx_t* ctx = user;

    rlr_instance_data_ui_t instance = {
        .color = color,
        .pos = pos,
        .size = size,
        .screen_anchor = screen_anchor,
        .uv = uv,
        .uv_size = uv_size,
        .italic_sheer = italic_sheer,
        .screen_px_range = px_range,
        .visible = ctx->label->visible == true ? 1 : 0,
    };
    uint64_t id = rlr_pipeline_ui_add_sprite_instance(ctx->cmd, instance);
    arrpush(ctx->label->instance_indices, id);
}

static void remove_instances(rlr_obj_label_t* label) {
    for(size_t i = 0; i < arrlenu(label->instance_indices); i++) {
        rlr_pipeline_ui_remove_sprite_instance(label->cmd_id, label->instance_indices[i]);
    }
    arrsetlen(label->instance_indices, 0);
}

rlr_obj_label_t* rlr_obj_label_create(rlr_rect_t rect, float size, uint32_t layer, const char* text, rlr_res_font_t* font_id) {
    return rlr_obj_label_create_ext(rect, size, layer, font_id, RLR_ANCHOR_TOP_LEFT, RLR_ANCHOR_TOP_LEFT, RLR_HORIZONTAL_ALIGNMENT_LEFT, RLR_VERTICAL_ALIGNMENT_TOP, text);
}

rlr_obj_label_t* rlr_obj_label_create_ext(rlr_rect_t rectangle, float text_size, uint32_t layer, rlr_res_font_t* font, rlr_anchor_t screen_anchor, rlr_anchor_t local_anhor, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, const char* format, ...) {
    rlr_obj_label_t* label = malloc(sizeof(rlr_obj_label_t));
    rlr_res_shader_t* text_shader = rlr_pipeline_ui_get_text_shader();
    if(label == NULL  || font == NULL || text_shader == NULL) {
        goto err;
    }

    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(font->texture, text_shader, layer);
    if(!cmd) {
        goto err;
    }
    label->cmd_id = cmd->id;
    label->font = font;
    label->rectangle = rectangle;
    label->size = text_size;
    label->screen_anchor = screen_anchor;
    label->local_anchor = local_anhor;
    label->horizontal_alignment = horizontal_alignment;
    label->vertical_alignment = vertical_alignment;
    label->instance_indices = NULL;
    label->visible = true;

    struct text_gen_ctx_t ctx = {
        .cmd = cmd,
        .label = label,
    };
    if(!rlr_word_generate(label->font, NULL, text_size, rectangle, horizontal_alignment, vertical_alignment, local_anhor, screen_anchor, gen_text, format, &ctx)) {
        goto err;
    }

    return label;
err:
    rlr_obj_label_free(label);
    return NULL;
}

void rlr_obj_label_set_text(rlr_obj_label_t* label, const char* text) {
    remove_instances(label);
    struct text_gen_ctx_t ctx = {
        .cmd = rlpp_get_unchecked(rlr()->pipeline_ui.commands, label->cmd_id),
        .label = label,
    };
    rlr_word_generate(label->font, NULL, label->size, label->rectangle, label->horizontal_alignment, label->vertical_alignment, label->local_anchor, label->screen_anchor, gen_text, text, &ctx);
}

void rlr_obj_label_set_visability(rlr_obj_label_t* label, bool visible) {
    label->visible = visible;
    for(size_t i = 0; i < arrlenu(label->instance_indices); i++) {
        rlr_pipeline_ui_set_sprite_instance_visability(label->cmd_id, label->instance_indices[i], visible);
    }
}

void rlr_obj_label_set_rectangle(rlr_obj_label_t* label, rlr_rect_t rect) {
    label->rectangle = rect;
}

void rlr_obj_label_free(rlr_obj_label_t* label) {
    if(!label) {
        return;
    }
    remove_instances(label);
    arrfree(label->instance_indices);
    free(label);
}