#pragma once
#include <stdbool.h>
#include <stdint.h>

#define RLR_SHADER_ERROR_LENGTH       4096
#define RLR_SHADER_INLINE(SRC)        "#version 330 core\n" #SRC
//#define RLR_SHADER_INLINE(SRC)        "#version 300 es\nprecision highp float;\nprecision highp int;\n" #SRC

typedef struct rlr_res_shader_t {
    uint64_t shader;
} rlr_res_shader_t;

rlr_res_shader_t* rlr_res_shader_create(const char* vertex, const char* fragment);
void rlr_res_shader_bind_uniform_slot(rlr_res_shader_t* shader, const char* block_var_name, uint8_t uniform_slot);
void rlr_res_shader_bind_texture_slot(rlr_res_shader_t* shader, const char* texture_var_name, uint8_t uniform_slot);
void rlr_res_shader_bind(rlr_res_shader_t* shader);
void rlr_res_shader_free(rlr_res_shader_t* shader);