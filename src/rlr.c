#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "backends/backend.h"
#include "resources/uniform.h"
#include "resources/shader.h"
#include "resources/font.h"
#include "objects/label.h"
#include "error.h"
#include "rlr.h"

typedef struct uniform_ui_t {
    float inv_x;
    float inv_y;
} uniform_ui_t;

static rlr_t* _rlr = NULL;

const char mtsdf_fragment[] = "#version 330\n"
"in vec2 frag_uv;\n"
"out vec4 final_color;\n"
"uniform sampler2D tex;\n"
"float median(float r, float g, float b) {\n"
"    return max(min(r,g), min(max(r,g),b));\n"
"}\n"
"float screen_px_range() {\n"
"    float px_range = 2.0;\n"
"    vec2 unit_range = vec2(px_range) / vec2(textureSize(tex, 0));\n"
"    vec2 screen_tex_size = vec2(1.0) / fwidth(frag_uv);\n"
"    return max(0.5 * dot(unit_range, screen_tex_size), 1.0);\n"
"}\n"
"void main() {\n"
"    vec3 msd = texture(tex, frag_uv).rgb;\n"
"    float sd = median(msd.r, msd.g, msd.b);\n"
"    float screen_px_distance = screen_px_range() * (sd - 0.5);\n"
"    float alpha = clamp(screen_px_distance + 0.5, 0.0, 1.0);\n"
"    final_color = vec4(1.0, 1.0, 1.0, alpha);\n"
"}\n";

const char mtsdf_vertex[] = "#version 330\n"
"layout (location = 0) in vec2 pos;\n"
"layout (location = 1) in vec2 uv;\n"
"layout(std140) uniform ui {\n"
"   float inv_x;\n"
"   float inv_y;\n"
"};\n"
"out vec2 frag_uv;\n"
"void main() {\n"
"frag_uv = uv;\n"
"gl_Position = vec4(pos.x * inv_x * 2.0 - 1.0, 1.0 - pos.y * inv_y * 2.0, 0.0, 1.0);\n"
"}\n";

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    _rlr = NULL;

    _rlr = malloc(sizeof(rlr_t));
    if(!_rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    _rlr->backend = NULL;
    _rlr->window = NULL;
    _rlr->shader_text = NULL;
    _rlr->obj_labels = NULL;

    _rlr->framebuffer_width = window_width;
    _rlr->framebuffer_height = window_height;

    if(!glfwInit()) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _rlr->window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
    if(!_rlr->window) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }
    glfwMakeContextCurrent(_rlr->window);
    glfwSwapInterval(0);

    rlr_backend_t* backend = rlr_backend_gl3((rlr_backend_loader_t)glfwGetProcAddress);
    _rlr->backend = backend;

    // if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     rlr_error_set(RLR_ERR_CONTEXT_CREATION);
    //     goto err;
    // }

    // int32_t major = 0;
    // int32_t minor = 0;
    // glGetIntegerv(GL_MAJOR_VERSION, &major); 
    // glGetIntegerv(GL_MINOR_VERSION, &minor);
    // bool is_compatible_version = (major == 3 && minor >= 3) || major == 4;
    // if(!is_compatible_version) {
    //     rlr_error_setf(RLR_ERR_OPENGL_INCOMPATIBLE_VERSION, "expected atleast OpenGL 3.3, but got OpenGL %d.%d", major, minor);
    //     goto err;
    // }

    _rlr->shader_text = rlr_shader_create(mtsdf_vertex, mtsdf_fragment);
    rlr_shader_bind_uniform_slot(_rlr->shader_text, "ui", 0);

    uniform_ui_t ubo_ui = {
        .inv_x = 1.0 / (float)window_width,
        .inv_y = 1.0 / (float)window_height
    };

    _rlr->ubo_ui = rlr_uniform_create_dynamic(sizeof(uniform_ui_t));
    rlr_uniform_update(_rlr->ubo_ui, 0, &ubo_ui, sizeof(uniform_ui_t));
    rlr_uniform_bind(_rlr->ubo_ui, 0);
    return;
err:
    rlr_free();
    return;
}

void rlr_free() {
    if(!_rlr) {
        return;
    }

    //remove objects
    for(int64_t i = 0; i < arrlen(_rlr->obj_labels); i++) {
        rlr_obj_label_free(&_rlr->obj_labels[i]);
    }
    arrfree(_rlr->obj_labels);

    //free resources
    rlr_uniform_free(_rlr->ubo_ui);
    rlr_shader_free(_rlr->shader_text);

    //free backend API and window
    if(_rlr->backend) {
        _rlr->backend->backend_free();
        free(_rlr->backend);
    }
    if(_rlr->window) {
        glfwDestroyWindow(_rlr->window);
    }
    free(_rlr);
    glfwTerminate();
    _rlr = NULL;
}

double rlr_time() {
    return glfwGetTime();
}

bool rlr_draw() {
    glfwPollEvents();

    if(glfwWindowShouldClose(_rlr->window)) {
        return false;
    }

    int32_t width = 0;
    int32_t height = 0;
    glfwGetFramebufferSize(_rlr->window, &width, &height);
    if(width != _rlr->framebuffer_width || height != _rlr->framebuffer_height) {
        uniform_ui_t ubo_ui = {
            .inv_x = 1.0 / (float)width,
            .inv_y = 1.0 / (float)height
        };
        rlr_uniform_update(_rlr->ubo_ui, 0, &ubo_ui, sizeof(uniform_ui_t));
        rlr_backend()->viewport_set(0, 0, width, height);
    }

    _rlr->backend->clear_color(0.1, 0.2, 0.3, 1.0);
    _rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR);

    //render objects
    rlr_shader_use(_rlr->shader_text);
    for(int64_t i = 0; i < arrlen(_rlr->obj_labels); i++) {
        rlr_obj_label_t* label = &_rlr->obj_labels[i];
        if(!label->visible) {
            continue;
        }
        rlr_texture_bind(label->font->texture, 0);
        _rlr->backend->vertex_array_bind(label->vao);
        _rlr->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
        _rlr->backend->draw_array(0, label->vertex_count);
    }

    glfwSwapBuffers(_rlr->window);
    return true;
}

rlr_backend_t* rlr_backend() {
    return _rlr->backend;
}

rlr_t* _rlr_raw() {
    return _rlr;
}