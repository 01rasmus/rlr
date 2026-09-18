#pragma once
#include <stdint.h>
#include "../def.h"

typedef struct rlr_res_cube_map_t rlr_res_cube_map_t;

rlr_res_cube_map_t* rlr_res_cube_map_load(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back);
rlr_res_cube_map_t* rlr_res_cube_map_default();
void rlr_res_cube_map_bind(const rlr_res_cube_map_t* cm, uint8_t texture_slot);
void rlr_res_cube_map_free(rlr_res_cube_map_t* cm);