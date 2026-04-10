#define RGFW_OPENGL
#include <RGFW.h>
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

    RGFW_glHints* hints = RGFW_getGlobalHints_OpenGL();
    hints->major = 3;
    hints->minor = 3;
    RGFW_setGlobalHints_OpenGL(hints);

    rlr->window = RGFW_createWindow(title, window_width, window_height, window_width, window_height, RGFW_windowAllowDND | RGFW_windowCenter | RGFW_windowScaleToMonitor | RGFW_windowOpenGL);
    if(!rlr->window) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }
    RGFW_window_swapInterval_OpenGL(rlr->window, 0);
    RGFW_window_setExitKey(rlr->window, RGFW_escape);
    RGFW_window_makeCurrentContext_OpenGL(rlr->window);

    rlr_backend_t* backend = rlr_backend_gl3((rlr_backend_loader_t)RGFW_getProcAddress_OpenGL);
    rlr->backend = backend;

    // if(!gladLoadGLLoader((GLADloadproc)RGFW_getProcAddress_OpenGL)) {
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
        rlr->backend->backend_free();
        free(rlr->backend);
    }
    free(rlr);
    RGFW_deinit();
}

bool rlr_render(rlr_t* rlr) {
    RGFW_event event;
    if(!RGFW_window_checkEvent(rlr->window, &event)) {
        if (event.type == RGFW_quit) {
            return false;
        }
    }

    rlr->backend->clear_color(0.2, 0.3, 0.3, 1.0);
    rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR);

    RGFW_window_swapBuffers_OpenGL(rlr->window);
    return true;
}