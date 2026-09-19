#pragma once
#include <stdint.h>
#include "math/vec.h"

#define RLR_DEBUG   1
#define RLR_NULL    ((uint64_t)0u)

#define RLR_INPUT_ACTION_RELEASE        0
#define RLR_INPUT_ACTION_PRESS          1
#define RLR_INPUT_ACTION_REPEAT         2

#define RLR_INPUT_MOD_SHIFT             0x0001
#define RLR_INPUT_MOD_CONTROL           0x0002
#define RLR_INPUT_MOD_ALT               0x0004
#define RLR_INPUT_MOD_SUPER             0x0008

#define RLR_INPUT_MOUSE_1               0
#define RLR_INPUT_MOUSE_2               1
#define RLR_INPUT_MOUSE_3               2
#define RLR_INPUT_MOUSE_4               3
#define RLR_INPUT_MOUSE_5               4
#define RLR_INPUT_MOUSE_6               5
#define RLR_INPUT_MOUSE_7               6
#define RLR_INPUT_MOUSE_8               7
#define RLR_INPUT_MOUSE_LEFT            RLR_INPUT_MOUSE_1
#define RLR_INPUT_MOUSE_RIGHT           RLR_INPUT_MOUSE_2
#define RLR_INPUT_MOUSE_MIDDLE          RLR_INPUT_MOUSE_3

#define RLR_INPUT_KEY_UNKNOWN           -1
#define RLR_INPUT_KEY_SPACE             32
#define RLR_INPUT_KEY_APOSTROPHE        39
#define RLR_INPUT_KEY_COMMA             44
#define RLR_INPUT_KEY_MINUS             45
#define RLR_INPUT_KEY_PERIOD            46 
#define RLR_INPUT_KEY_SLASH             47
#define RLR_INPUT_KEY_0                 48
#define RLR_INPUT_KEY_1                 49
#define RLR_INPUT_KEY_2                 50
#define RLR_INPUT_KEY_3                 51
#define RLR_INPUT_KEY_4                 52
#define RLR_INPUT_KEY_5                 53
#define RLR_INPUT_KEY_6                 54
#define RLR_INPUT_KEY_7                 55
#define RLR_INPUT_KEY_8                 56
#define RLR_INPUT_KEY_9                 57
#define RLR_INPUT_KEY_SEMICOLON         59
#define RLR_INPUT_KEY_EQUAL             61
#define RLR_INPUT_KEY_A                 65
#define RLR_INPUT_KEY_B                 66
#define RLR_INPUT_KEY_C                 67
#define RLR_INPUT_KEY_D                 68
#define RLR_INPUT_KEY_E                 69
#define RLR_INPUT_KEY_F                 70
#define RLR_INPUT_KEY_G                 71
#define RLR_INPUT_KEY_H                 72
#define RLR_INPUT_KEY_I                 73
#define RLR_INPUT_KEY_J                 74
#define RLR_INPUT_KEY_K                 75
#define RLR_INPUT_KEY_L                 76
#define RLR_INPUT_KEY_M                 77
#define RLR_INPUT_KEY_N                 78
#define RLR_INPUT_KEY_O                 79
#define RLR_INPUT_KEY_P                 80
#define RLR_INPUT_KEY_Q                 81
#define RLR_INPUT_KEY_R                 82
#define RLR_INPUT_KEY_S                 83
#define RLR_INPUT_KEY_T                 84
#define RLR_INPUT_KEY_U                 85
#define RLR_INPUT_KEY_V                 86
#define RLR_INPUT_KEY_W                 87
#define RLR_INPUT_KEY_X                 88
#define RLR_INPUT_KEY_Y                 89
#define RLR_INPUT_KEY_Z                 90
#define RLR_INPUT_KEY_LEFT_BRACKET      91
#define RLR_INPUT_KEY_BACKSLASH         92  
#define RLR_INPUT_KEY_RIGHT_BRACKET     93
#define RLR_INPUT_KEY_GRAVE_ACCENT      96
#define RLR_INPUT_KEY_WORLD_1           161
#define RLR_INPUT_KEY_WORLD_2           162
#define RLR_INPUT_KEY_ESCAPE            256
#define RLR_INPUT_KEY_ENTER             257
#define RLR_INPUT_KEY_TAB               258
#define RLR_INPUT_KEY_BACKSPACE         259
#define RLR_INPUT_KEY_INSERT            260
#define RLR_INPUT_KEY_DELETE            261
#define RLR_INPUT_KEY_RIGHT             262
#define RLR_INPUT_KEY_LEFT              263
#define RLR_INPUT_KEY_DOWN              264
#define RLR_INPUT_KEY_UP                265
#define RLR_INPUT_KEY_PAGE_UP           266
#define RLR_INPUT_KEY_PAGE_DOWN         267
#define RLR_INPUT_KEY_HOME              268
#define RLR_INPUT_KEY_END               269
#define RLR_INPUT_KEY_CAPS_LOCK         280
#define RLR_INPUT_KEY_SCROLL_LOCK       281
#define RLR_INPUT_KEY_NUM_LOCK          282
#define RLR_INPUT_KEY_PRINT_SCREEN      283
#define RLR_INPUT_KEY_PAUSE             284
#define RLR_INPUT_KEY_F1                290
#define RLR_INPUT_KEY_F2                291
#define RLR_INPUT_KEY_F3                292
#define RLR_INPUT_KEY_F4                293
#define RLR_INPUT_KEY_F5                294
#define RLR_INPUT_KEY_F6                295
#define RLR_INPUT_KEY_F7                296
#define RLR_INPUT_KEY_F8                297
#define RLR_INPUT_KEY_F9                298
#define RLR_INPUT_KEY_F10               299
#define RLR_INPUT_KEY_F11               300
#define RLR_INPUT_KEY_F12               301
#define RLR_INPUT_KEY_F13               302
#define RLR_INPUT_KEY_F14               303
#define RLR_INPUT_KEY_F15               304
#define RLR_INPUT_KEY_F16               305
#define RLR_INPUT_KEY_F17               306
#define RLR_INPUT_KEY_F18               307
#define RLR_INPUT_KEY_F19               308
#define RLR_INPUT_KEY_F20               309
#define RLR_INPUT_KEY_F21               310
#define RLR_INPUT_KEY_F22               311
#define RLR_INPUT_KEY_F23               312
#define RLR_INPUT_KEY_F24               313
#define RLR_INPUT_KEY_F25               314
#define RLR_INPUT_KEY_KP_0              320
#define RLR_INPUT_KEY_KP_1              321
#define RLR_INPUT_KEY_KP_2              322
#define RLR_INPUT_KEY_KP_3              323
#define RLR_INPUT_KEY_KP_4              324
#define RLR_INPUT_KEY_KP_5              325
#define RLR_INPUT_KEY_KP_6              326
#define RLR_INPUT_KEY_KP_7              327
#define RLR_INPUT_KEY_KP_8              328
#define RLR_INPUT_KEY_KP_9              329
#define RLR_INPUT_KEY_KP_DECIMAL        330
#define RLR_INPUT_KEY_KP_DIVIDE         331
#define RLR_INPUT_KEY_KP_MULTIPLY       332
#define RLR_INPUT_KEY_KP_SUBTRACT       333
#define RLR_INPUT_KEY_KP_ADD            334
#define RLR_INPUT_KEY_KP_ENTER          335
#define RLR_INPUT_KEY_KP_EQUAL          336
#define RLR_INPUT_KEY_LEFT_SHIFT        340
#define RLR_INPUT_KEY_LEFT_CONTROL      341
#define RLR_INPUT_KEY_LEFT_ALT          342
#define RLR_INPUT_KEY_LEFT_SUPER        343
#define RLR_INPUT_KEY_RIGHT_SHIFT       344
#define RLR_INPUT_KEY_RIGHT_CONTROL     345
#define RLR_INPUT_KEY_RIGHT_ALT         346
#define RLR_INPUT_KEY_RIGHT_SUPER       347
#define RLR_INPUT_KEY_MENU              348
#define RLR_INPUT_KEY_LAST              GLFW_KEY_MENU

