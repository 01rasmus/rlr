#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef void (*rlr_backend_proc_t)(void);
typedef rlr_backend_proc_t (*rlr_backend_loader_t)(const char*);

typedef enum rlr_backend_clear_flag_t {
    RLR_BACKEND_CLEAR_BIT_COLOR   = 0x00004000,
    RLR_BACKEND_CLEAR_BIT_DEPTH   = 0x00000100,
    RLR_BACKEND_CLEAR_BIT_STENCIL = 0x00000400
} rlr_backend_clear_flag_t;

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

typedef enum rlr_backend_vertex_array_attrib_type_t {
    RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX = 0,
    RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE = 1,
} rlr_backend_vertex_array_attrib_type_t;

typedef enum rlr_backend_stencil_func_t {
    RLR_BACKEND_STENCIL_FUNC_NEVER       = 0x0200,
    RLR_BACKEND_STENCIL_FUNC_LESS        = 0x0201,
    RLR_BACKEND_STENCIL_FUNC_LEQUAL      = 0x0203,
    RLR_BACKEND_STENCIL_FUNC_GREATER     = 0x0204,
    RLR_BACKEND_STENCIL_FUNC_GEQUAL      = 0x0206,
    RLR_BACKEND_STENCIL_FUNC_EQUAL       = 0x0202,
    RLR_BACKEND_STENCIL_FUNC_NOTEQUAL    = 0x0205,
    RLR_BACKEND_STENCIL_FUNC_ALWAYS      = 0x0207,
} rlr_backend_stencil_func_t;

typedef enum rlr_backend_stencil_op_t {
    RLR_BACKEND_STENCIL_OP_KEEP         = 0x1E00,
    RLR_BACKEND_STENCIL_OP_ZERO         = 0,
    RLR_BACKEND_STENCIL_OP_REPLACE      = 0x1E01,
    RLR_BACKEND_STENCIL_OP_INCR         = 0x1E02,
    RLR_BACKEND_STENCIL_OP_INCR_WRAP    = 0x8507,
    RLR_BACKEND_STENCIL_OP_DECR         = 0x1E03,
    RLR_BACKEND_STENCIL_OP_DECR_WRAP    = 0x8508,
    RLR_BACKEND_STENCIL_OP_INVERT       = 0x150A,
} rlr_backend_stencil_op_t;

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
    X(uint64_t,     create_vertex_array,            ()) \
    X(uint64_t,     create_texture,                 (uint8_t* color_data, uint32_t width, uint32_t height, int32_t channels, bool use_srgb_color_space, int32_t filter_min, int32_t filter_mag, int32_t wrap_s, int32_t wrap_t)) \
    X(uint64_t,     create_buffer,                  ()) \
    X(uint64_t,     create_linear_texture,          (uint8_t* color_data, uint32_t width, uint32_t height, int32_t channels, bool use_srgb_color_space)) \
    X(uint64_t,     create_linear_mipmap_texture,   (uint8_t* color_data, uint32_t width, uint32_t height, int32_t channels, bool use_srgb_color_space)) \
    X(uint64_t,     create_nearest_texture,         (uint8_t* color_data, uint32_t width, uint32_t height, int32_t channels, bool use_srgb_color_space)) \
    X(uint64_t,     create_cube_map_texture,        (uint8_t* right, uint8_t* left, uint8_t* top, uint8_t* bottom, uint8_t* front, uint8_t* back, uint32_t width, uint32_t height, int32_t channels)) \
    X(uint64_t,     create_shader,                  (const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size)) \
    X(void,         bind_vertex_array,              (uint64_t vao)) \
    X(void,         bind_buffer,                    (uint64_t buffer, rlr_backend_buffer_target_t target)) \
    X(void,         bind_texture,                   (uint64_t texture, rlr_backend_texture_type_t type, uint8_t texture_slot)) \
    X(void,         bind_shader,                    (uint64_t shader)) \
    X(void,         bind_shader_uniform_block,      (uint64_t shader, const char* uniform_block_name, uint32_t uniform_block_slot)) \
    X(void,         bind_shader_texture_slot,       (uint64_t shader, const char* texture_var_name, uint32_t texture_slot)) \
    X(void,         bind_uniform_buffer,            (uint64_t ubo, uint32_t uniform_block_slot)) \
    X(void,         set_vertex_array_attrib,        (rlr_backend_vertex_array_attrib_type_t attrib_type, uint32_t index, uint8_t count, rlr_backend_type_t type, bool normalized, uint32_t stride, uintptr_t vertex_offset)) \
    X(void,         set_vertex_array_attribi,       (rlr_backend_vertex_array_attrib_type_t attrib_type, uint32_t index, uint8_t count, rlr_backend_type_t type, uint32_t stride, uintptr_t vertex_offset)) \
    X(void,         set_viewport,                   (int32_t x, int32_t y, int32_t width, int32_t height)) \
    X(void,         set_scissor_test,               (bool use_scissor_test)) \
    X(void,         set_scissor,                    (int32_t x, int32_t y, int32_t width, int32_t height)) \
    X(void,         set_clear_color,                (float r, float g, float b, float a)) \
    X(void,         set_clear_stencil,              (int32_t stencil)) \
    X(void,         set_color_mask,                 (bool r, bool g, bool b, bool a)) \
    X(void,         set_depth_test,                 (bool use_depth_test)) \
    X(void,         set_depth_mask,                 (bool z)) \
    X(void,         set_stencil_test,               (bool use_stencil_test)) \
    X(void,         set_stencil_mask,               (uint8_t mask)) \
    X(void,         set_stencil_func,               (rlr_backend_stencil_func_t func, uint8_t ref, uint8_t mask)) \
    X(void,         set_stencil_op,                 (rlr_backend_stencil_op_t sfail, rlr_backend_stencil_op_t dpfail, rlr_backend_stencil_op_t dppass)) \
    X(void,         set_blending,                   (bool use_blending)) \
    X(void,         draw_arrays,                    (uint64_t offset, uint32_t vertex_count)) \
    X(void,         draw_arrays_instanced,          (uint64_t offset, uint32_t vertex_count, uint32_t instance_count)) \
    X(void,         draw_elements,                  (uint64_t offset, uint32_t element_count, rlr_backend_type_t type)) \
    X(void,         clear,                          (rlr_backend_clear_flag_t clear_flags)) \
    X(void,         update_buffer,                  (rlr_backend_buffer_target_t target, uint64_t size, const void* data, rlr_backend_buffer_usage_t update_type)) \
    X(uint64_t,     get_draw_call_count,            ()) \
    X(void,         reset_statistics,               ()) \
    X(void,         free_vertex_array,              (uint64_t vao)) \
    X(void,         free_buffer,                    (uint64_t buffer)) \
    X(void,         free_texture,                   (uint64_t texture)) \
    X(void,         free_shader,                    (uint64_t shader)) \
    X(void,         free_backend,                   ())

typedef struct rlr_backend_t {
    #define X(RET, NAME, PARAMS) RET (*NAME)PARAMS;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);