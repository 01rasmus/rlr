#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glad.h>
#include "../error.h"
#include "shader.h"

const char shader_vertex_default[] = RLR_SHADER_INLINE(
    layout (location = 0) in vec3 a_pos;

    void main()
    {
        gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
    }
);

const char shader_fragment_default[] = RLR_SHADER_INLINE(
    out vec4 frag_color;

    void main()
    {
        frag_color = vec4(1.0, 0.5, 0.2, 1.0);
    }
);

rlr_shader_t* rlr_shader_load(const char* vertex_string, const char* fragment_string) {
    rlr_shader_t* shader = malloc(sizeof(rlr_shader_t));
    if(!shader) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    const char* vertex = (vertex_string == NULL) ? shader_vertex_default : vertex_string;
    const char* fragment = (fragment_string == NULL) ? shader_fragment_default : fragment_string;

    char error_str[RLR_SHADER_ERROR_LENGTH];
    uint32_t vid = 0;
    uint32_t fid = 0;

    int32_t vertex_length[1] = { strlen(vertex) };
    int32_t fragment_length[1] = { strlen(fragment) };
    const char* vertex_strings[1] = { vertex };
    const char* fragment_strings[1] = { fragment };

    //compile vertex shader
    vid = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vid, 1, vertex_strings, vertex_length);
    glCompileShader(vid);
    if(_rlr_shader_compilation_error(vid, error_str, RLR_SHADER_ERROR_LENGTH)) {
        rlr_error_setf(RLR_ERR_OPENGL_VERTEX_SHADER_COMPILATION, "%s", error_str);
        goto err;
    }

    //compile fragment shader
    fid = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fid, 1, fragment_strings, fragment_length);
    glCompileShader(fid);
    if(_rlr_shader_compilation_error(fid, error_str, RLR_SHADER_ERROR_LENGTH)) {
        rlr_error_setf(RLR_ERR_OPENGL_FRAGMENT_SHADER_COMPILATION, "%s", error_str);
        goto err;
    }

    //linking to the program
    shader->program = glCreateProgram();
    glAttachShader(shader->program, vid);
    glAttachShader(shader->program, fid);
    glLinkProgram(shader->program);

    //finished linking, now we can detach and delete
    glDetachShader(shader->program, vid);
    glDetachShader(shader->program, fid);

    if(_rlr_shader_program_link_error(shader->program, error_str, RLR_SHADER_ERROR_LENGTH)) {
        rlr_error_setf(RLR_ERR_OPENGL_PROGRAM_LINKING, "%s", error_str);
        goto err;
    }

    glDeleteShader(vid);
    glDeleteShader(fid);
err:
    glDeleteShader(vid);
    glDeleteShader(fid);
    rlr_shader_free(shader);
    return NULL;
}

uint32_t rlr_shader_get_uniform_location(rlr_shader_t* shader, const char* name) {
    glUseProgram(shader->program);
    return glGetUniformLocation(shader->program, name);
}

void rlr_shader_set_uniform(rlr_shader_t* shader, uint32_t location, void* value, rlr_shader_uniform_type type) {
    glUseProgram(shader->program);
    switch(type) {
        case RLR_SHADER_UNIFORM_BOOL: {
            glUniform1i(location, *(int32_t*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_INT: {
            glUniform1i(location, *(int32_t*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_FLOAT: {
            glUniform1f(location, *(float*)value);
            break;
        }
        case RLR_SHADER_UNIFORM_VEC3: {
            float* vec3 = (float*)value;
            glUniform3f(location, vec3[0], vec3[1], vec3[2]);
            break;
        }
        case RLR_SHADER_UNIFORM_VEC2: {
            float* vec3 = (float*)value;
            glUniform2f(location, vec3[0], vec3[1]);
            break;
        }
        case RLR_SHADER_UNIFORM_MAT4X4: {
            float* mat4x4 = (float*)value;
            glUniformMatrix4fv(location, 1, GL_FALSE, mat4x4);
            break;
        }
    } 
}

void rlr_shader_free(rlr_shader_t* shader) {
    if(shader) {
        glDeleteProgram(shader->program);
    }
    free(shader);
}

bool _rlr_shader_compilation_error(uint32_t shader_id, char* error_str, size_t error_str_size) {
    int32_t status = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE) {
        return false;
    }

    int32_t length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
    if(length > 0) {
        glGetShaderInfoLog(shader_id, error_str_size, NULL, error_str);
    }
    return true;
}

bool _rlr_shader_program_link_error(uint32_t program_id, char* error_str, size_t error_str_size) {
    int32_t status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) {
        return false;
    }

    int32_t length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        glGetProgramInfoLog(program_id, error_str_size, NULL, error_str);
    }
    return true;
}