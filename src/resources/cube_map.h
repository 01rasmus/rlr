#pragma once
#include "backends/backend.h"

typedef struct rlr_cube_map_t {
    rlr_handle_t texture;
} rlr_cube_map_t;

rlr_cube_map_t* rlr_cube_map_load(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back);
void rlr_cube_map_bind(rlr_cube_map_t* cm, uint8_t texture_slot);
void rlr_cube_map_free(rlr_cube_map_t* cm);