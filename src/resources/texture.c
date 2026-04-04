#include <stdint.h>
#include "../../lib/glad.h"
#include "../../lib/stb_image.h"
#include "../error.h"
#include "texture.h"

rlr_texture_t* rlr_texture_load(const char* texture_path, bool generate_mipmaps) {
    rlr_texture_t* texture = malloc(sizeof(rlr_texture_t));
    if(!texture) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    texture->texture = 0;
    texture->height = 0;
    texture->width = 0;

    int32_t w = 0;
    int32_t h = 0;
    uint8_t* data = stbi_load(texture_path, &w, &h, NULL, 4);
    if(!data) {
        rlr_error_setf(RLR_ERR_IMAGE_NOT_LOADED, "file \"%s\"", texture_path);
        goto err;
    }

    texture->width = w;
    texture->height = h;

    glGenTextures(1, &texture->texture);
    if(texture->texture == 0) {
        rlr_error_setf(RLR_ERR_OPENGL_NULL_HANDLE, "file \"%s\"", texture_path);
        goto err;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    if(generate_mipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
    return texture;
err:
    stbi_image_free(data);
    rlr_texture_free(texture);
    return NULL;
}

void rlr_texture_free(rlr_texture_t* texture) {
    if(texture) {
        glDeleteTextures(1, &texture->texture);
    }
    free(texture);
}