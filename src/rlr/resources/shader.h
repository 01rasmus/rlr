#pragma once
#include <stdint.h>

typedef struct rlr_res_shader_t rlr_res_shader_t;

rlr_res_shader_t* rlr_res_shader_create(const char* vertex, const char* fragment);
void rlr_res_shader_bind_uniform_slot(rlr_res_shader_t* shader, const char* block_var_name, uint8_t uniform_slot);
void rlr_res_shader_bind_texture_slot(rlr_res_shader_t* shader, const char* texture_var_name, uint8_t uniform_slot);
void rlr_res_shader_bind(const rlr_res_shader_t* shader);
void rlr_res_shader_free(rlr_res_shader_t* shader);