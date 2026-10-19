#include <stdarg.h>
#include <utf8.h>
#include "../../external/stb_ds.h"
#include "../../internal/backends/backend.h"
#include "../../internal/core/entity_types.h"
#include "../../internal/core/res_types.h"
#include "../../internal/core/word.h"
#include "../../internal/impl.h"
#include "../resources/font.h"
#include "../math/vec.h"
#include "../rlr.h"
#include "label.h"

void gen_text(void* user, uint32_t color, float italic_sheer, rlr_vec2_t pos, rlr_vec2_t size, rlr_anchor_t screen_anchor, rlr_vec2_t uv, rlr_vec2_t uv_size, float px_range, uint32_t font_index) {
    rlr_lbl_t* label = user;

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
    rlr_label_instance_ctx_t* ctx = &label->instance_ctxs[font_index];
    rlr_pipeline_ui_draw_command_t* cmd = rlpp_get_unchecked(rlr()->pipeline_ui.commands, ctx->cmd_id);
    if(!cmd) {
        return;
    }

    uint64_t id = rlr_pipeline_ui_add_sprite_instance(cmd, instance);
    arrpush(ctx->instance_indices, id);
}

static void remove_instances(rlr_lbl_t* label) {
    for(size_t i = 0; i < label->instance_ctxs_len; i++) {
        rlr_label_instance_ctx_t* ctx = &label->instance_ctxs[i];

        for(size_t j = 0; j < arrlenu(ctx->instance_indices); j++) {
            rlr_pipeline_ui_remove_sprite_instance(ctx->cmd_id, ctx->instance_indices[j]);
        }
        arrsetlen(ctx->instance_indices, 0);
    }
}

static char* vstrcpy(const char* format, va_list args_orig, size_t* out_str_len) {
    va_list args[2];
    va_copy(args[0], args_orig);
    va_copy(args[1], args_orig);

    size_t str_len = vsnprintf(NULL, 0, format, args[0]);
    *out_str_len = str_len;
    if(str_len == 0) {
        goto err;
    }
    size_t str_cap = str_len + 1;
    char* copy = malloc(str_cap);
    if(!copy) {
        goto err;
    }
    vsnprintf(copy, str_cap, format, args[1]);

    va_end(args[0]);
    va_end(args[1]);
    return copy;
err:
    va_end(args[0]);
    va_end(args[1]);
    return NULL;
}

static rlr_lbl_t* _rlr_lbl_create_impl(rlr_font_t* font, rlr_rect_t rect, float text_size, rlr_hori_algn_t hori_algn, rlr_vert_algn_t vert_algn, uint32_t layer, const char* format, va_list args) {
    rlr_lbl_t* label = malloc(sizeof(rlr_lbl_t));
    rlr_shader_t* text_shader = rlr_pipeline_ui_get_text_shader();
    char* temp_str = NULL;
    if(label == NULL  || font == NULL || text_shader == NULL) {
        goto err;
    }

    size_t ctx_count = 1 + font->fallback_fonts_len;
    label->font = font;
    label->rect = rect;
    label->size = text_size;
    label->horizontal_alignment = hori_algn;
    label->vertical_alignment = vert_algn;
    label->visible = true;
    label->instance_ctxs = malloc(sizeof(rlr_label_instance_ctx_t) * ctx_count);
    label->instance_ctxs_len = ctx_count;
    if(!label->instance_ctxs) {
        goto err;
    }

    //setup the first main font context
    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(font->texture, text_shader, layer);
    if(!cmd) {
        goto err;
    }
    label->instance_ctxs[0] = (rlr_label_instance_ctx_t){
        .cmd_id = cmd->id,
        .instance_indices = NULL
    };

    //set up the fallback ones (mind the for loop parameters!!)
    for(size_t i = 1; i <= font->fallback_fonts_len; i++) {
        rlr_pipeline_ui_draw_command_t* fallback_cmd = rlr_pipeline_ui_find_draw_command(font->fallback_fonts[i - 1]->texture, text_shader, layer);
        if(!fallback_cmd) {
            goto err;
        }

        label->instance_ctxs[i] = (rlr_label_instance_ctx_t){
            .cmd_id = fallback_cmd->id,
            .instance_indices = NULL
        };
    }

    size_t len;
    temp_str = vstrcpy(format, args, &len);

    //only evaluate if there was a string
    if(len > 0) {
        if(!temp_str) {
            goto err;
        }
    
        if(!rlr_word_generate(label->font, NULL, text_size, rect, hori_algn, vert_algn, gen_text, temp_str, label)) {
            goto err;
        }
    }

    free(temp_str);
    return label;
err:
    free(temp_str);
    rlr_lbl_free(label);
    return NULL;
}

rlr_lbl_t* rlr_lbl_create(rlr_font_t* font, float x, float y, float size, uint32_t layer, const char* format, ...) {
    rlr_rect_t rect = rlr_rect_lcl(x, y, 10000000.0, 10000000.0, RLR_ANCH_TOP_LEFT);
    va_list args;
    va_start(args, format);
    rlr_lbl_t* label = _rlr_lbl_create_impl(font, rect, size, RLR_HORI_ALGN_LEFT, RLR_VERT_ALGN_TOP, layer, format, args);
    va_end(args);
    return label;
}

rlr_lbl_t* rlr_lbl_create_rect(rlr_font_t* font, rlr_rect_t rect, float size, uint32_t layer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    rlr_lbl_t* label = _rlr_lbl_create_impl(font, rect, size, RLR_HORI_ALGN_LEFT, RLR_VERT_ALGN_TOP, layer, format, args);
    va_end(args);
    return label;
}

rlr_lbl_t* rlr_lbl_create_rect_algn(rlr_font_t* font, rlr_rect_t rect, float text_size, rlr_hori_algn_t hori_algn, rlr_vert_algn_t vert_algn, uint32_t layer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    rlr_lbl_t* label = _rlr_lbl_create_impl(font, rect, text_size, hori_algn, vert_algn, layer, format, args);
    va_end(args);
    return label;
}

void rlr_lbl_set_textf(rlr_lbl_t* label, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len;
    char* temp_str = vstrcpy(format, args, &len);
    va_end(args);
    if(!temp_str || len == 0) {
        free(temp_str);
        return;
    }
    
    remove_instances(label);
    rlr_word_generate(label->font, NULL, label->size, label->rect, label->horizontal_alignment, label->vertical_alignment, gen_text, temp_str, label);
    free(temp_str);
}

void rlr_lbl_set_textf_rect(rlr_lbl_t* label, rlr_rect_t rect, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len;
    char* temp_str = vstrcpy(format, args, &len);
    va_end(args);
    if(!temp_str || len == 0) {
        free(temp_str);
        return;
    }
    
    label->rect = rect;
    remove_instances(label);
    rlr_word_generate(label->font, NULL, label->size, label->rect, label->horizontal_alignment, label->vertical_alignment, gen_text, temp_str, label);
    free(temp_str);
}

void rlr_lbl_set_vis(rlr_lbl_t* label, bool visible) {
    label->visible = visible;
    for(size_t i = 0; i < label->instance_ctxs_len; i++) {
        rlr_label_instance_ctx_t* ctx = &label->instance_ctxs[i];

        for(size_t j = 0; j < arrlenu(ctx->instance_indices); j++) {
            rlr_pipeline_ui_set_sprite_instance_visability(ctx->cmd_id, ctx->instance_indices[j], visible);
        }
    }
}

void rlr_lbl_free(rlr_lbl_t* label) {
    if(!label) {
        return;
    }
    remove_instances(label);
    free(label->instance_ctxs);
    free(label);
}