#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rlr.h"
#include "../error.h"
#include "shader.h"

rlr_res_shader_t* rlr_res_shader_create(const char* vertex_string, const char* fragment_string) {
    rlr_res_shader_t* shader = malloc(sizeof(rlr_res_shader_t));
    if(!shader) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    
    char error_str[RLR_SHADER_ERROR_LENGTH];
    shader->shader = _rlr_raw()->backend->shader_create(vertex_string, fragment_string, error_str, RLR_SHADER_ERROR_LENGTH);
    if(!shader->shader) {
        rlr_error_setf(RLR_ERR_BACKEND_SHADER_COMPILATION, "%s", error_str);
        goto err;
    }
    return shader;
err:
    rlr_res_shader_free(shader);
    return NULL;
}

void rlr_res_shader_bind_uniform_slot(rlr_res_shader_t* shader, const char* block_name, uint8_t uniform_slot) {
    rlr_backend()->shader_bind_uniform_block_slot(shader->shader, block_name, uniform_slot);
}

void rlr_res_shader_bind_texture_slot(rlr_res_shader_t* shader, const char* texture_var_name, uint8_t texture_slot) {
    rlr_backend()->shader_bind_texture_slot(shader->shader, texture_var_name, texture_slot);
}

void rlr_res_shader_use(rlr_res_shader_t* shader) {
    _rlr_raw()->backend->shader_use(shader->shader);
}

void rlr_res_shader_free(rlr_res_shader_t* shader) {
    if(shader) {
        _rlr_raw()->backend->shader_free(shader->shader);
    }
    free(shader);
}