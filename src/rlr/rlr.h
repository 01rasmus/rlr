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
void rlr_init_ext(const char* title, uint32_t window_width, uint32_t window_height, rlr_init_flags_t flags, rlr_log_callback_t log_callback, bool log_callback_add_new_line);

bool rlr_update();
void rlr_free();

void rlr_set_clear_color(float r, float g, float b, float a);
void rlr_set_cube_map(rlr_res_cube_map_t* cube_map_id);
rlr_vec2_t rlr_get_framebuffer_size();
rlr_vec2_t rlr_get_mouse_position();

//callbacks
void rlr_set_input_user(void* user);
void rlr_set_log_user(void* user);
void rlr_set_mouse_input_callback(rlr_input_mouse_callback_t func);
void rlr_set_key_input_callback(rlr_input_key_callback_t func);
void rlr_set_log_callback(rlr_log_callback_t func, bool add_new_line);

//screen / world convertions
bool rlr_screen_pos_to_ground(rlr_vec2_t mouse_pos, rlr_vec3_t* out_position);
rlr_vec2_t rlr_world_to_screen(rlr_vec3_t world_pos);

//default resources
rlr_res_texture_t* rlr_default_texture();

//camera
void rlr_set_camera(rlr_vec3_t pos, rlr_quat_t rotation);
rlr_vec3_t rlr_get_camera_pos();
rlr_quat_t rlr_get_camera_rot();

//backend information
const char* rlr_get_backend_implementation();
const char* rlr_get_backend_context();
const char* rlr_get_gpu_name();

//settings
/*
    if gpu evaluation is used, the relevant poses for an animated model
    are fetched from a texture in the vertex shader. This makes it 
    possible to have a lot of instances drawn together in one go.

    otherwise, the cpu will calculate the final pose for the model and upload
    it per instance to a UBO buffer. Since the ubo has limited space, instances
    may divided into different draw calls if they exceed the UBO space of 16 kB
    that the renderer is using as the limit

    in general, the cpu path can be faster on older hardware where
    texelFetch is an expensive operation
*/
void rlr_settings_set_animation_evaluation(rlr_setting_animation_evaluation_t setting);

//statistics
rlr_statistics_t* rlr_get_total_statistics();
rlr_statistics_t* rlr_get_statistics();