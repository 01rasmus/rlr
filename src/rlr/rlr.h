#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "math/quat.h"
#include "math/vec.h"
#include "def.h"

typedef struct rlr_statistics_t {
    uint64_t draw_call_count;
    uint64_t frame_count;
    double time;
} rlr_statistics_t;

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, rlr_init_flags_t flags);

bool rlr_update();
void rlr_free();

void rlr_set_cube_map(rlr_res_t cube_map_id);
rlr_vec2_t rlr_get_framebuffer_size();
rlr_vec2_t rlr_get_mouse_position();

//callbacks
void rlr_set_user(void* user);
void rlr_set_mouse_input_callback(rlr_input_mouse_callback_t func);
void rlr_set_key_input_callback(rlr_input_key_callback_t func);

//intersection checks
bool rlr_screen_pos_to_ground(rlr_vec2_t mouse_pos, rlr_vec3_t* out_position);

//camera
void rlr_set_camera(rlr_vec3_t pos, rlr_quat_t rotation);
rlr_vec3_t rlr_get_camera_pos();
rlr_quat_t rlr_get_camera_rot();

//backend information
const char* rlr_get_backend_implementation();
const char* rlr_get_backend_context();
const char* rlr_get_gpu_name();


//statistics
rlr_statistics_t* rlr_get_total_statistics();
rlr_statistics_t* rlr_get_statistics();