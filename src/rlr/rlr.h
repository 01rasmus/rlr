#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "types/input.h"
#include "types/misc.h"
#include "types/log.h"
#include "types/res.h"
#include "types/entity.h"
#include "math/quat.h"
#include "math/vec.h"

/**
 * @param title The window title
 * @param storage_identifier The identifier to use to store cached data or settings
 * @param flags Flags to set certain behaviour for the renderer. See @ref rlr_init_flag_t
 * 
 * @return Zero on failure, non-zero on success
 */
int32_t rlr_init(const char* title, const char* storage_identifier, rlr_init_flag_t flags);

/**
 * @param title The window title
 * @param storage_identifier The identifier to use to store cached data or settings
 * @param log_callback Override for the default logging function
 * @param log_user A user pointer that will is passed to the log callback function
 * @param flags Flags to set certain behaviour for the renderer. See @ref rlr_init_flag_t
 * 
 * @return Zero on failure, non-zero on success
 */
int32_t rlr_init_log(const char* title, const char* storage_identifier, rlr_log_callback_t log_callback, void* log_user, rlr_init_flag_t flags);

bool rlr_update();
void rlr_free();

void rlr_set_clear_color(float r, float g, float b, float a);
void rlr_set_cube_map(rlr_cub_map_t* cube_map_id);
rlr_vec2_t rlr_get_framebuffer_size();
rlr_vec2_t rlr_get_mouse_position();

//storage
FILE* rlr_storage_fopen(const char* sub_directory, uint64_t hash, const char* file_type, const char* modes);

//callbacks
void rlr_set_input_user(void* user);
void rlr_set_mouse_input_callback(rlr_input_mouse_callback_t func);
void rlr_set_key_input_callback(rlr_input_key_callback_t func);

//screen / world convertions
bool rlr_screen_pos_to_ground(rlr_vec2_t mouse_pos, rlr_vec3_t* out_position);
rlr_vec2_t rlr_world_to_screen(rlr_vec3_t world_pos);

//default resources
rlr_tex_t* rlr_default_texture();

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