#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../internal/impl.h"
#include "../error.h"
#include "../rlr.h"
#include "shader.h"

rlr_res_t rlr_res_shader_create(const char* vertex_string, const char* fragment_string) {
    rlr_res_t id = RLR_NULL;
    rlr_res_shader_t* shader = NULL;

    id = rlr_mem_man_allocate_res_shader(rlr_mem_man(), (rlr_res_shader_t){0});
    if(id == RLR_NULL) {
        rlr_log_error("could not allocate rlr handle for shader");
        goto err;
    }

    shader = rlr_mem_man_get_res_shader(rlr_mem_man(), id);
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
    return id;
err:
    rlr_res_shader_free(id);
    return RLR_NULL;
}

void rlr_res_shader_bind_uniform_slot(rlr_res_t id, const char* block_name, uint8_t uniform_slot) {
    rlr_res_shader_t* shader = rlr_mem_man_get_res_shader(rlr_mem_man(), id);
    rlr_backend()->bind_shader_uniform_block(shader->shader, block_name, uniform_slot);
}

void rlr_res_shader_bind_texture_slot(rlr_res_t id, const char* texture_var_name, uint8_t texture_slot) {
    rlr_res_shader_t* shader = rlr_mem_man_get_res_shader(rlr_mem_man(), id);
    rlr_backend()->bind_shader_texture_slot(shader->shader, texture_var_name, texture_slot);
}

void rlr_res_shader_bind(rlr_res_t id) {
    rlr_res_shader_t* shader = rlr_mem_man_get_res_shader(rlr_mem_man(), id);
    rlr_backend()->bind_shader(shader->shader);
}

void rlr_res_shader_free(rlr_res_t id) {
    if(id == RLR_NULL) {
        return;
    }
    rlr_res_shader_t* shader = rlr_mem_man_get_res_shader(rlr_mem_man(), id);
    if(shader) {
        rlr_backend()->free_shader(shader->shader);
    }
    rlr_mem_man_free_res_shader(rlr_mem_man(), id);
}