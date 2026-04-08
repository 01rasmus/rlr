#pragma once
#include <stdbool.h>
#include <stdint.h>

#define RLR_SHADER_ERROR_LENGTH       4096
#define RLR_SHADER_INLINE(SRC)        "#version 330 core\n" #SRC

typedef struct rlr_t rlr_t;

typedef enum rlr_shader_uniform_type {
    RLR_SHADER_UNIFORM_BOOL,
    RLR_SHADER_UNIFORM_INT,
    RLR_SHADER_UNIFORM_FLOAT,
    RLR_SHADER_UNIFORM_VEC2,
    RLR_SHADER_UNIFORM_VEC3,
    RLR_SHADER_UNIFORM_MAT4X4,
} rlr_shader_uniform_type;

typedef struct rlr_shader_t {
    uint64_t shader;
} rlr_shader_t;

rlr_shader_t* rlr_shader_load(rlr_t* rlr, const char* vertex, const char* fragment);
void rlr_shader_free(rlr_t* rlr, rlr_shader_t* shader);
uint64_t rlr_shader_get_uniform_location(rlr_t* rlr, rlr_shader_t* shader, const char* name);
void rlr_shader_set_uniform(rlr_t* rlr, rlr_shader_t* shader, uint64_t location, void* value, rlr_shader_uniform_type type);