#define RLR_LOG_LEVEL_INFO              0x0
#define RLR_LOG_LEVEL_WARNING           0x1
#define RLR_LOG_LEVEL_ERROR             0x2
#define RLR_LOG_LEVEL_DEBUG             0x3

typedef uint64_t rlr_handle_t;
typedef uint8_t rlr_log_level_t;

typedef struct rlr_res_animated_model_t rlr_res_animated_model_t;
typedef struct rlr_res_cube_map_t rlr_res_cube_map_t;
typedef struct rlr_res_font_t rlr_res_font_t;
typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_static_model_t rlr_res_static_model_t;
typedef struct rlr_res_texture_atlas_t rlr_res_texture_atlas_t;
typedef struct rlr_res_texture_atlas_tile_t rlr_res_texture_atlas_tile_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;
typedef struct rlr_res_uniform_t rlr_res_uniform_t;

typedef struct rlr_obj_animated_model_t rlr_obj_animated_model_t;
typedef struct rlr_obj_label_t rlr_obj_label_t;
typedef struct rlr_obj_model_occluder_t rlr_obj_model_occluder_t;
typedef struct rlr_obj_mouse_input_area_t rlr_obj_mouse_input_area_t;
typedef struct rlr_obj_nine_patch_t rlr_obj_nine_patch_t;
typedef struct rlr_obj_sprite_t rlr_obj_sprite_t;
typedef struct rlr_obj_static_model_t rlr_obj_static_model_t;

typedef void (*rlr_input_key_callback_t)(uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods, void* user);
typedef void (*rlr_input_mouse_callback_t)(uint32_t button, uint32_t action, uint32_t mods, rlr_vec2_t pos, void* user);
typedef void (*rlr_log_callback_t)(rlr_log_level_t log_level, const char* log, const char* file, uint64_t line, void* user);
typedef void (*rlr_input_area_on_enter_t)(void* user);
typedef void (*rlr_input_area_on_leave_t)(void* user);
typedef void (*rlr_input_area_on_pressed_t)(void* user);
typedef void (*rlr_input_area_on_release_t)(void* user);

typedef enum rlr_init_flags_t {
    RLR_INIT_FLAG_FULLSCREEN = 0x1,
    RLR_INIT_FLAG_VSYNC = 0x2,
} rlr_init_flags_t;