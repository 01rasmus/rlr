#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/shader.h"

typedef uintptr_t rlr_handle_t;
typedef void (*rlr_backend_proc_t)(void);
typedef rlr_backend_proc_t (*rlr_backend_loader_t)(const char*);

#define RLR_BACKEND_CLEAR_BIT_COLOR         0x00004000
#define RLR_BACKEND_CLEAR_BIT_DEPTH         0x00000100
#define RLR_BACKEND_CLEAR_BIT_STENCIL       0x00000400

#define RLR_BACKEND_FUNCTIONS(X) \
    X(rlr_handle_t, create_texture,             (uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps)) \
    X(void,         free_texture,               (rlr_handle_t handle)) \
    X(rlr_handle_t, compile_shader,             (const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size)) \
    X(void,         free_shader,                (rlr_handle_t shader)) \
    X(void,         use_shader,                 (rlr_handle_t shader)) \
    X(void,         shader_bind_uniform_block,  (rlr_handle_t shader, const char* uniform_block_name, uint32_t slot)) \
    X(rlr_handle_t, create_uniform_buffer,      (uint64_t size, void* init_data)) \
    X(void,         update_uniform_buffer,      (rlr_handle_t buffer, uint64_t offset, uint64_t size, void* data)) \
    X(void,         bind_uniform_buffer,        (rlr_handle_t buffer, uint32_t slot)) \
    X(void,         free_uniform_buffer,        (rlr_handle_t buffer)) \
    X(void,         clear,                      (uint64_t mask)) \
    X(void,         clear_color,                (float r, float g, float b, float a)) \
    X(void,         backend_free,               ())

typedef struct rlr_backend_t {
    #define X(RET, NAME, PARAMS) RET (*NAME)PARAMS;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_d3d10(void* window_handle);