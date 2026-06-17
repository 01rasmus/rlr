/*
    Since OpenGL 3.3 and OpenGL ES 3.0
    are very similar, this file is meant
    to act as a template for the respective
    implementations.

    To implement desktop OpenGL, define
    #define GL_IMPLEMENTATION_TEMPLATE_GL

    and to use the OpenGL ES, define
    #define GL_IMPLEMENTATION_TEMPLATE_GLES

    After this, functions to get the backend
    context will be implemented.

    Each implementation can only be used inside
    one C file, similar to a single header library.

    All the functions inside will be prefixed 
    depending on the implementation
*/

#if !defined(GL_IMPLEMENTATION_TEMPLATE_GL) && !defined(GL_IMPLEMENTATION_TEMPLATE_GLES)
#error "GL_IMPLEMENTATION_TEMPLATE_GL or GL_IMPLEMENTATION_TEMPLATE_GLES was not defined before include"
#endif

#if defined(GL_IMPLEMENTATION_TEMPLATE_GL) && defined(GL_IMPLEMENTATION_TEMPLATE_GLES)
#error "Both GL_IMPLEMENTATION_TEMPLATE_GL and GL_IMPLEMENTATION_TEMPLATE_GLES can not be defined at the same time"
#endif

/* OpenGL */
#ifdef GL_IMPLEMENTATION_TEMPLATE_GL
#define GL_TEMPLATE_PREFIX(FUNC)    gl3_##FUNC
#define GL_TEMPLATE_ENTRY           rlr_backend_gl3
#define GL_LOADER_FUNCTION          gladLoadGLContext

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
typedef GladGLContext glad_context_t;
#endif

/* OpenGL ES*/
#ifdef GL_IMPLEMENTATION_TEMPLATE_GLES
#define GL_TEMPLATE_PREFIX(FUNC)    gles3_##FUNC
#define GL_TEMPLATE_ENTRY           rlr_backend_gles3
#define GL_LOADER_FUNCTION          gladLoadGLES2Context

#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles2.h>
#undef GLAD_GLES2_IMPLEMENTATION
typedef GladGLES2Context glad_context_t;
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stb_image_resize2.h>
#include "error.h"
#include "backend.h"

#define TEXTURE_RESIZE_BUFFER   (4096 * 4096 * 4)

static uint8_t* texture_resize_buffer = NULL;
static glad_context_t* gl = NULL;
static uint64_t statistic_draw_call_count = 0;

static float current_height = 0.0;
static rlr_handle_t current_textures[16] = {0};
static rlr_handle_t current_shader = 0;

static bool texture_filter_uses_mipmaps(int32_t filter) {
    switch(filter) {
        case GL_NEAREST_MIPMAP_NEAREST:
        case GL_LINEAR_MIPMAP_NEAREST:
        case GL_NEAREST_MIPMAP_LINEAR:
        case GL_LINEAR_MIPMAP_LINEAR:
            return true;
        default:
            return false;
    }
}

