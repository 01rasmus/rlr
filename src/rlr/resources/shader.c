#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../error.h"
#include "../rlr.h"
#include "shader.h"

rlr_res_shader_t* rlr_res_shader_create(const char* vertex_string, const char* fragment_string) {
    rlr_res_shader_t* shader = NULL;

    shader = malloc(sizeof(rlr_res_shader_t));
    if(!shader) {
        rlr_log_error("pointer to the shader handle is null");
        goto err;
    }
    
    char error_str[RLR_SHADER_ERROR_LENGTH];
    shader->shader = rlr_backend()->create_shader(vertex_string, fragment_string, error_str, RLR_SHADER_ERROR_LENGTH);
    if(!shader->shader) {
        rlr_log_error("failed to compile shader:\n%s", error_str);
        goto err;
    }
    return shader;
err:
    rlr_res_shader_free(shader);
    return NULL;
}

void rlr_res_shader_bind_uniform_slot(rlr_res_shader_t* shader, const char* block_name, uint8_t uniform_slot) {
    rlr_backend()->bind_shader_uniform_block(shader->shader, block_name, uniform_slot);
}

void rlr_res_shader_bind_texture_slot(rlr_res_shader_t* shader, const char* texture_var_name, uint8_t texture_slot) {
    rlr_backend()->bind_shader_texture_slot(shader->shader, texture_var_name, texture_slot);
}

void rlr_res_shader_bind(const rlr_res_shader_t* shader) {
    rlr_backend()->bind_shader(shader->shader);
}

void rlr_res_shader_free(rlr_res_shader_t* shader) {
    if(!shader) {
        return;
    }
    rlr_backend()->free_shader(shader->shader);
    free(shader);
}