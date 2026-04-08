#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/shader.h"

typedef void (*rlr_backend_proc_t)(void);
typedef rlr_backend_proc_t (*rlr_backend_loader_t)(const char*);

typedef struct rlr_backend_t {
    uint32_t (*create_texture)(uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps);
    void (*free_texture)(uint32_t handle);
    uint32_t (*compile_shader)(const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size);
    void (*free_shader)(uint32_t shader);
    void (*use_shader)(uint32_t shader);
    uint32_t (*shader_uniform_location)(uint32_t shader, const char* name);
    void (*shader_uniform_set)(uint32_t shader, uint32_t location, void* data, rlr_shader_uniform_type type);

    void (*free)();
} rlr_backend_t;

rlr_backend_t* rlr_backend_gl3(rlr_backend_loader_t proc_loader);
rlr_backend_t* rlr_backend_gles3(rlr_backend_loader_t proc_loader);