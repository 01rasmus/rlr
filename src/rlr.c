#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "backends/backend.h"
#include "error.h"
#include "rlr.h"

static rlr_t* _rlr = NULL;

const char mtsdf_fragment[] = "#version 330\n"
"in vec2 frag_uv;\n"
"out vec4 final_color;\n"
"uniform sampler2D tex;\n"
"\n"
"float median(float r, float g, float b) {\n"
"    return max(min(r,g), min(max(r,g),b));\n"
"}\n"
"void main() {\n"
"    float px_range = -1.0;\n"
"    vec3 sample = texture(tex, frag_uv).rgb;\n"
"    float sd = median(sample.r, sample.g, sample.b);\n"
"    float smoothing = 1.0 / fwidth(sd);\n"
"    float screen_px_range = max(0.5 * px_range * smoothing, 1.0);\n"
"    float screen_px_distance = screen_px_range * (sd - 0.5);\n"
"    float alpha = clamp(screen_px_distance + 0.5, 0.0f, 1.0);\n"
"    final_color = vec4(1.0, 1.0, 1.0, alpha);\n"
"}\n";

const char mtsdf_vertex[] = "#version 330\n"
"layout (location = 0) in vec2 pos;\n"
"layout (location = 1) in vec2 uv;\n"
"out vec2 frag_uv;\n"
"void main() {\n"
"vec2 inv_screen = vec2(1.0 / 960.0, 1.0 / 540.0);\n"
"frag_uv = uv;\n"
"//gl_Position = vec4((pos.x / 960.0) * 2.0 - 1.0, (pos.y / 540.0) * 2.0 + 0.9, 0.0, 1.0);\n"
"gl_Position = vec4(pos.x * inv_screen.x * 2.0 - 1.0, 1.0 - pos.y * inv_screen.y * 2.0, 0.0, 1.0);\n"
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

    _rlr->backend->clear_color(0.0, 0.0, 0.0, 1.0);
    _rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR);

    //render objects
    rlr_shader_use(_rlr->shader_text);
    for(int64_t i = 0; i < arrlen(_rlr->obj_labels); i++) {
        rlr_obj_label_t* label = &_rlr->obj_labels[i];
        if(!label->visible) {
            continue;
        }
        rlr_texture_use(label->font->texture);
        _rlr->backend->vertex_array_bind(label->vao);
        _rlr->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
        _rlr->backend->draw_array(0, label->vertex_count);
    }

    glfwSwapBuffers(_rlr->window);
    return true;
}

rlr_t* _rlr_raw() {
    return _rlr;
}