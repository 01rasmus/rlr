#pragma once

typedef struct rlr_res_cube_map_t {
    uint64_t texture;
} rlr_res_cube_map_t;

rlr_res_cube_map_t* rlr_res_cube_map_load(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back);
void rlr_res_cube_map_bind(rlr_res_cube_map_t* cm, uint8_t texture_slot);
void rlr_res_cube_map_free(rlr_res_cube_map_t* cm);