#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include "backend.h"

static GladGLContext* gl = NULL;
static uint64_t current_shader = 0;

uint64_t gl_create_texture(uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps) {
    uint64_t texture = 0;
    gl->GenTextures(1, &texture);
    if(texture == 0) {
        return 0;
    }
    gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    if(generate_mipmaps) {
        gl->GenerateMipmap(GL_TEXTURE_2D);
    }
    return texture;
}

void gl_free_texture(uint64_t handle) {
    gl->DeleteTextures(1, &handle);
}

bool _rlr_shader_compilation_error(uint64_t shader_id, char* error_str, size_t error_str_size) {
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

bool _rlr_shader_program_link_error(uint64_t program_id, char* error_str, size_t error_str_size) {
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

uint64_t gl_compile_shader(const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size) {
    uint64_t vid = 0;
    uint64_t fid = 0;

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
    uint64_t program = gl->CreateProgram();
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
    return program;
err:
    gl->DeleteShader(vid);
    gl->DeleteShader(fid);
    return 0;
}

void gl_free_shader(uint64_t shader) {
    gl->DeleteProgram(shader);
}

void gl_use_shader(uint64_t shader) {
    if(shader != current_shader) {
        current_shader = shader;
        gl->UseProgram(shader);
    }
}

uint64_t gl_shader_uniform_location(uint64_t shader, const char* name) {
    gl_use_shader(shader);
    return gl->GetUniformLocation(shader, name);
}

void gl_shader_uniform_set(uint64_t shader, uint64_t location, void* value, rlr_shader_uniform_type type) {
    gl_use_shader(shader);
    switch(type) {
        case RLR_SHADER_UNIFORM_BOOL: {
            gl->Uniform1i(location, *(int32_t*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_INT: {
            gl->Uniform1i(location, *(int32_t*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_FLOAT: {
            gl->Uniform1f(location, *(float*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_VEC3: {
            float* vec3 = (float*)value;
            gl->Uniform3f(location, vec3[0], vec3[1], vec3[2]);
            break;
        }
        case RLR_SHADER_UNIFORM_VEC2: {
            float* vec3 = (float*)value;
            gl->Uniform2f(location, vec3[0], vec3[1]);
            break;
        }
        case RLR_SHADER_UNIFORM_MAT4X4: {
            float* mat4x4 = (float*)value;
            gl->UniformMatrix4fv(location, 1, GL_FALSE, mat4x4);
            break;
        }
    }
}

void gl_free() {
    free(gl);
    gl = NULL;
}

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader) {
    rlr_backend_t* backend = NULL;
    gl = malloc(sizeof(GladGLContext));
    if(!gl) {
        return NULL;
    }
    if(gladLoadGLContext(gl, proc_loader) == 0) {
        return NULL;
    }
    if(gl == NULL) {
        return NULL;
    }

    backend = malloc(sizeof(rlr_backend_t));
    if(!backend) {
        return NULL;
    }
    memset(backend, 0, sizeof(rlr_backend_t));

    backend->create_texture = gl_create_texture;
    backend->free_texture = gl_free_texture;
    backend->compile_shader = gl_compile_shader;
    backend->free_shader = gl_free_shader;
    backend->use_shader = gl_use_shader;
    backend->shader_uniform_location = gl_shader_uniform_location;
    backend->shader_uniform_set = gl_shader_uniform_set;
    backend->free = gl_free;

    return backend;
err:
    free(backend);
    gl_free();
    return NULL;
}