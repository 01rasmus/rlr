#define RGFW_OPENGL
#include <RGFW.h>
#include <glad.h>
#include "error.h"
#include "rlr.h"

static rlr_t* rlr = NULL;

bool rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    rlr = NULL;

    rlr = malloc(sizeof(rlr_t));
    if(!rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    rlr->window = RGFW_createWindow(title, window_width, window_height, window_width, window_height, RGFW_windowAllowDND | RGFW_windowCenter | RGFW_windowScaleToMonitor | RGFW_windowOpenGL);
    if(!rlr->window) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }
    RGFW_window_swapInterval_OpenGL(rlr->window, 0);
    RGFW_window_setExitKey(rlr->window, RGFW_escape);
    RGFW_window_makeCurrentContext_OpenGL(rlr->window);

    if(!gladLoadGLLoader((GLADloadproc)RGFW_getProcAddress_OpenGL)) {
        rlr_error_set(RLR_ERR_CONTEXT_CREATION);
        goto err;
    }

    return true;
err:
    rlr_deinit();
    return false;
}

void rlr_deinit() {
    if(rlr) {
        
    }
    free(rlr);
    rlr = NULL;
}

bool rlr_render() {
    RGFW_event event;
    if(!RGFW_window_checkEvent(rlr->window, &event)) {
        if (event.type == RGFW_quit) {
            return false;
        }
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RGFW_window_swapBuffers_OpenGL(rlr->window);
    return true;
}