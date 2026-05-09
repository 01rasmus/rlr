#include <stdlib.h>
#include "backend.h"

#if defined(_WIN32)
#include <windows.h>
#include <d3d10.h>
#include <d3dcompiler.h>
#endif

rlr_backend_t* rlr_backend_d3d10(rlr_backend_loader_t proc_loader) {
    #if !defined(_WIN32)
    return NULL;
    #else
    
    //TODO: implement directx 10
    return NULL;
    #endif
}