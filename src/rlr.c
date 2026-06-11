#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "backends/backend.h"
#include "error.h"
#include "rlr.h"

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
"    float screen_px_range  = max(0.5 * dot(px_range, smoothing), 1.0);\n"
"    float screen_px_distance = screen_px_range * (sd - 0.5); \n"
"    float alpha = clamp(screen_px_distance + 0.5, 0.0f, 1.0);\n"
"    final_color = vec4(1.0, 1.0, 1.0, alpha);\n"
"}\n";

const char mtsdf_vertex[] = "#version 330\n"
"layout (location = 0) in vec2 pos;\n"
"layout (location = 1) in vec2 uv;\n"
"out vec2 frag_uv;\n"
"void main() {\n"
"frag_uv = uv;\n"
"gl_Position = vec4((pos.x / 960.0) * 2.0 - 1.0, (pos.y / 540.0) * 2.0 + 0.9, 0.0, 1.0);\n"
"}\n";

rlr_t* rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    rlr_t* rlr = NULL;

    rlr = malloc(sizeof(rlr_t));
    if(!rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    rlr->backend = NULL;
    rlr->window = NULL;
    rlr->shader_text = NULL;
    rlr->obj_labels = NULL;

    if(!glfwInit()) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    rlr->window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
    if(!rlr->window) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }
    glfwMakeContextCurrent(rlr->window);
    glfwSwapInterval(0);

    rlr_backend_t* backend = rlr_backend_gl3((rlr_backend_loader_t)glfwGetProcAddress);
    rlr->backend = backend;

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

    rlr->shader_text = rlr_shader_create(rlr, mtsdf_vertex, mtsdf_fragment);
    return rlr;
err:
    rlr_free(rlr);
    return NULL;
}

void rlr_free(rlr_t* rlr) {
    if(!rlr) {
        return;
    }

    //remove objects
    for(int64_t i = 0; i < arrlen(rlr->obj_labels); i++) {
        rlr_obj_label_free(rlr, &rlr->obj_labels[i]);
    }
    arrfree(rlr->obj_labels);

    //free backend API and window
    if(rlr->backend) {
        rlr->backend->backend_free();
        free(rlr->backend);
    }
    if(rlr->window) {
        glfwDestroyWindow(rlr->window);
    }
    free(rlr);
    glfwTerminate();
}

double rlr_time() {
    return glfwGetTime();
}

bool rlr_draw(rlr_t* rlr) {
    glfwPollEvents();

    if(glfwWindowShouldClose(rlr->window)) {
        return false;
    }

    rlr->backend->clear_color(0.2, 0.3, 0.3, 1.0);
    rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR);

    //render objects
    rlr_shader_use(rlr, rlr->shader_text);
    for(int64_t i = 0; i < arrlen(rlr->obj_labels); i++) {
        rlr_obj_label_t* label = &rlr->obj_labels[i];
        if(!label->visible) {
            continue;
        }
        rlr_texture_use(rlr, label->font->texture);
        rlr->backend->vertex_array_bind(label->vao);
        rlr->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr->backend->draw_array(0, label->vertex_count);
    }

    glfwSwapBuffers(rlr->window);
    return true;
}