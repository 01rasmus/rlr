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

typedef enum rlr_backend_buffer_target_t {
    RLR_BACKEND_BUFFER_ARRAY      = 0x8892,
    RLR_BACKEND_BUFFER_UNIFORM    = 0x8A11,
} rlr_backend_buffer_target_t;

typedef enum rlr_backend_buffer_usage_t {
    RLR_BACKEND_BUFFER_USAGE_DYNAMIC   = 0x88E8,
    RLR_BACKEND_BUFFER_USAGE_STATIC    = 0x88E4
} rlr_backend_buffer_usage_t;

typedef enum rlr_backend_type_t {
    RLR_BACKEND_BUFFER_TYPE_S8          = 0x1400,
    RLR_BACKEND_BUFFER_TYPE_U8          = 0x1401,
    RLR_BACKEND_BUFFER_TYPE_S16         = 0x1402,
    RLR_BACKEND_BUFFER_TYPE_U16         = 0x1403,
    RLR_BACKEND_BUFFER_TYPE_FLOAT       = 0x1406
} rlr_backend_type_t;

#define RLR_BACKEND_FUNCTIONS(X) \
    X(rlr_handle_t, vertex_array_create,            ()) \
    X(void,         vertex_array_bind,              (rlr_handle_t handle)) \
    X(void,         vertex_array_attrib_pointer,    (rlr_handle_t handle, uint32_t index, uint8_t count, rlr_backend_type_t type, bool normalized, uint32_t stride, uintptr_t vertex_offset)) \
    X(void,         vertex_array_free,              (rlr_handle_t handle)) \
    X(rlr_handle_t, buffer_create,                  ()) \
    X(void, buffer_bind,                            (rlr_handle_t handle, rlr_backend_buffer_target_t target)) \
    X(void, buffer_update,                          (rlr_handle_t handle, rlr_backend_buffer_target_t target, uint64_t size, void* data, rlr_backend_buffer_usage_t update_type)) \
    X(void, buffer_free,                            (rlr_handle_t handle)) \
    X(rlr_handle_t, texture_create,                 (uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps)) \
    X(void,         texture_use,                    (rlr_handle_t handle)) \
    X(void,         texture_free,                   (rlr_handle_t handle)) \
    X(rlr_handle_t, shader_create,                  (const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size)) \
    X(void,         shader_free,                    (rlr_handle_t shader)) \
    X(void,         shader_use,                     (rlr_handle_t shader)) \
    X(void,         shader_bind_uniform_block,      (rlr_handle_t shader, const char* uniform_block_name, uint32_t slot)) \
    X(rlr_handle_t, uniform_buffer_create,          (uint64_t size, void* init_data)) \
    X(void,         uniform_buffer_update,          (rlr_handle_t buffer, uint64_t offset, uint64_t size, void* data)) \
    X(void,         uniform_buffer_bind,            (rlr_handle_t buffer, uint32_t slot)) \
    X(void,         uniform_buffer_free,            (rlr_handle_t buffer)) \
    X(void,         clear,                          (uint64_t mask)) \
    X(void,         clear_color,                    (float r, float g, float b, float a)) \
    X(void,         draw_array,                     (uint64_t offset, uint32_t vertex_count)) \
    X(void,         backend_free,                   ())

typedef struct rlr_backend_t {
    #define X(RET, NAME, PARAMS) RET (*NAME)PARAMS;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);