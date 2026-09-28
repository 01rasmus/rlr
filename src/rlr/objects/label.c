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

void gen_text(void* user, uint32_t color, float italic_sheer, rlr_vec2_t pos, rlr_vec2_t size, rlr_anchor_t screen_anchor, rlr_vec2_t uv, rlr_vec2_t uv_size, float px_range, uint32_t font_index) {
    rlr_obj_label_t* label = user;

    rlr_instance_data_ui_t instance = {
        .color = color,
        .pos = pos,
        .size = size,
        .screen_anchor = screen_anchor,
        .uv = uv,
        .uv_size = uv_size,
        .italic_sheer = italic_sheer,
        .px_range = px_range,
        .visible = label->visible == true ? 1 : 0,
    };
    rlr_obj_label_instance_ctx_t* ctx = &label->instance_ctxs[font_index];
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(rlr()->pipeline_ui.commands, ctx->cmd_id);
    if(!cmd) {
        return;
    }

    uint64_t id = rlr_pipeline_ui_add_sprite_instance(cmd, instance);
    arrpush(ctx->instance_indices, id);
}

static void remove_instances(rlr_obj_label_t* label) {
    for(size_t i = 0; i < label->instance_ctxs_len; i++) {
        rlr_obj_label_instance_ctx_t* ctx = &label->instance_ctxs[i];

        for(size_t j = 0; j < arrlenu(ctx->instance_indices); j++) {
            rlr_pipeline_ui_remove_sprite_instance(ctx->cmd_id, ctx->instance_indices[j]);
        }
        arrsetlen(ctx->instance_indices, 0);
    }
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

    size_t ctx_count = 1 + font->fallback_fonts_len;
    label->font = font;
    label->rectangle = rectangle;
    label->size = text_size;
    label->screen_anchor = screen_anchor;
    label->local_anchor = local_anhor;
    label->horizontal_alignment = horizontal_alignment;
    label->vertical_alignment = vertical_alignment;
    label->visible = true;
    label->instance_ctxs = malloc(sizeof(rlr_obj_label_instance_ctx_t) * ctx_count);
    label->instance_ctxs_len = ctx_count;
    if(!label->instance_ctxs) {
        goto err;
    }

    //setup the first main font context
    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(font->texture, text_shader, layer);
    if(!cmd) {
        goto err;
    }
    label->instance_ctxs[0] = (rlr_obj_label_instance_ctx_t){
        .cmd_id = cmd->id,
        .instance_indices = NULL
    };

    //set up the fallback ones (mind the for loop parameters!!)
    for(size_t i = 1; i <= font->fallback_fonts_len; i++) {
        rlr_pipeline_ui_draw_command_t* fallback_cmd = rlr_pipeline_ui_find_draw_command(font->fallback_fonts[i - 1]->texture, text_shader, layer);
        if(!fallback_cmd) {
            goto err;
        }

        label->instance_ctxs[i] = (rlr_obj_label_instance_ctx_t){
            .cmd_id = fallback_cmd->id,
            .instance_indices = NULL
        };
    }

    if(!rlr_word_generate(label->font, NULL, text_size, rectangle, horizontal_alignment, vertical_alignment, local_anhor, screen_anchor, gen_text, format, label)) {
        goto err;
    }

    return label;
err:
    rlr_obj_label_free(label);
    return NULL;
}

void rlr_obj_label_set_text(rlr_obj_label_t* label, const char* text) {
    remove_instances(label);
    rlr_word_generate(label->font, NULL, label->size, label->rectangle, label->horizontal_alignment, label->vertical_alignment, label->local_anchor, label->screen_anchor, gen_text, text, label);
}

void rlr_obj_label_set_visability(rlr_obj_label_t* label, bool visible) {
    label->visible = visible;
    for(size_t i = 0; i < label->instance_ctxs_len; i++) {
        rlr_obj_label_instance_ctx_t* ctx = &label->instance_ctxs[i];

        for(size_t j = 0; j < arrlenu(ctx->instance_indices); j++) {
            rlr_pipeline_ui_set_sprite_instance_visability(ctx->cmd_id, ctx->instance_indices[j], visible);
        }
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
    free(label->instance_ctxs);
    free(label);
}