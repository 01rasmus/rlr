#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/shader.h"

typedef void (*rlr_backend_proc_t)(void);
typedef rlr_backend_proc_t (*rlr_backend_loader_t)(const char*);

#define RLR_BACKEND_FUNCTIONS(X) \
    X(uint32_t, create_texture,             (uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps)) \
    X(void,     free_texture,               (uint32_t handle)) \
    X(uint32_t, compile_shader,             (const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size)) \
    X(void,     free_shader,                (uint32_t shader)) \
    X(void,     use_shader,                 (uint32_t shader)) \
    X(uint32_t, shader_uniform_location,    (uint32_t shader, const char* name)) \
    X(void,     shader_uniform_set,         (uint32_t shader, uint32_t location, void* data, rlr_shader_uniform_type type)) \
    X(void,     backend_free,               ())

typedef struct rlr_backend_t {
    #define X(RET, NAME, PARAMS) RET (*NAME)PARAMS;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);