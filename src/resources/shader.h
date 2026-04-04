#pragma once
#include <stdbool.h>
#include <stdint.h>

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
    uint32_t program;
} rlr_shader_t;

rlr_shader_t* rlr_shader_load(const char* vertex, const char* fragment);
void rlr_shader_free(rlr_shader_t* shader);
uint32_t rlr_shader_get_uniform_location(rlr_shader_t* shader, const char* name);
void rlr_shader_set_uniform(rlr_shader_t* shader, uint32_t location, void* value, rlr_shader_uniform_type type);

bool _rlr_shader_compilation_error(uint32_t shader_id, char* error_str, size_t error_str_size);
bool _rlr_shader_program_link_error(uint32_t program_id, char* error_str, size_t error_str_size);