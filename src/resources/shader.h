#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "backends/backend.h"

#define RLR_SHADER_ERROR_LENGTH       4096
#define RLR_SHADER_INLINE(SRC)        "#version 330 core\n" #SRC

typedef enum rlr_shader_uniform_type {
    RLR_SHADER_UNIFORM_BOOL,
    RLR_SHADER_UNIFORM_INT,
    RLR_SHADER_UNIFORM_FLOAT,
    RLR_SHADER_UNIFORM_VEC2,
    RLR_SHADER_UNIFORM_VEC3,
    RLR_SHADER_UNIFORM_MAT4X4,
} rlr_shader_uniform_type;

typedef struct rlr_shader_t {
    rlr_handle_t shader;
} rlr_shader_t;

rlr_shader_t* rlr_shader_create(const char* vertex, const char* fragment);
void rlr_shader_bind_uniform_slot(rlr_shader_t* shader, const char* block_var_name, uint8_t uniform_slot);
void rlr_shader_bind_texture_slot(rlr_shader_t* shader, const char* texture_var_name, uint8_t uniform_slot);
void rlr_shader_use(rlr_shader_t* shader);
void rlr_shader_free(rlr_shader_t* shader);