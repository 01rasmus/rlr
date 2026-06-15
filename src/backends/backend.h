#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t rlr_handle_t;
typedef void (*rlr_backend_proc_t)(void);
typedef rlr_backend_proc_t (*rlr_backend_loader_t)(const char*);

#define RLR_BACKEND_CLEAR_BIT_COLOR         0x00004000
#define RLR_BACKEND_CLEAR_BIT_DEPTH         0x00000100
#define RLR_BACKEND_CLEAR_BIT_STENCIL       0x00000400

typedef enum rlr_backend_buffer_target_t {
    RLR_BACKEND_BUFFER_ARRAY            = 0x8892,
    RLR_BACKEND_BUFFER_UNIFORM          = 0x8A11,
    RLR_BACKEND_BUFFER_ELEMENT_ARRAY    = 0x8893
} rlr_backend_buffer_target_t;

typedef enum rlr_backend_texture_type_t {
    RLR_BACKEND_TEXTURE_2D              = 0x0DE1,
    RLR_BACKEND_TEXTURE_2D_ARRAY        = 0x8C1A,
    RLR_BACKEND_TEXTURE_CUBE_MAP        = 0x8513,
} rlr_backend_texture_type_t;

typedef enum rlr_backend_buffer_usage_t {
    RLR_BACKEND_BUFFER_USAGE_DYNAMIC    = 0x88E8,
    RLR_BACKEND_BUFFER_USAGE_STATIC     = 0x88E4
} rlr_backend_buffer_usage_t;

typedef enum rlr_backend_type_t {
    RLR_BACKEND_BUFFER_TYPE_S8          = 0x1400,
    RLR_BACKEND_BUFFER_TYPE_U8          = 0x1401,
    RLR_BACKEND_BUFFER_TYPE_S16         = 0x1402,
    RLR_BACKEND_BUFFER_TYPE_U16         = 0x1403,
    RLR_BACKEND_BUFFER_TYPE_S32         = 0x1404,
    RLR_BACKEND_BUFFER_TYPE_U32         = 0x1405,
    RLR_BACKEND_BUFFER_TYPE_FLOAT       = 0x1406
} rlr_backend_type_t;

#define RLR_BACKEND_FUNCTIONS(X) \
    X(rlr_handle_t, vertex_array_create,            ()) \
    X(void,         vertex_array_bind,              (rlr_handle_t vao)) \
    X(void,         vertex_array_attrib_set,        (uint32_t index, uint8_t count, rlr_backend_type_t type, bool normalized, uint32_t stride, uintptr_t vertex_offset)) \
    X(void,         vertex_array_free,              (rlr_handle_t vao)) \
    X(rlr_handle_t, buffer_create,                  ()) \
    X(void,         buffer_bind,                    (rlr_handle_t buffer, rlr_backend_buffer_target_t target)) \
    X(void,         buffer_update,                  (rlr_backend_buffer_target_t target, uint64_t size, void* data, rlr_backend_buffer_usage_t update_type)) \
    X(void,         buffer_free,                    (rlr_handle_t buffer)) \
    X(rlr_handle_t, texture_create,                 (uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space, bool generate_mipmaps, int32_t filter_min, int32_t filter_max, int32_t wrap_s, int32_t wrap_t)) \
    X(rlr_handle_t, texture_create_linear,          (uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space, bool generate_mipmaps)) \
    X(rlr_handle_t, texture_create_nearest,         (uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space, bool generate_mipmaps)) \
    X(rlr_handle_t, texture_create_cube_map,        (uint8_t* right, uint8_t* left, uint8_t* top, uint8_t* bottom, uint8_t* front, uint8_t* back, uint32_t width, uint32_t height)) \
    X(void,         texture_bind,                   (rlr_handle_t texture, rlr_backend_texture_type_t type, uint8_t texture_slot)) \
    X(void,         texture_free,                   (rlr_handle_t texture)) \
    X(rlr_handle_t, shader_create,                  (const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size)) \
    X(void,         shader_free,                    (rlr_handle_t shader)) \
    X(void,         shader_use,                     (rlr_handle_t shader)) \
    X(void,         shader_bind_uniform_block_slot, (rlr_handle_t shader, const char* uniform_block_name, uint32_t uniform_block_slot)) \
    X(void,         shader_bind_texture_slot,       (rlr_handle_t shader, const char* texture_var_name, uint32_t texture_slot)) \
    X(void,         uniform_buffer_bind,            (rlr_handle_t buffer, uint32_t uniform_block_slot)) \
    X(void,         clear,                          (uint64_t mask)) \
    X(void,         clear_color,                    (float r, float g, float b, float a)) \
    X(void,         draw_array,                     (uint64_t offset, uint32_t vertex_count)) \
    X(void,         draw_elements,                  (uint64_t offset, uint32_t element_count, rlr_backend_type_t type)) \
    X(void,         viewport_set,                   (int32_t x, int32_t y, int64_t width, int64_t height)) \
    X(void,         depth_testing_set,              (bool enabled)) \
    X(void,         backend_free,                   ())

typedef struct rlr_backend_t {
    #define X(RET, NAME, PARAMS) RET (*NAME)PARAMS;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);