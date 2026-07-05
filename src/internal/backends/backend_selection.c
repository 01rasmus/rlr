#include <GLFW/glfw3.h>
#include "internal/backends/backend.h"
#include "backend_selection.h"

typedef bool (*rlr_backend_selection_function_t)(GLFWwindow** window, rlr_backend_t** backend, GLFWmonitor* monitor, uint32_t width, uint32_t height, const char* title);

static bool try_gl3(GLFWwindow** window, rlr_backend_t** backend, GLFWmonitor* monitor, uint32_t width, uint32_t height, const char* title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 0);

    (*window) = glfwCreateWindow(width, height, title, monitor, NULL);
    if(!(*window)) {
        goto err;
    }
    glfwMakeContextCurrent(*window);

    (*backend) = rlr_backend_gl3((rlr_backend_loader_t)glfwGetProcAddress);
    if(!(*backend)) {
        goto err;
    }
    return true;
err:
    glfwMakeContextCurrent(NULL);
    if(*window) {
        glfwDestroyWindow(*window);
    }
    (*window) = NULL;
    (*backend) = NULL;
    return false;
}

static bool try_gles3(GLFWwindow** window, rlr_backend_t** backend, GLFWmonitor* monitor, uint32_t width, uint32_t height, const char* title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    (*window) = glfwCreateWindow(width, height, title, monitor, NULL);
    if(!(*window)) {
        goto err;
    }
    glfwMakeContextCurrent(*window);
    
    (*backend) = rlr_backend_gles3((rlr_backend_loader_t)glfwGetProcAddress);
    if(!(*backend)) {
        goto err;
    }
    return true;
err:
    glfwMakeContextCurrent(NULL);
    if(*window) {
        glfwDestroyWindow(*window);
    }
    (*window) = NULL;
    (*backend) = NULL;
    return false;
}

static rlr_backend_selection_function_t backend_selection_functions[2] = {
    try_gl3,
    try_gles3,
};

bool rlr_internal_backend_selection(GLFWwindow** window, rlr_backend_t** backend, GLFWmonitor* monitor, uint32_t width, uint32_t height, const char* title) {
    if(!window || !backend) {
        return false;
    }

    for(size_t i = 0; i < sizeof(backend_selection_functions) / sizeof(backend_selection_functions[0]); i++) {
        rlr_backend_selection_function_t try_backend = backend_selection_functions[i];
        if(try_backend(window, backend, monitor, width, height, title)) {
            glfwShowWindow(*window);
            return true;
        }
    }

    return false;
}