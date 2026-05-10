#include <stdlib.h>
#include "backend.h"
#include "error.h"

#if defined(_WIN32)
#include <windows.h>
#include <d3d10.h>
#include <d3dcompiler.h>
#include "d3d10.h"

ID3D10Device* device = NULL;
IDXGISwapChain* swapchain = NULL;

rlr_handle_t d3d10_create_texture(uint8_t* rgba, uint32_t width, uint32_t height, bool generate_mipmaps) {
    d3d10_texture_t* texture = malloc(sizeof(d3d10_texture_t));
    

    return texture;
err:
    d3d10_free_texture((rlr_handle_t)texture);
    return NULL;
}

void d3d10_free_texture(rlr_handle_t texture) {

}

rlr_handle_t d3d10_compile_shader(const char* vertex_shader, const char* fragment_shader, char* error, uint64_t error_size) {

}

void d3d10_free_shader(rlr_handle_t shader) {

}

void d3d10_use_shader(rlr_handle_t shader) {

}

void d3d10_shader_bind_uniform_block(rlr_handle_t shader, const char* uniform_block_name, uint32_t slot) {

}

rlr_handle_t d3d10_create_uniform_buffer(uint64_t size, void* init_data) {

}

void d3d10_update_uniform_buffer(rlr_handle_t buffer, uint64_t offset, uint64_t size, void* data) {

}

void d3d10_bind_uniform_buffer(rlr_handle_t buffer, uint32_t slot) {

}

void d3d10_free_uniform_buffer(rlr_handle_t buffer) {

}

void d3d10_clear(uint64_t mask) {

}

void d3d10_clear_color(float r, float g, float b, float a) {

}

void d3d10_backend_free() {
    if(swapchain) {
        swapchain->lpVtbl->Release(swapchain);
    }
    if(device) {
        device->lpVtbl->Release(device);
    }
}

#endif

rlr_backend_t* rlr_backend_d3d10(void* window_handle) {
    #if !defined(_WIN32)
    return NULL;
    #else
    
    //create the device
    DXGI_SWAP_CHAIN_DESC swap_desc;
    ZeroMemory(&swap_desc, sizeof(swap_desc));

    swap_desc.BufferCount = 1;
    swap_desc.BufferDesc.Width = 1024;
    swap_desc.BufferDesc.Height = 768;
    swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_desc.OutputWindow = window_handle;
    swap_desc.SampleDesc.Count = 1;
    swap_desc.Windowed = TRUE;

    HRESULT hr = D3D10CreateDeviceAndSwapChain(
        NULL,
        D3D10_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        D3D10_SDK_VERSION,
        &swap_desc,
        &swapchain,
        &device
    );

    if (FAILED(hr)) {
        goto err;
    }

    rlr_backend_t* backend = NULL;
    backend = malloc(sizeof(rlr_backend_t));
    if(!backend) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    memset(backend, 0, sizeof(rlr_backend_t));
    
    #define X(RET, NAME, PARAMS) backend->NAME = d3d10_##NAME;
    RLR_BACKEND_FUNCTIONS(X)
    #undef X

    return backend;
err:
    d3d10_backend_free();
    return NULL;
    #endif
}