#include <utf8.h>
#include "../../external/stb_ds.h"
#include "../../internal/backends/backend.h"
#include "../../internal/core/word.h"
#include "../../internal/impl.h"
#include "../resources/font.h"
#include "../math/vec.h"
#include "../rlr.h"
#include "label.h"

static bool generate_instances_from_text(rlr_obj_label_t* label, rlr_res_font_t* font, rlr_pipeline_ui_draw_command_t* cmd, const char* text) {
    rlr_res_texture_t* font_texture = rlr_mem_man_get_res_texture(rlr_mem_man(), font->texture);
    if(!font_texture || !label || !font || !cmd) {
        return false;
    }

    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(label->local_anchor);
    rlr_res_font_glyph_t* space_glyph = rlr_res_font_get_glyph(label->font, ' ');
    rlr_res_font_glyph_t* unknown_glyph = rlr_res_font_get_glyph(label->font, '?');
    float x = label->rectangle.x - (label->rectangle.width * local_anchor_vec.x);
    float y = (label->rectangle.y + label->size) - (label->rectangle.height * local_anchor_vec.y);
    float space_width = space_glyph ? ((space_glyph->advance * label->size)) : label->size;
    float start_x = x;

    word_measure_context_t* ctx = rlr_word_measure(unknown_glyph, label->font, label->size, space_width, label->rectangle.height, label->rectangle.width, text);
    y += rlr_vertical_start_position(label->vertical_alignment, label->rectangle.height - (ctx->row_count * label->size));

    uint32_t current_word = 0;
    uint32_t current_row = 0;

    if(ctx->glyphs == NULL) {
        return true;
    }

    uint32_t row_word_count = 0;

    float rem_width = rlr_word_calculate_remaining_row_width(ctx, label->rectangle.width, 0, label->horizontal_alignment, label->size, space_width, &row_word_count);
    float sw = rlr_word_calculate_space_width(rem_width, space_width, row_word_count, label->horizontal_alignment);
    x += rlr_horizontal_start_position(label->horizontal_alignment, rem_width);

    for(uint32_t i = 0; i < arrlenu(ctx->glyphs); i++) {
        rlr_measured_glyph_t* g = &ctx->glyphs[i];

        if(current_word != g->word_id) {
            x += sw;
            current_word = g->word_id;
        }
        
        if(current_row != g->row) {
            rem_width = rlr_word_calculate_remaining_row_width(ctx, label->rectangle.width, i, label->horizontal_alignment, label->size, space_width, &row_word_count);
            sw = rlr_word_calculate_space_width(rem_width, space_width, row_word_count, label->horizontal_alignment);
            current_row = g->row;
            y += label->size;
            x = start_x + rlr_horizontal_start_position(label->horizontal_alignment, rem_width);
        }

        rlr_res_font_glyph_t* glyph = g->glyph;
        float draw_x1 = x + glyph->plane_left * label->size;
        float draw_y1 = y - glyph->plane_top * label->size;
        float draw_x2 = x + glyph->plane_right * label->size;
        float draw_y2 = y - glyph->plane_bottom * label->size;

        float u1 = glyph->atlas_left;
        float v2 = glyph->atlas_bottom;
        float u2 = glyph->atlas_right;
        float v1 = glyph->atlas_top;

        //calculate the screen px range
        float texture_width = (u2 - u1) * font_texture->width;
        float quad_width = draw_x2 - draw_x1;
        float screen_px_range = quad_width / texture_width * (font->px_range);
        if(screen_px_range < 1.0) {
            screen_px_range = 1.0;
        }

        rlr_instance_data_ui_t instance = {
            .color = UINT32_MAX,
            .pos = rlr_vec2(draw_x1, draw_y1),
            .size = rlr_vec2(draw_x2 - draw_x1, draw_y2 - draw_y1),
            .screen_anchor = label->screen_anchor,
            .uv = rlr_vec2(u1, v1),
            .uv_size = rlr_vec2(u2 - u1, v2 - v1),
        };
        arrpush(label->instance_indices, rlr_pipeline_ui_add_sprite_instance(cmd, instance));
        x += glyph->advance * label->size;
    }
    
    return true;
}

static void remove_instances(uint64_t cmd, uint64_t* instance_ids, uint32_t instance_count) {
    for(uint32_t i = 0; i < instance_count; i++) {
        rlr_pipeline_ui_remove_sprite_instance(cmd, instance_ids[i]);
    }
}

rlr_obj_t rlr_obj_label_create(rlr_rect_t rect, float size, uint32_t layer, const char* text, rlr_res_t font_id) {
    return rlr_obj_label_create_ext(rect, size, layer, font_id, RLR_ANCHOR_TOP_LEFT, RLR_ANCHOR_TOP_LEFT, RLR_HORIZONTAL_ALIGNMENT_LEFT, RLR_VERTICAL_ALIGNMENT_TOP, text);
}

rlr_obj_t rlr_obj_label_create_ext(rlr_rect_t rectangle, float text_size, uint32_t layer, rlr_res_t font_id, rlr_anchor_t screen_anchor, rlr_anchor_t local_anhor, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, const char* format, ...) {
    rlr_obj_t id = rlr_mem_man_allocate_obj_label(rlr_mem_man(), (rlr_obj_label_t){0});
    rlr_obj_label_t* label = rlr_mem_man_get_obj_label(rlr_mem_man(), id);
    rlr_res_font_t* font = rlr_mem_man_get_res_font(rlr_mem_man(), font_id);
    rlr_res_t text_shader_id = rlr_pipeline_ui_get_text_shader();
    if(id == RLR_NULL || label == NULL || font_id == RLR_NULL || font == NULL || text_shader_id == RLR_NULL) {
        goto err;
    }

    rlr_pipeline_ui_draw_command_t* cmd = rlr_pipeline_ui_find_draw_command(font->texture, text_shader_id, layer);
    if(!cmd) {
        goto err;
    }
    label->cmd_id = cmd->id;
    label->font = font_id;
    label->rectangle = rectangle;
    label->size = text_size;
    label->screen_anchor = screen_anchor;
    label->local_anchor = local_anhor;
    label->horizontal_alignment = horizontal_alignment;
    label->vertical_alignment = vertical_alignment;

    if(!generate_instances_from_text(label, font, cmd, format)) {
        goto err;
    }
    
    return id;
err:
    rlr_obj_label_free(id);
    return RLR_NULL;
}

void rlr_obj_label_set_text(rlr_obj_t id, const char* text) {
    rlr_obj_label_t* label = rlr_mem_man_get_obj_label(rlr_mem_man(), id);
    remove_instances(label->cmd_id, label->instance_indices, arrlenu(label->instance_indices));
    rlr_res_font_t* font = rlr_mem_man_get_res_font(rlr_mem_man(), label->font);
    generate_instances_from_text(label, font, rlpp_get_unchecked(rlr()->pipeline_ui.commands, label->cmd_id), text);
}

void rlr_obj_label_set_visability(rlr_obj_t label, bool visible) {

}

void rlr_obj_label_free(rlr_obj_t obj) {
    rlr_obj_label_t* label = rlr_mem_man_get_obj_label(rlr_mem_man(), obj);
    if(!label || obj == RLR_NULL) {
        return;
    }
    remove_instances(label->cmd_id, label->instance_indices, arrlenu(label->instance_indices));
    arrfree(label->instance_indices);
    rlr_mem_man_free_obj_label(rlr_mem_man(), obj);
}