rlr_handle_t GL_TEMPLATE_PREFIX(texture_create)(uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space, int32_t filter_min, int32_t filter_mag, int32_t wrap_s, int32_t wrap_t) {
    uint32_t texture = 0;
    gl->GenTextures(1, &texture);
    if(texture == 0) {
        return 0;
    }
    gl->BindTexture(GL_TEXTURE_2D, texture);

    int32_t internal_format = use_srgb_color_space ? GL_SRGB8_ALPHA8 : GL_RGBA8;

    if(use_srgb_color_space) {
        int32_t output_w = 64;
        int32_t output_h = 64;
        rgba = stbir_resize_uint8_srgb(rgba, width, height, 0, texture_resize_buffer, output_w, output_h, 0, STBIR_RGBA);
        gl->TexImage2D(GL_TEXTURE_2D, 0, internal_format, output_w, output_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    } else {
        gl->TexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    }

    if(texture_filter_uses_mipmaps(filter_min)) {
        gl->GenerateMipmap(GL_TEXTURE_2D);
    }

    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    return (rlr_handle_t)texture;
}

rlr_handle_t GL_TEMPLATE_PREFIX(texture_create_linear)(uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space) {
    return GL_TEMPLATE_PREFIX(texture_create)(rgba, width, height, use_srgb_color_space, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

rlr_handle_t GL_TEMPLATE_PREFIX(texture_create_nearest)(uint8_t* rgba, uint32_t width, uint32_t height, bool use_srgb_color_space) {
    return GL_TEMPLATE_PREFIX(texture_create)(rgba, width, height, use_srgb_color_space, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

rlr_handle_t GL_TEMPLATE_PREFIX(texture_create_cube_map)(uint8_t* right, uint8_t* left, uint8_t* top, uint8_t* bottom, uint8_t* front, uint8_t* back, uint32_t width, uint32_t height) {
    uint32_t texture = 0;
    gl->GenTextures(1, &texture);
    if(texture == 0) {
        return 0;
    }
    gl->BindTexture(GL_TEXTURE_CUBE_MAP, texture);

    const uint8_t* texture_data[6] = {
        right,
        left,
        top,
        bottom,
        front,
        back
    };

    for(int64_t i = 0; i < sizeof(texture_data) / sizeof(texture_data[0]); i++) {
        if(texture_data[i]) {
            int32_t output_w = 64;
            int32_t output_h = 64;
            stbir_resize_uint8_linear(texture_data[i], width, height, 0, texture_resize_buffer, output_w, output_h, 0, STBIR_RGBA);
            gl->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, output_w, output_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_resize_buffer);
        }
    }

    gl->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texture;
}

void GL_TEMPLATE_PREFIX(texture_bind)(rlr_handle_t texture, rlr_backend_texture_type_t type, uint8_t texture_slot) {
    if(current_textures[texture_slot] == texture) {
        return;
    }
    gl->ActiveTexture(GL_TEXTURE0 + texture_slot);
    gl->BindTexture(type, texture);
    current_textures[texture_slot] = texture;
}

void GL_TEMPLATE_PREFIX(texture_free)(rlr_handle_t texture) {
    uint32_t tex = (uint32_t)texture;
    gl->DeleteTextures(1, &tex);
}

static bool _rlr_shader_compilation_error(uint32_t shader_id, char* error_str, size_t error_str_size) {
    int32_t status = 0;
    gl->GetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE) {
        return false;
    }

    int32_t length = 0;
    gl->GetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
    if(length > 0) {
        gl->GetShaderInfoLog(shader_id, error_str_size, NULL, error_str);
    }
    return true;
}

static bool _rlr_shader_program_link_error(uint32_t program_id, char* error_str, size_t error_str_size) {
    int32_t status = 0;
    gl->GetProgramiv(program_id, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) {
        return false;
    }

    int32_t length = 0;
    gl->GetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        gl->GetProgramInfoLog(program_id, error_str_size, NULL, error_str);
    }
    return true;
}

rlr_handle_t GL_TEMPLATE_PREFIX(shader_create)(const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size) {
    uint32_t vid = 0;
    uint32_t fid = 0;

    int32_t vertex_length[1] = { strlen(vertex_shader) };
    int32_t fragment_length[1] = { strlen(fragment_shader) };
    const char* vertex_strings[1] = { vertex_shader };
    const char* fragment_strings[1] = { fragment_shader };

    //compile vertex shader
    vid = gl->CreateShader(GL_VERTEX_SHADER);
    gl->ShaderSource(vid, 1, vertex_strings, vertex_length);
    gl->CompileShader(vid);
    if(_rlr_shader_compilation_error(vid, error, error_size)) {
        goto err;
    }

    //compile fragment shader
    fid = gl->CreateShader(GL_FRAGMENT_SHADER);
    gl->ShaderSource(fid, 1, fragment_strings, fragment_length);
    gl->CompileShader(fid);
    if(_rlr_shader_compilation_error(fid, error, error_size)) {
        goto err;
    }

    //linking to the program
    uint32_t program = gl->CreateProgram();
    gl->AttachShader(program, vid);
    gl->AttachShader(program, fid);
    gl->LinkProgram(program);
    if(_rlr_shader_program_link_error(program, error, error_size)) {
        goto err;
    }

    //finished linking, now we can detach and delete
    gl->DetachShader(program, vid);
    gl->DetachShader(program, fid);
    gl->DeleteShader(vid);
    gl->DeleteShader(fid);
    return (rlr_handle_t)program;
err:
    gl->DeleteShader(vid);
    gl->DeleteShader(fid);
    return 0;
}

void GL_TEMPLATE_PREFIX(shader_free)(rlr_handle_t shader) {
    gl->DeleteProgram((rlr_handle_t)shader);
}

void GL_TEMPLATE_PREFIX(shader_use)(rlr_handle_t shader) {
    if(shader == current_shader) {
        return;
    }
    current_shader = shader;
    gl->UseProgram((uint32_t)shader);
}

void GL_TEMPLATE_PREFIX(shader_bind_uniform_block_slot)(rlr_handle_t shader, const char* uniform_block_name, uint32_t slot) {
    GL_TEMPLATE_PREFIX(shader_use)(shader);
    int32_t block_index = gl->GetUniformBlockIndex((uint32_t)shader, uniform_block_name);
    if(block_index == GL_INVALID_INDEX) {
        //ERROR
        return;
    }
    gl->UniformBlockBinding((uint32_t)shader, block_index, slot);
}

void GL_TEMPLATE_PREFIX(shader_bind_texture_slot)(rlr_handle_t shader, const char* texture_var_name, uint32_t texture_slot) {
    GL_TEMPLATE_PREFIX(shader_use)(shader);
    int32_t location = gl->GetUniformLocation((uint32_t)shader, texture_var_name);
    if(location == GL_INVALID_INDEX) {
        //ERROR
        return;
    }
    gl->Uniform1i(location, texture_slot);
}

void GL_TEMPLATE_PREFIX(uniform_buffer_bind)(rlr_handle_t buffer, uint32_t slot) {
    gl->BindBufferBase(GL_UNIFORM_BUFFER, slot, (uint32_t)buffer);
}

void GL_TEMPLATE_PREFIX(clear)(uint64_t mask) {
    gl->Clear(mask);
}

void GL_TEMPLATE_PREFIX(clear_color)(float r, float g, float b, float a) {
    gl->ClearColor(r, g, b, a);
}

rlr_handle_t GL_TEMPLATE_PREFIX(vertex_array_create)() {
    rlr_handle_t handle = 0;
    gl->GenVertexArrays(1, (uint32_t*)&handle);
    return handle;
}

void GL_TEMPLATE_PREFIX(vertex_array_bind)(rlr_handle_t vao) {
    gl->BindVertexArray((uint32_t)vao);
}

void GL_TEMPLATE_PREFIX(vertex_array_attrib_set)(rlr_backen_vertex_array_attrib_type_t attrib_type, uint32_t index, uint8_t count, rlr_backend_type_t type, bool normalized, uint32_t stride, uintptr_t vertex_offset) {
    gl->VertexAttribPointer(index, count, type, normalized, stride, (void*)vertex_offset);
    gl->EnableVertexAttribArray(index);
    gl->VertexAttribDivisor(index, attrib_type);
}

void GL_TEMPLATE_PREFIX(vertex_array_free)(rlr_handle_t vao) {
    rlr_handle_t handles[] = { vao };
    gl->DeleteVertexArrays(1, (uint32_t*)handles);
}

rlr_handle_t GL_TEMPLATE_PREFIX(buffer_create)() {
    rlr_handle_t handle = 0;
    gl->GenBuffers(1, (uint32_t*)&handle);
    return handle;
}

void GL_TEMPLATE_PREFIX(buffer_bind)(rlr_handle_t buffer, rlr_backend_buffer_target_t target) {
    gl->BindBuffer(target, buffer);
}

void GL_TEMPLATE_PREFIX(buffer_update)(rlr_backend_buffer_target_t target, uint64_t size, const void* data, rlr_backend_buffer_usage_t usage_type) {
    gl->BufferData(target, size, data, usage_type);
}

void GL_TEMPLATE_PREFIX(buffer_free)(rlr_handle_t buffer) {
    rlr_handle_t handles[1] = { buffer };
    gl->DeleteBuffers(1, (uint32_t*)handles);
}

void GL_TEMPLATE_PREFIX(draw_array)(uint64_t offset, uint32_t vertex_count) {
    gl->DrawArrays(GL_TRIANGLES, offset, vertex_count);
    statistic_draw_call_count++;
}

void GL_TEMPLATE_PREFIX(draw_array_instanced)(uint64_t offset, uint32_t vertex_count, uint32_t instance_count) {
    gl->DrawArraysInstanced(GL_TRIANGLES, offset, vertex_count, instance_count);
    statistic_draw_call_count++;
}

void GL_TEMPLATE_PREFIX(draw_elements)(uint64_t offset, uint32_t index_count, rlr_backend_type_t type) {
    gl->DrawElements(GL_TRIANGLES, index_count, type, (void*)offset);
    statistic_draw_call_count++;
}

void GL_TEMPLATE_PREFIX(viewport_set)(int32_t x, int32_t y, int64_t width, int64_t height) {
    gl->Viewport(x, y, width, height);
    current_height = height;
}

void GL_TEMPLATE_PREFIX(depth_testing_set)(bool enabled) {
    if(enabled) {
        gl->Enable(GL_DEPTH_TEST);
        gl->DepthFunc(GL_LESS);
    } else {
        gl->Disable(GL_DEPTH_TEST);
    }
}

void GL_TEMPLATE_PREFIX(blending_set)(bool enabled) {
    if(enabled) {
        gl->Enable(GL_BLEND);
        gl->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        gl->Disable(GL_BLEND);
    }
}

void GL_TEMPLATE_PREFIX(scissor_set)(float x, float y, float width, float height) {
    gl->Enable(GL_SCISSOR_TEST);
    gl->Scissor(x, current_height - y - height, width, height);
}

void GL_TEMPLATE_PREFIX(scissor_disable)() {
    gl->Disable(GL_SCISSOR_TEST);
}

uint64_t GL_TEMPLATE_PREFIX(statistics_draw_calls)() {
    return statistic_draw_call_count;
}

void GL_TEMPLATE_PREFIX(statistics_reset)() {
    statistic_draw_call_count = 0;
}


void GL_TEMPLATE_PREFIX(backend_free)() {
    free(gl);
    free(texture_resize_buffer);
    gl = NULL;
}

rlr_backend_t* GL_TEMPLATE_ENTRY(rlr_backend_loader_t proc_loader) {
    rlr_backend_t* backend = NULL;
    gl = malloc(sizeof(glad_context_t));
    if(!gl) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    if(GL_LOADER_FUNCTION(gl, proc_loader) == 0) {
        goto err;
    }

    backend = malloc(sizeof(rlr_backend_t));
    if(!backend) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    memset(backend, 0, sizeof(rlr_backend_t));

    texture_resize_buffer = malloc(TEXTURE_RESIZE_BUFFER);
    if(!texture_resize_buffer) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    #define X(RET, NAME, PARAMS) backend->NAME = GL_TEMPLATE_PREFIX(NAME);
    RLR_BACKEND_FUNCTIONS(X)
    #undef X

    #ifdef GL_IMPLEMENTATION_TEMPLATE_GL
        gl->Disable(GL_MULTISAMPLE);
    #endif

    gl->Enable(GL_CULL_FACE);
    gl->CullFace(GL_BACK);
    statistic_draw_call_count = 0;
    return backend;
err:
    GL_TEMPLATE_PREFIX(backend_free)();
    free(backend);
    return NULL;
}

/* undefines */
#undef GL_TEMPLATE_PREFIX
#undef GL_TEMPLATE_ENTRY
#undef GL_LOADER_FUNCTION
#ifdef GL_IMPLEMENTATION_TEMPLATE_GL
#undef GL_IMPLEMENTATION_TEMPLATE_GL
#endif
#ifdef GL_IMPLEMENTATION_TEMPLATE_GLES
#undef GL_IMPLEMENTATION_TEMPLATE_GLES
#endif