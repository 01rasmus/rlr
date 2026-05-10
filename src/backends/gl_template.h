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
#include "error.h"
#include "backend.h"

static glad_context_t* gl = NULL;
static rlr_handle_t current_shader = 0;

rlr_handle_t GL_TEMPLATE_PREFIX(create_texture)(uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps) {
    uint32_t texture = 0;
    gl->GenTextures(1, &texture);
    if(texture == 0) {
        return 0;
    }
    gl->BindTexture(GL_TEXTURE_2D, texture);
    gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    if(generate_mipmaps) {
        gl->GenerateMipmap(GL_TEXTURE_2D);
    }
    return (rlr_handle_t)texture;
}

void GL_TEMPLATE_PREFIX(free_texture)(rlr_handle_t handle) {
    uint32_t tex = (uint32_t)handle;
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

rlr_handle_t GL_TEMPLATE_PREFIX(compile_shader)(const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size) {
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

    //finished linking, now we can detach and delete
    gl->DetachShader(program, vid);
    gl->DetachShader(program, fid);

    if(_rlr_shader_program_link_error(program, error, error_size)) {
        goto err;
    }
    gl->DeleteShader(vid);
    gl->DeleteShader(fid);
    return (rlr_handle_t)program;
err:
    gl->DeleteShader(vid);
    gl->DeleteShader(fid);
    return 0;
}

void GL_TEMPLATE_PREFIX(free_shader)(rlr_handle_t shader) {
    gl->DeleteProgram((rlr_handle_t)shader);
}

void GL_TEMPLATE_PREFIX(use_shader)(rlr_handle_t shader) {
    if(shader != current_shader) {
        current_shader = shader;
        gl->UseProgram((uint32_t)shader);
    }
}

void GL_TEMPLATE_PREFIX(shader_bind_uniform_block)(rlr_handle_t shader, const char* uniform_block_name, uint32_t slot) {
    GL_TEMPLATE_PREFIX(use_shader)(shader);
    uint32_t block_index = gl->GetUniformBlockIndex((uint32_t)shader, uniform_block_name);
    if(block_index == GL_INVALID_INDEX) {
        //ERROR
        return;
    }
    gl->UniformBlockBinding((uint32_t)shader, block_index, slot);
}

rlr_handle_t GL_TEMPLATE_PREFIX(create_uniform_buffer)(uint64_t size, void* init_data) {
    uint32_t buffer = 0;
    gl->GenBuffers(1, &buffer);
    gl->BindBuffer(GL_UNIFORM_BUFFER, buffer);
    gl->BufferData(GL_UNIFORM_BUFFER, size, init_data, GL_DYNAMIC_DRAW);
    gl->BindBuffer(GL_UNIFORM_BUFFER, 0);
    return (rlr_handle_t)buffer;
}

void GL_TEMPLATE_PREFIX(update_uniform_buffer)(rlr_handle_t buffer, uint64_t offset, uint64_t size, void* data) {
    gl->BindBuffer(GL_UNIFORM_BUFFER, (uint32_t)buffer);
    gl->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    gl->BindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GL_TEMPLATE_PREFIX(bind_uniform_buffer)(rlr_handle_t buffer, uint32_t slot) {
    gl->BindBufferBase(GL_UNIFORM_BUFFER, slot, (uint32_t)buffer);
}

void GL_TEMPLATE_PREFIX(free_uniform_buffer)(rlr_handle_t buffer) {
    uint32_t handle = (uint32_t)buffer;
    gl->DeleteBuffers(1, &handle);
}

void GL_TEMPLATE_PREFIX(clear)(uint64_t mask) {
    gl->Clear(mask);
}

void GL_TEMPLATE_PREFIX(clear_color)(float r, float g, float b, float a) {
    gl->ClearColor(r, g, b, a);
}

void GL_TEMPLATE_PREFIX(backend_free)() {
    free(gl);
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

    #define X(RET, NAME, PARAMS) backend->NAME = GL_TEMPLATE_PREFIX(NAME);
    RLR_BACKEND_FUNCTIONS(X)
    #undef X

    return backend;
err:
    free(backend);
    GL_TEMPLATE_PREFIX(backend_free)();
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