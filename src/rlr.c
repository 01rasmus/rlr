#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "backends/backend.h"
#include "error.h"
#include "rlr.h"

rlr_t* rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    rlr_t* rlr = NULL;

    rlr = malloc(sizeof(rlr_t));
    if(!rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    rlr->backend = NULL;
    rlr->window = NULL;

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
    glfwSwapInterval(1);
    //RGFW_window_setExitKey(rlr->window, RGFW_escape);
    glfwMakeContextCurrent(rlr->window);

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

    return rlr;
err:
    rlr_free(rlr);
    return NULL;
}

void rlr_free(rlr_t* rlr) {
    if(rlr) {
        if(rlr->backend) {
            rlr->backend->backend_free();
            free(rlr->backend);
        }
        if(rlr->window) {
            glfwDestroyWindow(rlr->window);
        }
    }
    free(rlr);
    glfwTerminate();
}

bool rlr_render(rlr_t* rlr) {
    glfwPollEvents();

    if(glfwWindowShouldClose(rlr->window)) {
        return false;
    }

    rlr->backend->clear_color(0.2, 0.3, 0.3, 1.0);
    rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR);

    glfwSwapBuffers(rlr->window);
    return true;
}