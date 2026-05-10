#pragma once
#if defined(_WIN32)
#include <d3d10.h>

typedef struct d3d10_texture_t {
    ID3D10Texture2D* texture;
    ID3D10ShaderResourceView* srv;
} d3d10_texture_t;

#endif