#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../io/str.h"
#include "../rlr.h"
#include "shader.h"

rlr_shader_t* rlr_shader_load(const char* file_vertex, const char* file_fragment) {
    char* string_vertex = rlr_io_str_load_from_file(file_vertex);
    char* string_fragment = rlr_io_str_load_from_file(file_fragment);
    if(!string_vertex) {
        rlr_log_error("could not load vertex shader from \"%s\"", file_vertex);
        goto err;
    }
    if(!string_fragment) {
        rlr_log_error("could not load fragment shader from \"%s\"", file_fragment);
        goto err;
    }

    rlr_shader_t* shader = rlr_shader_load_mem(string_vertex, string_fragment);
    free(string_vertex);
    free(string_fragment);
    return shader;
err:
    free(string_vertex);
    free(string_fragment);
    return NULL;
}

rlr_shader_t* rlr_shader_load_mem(const char* string_vertex, const char* string_fragment) {
    rlr_shader_t* shader = NULL;

    shader = malloc(sizeof(rlr_shader_t));
    if(!shader) {
        rlr_log_error("pointer to the shader handle is null");
        goto err;
    }
    
    char error_str[RLR_SHADER_ERROR_LENGTH];
    shader->shader = rlr_backend()->create_shader(string_vertex, string_fragment, error_str, RLR_SHADER_ERROR_LENGTH);
    if(!shader->shader) {
        rlr_log_error("failed to compile shader:\n%s", error_str);
        goto err;
    }
    return shader;
err:
    rlr_shader_free(shader);
    return NULL;
}

void rlr_shader_bind_ubo_slot(rlr_shader_t* shader, const char* block_name, uint8_t uniform_slot) {
    rlr_backend()->bind_shader_uniform_block(shader->shader, block_name, uniform_slot);
}

void rlr_shader_bind_tex_slot(rlr_shader_t* shader, const char* texture_var_name, uint8_t texture_slot) {
    rlr_backend()->bind_shader_texture_slot(shader->shader, texture_var_name, texture_slot);
}

void rlr_shader_bind(const rlr_shader_t* shader) {
    rlr_backend()->bind_shader(shader->shader);
}

void rlr_shader_free(rlr_shader_t* shader) {
    if(!shader) {
        return;
    }
    rlr_backend()->free_shader(shader->shader);
    free(shader);
}